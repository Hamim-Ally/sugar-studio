#pragma once

#include <vector>
#include <filesystem>
#include <string>
#include <memory>

namespace sugar {

/**
 * Abstract toolchain for C++ compilation and linking.
 * Defines interface for compiling sources and linking object files.
 */
class Toolchain {
public:
    explicit Toolchain(const std::string& name) : name(name) {}
    virtual ~Toolchain() = default;

    /**
     * Compile a source file to an object file.
     */
    virtual bool compileObject(
        const std::filesystem::path& source_file,
        const std::filesystem::path& output_file,
        const std::vector<std::filesystem::path>& include_dirs = {},
        const std::vector<std::string>& flags = {}
    ) = 0;

    /**
     * Link object files into an executable.
     */
    virtual bool linkExecutable(
        const std::vector<std::filesystem::path>& object_files,
        const std::filesystem::path& output_file,
        const std::vector<std::filesystem::path>& lib_dirs = {},
        const std::vector<std::string>& libraries = {},
        const std::vector<std::string>& flags = {}
    ) = 0;

    /**
     * Link object files into a static library.
     */
    virtual bool linkStaticLibrary(
        const std::vector<std::filesystem::path>& object_files,
        const std::filesystem::path& output_file,
        const std::vector<std::string>& flags = {}
    ) = 0;

    /**
     * Link object files into a shared library.
     */
    virtual bool linkSharedLibrary(
        const std::vector<std::filesystem::path>& object_files,
        const std::filesystem::path& output_file,
        const std::vector<std::filesystem::path>& lib_dirs = {},
        const std::vector<std::string>& libraries = {},
        const std::vector<std::string>& flags = {}
    ) = 0;

    /**
     * Get file extension for object files.
     */
    virtual std::string getObjectExtension() const = 0;

    /**
     * Factory method to create appropriate toolchain.
     */
    static std::unique_ptr<Toolchain> create(const std::string& toolchain_name);

protected:
    std::string name;
};

} // namespace sugar