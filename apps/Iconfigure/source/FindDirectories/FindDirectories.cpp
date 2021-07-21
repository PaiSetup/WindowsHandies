#include "FindDirectories.h"

std::vector<std::fs::path> getSubdirectories(const std::vector<std::fs::path> &directories) {
    std::vector<std::fs::path> subdirectories{};
    for (const auto &directory : directories) {
        for (std::fs::path subdirectory : std::fs::directory_iterator(directory)) {
            if (std::fs::is_directory(subdirectory)) {
                subdirectories.push_back(subdirectory);
            }
        }
    }
    return subdirectories;
}
