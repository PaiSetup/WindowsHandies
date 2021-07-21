#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace std {
namespace fs = std::filesystem;
}

std::vector<std::fs::path> getSubdirectories(const std::vector<std::fs::path> &directories);
