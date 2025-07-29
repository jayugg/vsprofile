//
// Created by Jacopo Uggeri on 28/07/2025.
//
#include "Core.hpp"

namespace vsprofile {

    Core::Core(struct Config config) : config_(std::move(config)) {}

    std::string Core::GenNonEmptyName(std::string_view nameIn) {
        if (nameIn.empty()) {
            return std::format("stash-{}", utils::GetTimeStamp());
        }
        return std::string{nameIn};
    }


    void Core::SaveProfile(const std::string& nameIn) {
        std::string name {GenNonEmptyName(nameIn)};
        std::filesystem::create_directories(config_.profilesPath);
        // Check if the profile directory exists
        std::filesystem::path profilePath {config_.profilesPath / name};
        if (std::filesystem::exists(profilePath)) {
            utils::PrintErr(std::format("Profile directory '{}' already exists.\n", profilePath.string()));
            utils::PrintErr(std::format("Use 'update {}' to update profile.\n", name));
            return;
        }
        // Create profile directory
        std::filesystem::create_directories(profilePath);
        // Copy mods to the profile directory
        utils::CopyContents(config_.modsPath, profilePath);
        // Activate the profile
        config_.activeProfile = std::string(name);
        utils::PrintLog(std::format("Saved profile {}\n", name));
    }

    void Core::UpdateProfile(const std::string& name) const {
        // Check that the profile exists
        std::filesystem::path profilePath {config_.profilesPath / name};
        if (!std::filesystem::exists(profilePath)) {
            utils::PrintErr(std::format("Profile directory '{}' does not exist.\n", profilePath.string()));
            utils::PrintErr( std::format("Use 'save {}' to save a new profile with this name.\n", name));
            return;
        }
        // Clear profile path
        utils::ClearDirectoryContents(profilePath);
        // Copy mods to the profile directory
        utils::CopyContents(config_.modsPath, profilePath);
        utils::PrintLog(std::format("Updated profile {}\n", name));
    }

    void Core::SetActive(const std::string& profileName) {
        config_.activeProfile = profileName;
        if (profileName.empty()) {
            utils::PrintLog("No active profile :£\n");
        } else {
            utils::PrintLog(std::format("Profile '{}' activated :3\n", profileName));
        }
        config_.Save(constants::kConfigPath);
    }

    void Core::ActivateProfile(const std::string& profileName, const std::string& stashNameIn) {
        if (profileName == config_.activeProfile) {
            utils::PrintErr(std::format("Profile '{}' is already active!\n", profileName));
            return;
        }
        std::string stashName {GenNonEmptyName(stashNameIn)};
        // Ensure profile exists
        std::filesystem::path profilePath {config_.profilesPath / profileName};
        if (!std::filesystem::exists(profilePath)) {
            utils::PrintErr(std::format("Profile directory '{}' does not exist.\n", profilePath.string()));
            utils::PrintErr(std::format("Use 'save {}' to save a new profile with this name.\n", profileName));
            return;
        }
        std::filesystem::path stashPath {config_.profilesPath / stashName};
        if (std::filesystem::exists(stashPath)) {
            utils::PrintErr(std::format("Stash directory '{}' already exists, retry with a different name.\n", stashName));
            return;
        }
        utils::PrintLog(std::format("Activating profile '{}'\n", profileName));
        std::filesystem::create_directories(stashPath); // Now both directories exist
        // Copy contents of Mods into stashName
        utils::PrintLog(std::format("Stashing current mods to profile '{}'\n", stashName));
        utils::CopyContents(config_.modsPath, stashPath);
        // Clear contents of Mods
        utils::ClearDirectoryContents(config_.modsPath);
        // Copy contents of profileName into Mods
        utils::CopyContents(profilePath, config_.modsPath);
        SetActive(profileName);
    }

    void Core::PrintInfo() const {
        std::cout << utils::Bold(std::format("[{} v{}] >.<\n", constants::kAppName, constants::kAppVersion));
        const std::string shown = config_.activeProfile.empty() ? utils::Italics("none") : config_.activeProfile;
        std::cout << std::format("Last active profile: '{}'\n", shown);
        std::cout << std::format("Profiles path: '{}'\n", utils::Italics(config_.profilesPath.string()));
        std::cout << std::format("Mods path: '{}'\n",      utils::Italics(config_.modsPath.string()));
    }

    void Core::PrintExtraInfo() const {
        std::cout << std::format("VintagestoryData folder path: '{}'\n", utils::Italics(config_.vintagestoryDataPath.string()));
        std::cout << std::format("Vintage Story executable path: '{}'\n", utils::Italics(config_.vintagestoryExePath.string()));
        std::cout << std::format("Config path: '{}'\n", utils::Italics(constants::kConfigPath.string()));
    }

    void Core::ClearAllProfiles() {
        std::string line;
        while (true) {
            utils::PrintWarn("This will clear all profile folders, do you wish to continue? y/n\n");
            std::cout << utils::Blink(">> ");
            if (!std::getline(std::cin, line)) break;
            if (line[0] != 'y') {
                utils::PrintErr("Process aborted.\n");
                return;
            } else {
                break;
            }
        }
        utils::PrintLog(utils::Bold("Clearing All Profiles...\n"));
        utils::ClearDirectoryContents(config_.profilesPath, true);
        utils::PrintLog(utils::Bold("Cleared All Profiles! :3\n"));
        SetActive("");
    }

    void Core::BuildCommands() {
        cmds_.clear();

        cmds_.emplace("profiles", Command{
                "profiles", "List available profiles.",
                [this](const std::vector<std::string>&){
                    utils::PrintLog(utils::Bold("[Available profiles]\n"));
                    utils::ListDirectoryContents(config_.profilesPath);
                }
        });

        cmds_.emplace("mods", Command{
                "mods", "List current mods.",
                [this](const std::vector<std::string>&){
                    utils::PrintLog(utils::Bold("[Installed mods]\n"));
                    utils::ListDirectoryContents(config_.modsPath);
                }
        });

        cmds_.emplace("save", Command{
                "save", "Save a profile from the current mods folder.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { utils::PrintErr("usage: save <name>\n"); return; }
                    this->SaveProfile(args[1]);
                }
        });

        cmds_.emplace("update", Command{
                "update", "Update a profile from the current mods folder.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { utils::PrintErr("usage: update <name>\n"); return; }
                    this->UpdateProfile(args[1]);
                }
        });

        cmds_.emplace("activate", Command{
                "activate", "Move mods contained in the given profile name in the Mods folder. Stashes current mod list.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { utils::PrintErr("usage: activate <profile>\n"); return; }
                    this->ActivateProfile(args[1], args[2]);
                }
        });

        cmds_.emplace("info", Command{
                "info", "Show extra information on the current configuration.",
                [this](const std::vector<std::string>&){ this->PrintExtraInfo(); }
        });

        cmds_.emplace("clearall", Command{
                "clearall", "Clear all existing profiles.",
                [this](const std::vector<std::string>&){
                    this->ClearAllProfiles();
                }
        });

        cmds_.emplace("help", Command{
                "help", "List available commands.",
                [this](const std::vector<std::string>&){
                    for (auto& [_, cmd] : this->cmds_) cmd.PrintDescription();
                }
        });
    }

    bool Core::Dispatch(const std::string& cmd, const std::vector<std::string>& args) {
        if (cmds_.empty()) {
            utils::PrintErr("No commands loaded. Did you forget to call BuildCommands?.\n");
        }
        auto it = cmds_.find(cmd);
        if (it == cmds_.end()) {
            utils::PrintErr("Unknown command. Try 'help'.\n");
            return false;
        }
        it->second.run(args);
        return true;
    }

}