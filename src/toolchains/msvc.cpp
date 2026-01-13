#include "msvc.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <cstdio>
#endif

namespace sugar {

static std::string quote(const std::string& s) {
    return (s.find(' ') != std::string::npos) ? ("\"" + s + "\"") : s;
}

MSVCToolchain::MSVCToolchain() : Toolchain("MSVC") {
    setupMSVCEnvironment();
    cl_exe = findClExe();
    link_exe = findLinkExe();
    lib_exe = findLibExe();
}

void MSVCToolchain::setupMSVCEnvironment() {
#ifdef _WIN32
    std::vector<std::filesystem::path> vsdevcmd_paths = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/Tools/VsDevCmd.bat",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/Common7/Tools/VsDevCmd.bat",
    };

    std::filesystem::path vsdevcmd;
    for (const auto& p : vsdevcmd_paths) {
        if (std::filesystem::exists(p)) {
            vsdevcmd = p;
            break;
        }
    }

    if (vsdevcmd.empty()) {
        std::cerr << "[MSVC] Warning: VsDevCmd.bat not found.\n";
        return;
    }

    std::string cmd = "call \"" + vsdevcmd.string() + "\" -arch=x64 && set";
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return;

    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            if (!val.empty() && val.back() == '\n') val.pop_back();
            _putenv((key + "=" + val).c_str());
        }
    }
    _pclose(pipe);
#endif
}

std::string MSVCToolchain::findClExe() {
#ifdef _WIN32
    std::vector<std::filesystem::path> roots = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC"
    };

    for (auto& root : roots) {
        if (!std::filesystem::exists(root)) continue;
        std::vector<std::filesystem::path> versions;
        for (auto& d : std::filesystem::directory_iterator(root))
            if (d.is_directory()) versions.push_back(d.path());
        std::sort(versions.begin(), versions.end(), std::greater<>());
        for (auto& v : versions) {
            auto p = v / "bin" / "Hostx64" / "x64" / "cl.exe";
            if (std::filesystem::exists(p)) return p.string();
        }
    }
#endif
    return "cl.exe";
}

std::string MSVCToolchain::findLinkExe() {
#ifdef _WIN32
    std::vector<std::filesystem::path> roots = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC"
    };

    for (auto& root : roots) {
        if (!std::filesystem::exists(root)) continue;
        for (auto& v : std::filesystem::directory_iterator(root)) {
            auto p = v.path() / "bin" / "Hostx64" / "x64" / "link.exe";
            if (std::filesystem::exists(p)) return p.string();
        }
    }
#endif
    return "link.exe";
}

std::string MSVCToolchain::findLibExe() {
#ifdef _WIN32
    std::vector<std::filesystem::path> roots = {
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC",
        "C:/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC"
    };

    for (auto& root : roots) {
        if (!std::filesystem::exists(root)) continue;
        for (auto& v : std::filesystem::directory_iterator(root)) {
            auto p = v.path() / "bin" / "Hostx64" / "x64" / "lib.exe";
            if (std::filesystem::exists(p)) return p.string();
        }
    }
#endif
    return "lib.exe";
}

bool MSVCToolchain::compileObject(const std::filesystem::path& src,
                                  const std::filesystem::path& out,
                                  const std::vector<std::filesystem::path>& incs,
                                  const std::vector<std::string>& flags) {
    std::stringstream cmd;
    cmd << quote(cl_exe) << " /c /Fo" << quote(out.string())
        << " /std:c++17 /EHsc " << quote(src.string());

    for (auto& i : incs)
        cmd << " /I" << quote(i.string());
    for (auto& f : flags)
        cmd << " " << f;

    std::cout << "[MSVC] " << cmd.str() << "\n";
    return system(cmd.str().c_str()) == 0;
}

bool MSVCToolchain::linkExecutable(const std::vector<std::filesystem::path>& objs,
                                   const std::filesystem::path& out,
                                   const std::vector<std::filesystem::path>& libs,
                                   const std::vector<std::string>& names,
                                   const std::vector<std::string>& flags) {
    std::stringstream cmd;
    cmd << quote(link_exe) << " /OUT:" << quote(out.string());

    for (auto& o : objs)
        cmd << " " << quote(o.string());
    for (auto& l : libs)
        cmd << " /LIBPATH:" << quote(l.string());
    for (auto& n : names)
        cmd << " " << n << ".lib";
    for (auto& f : flags)
        cmd << " " << f;

    std::cout << "[MSVC] " << cmd.str() << "\n";
    return system(cmd.str().c_str()) == 0;
}

bool MSVCToolchain::linkStaticLibrary(const std::vector<std::filesystem::path>& objs,
                                      const std::filesystem::path& out,
                                      const std::vector<std::string>& flags) {
    std::stringstream cmd;
    cmd << quote(lib_exe) << " /OUT:" << quote(out.string());

    for (auto& o : objs)
        cmd << " " << quote(o.string());
    for (auto& f : flags)
        cmd << " " << f;

    std::cout << "[MSVC] " << cmd.str() << "\n";
    return system(cmd.str().c_str()) == 0;
}

bool MSVCToolchain::linkSharedLibrary(const std::vector<std::filesystem::path>& objs,
                                      const std::filesystem::path& out,
                                      const std::vector<std::filesystem::path>& libs,
                                      const std::vector<std::string>& names,
                                      const std::vector<std::string>& flags) {
    std::stringstream cmd;
    cmd << quote(link_exe) << " /DLL /OUT:" << quote(out.string());

    for (auto& o : objs)
        cmd << " " << quote(o.string());
    for (auto& l : libs)
        cmd << " /LIBPATH:" << quote(l.string());
    for (auto& n : names)
        cmd << " " << n << ".lib";
    for (auto& f : flags)
        cmd << " " << f;

    std::cout << "[MSVC] " << cmd.str() << "\n";
    return system(cmd.str().c_str()) == 0;
}

} // namespace sugar
