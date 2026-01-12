#include <iostream>
#include <string>
#include <vector>
#include "commands/configure.h"
#include "commands/build.h"

void printHelp() {
    std::cout << R"(
SugarBuilder - Manual C++ Build Tool

Usage:
  sugar-builder <command> [options]

Commands:
  configure [--config <path>]    Validate sugar.toml configuration
  build [--config <path>]        Compile and link the C++ project
  help                           Show this help message

Options:
  --config <path>                Path to sugar.toml (defaults to ./sugar.toml)

Examples:
  sugar-builder configure
  sugar-builder build
  sugar-builder build --config custom.toml

For detailed command help:
  sugar-builder configure --help
  sugar-builder build --help
)" << std::endl;
}

int main(int argc, char* argv[]) {
    using namespace sugar;

    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::string command_name = argv[1];
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // Handle help
    if (command_name == "--help" || command_name == "-h" || command_name == "help") {
        printHelp();
        return 0;
    }

    // Parse config path if provided
    std::string config_path;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--config" && i + 1 < args.size()) {
            config_path = args[i + 1];
            break;
        }
    }

    try {
        if (command_name == "configure") {
            ConfigureCommand cmd;
            return cmd.execute(config_path);
        } else if (command_name == "build") {
            BuildCommand cmd;
            return cmd.execute(config_path);
        } else {
            std::cerr << "Error: Unknown command '" << command_name << "'" << std::endl;
            printHelp();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}