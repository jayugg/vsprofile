//
// Created by Jacopo Uggeri on 28/07/2025.
//
#pragma once
#include <string>
#include <filesystem>
#include <cstdlib>
#define VSPROFILE_VERSION "0.1.0"

namespace vsprofile::constants {

    namespace fs = std::filesystem;

    inline const std::string_view kAppVersion = VSPROFILE_VERSION;
    inline constexpr std::string_view kAppName = "vsprofile";

    // Base directory for per-user app data/config (OS-dependent)
    inline const fs::path kAppDataDir = [] {
        const char* home = std::getenv("HOME");

#if defined(_WIN32)
        if (const char* appdata = std::getenv("APPDATA")) {
        return fs::path(appdata);                         // e.g. C:\Users\Me\AppData\Roaming
        }
        if (home) {
            return fs::path(home) / "AppData" / "Roaming";
        }
#elif defined(__APPLE__)
        if (home) {
            return fs::path(home) / ".config"; // Use this because Vintage Story stores it here on Mac for v > 1.21.0
        }
#else // Linux / Unix
        if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg && *xdg) {
        return fs::path(xdg);                             // e.g. /home/me/.config
        }
        if (home) {
            return fs::path(home) / ".config";               // XDG default
        }
#endif
        return fs::path{"."};                                 // fallback
    }();

// App-specific directory and files
    inline const fs::path kAppDir        = kAppDataDir / kAppName;
    inline const fs::path kConfigPath    = kAppDir / "Config.json";
    inline const fs::path kVintageStoryDataPath = kAppDataDir / "VintagestoryData";

}