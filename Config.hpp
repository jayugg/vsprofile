#pragma once
#include "include/json.hpp"
#include "TimeUtils.hpp"
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using json = nlohmann::json;

namespace vsprofile {
    struct Config {
        std::filesystem::path vintagestoryDataPath;
        std::filesystem::path vintagestoryExePath;
        std::filesystem::path profilesPath = vintagestoryDataPath / "Profiles";
        std::filesystem::path modsPath = vintagestoryDataPath / "Mods";
        std::string activeProfile;

        Config() = default;

        void Save(const std::filesystem::path& configPath) const;
        static Config Load(const std::filesystem::path& configPath);
        static std::filesystem::path GetConfigPath(const std::string& appName);

    private:
        static void HandleCorruptConfig(const std::filesystem::path& configPath);
    };

    void to_json(json &j, const Config &p);
    void from_json(const json &j, Config &p);
}
