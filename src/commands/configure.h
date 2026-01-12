#pragma once

#include "base.h"

namespace sugar {

/**
 * Configure command validates the sugar.toml configuration.
 * Loads and validates project configuration without building.
 */
class ConfigureCommand : public Command {
public:
    ConfigureCommand();
    ~ConfigureCommand() override = default;

    int execute(const std::string& config_path = "") override;
    std::string getHelp() const override;
};

} // namespace sugar