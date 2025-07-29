//
// Created by Jacopo Uggeri on 27/07/2025.
//
#pragma once
#include "../Utils/TextUtils.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <functional>
#include <sstream>

namespace utl = vsprofile::utils;

namespace vsprofile {

    struct Command {
        std::string name;
        std::string description;
        std::function<void(const std::vector<std::string>&)> run;

        void PrintDescription() const {
            std::cout << utl::Bold(name) << " — " << utl::Italics(description) << '\n';
        }

        static std::vector<std::string> SplitArgs(const std::string& line) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string t;
            while (iss >> std::quoted(t) || (!iss.fail() && iss >> t)) {
                tokens.push_back(t);
            }
            if (tokens.empty()) {               // fallback simple split
                std::istringstream iss2(line);
                while (iss2 >> t) tokens.push_back(t);
            }
            return tokens;
        }
    };

}