#include "../Utils/Command.hpp"
#include "../Include/json.hpp"
#include "Config.hpp"
#include "Core.hpp"
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace vsprofile;

int main() {
    const Config config = Config::Load(constants::kConfigPath);
    config.Save(constants::kConfigPath);
    Core core = Core(config);
    core.BuildCommands();
    std::unordered_map<std::string, Command> cmds = core.Commands();

    std::string line;
    while (true) {
        core.PrintInfo();
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        auto args = Command::SplitArgs(line);
        if (args.empty()) continue;

        if (!core.Dispatch(args[0], args)) {
            continue;
        }
    }

}