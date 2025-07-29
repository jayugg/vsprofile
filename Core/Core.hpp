//
// Created by Jacopo Uggeri on 27/07/2025.
//
#pragma once
#include "../Include/json.hpp"
#include "Command.hpp"
#include "Config.hpp"
#include <string>
#include <vector>
#include <format>

using json = nlohmann::json;

namespace vsprofile {

    class Core{
        std::unordered_map<std::string, Command> cmds_;
        Config config_;

    public:
        explicit Core(Config config);

        void BuildCommands();
        [[nodiscard]] const auto& Commands() const { return cmds_; }
        bool Dispatch(const std::string& cmd, const std::vector<std::string>& args);

        void SetActive(const std::string& profileName);
        void ActivateProfile(const std::string& profileName, const std::string& stashName = "");
        void ClearAllProfiles();

        void PrintInfo() const;
        void PrintExtraInfo() const;
        void SaveProfile(const std::string& nameIn = "");
        void UpdateProfile(const std::string& name) const;

        [[nodiscard]] static std::string GenNonEmptyName(std::string_view nameIn);
    };

}