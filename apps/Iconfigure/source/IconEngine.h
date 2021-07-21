#pragma once

#include "source/Utility/Filesystem.h"

#include <vector>

class IconEngine {
public:
    std::vector<std::fs::path> getSubdirectories(const std::vector<std::fs::path> &directories);
    std::fs::path findIconFile(const std::fs::path &directory, bool recursiveSearch, const std::fs::path &iconLibraryPath);

    bool hasIcon(const std::fs::path &directory);
    void setIcon(const std::fs::path &directory, const std::fs::path &iconFile, int iconIndex);

private:
    std::fs::path findIconFileInIconLibrary(const std::fs::path &directory, const std::fs::path &iconLibrary);
    std::fs::path findIconFileHardcoded(const std::fs::path &directory);
    std::fs::path findIconFileRecursively(const std::fs::path &rootDirectory, const std::fs::path &directory);
};
