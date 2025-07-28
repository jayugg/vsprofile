//
// Created by Jacopo Uggeri on 27/07/2025.
//
#pragma once
#include "include/json.hpp"
#include "TimeUtils.hpp"
#include "Config.hpp"
#include "Command.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <format>
using json = nlohmann::json;

namespace vsprofile {

    class Core{
        Config config;

    public:
        Core(const Config& config);
        void ListAllProfiles() const;
        std::vector<std::string> LoadAllProfiles() const;
        void SaveProfile(const std::string& name_in = "") const;
        void ListMods() const;
        void SwapFolders(const std::string& profile_name, std::string swapped_name = {}) const;
        void PrintInfo() const;
        static std::unordered_map<std::string, Command> GenCommands(const Core& core);
        static std::string GenNonEmptyName(const std::string& name_in);

    private:
        void clearMods() const;
    };

}