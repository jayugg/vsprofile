//
// Created by Jacopo Uggeri on 27/07/2025.
//
#pragma once
#include "../Include/json.hpp"
#include "../Utils/TimeUtils.hpp"
#include "../Utils/Command.hpp"
#include "../Utils/AppConstants.hpp"
#include "Config.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <format>
#include <utility>
using json = nlohmann::json;

namespace vsprofile {

    class Core{

    public:
        explicit Core(struct Config  config);

        void BuildCommands();              // populate cmds_
        [[nodiscard]] const auto& Commands() const { return cmds_; }
        bool Dispatch(const std::string& cmd, const std::vector<std::string>& args);

        void SetActive(const std::string& profileName);
        void SwapProfiles(const std::string& profileName, const std::string& swappedName = {});

        void ListAllProfiles() const;
        void ListMods() const;
        void PrintInfo() const;
        void PrintExtraInfo() const;
        void SaveProfile(const std::string& name_in = "") const;
        [[nodiscard]] std::vector<std::string> LoadAllProfiles() const;

        static std::string GenNonEmptyName(const std::string& name_in);

        struct Config Config;
    private:
        std::unordered_map<std::string, Command> cmds_;

        void swapFolders(const std::string& profileName, std::string swappedName = {}) const;
        void clearMods() const;
    };

}