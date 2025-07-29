#pragma once
#include "../Include/json.hpp"
#include "../Utils/TimeUtils.hpp"
#include "../Utils/AppConstants.hpp"
#include "../Utils/TextUtils.hpp"
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using json = nlohmann::json;

namespace vsprofile {
    struct Config {
        std::filesystem::path vintagestoryDataPath {constants::kVintageStoryDataPath};
        std::filesystem::path profilesPath {constants::kAppDir / "Profiles"};
        std::filesystem::path modsPath {constants::kVintageStoryDataPath / "Mods"};
        std::filesystem::path vintagestoryExePath;
        std::string activeProfile;

        Config() = default;

        void Save(const std::filesystem::path& configPath) const;
        static Config Load(const std::filesystem::path& configPath);

    private:
        static void HandleCorruptConfig(const std::filesystem::path& configPath);
    };

    void to_json(json &j, const Config &p);
    void from_json(const json &j, Config &p);
}
