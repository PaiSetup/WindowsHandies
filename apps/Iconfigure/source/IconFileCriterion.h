#pragma once

#include <functional>
#include "source/Utility/Filesystem.h"
#include <string>

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
