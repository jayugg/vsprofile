//
// Created by Jacopo Uggeri on 28/07/2025.
//
#include "Core.hpp"

namespace vsprofile {

    Core::Core(const Config& config) : config(config) {}

    void Core::ListAllProfiles() const {
        for (const auto& entry : std::filesystem::directory_iterator(config.profilesPath)) {
            std::cout << std::format("•{}",entry.path().filename().string()) << '\n';
        }
    }

    std::vector<std::string> Core::LoadAllProfiles() const {
        std::vector<std::string> allProfiles;
        for (const auto& entry : std::filesystem::directory_iterator(config.profilesPath)) {
            allProfiles.push_back(entry.path().filename().string());
        }
        return allProfiles;
    }

    std::string Core::GenNonEmptyName(const std::string& name_in) {
        std::string name = name_in;
        if (name_in.empty()) {
            name = "snapshot-" + GetTimeStamp();
        }
        return name;
    }

    void Core::SaveProfile(const std::string& name_in) const {
        std::string name = GenNonEmptyName(name_in);
        std::vector<std::string> profiles = LoadAllProfiles();
        // Check if the profile already exists
        for (const auto& profile : profiles) {
            if (profile == name) {
                std::cerr << "Profile with name '" << name << "' already exists." << '\n';
                return;
            }
        }
        // Check if the directory already exists
        std::filesystem::path profilePath = config.profilesPath / name;
        if (std::filesystem::exists(profilePath)) {
            std::cerr << "Profile directory '" << profilePath.string() << "' already exists." << '\n';
            return;
        }

        std::filesystem::create_directories(profilePath);

        // Copy mods to the profile directory
        for (const auto& entry : std::filesystem::directory_iterator(config.modsPath)) {
            if (entry.is_regular_file()) {
                std::filesystem::copy(entry.path(), profilePath / entry.path().filename());
            }
        }
    }

    void Core::ListMods() const {
        std::cout << "Currently loaded mods:" << '\n';
        for (const auto& entry : std::filesystem::directory_iterator(config.modsPath)) {
            if (entry.is_regular_file()) {
                std::cout << entry.path().filename() << '\n';
            }
        }
    }

    void Core::clearMods() const {
        std::cout << "Clearing mods:" << '\n';
        for (const auto& entry : std::filesystem::directory_iterator(config.modsPath)) {
            if (entry.is_regular_file()) {
                std::cout << entry.path().filename() << '\n';
                std::filesystem::remove(entry.path());
            }
        }
    }

    void Core::SwapFolders(const std::string& profile_name, std::string swapped_name) const {
        if (swapped_name.empty())
            swapped_name = profile_name + "_swapped_" + GetTimeStamp();

        if (std::filesystem::exists(config.modsPath) && !std::filesystem::is_empty(config.modsPath)) {
            SaveProfile(swapped_name);   // or std::move(swapped_name)
            clearMods();
        }

        std::filesystem::path profilePath = config.profilesPath / profile_name;
        if (!std::filesystem::exists(profilePath)) {
            throw std::invalid_argument("Could not find profile directory " + profilePath.string());
        }

        for (const auto& entry : std::filesystem::directory_iterator(profilePath)) {
            if (entry.is_regular_file()) {
                std::filesystem::copy(entry.path(), config.modsPath / entry.path().filename());
            }
        }
    }

    void Core::PrintInfo() const {
        std::cout << "Vintagestory data path: " << config.vintagestoryDataPath << '\n';
        std::cout << "Vintagestory exe path: " << config.vintagestoryExePath << '\n';\
        std::cout << "Profiles path: " << config.profilesPath << '\n';\
        std::cout << "Mods path: " << config.modsPath << '\n';\
        std::cout << "Config path: " << Config::GetConfigPath("vsprofile") << "\n";
        std::cout << "Active profile: " << config.activeProfile << '\n';\
    }

    std::unordered_map<std::string, Command> Core::GenCommands(const Core& core) {
        std::unordered_map<std::string, Command> cmds;
        cmds.emplace("list", Command{
                "list",
                "List available profiles.",
                [&](const std::vector<std::string>&){
                    core.ListAllProfiles();
                }
        });

        cmds.emplace("mods", Command{
                "mods",
                "List current mods.",
                [&](const std::vector<std::string>&){
                    core.ListMods();
                }
        });

        cmds.emplace("save", Command{
                "save",
                "Saves a profile based on the current mods folder with the given profile name.",
                [&](const std::vector<std::string>& args){
                    core.SaveProfile(args[1]);
                }
        });

        cmds.emplace("swap", Command{
                "swap",
                "Swaps current mods with given profile name.",
                [&](const std::vector<std::string>& args){
                    core.SwapFolders(args[1], args[2]);
                }
        });

        cmds.emplace("info", Command{
                "info",
                "Provides info on the current configuration.",
                [&](const std::vector<std::string>&){
                    core.PrintInfo();
                }
        });

        cmds.emplace("help", Command{
                "help",
                "List available commands.",
                [&](const std::vector<std::string>&){
                    for (auto& [_, cmd] : cmds) cmd.PrintDescription();
                }
        });
        return cmds;
    }

}