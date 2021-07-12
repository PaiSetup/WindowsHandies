#pragma once

#include <functional>
#include <string>

enum class IconFileCriterionResult {
    Accept,
    Deny,
    DontKnow,
};

struct IconFileCriterionInput {
    std::wstring rootDirectoryBaseName;
    std::wstring fileFullPath;
    std::wstring fileNameWithoutExtension;
    std::wstring fileExtension;
};

using IconFileCriterion = std::function<IconFileCriterionResult(const IconFileCriterionInput &input)>;

extern const IconFileCriterion iconFileFinders[];
extern const unsigned int iconFileFindersCount;
