#pragma once

#include <string>

namespace sugar {

/**
 * Abstract base class for SugarBuilder commands.
 * Defines the interface for all CLI commands.
 */
class Command {
public:
    explicit Command(const std::string& name) : name(name) {}
    virtual ~Command() = default;

    /**
     * Execute the command.
     * @param config_path Optional path to sugar.toml configuration file.
     * @return Exit code (0 for success, non-zero for failure).
     */
    virtual int execute(const std::string& config_path = "") = 0;

    /**
     * Get help text for this command.
     */
    virtual std::string getHelp() const {
        return "Command: " + name;
    }

protected:
    std::string name;
};

} // namespace sugar