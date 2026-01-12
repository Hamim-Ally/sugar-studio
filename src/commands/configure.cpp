#include "configure.h"
#include "../core/config.h"
#include "../core/project.h"
#include <iostream>
#include <stdexcept>

namespace sugar {

ConfigureCommand::ConfigureCommand() : Command("configure") {
}

int ConfigureCommand::execute(const std::string& config_path) {
    try {
        std::string cfg_path = config_path.empty() ? "sugar.toml" : config_path;
        std::cout << "Configuring from: " << cfg_path << std::endl;

        Config config = Config::load(cfg_path);
        config.validate();

        Project project(config);

        std::cout << "Configuration validation successful!" << std::endl;
        std::cout << "  Project: " << config.project_name << std::endl;
        std::cout << "  Type: " << config.project_type << std::endl;
        std::cout << "  Compiler: " << config.compiler << std::endl;
        std::cout << "  Platform: " << config.platform << std::endl;
        std::cout << "  Source paths: ";
        for (size_t i = 0; i < config.source_paths.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << config.source_paths[i];
        }
        std::cout << std::endl;
        std::cout << "  Build path: " << config.build_path << std::endl;
        std::cout << "  Output path: " << config.output_path << std::endl;
        if (!config.link_dependencies.empty()) {
            std::cout << "  Dependencies: ";
            for (size_t i = 0; i < config.link_dependencies.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << config.link_dependencies[i];
            }
            std::cout << std::endl;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

std::string ConfigureCommand::getHelp() const {
    return R"(
configure - Validate sugar.toml configuration

Usage: sugar-builder configure [--config <path>]

Options:
  --config <path>    Path to sugar.toml (defaults to ./sugar.toml)

Description:
  Loads and validates the sugar.toml configuration file without building.
  Checks for required fields and validates project settings.
)";
}

} // namespace sugar