#pragma once

#include <string>
#include <vector>
#include <memory>

namespace sugar {

/**
 * Configuration container for a SugarBuilder project.
 * Loads and validates sugar.toml configuration file.
 */
class Config {
public:
    std::string project_name;
    std::string project_type;  // exe, static, shared
    std::string compiler;      // MSVC, GCC, Clang
    std::string platform;      // Windows, Linux, macOS
    std::vector<std::string> source_paths;
    std::string build_path;
    std::string output_path;
    std::vector<std::string> include_paths;
    std::vector<std::string> link_dependencies;

    /**
     * Load configuration from sugar.toml file.
     * @param config_path Path to sugar.toml file.
     * @return Config object.
     * @throws std::runtime_error if file not found or invalid.
     */
    static Config load(const std::string& config_path);

    /**
     * Validate the configuration.
     * @throws std::runtime_error if configuration is invalid.
     */
    void validate() const;

private:
    static Config fromDict(const std::string& content);
};

} // namespace sugar