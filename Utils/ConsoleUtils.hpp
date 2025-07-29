//
// Created by Jacopo Uggeri on 29/07/2025.
//
#pragma once
#include "TextUtils.hpp"
#include <string_view>

namespace vsprofile::utils {

    inline bool RequestConfirmation(const std::string_view prompt) {
        std::string line;
        while (true) {
            PrintWarn(prompt);
            std::cout << Blink(">> ");
            if (!std::getline(std::cin, line)) break;
            if (line[0] != 'y') {
                PrintErr("Process aborted.\n");
                return false;
            }
            break;
        }
        return true;
    }

}