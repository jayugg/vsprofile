//
// Created by Jacopo Uggeri on 28/07/2025.
//
#include "FileUtils.hpp"

namespace vsprofile::utils {

    bool vExistsCheck(const fs::path &path) {
        if (exists(path)) { return true; }
        PrintErr(std::format("Aborted: Path {} does not exist!\n", path.string()));
        return false;
    }

    bool vDirectoryCheck(const fs::path &path) {
        if (is_directory(path)) { return true; }
        PrintErr(std::format("Aborted: Path {} is not a directory!\n", path.string()));
        return false;
    }

    bool vExistsDirectoryCheck(const fs::path &path) {
        return (vExistsCheck(path) && vDirectoryCheck(path));
    }

    void CopyContents(const fs::path& fromPath, const fs::path& toPath) {
        if (!vExistsDirectoryCheck(fromPath) || !vExistsDirectoryCheck(toPath)) return;

        std::error_code ec;
        fs::create_directories(toPath, ec);
        if (ec) {
            PrintErr(std::format("Failed to create '{}': {}\n", toPath.string(), ec.message()));
            return;
        }

        for (const fs::directory_entry& e : fs::directory_iterator(fromPath)) {
            if (!e.is_regular_file(ec)) continue;
            const fs::path dst = toPath / e.path().filename();
            fs::copy_file(e.path(), dst, fs::copy_options::overwrite_existing, ec);
            if (ec) {
                PrintErr(std::format("Copy failed: '{}' -> '{}': {}\n",e.path().string(), dst.string(), ec.message()));
                ec.clear();
            }
        }
    }

    void ListDirectoryContents(const fs::path& path) {
        if (!vExistsDirectoryCheck(path)) { return; } // Ensure directory exists
        for (const auto& entry : fs::directory_iterator(path)) {
            PrintLog(std::format("– {}\n",entry.path().filename().string()));
        }
    }

    void ClearDirectoryContents(const fs::path& path, bool recursive) {
        if (!vExistsDirectoryCheck(path)) { return; } // Ensure directory exists
        for (const auto& entry : fs::directory_iterator(path)) {
            if (recursive || entry.is_regular_file()) {
                PrintLog(std::format("Removing \"{}\"\n", entry.path().filename().string()));
                fs::remove_all(entry.path());
            }
        }
    }

    void SwapDirectoryContents(const fs::path& path1, const fs::path& path2) {
        if (!vExistsDirectoryCheck(path1) && !vExistsDirectoryCheck(path2)) { return; }
    }

    std::vector<std::string> GetContentsList(const fs::path& path) {
        std::vector<std::string> allFiles;
        if (!vExistsDirectoryCheck(path)) { return allFiles; } // Ensure directory exists
        for (const auto& entry : fs::directory_iterator(path)) {
            allFiles.push_back(entry.path().filename().string());
        }
        return allFiles;
    }

}