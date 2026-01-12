#include "config.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <regex>

namespace sugar {

Config Config::load(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        throw std::runtime_error("Configuration file not found: " + config_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    return fromDict(content);
}

Config Config::fromDict(const std::string& content) {
    Config config;

    // Simple TOML parser - extract key-value pairs
    std::regex string_pattern("\"([^\"]+)\"");
    std::regex array_pattern("\\[([^\\]]+)\\]");
    std::regex key_value_pattern("(\\w+)\\s*=\\s*(.+)");

    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        // Remove comments
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        std::smatch match;
        if (std::regex_search(line, match, key_value_pattern)) {
            std::string key = match[1].str();
            std::string value = match[2].str();

            // Trim quotes from value if present
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }

            if (key == "project_name") {
                config.project_name = value;
            } else if (key == "project_type") {
                config.project_type = value;
            } else if (key == "compiler") {
                config.compiler = value;
            } else if (key == "platform") {
                config.platform = value;
            } else if (key == "build_path") {
                config.build_path = value;
            } else if (key == "output_path") {
                config.output_path = value;
            } else if (key == "source_paths") {
                // Parse array: ["src", "src2"]
                std::smatch array_match;
                if (std::regex_search(value, array_match, array_pattern)) {
                    std::string array_content = array_match[1].str();
                    std::istringstream array_stream(array_content);
                    std::string item;
                    while (std::getline(array_stream, item, ',')) {
                        // Trim and remove quotes
                        item.erase(0, item.find_first_not_of(" \t\""));
                        item.erase(item.find_last_not_of(" \t\"") + 1);
                        if (!item.empty()) {
                            config.source_paths.push_back(item);
                        }
                    }
                }
            } else if (key == "include_paths") {
                std::smatch array_match;
                if (std::regex_search(value, array_match, array_pattern)) {
                    std::string array_content = array_match[1].str();
                    std::istringstream array_stream(array_content);
                    std::string item;
                    while (std::getline(array_stream, item, ',')) {
                        item.erase(0, item.find_first_not_of(" \t\""));
                        item.erase(item.find_last_not_of(" \t\"") + 1);
                        if (!item.empty()) {
                            config.include_paths.push_back(item);
                        }
                    }
                }
            } else if (key == "link_dependencies") {
                std::smatch array_match;
                if (std::regex_search(value, array_match, array_pattern)) {
                    std::string array_content = array_match[1].str();
                    std::istringstream array_stream(array_content);
                    std::string item;
                    while (std::getline(array_stream, item, ',')) {
                        item.erase(0, item.find_first_not_of(" \t\""));
                        item.erase(item.find_last_not_of(" \t\"") + 1);
                        if (!item.empty()) {
                            config.link_dependencies.push_back(item);
                        }
                    }
                }
            }
        }
    }

    // Validate required fields
    if (config.project_name.empty()) {
        throw std::runtime_error("Missing required field: project_name");
    }
    if (config.project_type.empty()) {
        throw std::runtime_error("Missing required field: project_type");
    }
    if (config.compiler.empty()) {
        throw std::runtime_error("Missing required field: compiler");
    }
    if (config.platform.empty()) {
        throw std::runtime_error("Missing required field: platform");
    }
    if (config.source_paths.empty()) {
        throw std::runtime_error("Missing required field: source_paths");
    }
    if (config.build_path.empty()) {
        throw std::runtime_error("Missing required field: build_path");
    }
    if (config.output_path.empty()) {
        throw std::runtime_error("Missing required field: output_path");
    }

    // Validate project_type
    if (config.project_type != "exe" && config.project_type != "static" && config.project_type != "shared") {
        throw std::runtime_error("Invalid project_type: " + config.project_type + ". Must be 'exe', 'static', or 'shared'");
    }

    // Validate compiler
    if (config.compiler != "MSVC" && config.compiler != "GCC" && config.compiler != "Clang") {
        throw std::runtime_error("Invalid compiler: " + config.compiler + ". Must be 'MSVC', 'GCC', or 'Clang'");
    }

    // Validate platform
    if (config.platform != "Windows" && config.platform != "Linux" && config.platform != "macOS") {
        throw std::runtime_error("Invalid platform: " + config.platform + ". Must be 'Windows', 'Linux', or 'macOS'");
    }

    return config;
}

void Config::validate() const {
    if (project_name.empty() || project_name.find_first_not_of(" \t") == std::string::npos) {
        throw std::runtime_error("project_name cannot be empty");
    }

    if (source_paths.empty()) {
        throw std::runtime_error("source_paths cannot be empty");
    }

    for (const auto& path : source_paths) {
        if (path.empty() || path.find_first_not_of(" \t") == std::string::npos) {
            throw std::runtime_error("source_paths cannot contain empty strings");
        }
    }
}

} // namespace sugar