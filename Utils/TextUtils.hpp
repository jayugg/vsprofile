//
// Created by Jacopo Uggeri on 29/07/2025.
//
#pragma once
#include <string>
#include <format>
#include <iostream>

namespace vsprofile::utils {

    inline std::string Gray(std::string_view s) {
        return std::format("\x1b[37m{}\x1b[0m", s);
    }

    inline std::string Red(std::string_view s) {
        return std::format("\x1b[31m{}\x1b[0m", s);
    }

    inline std::string Italics(std::string_view s) {
        return std::format("\x1b[3m{}\x1b[0m", s);
    }

    inline std::string Bold(std::string_view s) {
        return std::format("\x1b[1m{}\x1b[0m", s);
    }

    inline std::string Blink(std::string_view s) {
        return std::format("\x1b[5m{}\x1b[0m", s);
    }

    inline std::string Yellow(std::string_view s) {
        return std::format("\x1b[93m{}\x1b[0m", s);
    }

    inline std::string BoldItalics(std::string_view s) {
        return Bold(Italics(s));
    }

    inline std::string Framed(std::string_view s) {
        return std::format("\x1b[51m{}\x1b[0m", s);
    }

    inline void PrintErr(std::string_view s) {
        std::cout << Red(s);
    }

    inline void PrintLog(std::string_view s) {
        std::cout << Gray(s);
    }

    inline void PrintWarn(std::string_view s) {
        std::cout << Yellow(Bold(s));
    }
}