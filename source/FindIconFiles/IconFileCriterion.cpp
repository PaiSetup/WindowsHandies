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

    // Accept hardcoded names
    +[](const std::wstring &directoryName, const std::wstring &fileName) {
        std::vector<std::pair<std::wstring, std::wstring>> acceptMap{
            {L"Bandicut", L"bdcut"},
            {L"BeyondCompare", L"BCompare"},
            {L"Beyond Compare", L"BCompare"},
            {L"cmake", L"cmake-gui"},
        };

        for (auto i = 0u; i < acceptMap.size(); i++) {
            const auto names = acceptMap[i];
            const bool directoryMatch = StringHelper::compareCaseInsensitive(names.first, directoryName);
            const bool fileMatch = StringHelper::compareCaseInsensitive(names.second, fileName);
            if (directoryMatch && fileMatch) {
                return IconFileCriterionResult::Accept;
            }
        }
        return IconFileCriterionResult::DontKnow;
    },
};

const unsigned int iconFileFindersCount = sizeof(iconFileFinders) / sizeof(IconFileCriterion);
