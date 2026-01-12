#pragma once

#include "base.h"

namespace sugar {

/**
 * Clang/LLVM toolchain (clang++, lld, llvm-ar).
 */
class ClangToolchain : public Toolchain {
public:
    ClangToolchain();
    ~ClangToolchain() override = default;

    bool compileObject(
        const std::filesystem::path& source_file,
        const std::filesystem::path& output_file,
        const std::vector<std::filesystem::path>& include_dirs = {},
        const std::vector<std::string>& flags = {}
    ) override;

    bool linkExecutable(
        const std::vector<std::filesystem::path>& object_files,
        const std::filesystem::path& output_file,
        const std::vector<std::filesystem::path>& lib_dirs = {},
        const std::vector<std::string>& libraries = {},
        const std::vector<std::string>& flags = {}
    ) override;

    bool linkStaticLibrary(
        const std::vector<std::filesystem::path>& object_files,
        const std::filesystem::path& output_file,
        const std::vector<std::string>& flags = {}
    ) override;

    bool linkSharedLibrary(
        const std::vector<std::filesystem::path>& object_files,
        const std::filesystem::path& output_file,
        const std::vector<std::filesystem::path>& lib_dirs = {},
        const std::vector<std::string>& libraries = {},
        const std::vector<std::string>& flags = {}
    ) override;

    std::string getObjectExtension() const override { return ".o"; }
};

} // namespace sugar