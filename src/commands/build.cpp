#include "build.h"
#include "../core/config.h"
#include "../core/project.h"
#include "../toolchains/base.h"
#include <iostream>
#include <stdexcept>
#include <set>
#include <filesystem>

namespace sugar {

BuildCommand::BuildCommand() : Command("build") {
}

int BuildCommand::execute(const std::string& config_path) {
    try {
        std::string cfg_path = config_path.empty() ? "sugar.toml" : config_path;
        std::cout << "Building from: " << cfg_path << std::endl;

        Config config = Config::load(cfg_path);
        config.validate();

        Project project(config);

        // Create directories if they don't exist
        std::filesystem::path build_dir = project.getBuildDirectory();
        std::filesystem::path output_dir = project.getOutputDirectory();
        std::filesystem::create_directories(build_dir);
        std::filesystem::create_directories(output_dir);

        std::cout << "Build directory: " << build_dir << std::endl;
        std::cout << "Output directory: " << output_dir << std::endl;

        // Get toolchain
        auto toolchain = Toolchain::create(config.compiler);

        // Get source files
        auto source_files = project.getSourceFiles();
        if (source_files.empty()) {
            std::cerr << "Warning: No source files found!" << std::endl;
            return 1;
        }

        // Filter to only compilable files (exclude .h, .hpp)
        std::set<std::string> compilable_extensions = {".cpp", ".cc", ".cxx", ".c"};
        std::vector<std::filesystem::path> compilable_files;
        for (const auto& file : source_files) {
            if (compilable_extensions.count(file.extension().string())) {
                compilable_files.push_back(file);
            }
        }

        if (compilable_files.empty()) {
            std::cerr << "Warning: No compilable source files found!" << std::endl;
            return 1;
        }

        std::cout << "Found " << source_files.size() << " source files (" 
                  << compilable_files.size() << " compilable)" << std::endl;

        // Compile sources to objects
        std::vector<std::filesystem::path> object_files;
        std::string obj_ext = toolchain->getObjectExtension();

        for (const auto& source_file : compilable_files) {
            std::filesystem::path obj_file = build_dir / (source_file.stem().string() + obj_ext);

            std::cout << "Compiling: " << source_file.filename().string() 
                      << " -> " << obj_file.filename().string() << std::endl;

            // Include directories: source paths + configured include paths
            std::vector<std::filesystem::path> include_dirs;
            for (const auto& src_path : config.source_paths) {
                include_dirs.push_back(std::filesystem::path(src_path));
            }
            for (const auto& inc_path : config.include_paths) {
                include_dirs.push_back(std::filesystem::path(inc_path));
            }

            bool success = toolchain->compileObject(source_file, obj_file, include_dirs);
            if (!success) {
                std::cerr << "Error compiling " << source_file << std::endl;
                return 1;
            }

            object_files.push_back(obj_file);
        }

        // Link objects into target
        std::string target_name = project.getTargetFilename();
        std::filesystem::path target_path = output_dir / target_name;

        std::cout << "\nLinking: " << target_name << std::endl;

        bool success = false;
        if (config.project_type == "exe") {
            success = toolchain->linkExecutable(
                object_files,
                target_path,
                {},
                config.link_dependencies
            );
        } else if (config.project_type == "static") {
            success = toolchain->linkStaticLibrary(object_files, target_path);
        } else if (config.project_type == "shared") {
            success = toolchain->linkSharedLibrary(
                object_files,
                target_path,
                {},
                config.link_dependencies
            );
        } else {
            throw std::runtime_error("Unknown project type: " + config.project_type);
        }

        if (!success) {
            std::cerr << "Error during linking" << std::endl;
            return 1;
        }

        std::cout << "\nBuild successful!" << std::endl;
        std::cout << "Target: " << target_path << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

std::string BuildCommand::getHelp() const {
    return R"(
build - Compile and link the C++ project

Usage: sugar-builder build [--config <path>]

Options:
  --config <path>    Path to sugar.toml (defaults to ./sugar.toml)

Description:
  Builds the C++ project by:
  1. Validating sugar.toml configuration
  2. Creating build and output directories
  3. Compiling all source files to object files
  4. Linking object files into final executable/library

The project type (exe/static/shared) determines linking behavior.
Dependencies are linked as specified in the configuration.
)";
}

} // namespace sugar