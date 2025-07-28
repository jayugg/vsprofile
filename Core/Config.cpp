//
// Created by Jacopo Uggeri on 27/07/2025.
//
#include "Config.hpp"
namespace fs = std::filesystem;
using json = nlohmann::json;
namespace vsprofile {

    Config Config::Load(const fs::path& configPath) {
        // Ensure directory exists
        fs::create_directories(configPath.parent_path());

        // Default‑construct a Config
        Config cfg;

        // Try to open and parse
        if (std::ifstream in{configPath}; in && in.peek() != EOF) {
            try {
                json j;
                in >> j;
                cfg = j.get<Config>();
            }
            catch (const json::parse_error&) {
                // corrupt JSON - keep defaults
                HandleCorruptConfig(configPath);
            }
            catch (const json::type_error&) {
                // unexpected shape - keep defaults
                HandleCorruptConfig(configPath);
            }
        }

        return cfg;
    }

    void Config::Save(const fs::path& configPath) const {
        fs::create_directories(configPath.parent_path());
        std::ofstream out{configPath};
        json j = *this;
        out << j.dump(4);   // pretty‑print with indent
    }

    // Required by JSON to serialise
    void to_json(json &j, const Config &c) {
        j = json{
                {"vintagestoryDataPath", c.vintagestoryDataPath},
                {"profilesPath",         c.profilesPath},
                {"activeProfile",        c.activeProfile},
                {"vintagestoryExePath",  c.vintagestoryExePath},
        };
    }

    // Required by JSON to deserialise
    void from_json(const json &j, Config &c) {
        c.vintagestoryDataPath = j.at("vintagestoryDataPath").get<fs::path>();
        c.profilesPath = j.at("profilesPath").get<fs::path>();
        c.activeProfile = j.at("activeProfile").get<std::string>();
        c.vintagestoryExePath = j.at("vintagestoryExePath").get<fs::path>();
    }

    void Config::HandleCorruptConfig(const fs::path& configPath) {
        std::string tempName = utils::GetTimeStamp() + configPath.filename().string() + ".bak";
        fs::copy(configPath, configPath.parent_path() / tempName);
        std::cout << std::format("Corrupt Config file detected, backing up to {}", tempName);
    }
}