#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace std {
namespace fs = std::filesystem;
}

std::fs::path findIconFile(const std::fs::path &directory, bool recursiveSearch, const std::fs::path &iconLibraryPath);
