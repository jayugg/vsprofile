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

    void Config::Save(const fs::path& path) const {
        fs::create_directories(path.parent_path());
        const fs::path tmp = path.string() + ".tmp";
        // Save to a temporary file
        {
            std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
            if (!out) throw std::runtime_error(std::format("open '{}' failed", tmp.string()));
            out << json(*this).dump(4) << '\n';
            out.flush();
            if (!out) throw std::runtime_error(std::format("write '{}' failed", tmp.string()));
        }
        // replace target (rename, else remove+rename, else copy)
        std::error_code ec;
        fs::rename(tmp, path, ec);
        if (ec) {
            fs::remove(path, ec); ec.clear();
            fs::rename(tmp, path, ec);
            if (ec) {
                fs::copy_file(tmp, path, fs::copy_options::overwrite_existing, ec);
                fs::remove(tmp);
                if (ec) throw std::runtime_error(std::format("Failed replacing '{}': {}", path.string(), ec.message()));
            }
        }
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
        std::string tempName {utils::GetTimeStamp() + configPath.filename().string() + ".bak"};
        fs::copy(configPath, configPath.parent_path() / tempName);
        utils::PrintLog(std::format("Corrupt Config file detected, backing up to {}", tempName));
    }
}