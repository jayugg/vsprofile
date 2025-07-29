//
// Created by Jacopo Uggeri on 28/07/2025.
//
#include "Core.hpp"

#include "../Utils/ConsoleUtils.hpp"
#include "../Utils/TextUtils.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Utils/TimeUtils.hpp"

namespace utl = vsprofile::utils;
namespace fs = std::filesystem;

namespace vsprofile {

    Core::Core(Config config) : config_(std::move(config)) {}

    std::string Core::GenNonEmptyName(const std::string_view nameIn) const{
        if (nameIn.empty()) {
            return std::format("stash_{}_{}", utils::GetTimeStamp(), config_.activeProfile);
        }
        return std::string{nameIn};
    }

    void Core::SaveProfile(const std::string& nameIn) {
        std::string name {GenNonEmptyName(nameIn)};
        fs::create_directories(config_.profilesPath);
        // Check if the profile directory exists
        const fs::path profilePath {config_.profilesPath / name};
        if (fs::exists(profilePath)) {
            utl::PrintErr(std::format("Profile directory '{}' already exists.\n", profilePath.string()));
            utl::PrintErr(std::format("Use 'update {}' to update profile.\n", name));
            return;
        }
        // Create profile directory
        fs::create_directories(profilePath);
        // Copy mods to the profile directory
        utl::CopyContents(config_.modsPath, profilePath);
        // Activate the profile
        config_.activeProfile = std::string(name);
        utl::PrintLog(std::format("Saved profile {}\n", name));
    }

    void Core::UpdateProfile(const std::string& name) const {
        // Check that the profile exists
        fs::path profilePath {config_.profilesPath / name};
        if (!fs::exists(profilePath)) {
            utl::PrintErr(std::format("Profile directory '{}' does not exist.\n", profilePath.string()));
            utl::PrintErr( std::format("Use 'save {}' to save a new profile with this name.\n", name));
            return;
        }
        // Clear profile path
        utl::ClearDirectoryContents(profilePath);
        // Copy mods to the profile directory
        utl::CopyContents(config_.modsPath, profilePath);
        utl::PrintLog(std::format("Updated profile {}\n", name));
    }

    void Core::SetActive(const std::string& profileName) {
        config_.activeProfile = profileName;
        if (profileName.empty()) {
            utl::PrintLog("No active profile :£\n");
        } else {
            utl::PrintLog(std::format("Profile '{}' activated :3\n", profileName));
        }
        config_.Save(constants::kConfigPath);
    }

    void Core::ActivateProfile(const std::string& profileName, const std::string& stashNameIn) {
        if (profileName == config_.activeProfile) {
            utl::PrintErr(std::format("Profile '{}' is already active!\n", profileName));
            return;
        }
        std::string stashName {GenNonEmptyName(stashNameIn)};
        // Ensure profile exists
        const fs::path profilePath {config_.profilesPath / profileName};
        if (!fs::exists(profilePath)) {
            utl::PrintErr(std::format("Profile directory '{}' does not exist.\n", profilePath.string()));
            utl::PrintErr(std::format("Use 'save {}' to save a new profile with this name.\n", profileName));
            return;
        }
        const fs::path stashPath {config_.profilesPath / stashName};
        if (fs::exists(stashPath)) {
            utl::PrintErr(std::format("Stash directory '{}' already exists, retry with a different name.\n", stashName));
            return;
        }
        utl::PrintLog(std::format("Activating profile '{}'\n", profileName));
        fs::create_directories(stashPath); // Now both directories exist
        // Copy contents of Mods into stashName
        utl::PrintLog(std::format("Stashing current mods to profile '{}'\n", stashName));
        utl::CopyContents(config_.modsPath, stashPath);
        // Clear contents of Mods
        utl::ClearDirectoryContents(config_.modsPath);
        // Copy contents of profileName into Mods
        utl::CopyContents(profilePath, config_.modsPath);
        SetActive(profileName);
    }

    void Core::PrintInfo() const {
        std::cout << utl::Bold(std::format("[{} v{}] >.<\n", constants::kAppName, constants::kAppVersion));
        const std::string shown = config_.activeProfile.empty() ? utl::Italics("none") : config_.activeProfile;
        std::cout << std::format("Last active profile: '{}'\n", shown);
        std::cout << std::format("Profiles path: '{}'\n", utl::Italics(config_.profilesPath.string()));
        std::cout << std::format("Mods path: '{}'\n",      utl::Italics(config_.modsPath.string()));
    }

    void Core::PrintExtraInfo() const {
        std::cout << std::format("VintagestoryData folder path: '{}'\n", utl::Italics(config_.vintagestoryDataPath.string()));
        std::cout << std::format("Vintage Story executable path: '{}'\n", utl::Italics(config_.vintagestoryExePath.string()));
        std::cout << std::format("Config path: '{}'\n", utl::Italics(constants::kConfigPath.string()));
    }

    void Core::ClearAllProfiles() {
        std::string line;
        if (utl::RequestConfirmation("This will clear all profile folders, do you wish to continue? y/n\n")) {
            return;
        }
        utl::PrintLog(utl::Bold("Clearing All Profiles...\n"));
        utl::ClearDirectoryContents(config_.profilesPath, true);
        utl::PrintLog(utl::Bold("Cleared All Profiles! :3\n"));
        SetActive("");
    }

    void Core::BuildCommands() {
        cmds_.clear();

        cmds_.emplace("profiles", Command{
                "profiles", "List available profiles.",
                [this](const std::vector<std::string>&){
                    utl::PrintLog(utl::Bold("[Available profiles]\n"));
                    fs::create_directories(config_.profilesPath);
                    utl::ListDirectoryContents(config_.profilesPath);
                }
        });

        cmds_.emplace("profile", Command{
        "profile", "List mods in profile.",
        [this](const std::vector<std::string>& args){
            if (args.size() < 2) { utl::PrintErr("usage: profile <name>\n"); return; }
            utl::PrintLog(utl::Bold(std::format("[Mods in '{}']\n", args[1])));
            utl::ListDirectoryContents(config_.profilesPath / args[1]);
        }
});

        cmds_.emplace("mods", Command{
                "mods", "List current mods.",
                [this](const std::vector<std::string>&){
                    utl::PrintLog(utl::Bold("[Installed mods]\n"));
                    utl::ListDirectoryContents(config_.modsPath);
                }
        });

        cmds_.emplace("save", Command{
                "save", "Save a profile from the current mods folder.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { utl::PrintErr("usage: save <name>\n"); return; }
                    this->SaveProfile(args[1]);
                }
        });

        cmds_.emplace("update", Command{
                "update", "Update a profile from the current mods folder.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { utl::PrintErr("usage: update <name>\n"); return; }
                    this->UpdateProfile(args[1]);
                }
        });

        cmds_.emplace("activate", Command{
                "activate", "Move mods contained in the given profile name in the Mods folder. Stashes current mod list.",
                [this](const std::vector<std::string>& args){
                    if (args.size() < 2) { utl::PrintErr("usage: activate <profile>\n"); return; }
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
                    for (auto &cmd: this->cmds_ | std::views::values) cmd.PrintDescription();
                }
        });
    }

    bool Core::Dispatch(const std::string& cmd, const std::vector<std::string>& args) {
        if (cmds_.empty()) {
            utl::PrintErr("No commands loaded. Did you forget to call BuildCommands?.\n");
        }
        auto it = cmds_.find(cmd);
        if (it == cmds_.end()) {
            utl::PrintErr("Unknown command. Try 'help'.\n");
            return false;
        }
        it->second.run(args);
        return true;
    }

}