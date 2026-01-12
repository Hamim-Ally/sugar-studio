#pragma once

#include "config.h"
#include <string>
#include <vector>
#include <filesystem>

namespace sugar {

/**
 * Represents a SugarBuilder C++ project.
 * Manages project configuration, source files, and build artifacts.
 */
class Project {
public:
    Project(const Config& config, const std::filesystem::path& root_dir = ".");

    /**
     * Collect all C++ source files from configured source paths.
     * @return List of paths to C++ source files (.cpp, .cc, .cxx, .c, .h, .hpp).
     */
    std::vector<std::filesystem::path> getSourceFiles() const;

    /**
     * Get the build directory path.
     */
    std::filesystem::path getBuildDirectory() const;

    /**
     * Get the output directory path.
     */
    std::filesystem::path getOutputDirectory() const;

    /**
     * Get the target executable/library filename based on project type and platform.
     */
    std::string getTargetFilename() const;

    const Config& config;

private:
    std::filesystem::path root_dir_;
};

} // namespace sugar