//
// Created by Jacopo Uggeri on 27/07/2025.
//
#pragma once
#include <chrono>
#include <format>

namespace vsprofile::utils {

    inline std::string GetTimeStamp(){
        namespace ch = std::chrono;
        const auto now = ch::system_clock::now();
        auto tp = ch::floor<ch::seconds>(now);
        return std::format("{:%Y-%m-%d_%H-%M-%S}", tp);
    }

}