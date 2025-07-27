#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include "include/json.hpp"

using json = nlohmann::json;
using sys_seconds = std::chrono::sys_time<std::chrono::seconds>;

struct Profile {
    std::string name;
    sys_seconds date_created{};
    sys_seconds date_modified{};

    Profile() = default;
    Profile(std::string n);

    void touch();
};

void to_json(json& j, const Profile& p);
void from_json(const json& j, Profile& p);
