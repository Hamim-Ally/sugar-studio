#include "msvc.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <cstdio>
#else
#include <cstdio>
#endif

namespace sugar {

MSVCToolchain::MSVCToolchain() : Toolchain("MSVC") {
    setupMSVCEnvironment();
    cl_exe = findClExe();
    link_exe = findLinkExe();
    lib_exe = findLibExe();
}

void MSVCToolchain::setupMSVCEnvironment() {
    // Find VsDevCmd.bat
    std::vector<std::filesystem::path> vsdevcmd_paths = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/Tools/VsDevCmd.bat",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/Common7/Tools/VsDevCmd.bat",
    };

    std::filesystem::path vsdevcmd;
    for (const auto& path : vsdevcmd_paths) {
        if (std::filesystem::exists(path)) {
            vsdevcmd = path;
            break;
        }
    }

    if (vsdevcmd.empty()) {
        std::cerr << "[MSVC] Warning: VsDevCmd.bat not found. Some environment variables may not be set." << std::endl;
        return;
    }

#ifdef _WIN32
    // Set up environment by calling VsDevCmd.bat
    std::string cmd = "call \"" + vsdevcmd.string() + "\" -arch=x64 && set";
    
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            std::string line(buffer);
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                // Remove newline
                if (!value.empty() && value.back() == '\n') {
                    value.pop_back();
                }
                _putenv((key + "=" + value).c_str());
            }
        }
        _pclose(pipe);
    }
#endif
}

std::string MSVCToolchain::findClExe() {
#ifdef _WIN32
    // Search Visual Studio installations - prefer newer versions
    std::vector<std::filesystem::path> vs_paths = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Tools/MSVC",
    };

    for (const auto& vs_path : vs_paths) {
        if (std::filesystem::exists(vs_path)) {
            // Find latest version
            std::vector<std::filesystem::path> versions;
            for (const auto& entry : std::filesystem::directory_iterator(vs_path)) {
                if (entry.is_directory()) {
                    versions.push_back(entry.path());
                }
            }
            std::sort(versions.begin(), versions.end(), std::greater<>());
            
            for (const auto& version_dir : versions) {
                std::filesystem::path cl_path = version_dir / "bin" / "Hostx64" / "x64" / "cl.exe";
                if (std::filesystem::exists(cl_path)) {
                    return cl_path.string();
                }
                // Try x86 host as fallback
                cl_path = version_dir / "bin" / "Hostx86" / "x86" / "cl.exe";
                if (std::filesystem::exists(cl_path)) {
                    return cl_path.string();
                }
            }
        }
    }
    
    // If not found in standard locations, try PATH
    return "cl.exe";
#else
    return "cl.exe";
#endif
}

std::string MSVCToolchain::findLinkExe() {
#ifdef _WIN32
    // Search Visual Studio installations - same locations as cl.exe
    std::vector<std::filesystem::path> vs_paths = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Tools/MSVC",
    };

    for (const auto& vs_path : vs_paths) {
        if (std::filesystem::exists(vs_path)) {
            std::vector<std::filesystem::path> versions;
            for (const auto& entry : std::filesystem::directory_iterator(vs_path)) {
                if (entry.is_directory()) {
                    versions.push_back(entry.path());
                }
            }
            std::sort(versions.begin(), versions.end(), std::greater<>());
            
            for (const auto& version_dir : versions) {
                std::filesystem::path link_path = version_dir / "bin" / "Hostx64" / "x64" / "link.exe";
                if (std::filesystem::exists(link_path)) {
                    return link_path.string();
                }
            }
        }
    }
    
    return "link.exe";
#else
    return "link.exe";
#endif
}

std::string MSVCToolchain::findLibExe() {
#ifdef _WIN32
    // Search Visual Studio installations - same locations as cl.exe
    std::vector<std::filesystem::path> vs_paths = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Tools/MSVC",
    };

    for (const auto& vs_path : vs_paths) {
        if (std::filesystem::exists(vs_path)) {
            std::vector<std::filesystem::path> versions;
            for (const auto& entry : std::filesystem::directory_iterator(vs_path)) {
                if (entry.is_directory()) {
                    versions.push_back(entry.path());
                }
            }
            std::sort(versions.begin(), versions.end(), std::greater<>());
            
            for (const auto& version_dir : versions) {
                std::filesystem::path lib_path = version_dir / "bin" / "Hostx64" / "x64" / "lib.exe";
                if (std::filesystem::exists(lib_path)) {
                    return lib_path.string();
                }
            }
        }
    }
    
    return "lib.exe";
#else
    return "lib.exe";
#endif
}

std::vector<std::filesystem::path> MSVCToolchain::findIncludeDirs() {
    std::vector<std::filesystem::path> includes;
    // Implementation similar to Python version
    return includes;
}

std::vector<std::filesystem::path> MSVCToolchain::findMSVCLibDirs() {
    std::vector<std::filesystem::path> lib_dirs;
#ifdef _WIN32
    std::vector<std::filesystem::path> vs_paths = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC",
    };

    for (const auto& vs_path : vs_paths) {
        if (std::filesystem::exists(vs_path)) {
            for (const auto& version_dir : std::filesystem::directory_iterator(vs_path)) {
                std::filesystem::path lib_path = version_dir.path() / "lib" / "x64";
                if (std::filesystem::exists(lib_path)) {
                    lib_dirs.push_back(lib_path);
                    break;
                }
            }
        }
    }
#endif
    return lib_dirs;
}

std::vector<std::filesystem::path> MSVCToolchain::findSDKLibDirs() {
    std::vector<std::filesystem::path> lib_dirs;
#ifdef _WIN32
    std::vector<std::filesystem::path> sdk_paths = {
        "C:/Program Files (x86)/Windows Kits/10/Lib",
        "C:/Program Files/Windows Kits/10/Lib",
    };

    for (const auto& sdk_path : sdk_paths) {
        if (std::filesystem::exists(sdk_path)) {
            for (const auto& version_dir : std::filesystem::directory_iterator(sdk_path)) {
                std::filesystem::path ucrt_lib = version_dir.path() / "ucrt" / "x64";
                std::filesystem::path um_lib = version_dir.path() / "um" / "x64";
                if (std::filesystem::exists(ucrt_lib)) {
                    lib_dirs.push_back(ucrt_lib);
                }
                if (std::filesystem::exists(um_lib)) {
                    lib_dirs.push_back(um_lib);
                }
                break;
            }
        }
    }
#endif
    return lib_dirs;
}

bool MSVCToolchain::compileObject(
    const std::filesystem::path& source_file,
    const std::filesystem::path& output_file,
    const std::vector<std::filesystem::path>& include_dirs,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    // Normalize paths to Windows format and wrap with cmd.exe /c for proper handling
    std::filesystem::path cl_path_obj(cl_exe);
    std::string cl_path = cl_path_obj.make_preferred().string();
    std::filesystem::path src_path_obj(source_file);
    std::string src_path = src_path_obj.make_preferred().string();
    std::filesystem::path out_path_obj(output_file);
    std::string out_path = out_path_obj.make_preferred().string();
    
    cmd << "cmd.exe /c \"";
    if (cl_path.find(' ') != std::string::npos) {
        cmd << "\"" << cl_path << "\"";
    } else {
        cmd << cl_path;
    }
    cmd << " /c /Fo\\\"" << out_path << "\\\" "
        << "/std:c++17 /EHsc /D_CRT_SECURE_NO_WARNINGS "
        << "\\\"" << src_path << "\\\"";

    for (const auto& inc_dir : include_dirs) {
        std::filesystem::path inc_path_obj(inc_dir);
        std::string inc_path = inc_path_obj.make_preferred().string();
        cmd << " /I\\\"" << inc_path << "\\\"";
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }
    cmd << "\"";

    std::cout << "[MSVC] Compiling " << source_file.filename().string() 
              << " -> " << output_file.filename().string() << std::endl;

#ifdef _WIN32
    int result = system(cmd.str().c_str());
    return result == 0;
#else
    return false;
#endif
}

bool MSVCToolchain::linkExecutable(
    const std::vector<std::filesystem::path>& object_files,
    const std::filesystem::path& output_file,
    const std::vector<std::filesystem::path>& lib_dirs,
    const std::vector<std::string>& libraries,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    // Normalize paths to Windows format and wrap with cmd.exe /c
    std::filesystem::path link_path_obj(link_exe);
    std::string link_path = link_path_obj.make_preferred().string();
    std::filesystem::path out_path_obj(output_file);
    std::string out_path = out_path_obj.make_preferred().string();
    
    cmd << "cmd.exe /c \"";
    if (link_path.find(' ') != std::string::npos) {
        cmd << "\"" << link_path << "\"";
    } else {
        cmd << link_path;
    }
    cmd << " /OUT:\\\"" << out_path << "\\\"";

    for (const auto& obj : object_files) {
        std::filesystem::path obj_path_obj(obj);
        std::string obj_path = obj_path_obj.make_preferred().string();
        cmd << " \\\"" << obj_path << "\\\"";
    }

    auto msvc_libs = findMSVCLibDirs();
    auto sdk_libs = findSDKLibDirs();

    for (const auto& lib_dir : msvc_libs) {
        std::filesystem::path lib_path_obj(lib_dir);
        std::string lib_path = lib_path_obj.make_preferred().string();
        cmd << " /LIBPATH:\\\"" << lib_path << "\\\"";
    }
    for (const auto& lib_dir : sdk_libs) {
        std::filesystem::path lib_path_obj(lib_dir);
        std::string lib_path = lib_path_obj.make_preferred().string();
        cmd << " /LIBPATH:\\\"" << lib_path << "\\\"";
    }
    for (const auto& lib_dir : lib_dirs) {
        std::filesystem::path lib_path_obj(lib_dir);
        std::string lib_path = lib_path_obj.make_preferred().string();
        cmd << " /LIBPATH:\\\"" << lib_path << "\\\"";
    }

    cmd << " kernel32.lib user32.lib msvcrt.lib libcmt.lib libcpmt.lib uuid.lib ws2_32.lib";

    for (const auto& lib : libraries) {
        cmd << " " << lib << ".lib";
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }
    cmd << "\"";

    std::cout << "[MSVC] Linking executable: " << output_file.filename().string() << std::endl;

#ifdef _WIN32
    int result = system(cmd.str().c_str());
    return result == 0;
#else
    return false;
#endif
}

bool MSVCToolchain::linkStaticLibrary(
    const std::vector<std::filesystem::path>& object_files,
    const std::filesystem::path& output_file,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    // Normalize paths to Windows format and wrap with cmd.exe /c
    std::filesystem::path lib_path_obj(lib_exe);
    std::string lib_path = lib_path_obj.make_preferred().string();
    std::filesystem::path out_path_obj(output_file);
    std::string out_path = out_path_obj.make_preferred().string();
    
    cmd << "cmd.exe /c \"";
    if (lib_path.find(' ') != std::string::npos) {
        cmd << "\"" << lib_path << "\"";
    } else {
        cmd << lib_path;
    }
    cmd << " /OUT:\\\"" << out_path << "\\\"";

    for (const auto& obj : object_files) {
        std::filesystem::path obj_path_obj(obj);
        std::string obj_path = obj_path_obj.make_preferred().string();
        cmd << " \\\"" << obj_path << "\\\"";
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }
    cmd << "\"";

    std::cout << "[MSVC] Creating static library: " << output_file.filename().string() << std::endl;

#ifdef _WIN32
    int result = system(cmd.str().c_str());
    return result == 0;
#else
    return false;
#endif
}

bool MSVCToolchain::linkSharedLibrary(
    const std::vector<std::filesystem::path>& object_files,
    const std::filesystem::path& output_file,
    const std::vector<std::filesystem::path>& lib_dirs,
    const std::vector<std::string>& libraries,
    const std::vector<std::string>& flags
) {
    std::stringstream cmd;
    // Normalize paths to Windows format and wrap with cmd.exe /c
    std::filesystem::path link_path_obj(link_exe);
    std::string link_path = link_path_obj.make_preferred().string();
    std::filesystem::path out_path_obj(output_file);
    std::string out_path = out_path_obj.make_preferred().string();
    
    cmd << "cmd.exe /c \"";
    if (link_path.find(' ') != std::string::npos) {
        cmd << "\"" << link_path << "\"";
    } else {
        cmd << link_path;
    }
    cmd << " /DLL /OUT:\\\"" << out_path << "\\\"";

    for (const auto& obj : object_files) {
        std::filesystem::path obj_path_obj(obj);
        std::string obj_path = obj_path_obj.make_preferred().string();
        cmd << " \\\"" << obj_path << "\\\"";
    }

    for (const auto& lib_dir : lib_dirs) {
        std::filesystem::path lib_path_obj(lib_dir);
        std::string lib_path = lib_path_obj.make_preferred().string();
        cmd << " /LIBPATH:\\\"" << lib_path << "\\\"";
    }

    for (const auto& lib : libraries) {
        cmd << " " << lib << ".lib";
    }

    for (const auto& flag : flags) {
        cmd << " " << flag;
    }
    cmd << "\"";

    std::cout << "[MSVC] Linking shared library: " << output_file.filename().string() << std::endl;

#ifdef _WIN32
    int result = system(cmd.str().c_str());
    return result == 0;
#else
    return false;
#endif
}

} // namespace sugar