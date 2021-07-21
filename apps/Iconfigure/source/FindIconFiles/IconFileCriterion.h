#pragma once

#include <functional>
#include <filesystem>
#include <string>

namespace std {
namespace fs = std::filesystem;
}

enum class IconFileCriterionResult {
    Accept,
    Deny,
    DontKnow,
};

struct IconFileCriterionInput {
    std::fs::path rootDirectoryBaseName;
    std::fs::path fileFullPath;
    std::fs::path fileNameWithoutExtension;
    std::fs::path fileExtension;
};

using IconFileCriterion = std::function<IconFileCriterionResult(const IconFileCriterionInput &input)>;

extern const IconFileCriterion iconFileFinders[];
extern const unsigned int iconFileFindersCount;
