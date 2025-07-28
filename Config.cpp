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

    // Required by json to serialise
    void to_json(json &j, const Config &c) {
        j = json{
                {"vintagestoryDataPath", c.vintagestoryDataPath},
                {"vintagestoryExePath",  c.vintagestoryExePath},
                {"profilesPath",         c.profilesPath},
                {"activeProfile",        c.activeProfile}
        };
    }

    // Required by json to deserialise
    void from_json(const json &j, Config &c) {
        c.vintagestoryDataPath = j.at("vintagestoryDataPath").get<fs::path>();
        c.vintagestoryExePath = j.at("vintagestoryExePath").get<fs::path>();
        c.profilesPath = j.at("profilesPath").get<fs::path>();
        c.activeProfile = j.at("activeProfile").get<std::string>();
    }

     fs::path Config::GetConfigPath(const std::string &appName) {
        const char *home = std::getenv("HOME");
#ifdef _WIN32
        const char* appdata = std::getenv("APPDATA");
        if (appdata) {
            return fs::path(appdata) / appName / "config.json";
        } else if (home) {
            return fs::path(home) / "AppData" / "Roaming" / appName / "config.json";
        }
#elif __APPLE__
        if (home) {
            return fs::path(home) / "Library" / "Application Support" / appName / "config.json";
        }
#else // Linux or other Unix-like
        const char* xdg = std::getenv("XDG_CONFIG_HOME");
        if (xdg) {
            return fs::path(xdg) / appName / "config.json";
        } else if (home) {
            return fs::path(home) / ".config" / appName / "config.json";
        }
#endif
        // Fallback: current directory
        return "config.json";
    }

    void Config::HandleCorruptConfig(const fs::path& configPath) {
        std::string tempName = GetTimeStamp() + configPath.filename().string() + ".bak";
        fs::copy(configPath, configPath.parent_path() / tempName);
        std::cout << std::format("Corrupt config file detected, backing up to {}", tempName);
    }
}