//
// Created by Jacopo Uggeri on 28/07/2025.
//
#pragma once
#include <filesystem>
#include <string>
#include <format>

namespace vsprofile::utils {

    namespace fs = std::filesystem;

    // verbose checks
    [[nodiscard]] bool vExistsCheck(const fs::path& path);
    [[nodiscard]] bool vDirectoryCheck(const fs::path& path);
    [[nodiscard]] bool vExistsDirectoryCheck(const fs::path& path);

    void CopyContents(const fs::path& fromPath, const fs::path& toPath);
    void ListDirectoryContents(const fs::path& path); // Lists all contents
    void ClearDirectoryContents(const fs::path& path, bool recursive = false); // Clears files
    void SwapDirectoryContents(const fs::path& path1, const fs::path& path2);
    [[nodiscard]] std::vector<std::string> GetContentsList(const fs::path& path);

}