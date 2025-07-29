#include "Command.hpp"
#include "../Include/json.hpp"
#include "Config.hpp"
#include "Core.hpp"
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace vsprofile;

int main() {
    const Config config {Config::Load(constants::kConfigPath)};
    config.Save(constants::kConfigPath);
    Core core {Core(config)};
    core.BuildCommands();

    std::string line;
    while (true) {
        core.PrintInfo();
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;

        auto args = Command::SplitArgs(line);
        if (args.empty()) continue;

        // Quit program
        if (args[0] == "quit" || args[0] == "exit" || args[0] == "q") break;

        core.Dispatch(args[0], args);
    }
    std::cout << "Goodbye! :3";
}