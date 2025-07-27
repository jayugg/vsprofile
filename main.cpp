#include <iostream>
#include <filesystem>
#include <fstream>
#include "include/json.hpp"
#include "Profile.hpp"
#include "Profile.cpp"
#include "Command.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

const fs::path dataPath = "Profiles";
const fs::path dataFile = dataPath / "profiles.json";
const fs::path vsPath = "VintagestoryData";
const fs::path modPath = vsPath / "Mods";

json loadData() {
    fs::create_directories(dataFile.parent_path());
    json data;
    // Load existing JSON if the file exists and is non-empty
    if (fs::exists(dataFile) && fs::file_size(dataFile) > 0) {
        std::ifstream in(dataFile);
        try {
            in >> data;              // operator>> parses stream
        } catch (const json::parse_error& e) {
            // corrupt file? start fresh
            data = json::array();
        }
    } else {
        data = json::array();
    }
    // Ensure it's an array (if the file contained something else)
    if (!data.is_array()) data = json::array();
    return data;
}

Profile loadProfile(std::string_view profile_name) {
    json data = loadData();                    // assumes it returns an array
    for (const auto& item : data) {
        if (!item.is_object() || !item.contains("name")) continue;

        // zero-copy access to the stored string
        if (item["name"].get_ref<const std::string&>() == profile_name) {
            return item.get<Profile>();        // parse only on match
        }
    }
    throw std::invalid_argument("Could not find requested profile");
}

std::vector<Profile> loadAllProfiles() {
    json data = loadData();
    std::vector<Profile> profiles;
    for (const auto& item : data) {
        profiles.push_back(item.get<Profile>());
    }
    return profiles;
}

void restoreData() {
    fs::create_directories(dataFile.parent_path());

    // Check if every profile has a matching directory
    std::vector<Profile> all_profiles = loadAllProfiles();
    json good_profiles;
    json broken_profiles;
    for (const auto& profile : all_profiles) {
        if (fs::exists(dataPath / profile.name)) {
            good_profiles.push_back((json)profile);
            continue;
        }
        std::cerr << "Couldn't find folder for profile: " << profile.name << '\n';
        broken_profiles.push_back((json)profile);
    }

    // Save back prettily
    std::ofstream out(dataFile);
    out << good_profiles.dump(4) << '\n';
}

void _saveProfile(const Profile& profile) {
    // 1. Load (or create) the array
    json data = loadData();
    // 2. Append or update
    data.push_back(profile);
    // 3. Save atomically
    fs::create_directories(dataFile.parent_path());
    fs::path tmp = dataFile;
    tmp += ".tmp";
    {
        std::ofstream out(tmp);
        out << data.dump(4) << '\n';
    }
    fs::rename(tmp, dataFile);
}

std::string getTimeStamp(){
    namespace ch = std::chrono;
    auto now = ch::system_clock::now();
    auto tp = ch::floor<ch::seconds>(now);
    return std::format("{:%Y-%m-%d_%H-%M-%S}Z", tp);
}

std::string genName(const std::string& name_in) {
    std::string name = name_in;
    if (name_in.empty()) {
        name = getTimeStamp();
    }
    return name;
}

void genProfile(const std::string& name_in) {
    std::string name = genName(name_in);
    std::vector<Profile> profiles = loadAllProfiles();
    // Check if the profile already exists
    for (const auto& profile : profiles) {
        if (profile.name == name) {
            std::cerr << "Profile with name '" << name << "' already exists." << '\n';
            return;
        }
    }
    // Check if the directory already exists
    fs::path profilePath = dataPath / name;
    if (fs::exists(profilePath)) {
        std::cerr << "Profile directory '" << profilePath.string() << "' already exists." << '\n';
        return;
    }

    fs::create_directories(profilePath);

    // Copy mods to the profile directory
    for (const auto& entry : fs::directory_iterator(modPath)) {
        if (entry.is_regular_file()) {
            fs::copy(entry.path(), profilePath / entry.path().filename());
        }
    }
    // Add the new profile to the data
    _saveProfile(Profile(name));
}

void genProfile() {
    const std::string name = getTimeStamp();
    genProfile(name);
}

void listProfiles() {
    std::vector<Profile> profiles = loadAllProfiles();
    std::cout << "Available profiles:" << '\n';
    for (const auto& profile : profiles) {
        std::cout << "•" << profile.name << '\n';
    }
}

void listMods() {
    std::cout << "Currently loaded mods:" << '\n';
    for (const auto& entry : fs::directory_iterator(modPath)) {
        if (entry.is_regular_file()) {
            std::cout << entry.path().filename() << '\n';
        }
    }
}

void _clearMods() {
    std::cout << "Clearing mods:" << '\n';
    for (const auto& entry : fs::directory_iterator(modPath)) {
        if (entry.is_regular_file()) {
            std::cout << entry.path().filename() << '\n';
            fs::remove(entry.path());
        }
    }
}

void swap(const std::string& profile_name, std::string swapped_name = {}) {
    if (swapped_name.empty())
        swapped_name = profile_name + "_swapped_" + getTimeStamp();

    if (fs::exists(modPath) && !fs::is_empty(modPath)) {
        genProfile(swapped_name);   // or std::move(swapped_name)
        _clearMods();
    }

    fs::path profile_path = dataPath / profile_name;
    if (!fs::exists(profile_path)) {
        throw std::invalid_argument("Could not find profile directory " + profile_path.string());
    }

    for (const auto& entry : fs::directory_iterator(profile_path)) {
        if (entry.is_regular_file()) {
            fs::copy(entry.path(),
                     modPath / entry.path().filename(),
                     fs::copy_options::overwrite_existing); // optional
        }
    }
}

int main() {
    std::unordered_map<std::string, Command> cmds;

    cmds.emplace("list", Command{
            "list",
            "List available profiles.",
            [](const std::vector<std::string>&){
                listProfiles();
            }
    });

    cmds.emplace("mods", Command{
            "mods",
            "List current mods.",
            [](const std::vector<std::string>&){
                listMods();
            }
    });

    cmds.emplace("restore", Command{
            "restore",
            "Restore data.",
            [](const std::vector<std::string>&){
                restoreData();
            }
    });

    cmds.emplace("gen", Command{
            "gen",
            "Generates a profile with the given profile name.",
            [](const std::vector<std::string>& args){
                genProfile(args[1]);
            }
    });

    cmds.emplace("swap", Command{
            "swap",
            "Swaps current mods with given profile name.",
            [](const std::vector<std::string>& args){
                swap(args[1], args[2]);
            }
    });

    cmds.emplace("help", Command{
            "help",
            "List available commands.",
            [&](const std::vector<std::string>&){
                for (auto& [_, cmd] : cmds) cmd.print_desc();
            }
    });

    std::string line;
    while (true) {
        restoreData();
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        auto args = split_args(line);
        if (args.empty()) continue;

        auto it = cmds.find(args[0]);
        if (it == cmds.end()) {
            std::cerr << "Unknown command. Try 'help'." << '\n';
            continue;
        }
        it->second.run(args);
    }
}