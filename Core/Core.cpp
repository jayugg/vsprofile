//
// Created by Jacopo Uggeri on 28/07/2025.
//
#include "Core.hpp"

namespace vsprofile {

    Core::Core(struct Config  config) : Config(std::move(config)) {}

    void Core::ListAllProfiles() const {
        std::filesystem::create_directories(Config.profilesPath); // Ensure profiles path exists
        for (const auto& entry : std::filesystem::directory_iterator(Config.profilesPath)) {
            std::cout << std::format("•{}",entry.path().filename().string()) << '\n';
        }
    }

    std::vector<std::string> Core::LoadAllProfiles() const {
        std::vector<std::string> allProfiles;
        for (const auto& entry : std::filesystem::directory_iterator(Config.profilesPath)) {
            allProfiles.push_back(entry.path().filename().string());
        }
        return allProfiles;
    }

    std::string Core::GenNonEmptyName(const std::string& name_in) {
        std::string name = name_in;
        if (name_in.empty()) {
            name = "snapshot-" + utils::GetTimeStamp();
        }
        return name;
    }

    void Core::SaveProfile(const std::string& name_in) const {
        std::string name = GenNonEmptyName(name_in);
        std::filesystem::create_directories(Config.profilesPath);
        std::vector<std::string> profiles = LoadAllProfiles();
        // Check if the profile already exists
        for (const auto& profile : profiles) {
            if (profile == name) {
                std::cerr << "Profile with name '" << name << "' already exists." << '\n';
                return;
            }
        }
        // Check if the directory already exists
        std::filesystem::path profilePath = Config.profilesPath / name;
        if (std::filesystem::exists(profilePath)) {
            std::cerr << "Profile directory '" << profilePath.string() << "' already exists." << '\n';
            return;
        }

        std::filesystem::create_directories(profilePath);

        // Copy mods to the profile directory
        for (const auto& entry : std::filesystem::directory_iterator(Config.modsPath)) {
            if (entry.is_regular_file()) {
                std::filesystem::copy(entry.path(), profilePath / entry.path().filename());
            }
        }
    }

    void Core::ListMods() const {
        std::cout << "Currently loaded mods:" << '\n';
        for (const auto& entry : std::filesystem::directory_iterator(Config.modsPath)) {
            if (entry.is_regular_file()) {
                std::cout << entry.path().filename() << '\n';
            }
        }
    }

    void Core::clearMods() const {
        std::cout << "Clearing mods:" << '\n';
        for (const auto& entry : std::filesystem::directory_iterator(Config.modsPath)) {
            if (entry.is_regular_file()) {
                std::cout << entry.path().filename() << '\n';
                std::filesystem::remove(entry.path());
            }
        }
    }

    void Core::swapFolders(const std::string& profileName, std::string swappedName) const {
        if (swappedName.empty())
            swappedName = profileName + "_swapped_" + utils::GetTimeStamp();

        if (std::filesystem::exists(Config.modsPath) && !std::filesystem::is_empty(Config.modsPath)) {
            SaveProfile(swappedName);
            clearMods();
        }

        std::filesystem::path profilePath = Config.profilesPath / profileName;
        if (!std::filesystem::exists(profilePath)) {
            throw std::invalid_argument("Could not find profile directory " + profilePath.string());
        }

        for (const auto& entry : std::filesystem::directory_iterator(profilePath)) {
            if (entry.is_regular_file()) {
                std::filesystem::copy(entry.path(), Config.modsPath / entry.path().filename());
            }
        }
    }

    void Core::SetActive(const std::string& profileName) {
        Config.activeProfile = profileName;
    }

    void Core::SwapProfiles(const std::string& profileName, const std::string& swappedName) {
        swapFolders(profileName, swappedName);
        SetActive(swappedName);
    }

    void Core::PrintInfo() const {
        std::cout << std::format("{} v{}", constants::kAppName, constants::kAppVersion) << '\n';
        std::cout << "Last active profile: " << (Config.activeProfile.empty() ? std::format("\033[3m{}\033[3m", "none") : Config.activeProfile) << '\n';\
        std::cout << "Profiles path: " << Config.profilesPath << '\n';\
        std::cout << "Mods path: " << Config.modsPath << '\n';\
    }

    void Core::PrintExtraInfo() const {
        std::cout << "VintagestoryData folder path: " << Config.vintagestoryDataPath << '\n';
        std::cout << "Vintage Story executable path: " << Config.vintagestoryExePath << '\n';\
        std::cout << "Config path: " << constants::kConfigPath << '\n';
    }

    void Core::BuildCommands() {
        cmds_.clear();

        cmds_.emplace("list", Command{
                "list", "List available profiles.",
                [this](const std::vector<std::string>&){ this->ListAllProfiles(); }
        });

        cmds_.emplace("mods", Command{
                "mods", "List current mods.",
                [this](const std::vector<std::string>&){ this->ListMods(); }
        });

        cmds_.emplace("save", Command{
                "save", "Save a profile from the current mods folder.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { std::cerr << "usage: save <name>\n"; return; }
                    this->SaveProfile(args[1]);
                }
        });

        cmds_.emplace("swap", Command{
                "swap", "Swap current mods with the given profile name.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { std::cerr << "usage: swap <profile> <swapped>\n"; return; }
                    this->SwapProfiles(args[1], args[2]);
                }
        });

        cmds_.emplace("info", Command{
                "info", "Show current configuration.",
                [this](const std::vector<std::string>&){ this->PrintExtraInfo(); }
        });

        cmds_.emplace("help", Command{
                "help", "List available commands.",
                [this](const std::vector<std::string>&){
                    for (auto& [_, cmd] : this->cmds_) cmd.PrintDescription();
                }
        });

        cmds_.emplace("vsexepath", Command{
                "vsexepath", "Set Vintage Story executable path",
                [this](const std::vector<std::string>& args){ this->Config.vintagestoryExePath = args[1]; }
        });

        cmds_.emplace("vsdatapath", Command{
                "vsdatapath", "Set Vintage Story data path",
                [this](const std::vector<std::string>& args){ this->Config.vintagestoryDataPath = args[1]; }
        });
    }

    bool Core::Dispatch(const std::string& cmd, const std::vector<std::string>& args) {
        auto it = cmds_.find(cmd);
        if (it == cmds_.end()) { std::cerr << "Unknown command. Try 'help'.\n"; return false; }
        it->second.run(args);
        Config.Save(constants::kConfigPath);
        return true;
    }

}