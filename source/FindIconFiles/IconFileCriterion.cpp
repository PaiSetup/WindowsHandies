#include "IconFileCriterion.h"

#include "source/Utility/StringHelper.h"

#include <unordered_map>

const IconFileCriterion iconFileFinders[] = {
    // Simple compare
    +[](const std::wstring &directoryName, const std::wstring &fileName) {
        if (StringHelper::compareCaseInsensitive(directoryName, fileName)) {
            return IconFileCriterionResult::Accept;
        }
        return IconFileCriterionResult::DontKnow;
    },

    // Strip trash from the end (digits, whitespace, hyphens, dots, etc.) and compare
    +[](const std::wstring &directoryName, const std::wstring &fileName) {
        const auto strippedDirectoryName = StringHelper::stripTrashFromTheEnd(directoryName);
        const auto strippedFileName = StringHelper::stripTrashFromTheEnd(fileName);
        if (StringHelper::compareCaseInsensitive(strippedDirectoryName, strippedFileName)) {
            return IconFileCriterionResult::Accept;
        }
        return IconFileCriterionResult::DontKnow;
    },

};

const unsigned int iconFileFindersCount = sizeof(iconFileFinders) / sizeof(IconFileCriterion);
