#include "project.h"
#include <set>

namespace sugar {

Project::Project(const Config& config, const std::filesystem::path& root_dir)
    : config(config), root_dir_(root_dir) {
}

std::vector<std::filesystem::path> Project::getSourceFiles() const {
    std::vector<std::filesystem::path> source_files;
    std::set<std::string> source_extensions = {".cpp", ".cc", ".cxx", ".c", ".h", ".hpp"};

    for (const auto& src_path : config.source_paths) {
        std::filesystem::path src_dir = root_dir_ / src_path;
        if (!std::filesystem::exists(src_dir)) {
            continue;
        }

        // Search only in the specified directory (not subdirectories)
        for (const auto& ext : source_extensions) {
            for (const auto& entry : std::filesystem::directory_iterator(src_dir)) {
                if (entry.is_regular_file() && entry.path().extension() == ext) {
                    source_files.push_back(entry.path());
                }
            }
        }
    }

    return source_files;
}

std::filesystem::path Project::getBuildDirectory() const {
    return root_dir_ / config.build_path;
}

std::filesystem::path Project::getOutputDirectory() const {
    return root_dir_ / config.output_path;
}

std::string Project::getTargetFilename() const {
    std::string target_name = config.project_name;

    if (config.platform == "Windows") {
        if (config.project_type == "exe") {
            return target_name + ".exe";
        } else if (config.project_type == "static") {
            return target_name + ".lib";
        } else if (config.project_type == "shared") {
            return target_name + ".dll";
        }
    } else if (config.platform == "Linux") {
        if (config.project_type == "exe") {
            return target_name;
        } else if (config.project_type == "static") {
            return "lib" + target_name + ".a";
        } else if (config.project_type == "shared") {
            return "lib" + target_name + ".so";
        }
    } else if (config.platform == "macOS") {
        if (config.project_type == "exe") {
            return target_name;
        } else if (config.project_type == "static") {
            return "lib" + target_name + ".a";
        } else if (config.project_type == "shared") {
            return "lib" + target_name + ".dylib";
        }
    }

    return target_name;
}

} // namespace sugar