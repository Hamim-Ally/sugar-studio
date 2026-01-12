#include "gcc.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

namespace sugar {

GCCToolchain::GCCToolchain() : Toolchain("GCC") {
}

bool GCCToolchain::compileObject(
    const std::filesystem::path& source_file,
    const std::filesystem::path& output_file,
    const std::vector<std::filesystem::path>& include_dirs,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    cmd << "g++ -c -o \"" << output_file.string() << "\" \"" << source_file.string() << "\"";

    for (const auto& inc_dir : include_dirs) {
        cmd << " -I\"" << inc_dir.string() << "\"";
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }

    std::cout << "[GCC] Compiling " << source_file.filename().string() 
              << " -> " << output_file.filename().string() << std::endl;

    int result = system(cmd.str().c_str());
    return result == 0;
}

bool GCCToolchain::linkExecutable(
    const std::vector<std::filesystem::path>& object_files,
    const std::filesystem::path& output_file,
    const std::vector<std::filesystem::path>& lib_dirs,
    const std::vector<std::string>& libraries,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    cmd << "g++ -o \"" << output_file.string() << "\"";

    for (const auto& obj : object_files) {
        cmd << " \"" << obj.string() << "\"";
    }

    for (const auto& lib_dir : lib_dirs) {
        cmd << " -L\"" << lib_dir.string() << "\"";
    }

    for (const auto& lib : libraries) {
        cmd << " -l" << lib;
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }

    std::cout << "[GCC] Linking executable: " << output_file.filename().string() << std::endl;

    int result = system(cmd.str().c_str());
    return result == 0;
}

bool GCCToolchain::linkStaticLibrary(
    const std::vector<std::filesystem::path>& object_files,
    const std::filesystem::path& output_file,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    cmd << "ar rcs \"" << output_file.string() << "\"";

    for (const auto& obj : object_files) {
        cmd << " \"" << obj.string() << "\"";
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }

    std::cout << "[GCC] Creating static library: " << output_file.filename().string() << std::endl;

    int result = system(cmd.str().c_str());
    return result == 0;
}

bool GCCToolchain::linkSharedLibrary(
    const std::vector<std::filesystem::path>& object_files,
    const std::filesystem::path& output_file,
    const std::vector<std::filesystem::path>& lib_dirs,
    const std::vector<std::string>& libraries,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    cmd << "g++ -shared -o \"" << output_file.string() << "\"";

    for (const auto& obj : object_files) {
        cmd << " \"" << obj.string() << "\"";
    }

    for (const auto& lib_dir : lib_dirs) {
        cmd << " -L\"" << lib_dir.string() << "\"";
    }

    for (const auto& lib : libraries) {
        cmd << " -l" << lib;
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }

    std::cout << "[GCC] Linking shared library: " << output_file.filename().string() << std::endl;

    int result = system(cmd.str().c_str());
    return result == 0;
}

} // namespace sugar