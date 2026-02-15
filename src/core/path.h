#pragma once
#include <filesystem>

namespace path {
    std::filesystem::path executablePath();
    std::filesystem::path assetPath(const std::filesystem::path& relativePath);
}
