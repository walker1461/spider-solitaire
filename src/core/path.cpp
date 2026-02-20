#include "path.h"
#include <iostream>

#if defined(__APPLE__)
    #include <mach-o/dyld.h>
#elif defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
#endif

namespace path {

    std::filesystem::path executablePath() {
#if defined(__APPLE__)
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);

        std::string buffer(size, '\0');
        _NSGetExecutablePath(buffer.data(), &size);

        return std::filesystem::canonical(buffer);

#elif defined(_WIN32)
        char buffer[MAX_PATH];
        DWORD const length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        return std::filesystem::path(std::string(buffer, length));

#elif defined(__linux__)
        return std::filesystem::canonical("/proc/self/exe");

#else
        static_assert(false, "unsupported platform");
#endif
    }

    // std::filesystem::path assetPath(const std::filesystem::path& relativePath) {
    //     const auto exe = executablePath();
    //     const auto basePath = exe.parent_path().parent_path();
    //     const auto full = basePath / "assets" / relativePath;
    //
    //     std::cout << "Trying asset path: " << full << std::endl;
    //
    //     return full;
    // }
    std::filesystem::path assetPath(const std::filesystem::path& relativePath) {
        return std::filesystem::current_path() / "assets" / relativePath;
    }
}