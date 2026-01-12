#pragma once

#include "base.h"
#include <string>

namespace sugar {

/**
 * Microsoft Visual C++ toolchain (cl.exe, link.exe, lib.exe).
 */
class MSVCToolchain : public Toolchain {
public:
    MSVCToolchain();
    ~MSVCToolchain() override = default;

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

    std::string getObjectExtension() const override { return ".obj"; }

private:
    static void setupMSVCEnvironment();
    static std::vector<std::filesystem::path> findIncludeDirs();
    static std::string findClExe();
    static std::string findLinkExe();
    static std::string findLibExe();
    static std::vector<std::filesystem::path> findMSVCLibDirs();
    static std::vector<std::filesystem::path> findSDKLibDirs();

    std::string cl_exe;
    std::string link_exe;
    std::string lib_exe;
};

} // namespace sugar