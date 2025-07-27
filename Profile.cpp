//
// Created by Jacopo Uggeri on 24/07/2025.
//

#include "Profile.hpp"
#include "include/json.hpp"
namespace fs = std::filesystem;
using namespace std::chrono;

using json = nlohmann::json;

static sys_time<seconds> now_seconds() {
    return floor<seconds>(system_clock::now());
}

Profile::Profile(std::string n) :
    name(std::move(n)),
    date_created(now_seconds()),
    date_modified(date_created) { }

void Profile::touch() {
    date_modified = now_seconds();
}

void to_json(json& j, const Profile& p) {
    j = json{
            {"name",  p.name},
            {"date_created", p.date_created.time_since_epoch().count()},
            {"date_modified", p.date_modified.time_since_epoch().count()}
    };
}

void from_json(const json& j, Profile& p) {
    p.name = j.at("name").get<std::string>();
    p.date_created = sys_time<seconds>(seconds(j.at("date_created").get<int64_t>()));
    p.date_modified = sys_time<seconds>(seconds(j.at("date_modified").get<int64_t>()));
}

