#pragma once

#include "base.h"

namespace sugar {

/**
 * Build command compiles and links a C++ project.
 * Compiles source files to object files and links them into final target.
 */
class BuildCommand : public Command {
public:
    BuildCommand();
    ~BuildCommand() override = default;

    int execute(const std::string& config_path = "") override;
    std::string getHelp() const override;
};

} // namespace sugar