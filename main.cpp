#include "Command.hpp"
#include "include/json.hpp"
#include "Config.hpp"
#include "Core.hpp"
#include <filesystem>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace vsprofile;

int main() {
    const fs::path configPath = Config::GetConfigPath("vsprofile");
    const Config config = Config::Load(configPath);
    config.Save(configPath);
    Core core = Core(config);
    std::unordered_map<std::string, Command> cmds = Core::GenCommands(core);

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        auto args = Command::SplitArgs(line);
        if (args.empty()) continue;

        auto it = cmds.find(args[0]);
        if (it == cmds.end()) {
            std::cerr << "Unknown command. Try 'help'." << '\n';
            continue;
        }
        it->second.run(args);
    }
}