#pragma once

#include <functional>
#include <string>

enum class IconFileCriterionResult {
    Accept,
    Deny,
    DontKnow,
};
using IconFileCriterion = std::function<IconFileCriterionResult(const std::wstring &, const std::wstring &)>;

extern const IconFileCriterion iconFileFinders[];
extern const unsigned int iconFileFindersCount;
