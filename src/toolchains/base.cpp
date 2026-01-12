#include "base.h"
#include "msvc.h"
#include "gcc.h"
#include "clang.h"
#include <stdexcept>

namespace sugar {

std::unique_ptr<Toolchain> Toolchain::create(const std::string& toolchain_name) {
    if (toolchain_name == "MSVC") {
        return std::make_unique<MSVCToolchain>();
    } else if (toolchain_name == "GCC") {
        return std::make_unique<GCCToolchain>();
    } else if (toolchain_name == "Clang") {
        return std::make_unique<ClangToolchain>();
    } else {
        throw std::runtime_error("Unsupported toolchain: " + toolchain_name);
    }
}

} // namespace sugar