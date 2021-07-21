#include "IconFileCriterion.h"

#include "source/Utility/StringHelper.h"

#include <unordered_map>

const IconFileCriterion iconFileFinders[] = {
    // Deny non-files
    +[](const IconFileCriterionInput &input) {
        if (!std::fs::is_regular_file(input.fileFullPath)) {
            return IconFileCriterionResult::Deny;
        }
        return IconFileCriterionResult::DontKnow;
    },

    // Deny no-icon files
    +[](const IconFileCriterionInput &input) {
        const std::fs::path iconCompatibleExtensions[] = {".exe", ".ico"};
        for (const std::fs::path &iconCompatibleExtension : iconCompatibleExtensions) {
            if (input.fileExtension == iconCompatibleExtension) {
                return IconFileCriterionResult::DontKnow;
            }
        }
        return IconFileCriterionResult::Deny;
    },

    // Deny VisualStudio (for some reason setting icon to it does not work)
    +[](const IconFileCriterionInput &input) {
        if (StringHelper::isSubstringCaseInsensitive(input.rootDirectoryBaseName, L"VisualStudio2")) {
            return IconFileCriterionResult::Deny;
        }
        return IconFileCriterionResult::DontKnow;
    },

    // Simple compare
    +[](const IconFileCriterionInput &input) {
        if (StringHelper::compareCaseInsensitive(input.rootDirectoryBaseName, input.fileNameWithoutExtension)) {
            return IconFileCriterionResult::Accept;
        }
        return IconFileCriterionResult::DontKnow;
    },

    // Strip trash from the filename (digits, whitespace, hyphens, dots, etc.) and check if it matches
    +[](const IconFileCriterionInput &input) {
        const auto strippedDirectoryName = StringHelper::stripTrashFromFilename(input.rootDirectoryBaseName);
        const auto strippedFileName = StringHelper::stripTrashFromFilename(input.fileNameWithoutExtension);
        if (StringHelper::isSubstringCaseInsensitive(strippedFileName, strippedDirectoryName)) {
            return IconFileCriterionResult::Accept;
        }
        return IconFileCriterionResult::DontKnow;
    },
};

const unsigned int iconFileFindersCount = sizeof(iconFileFinders) / sizeof(IconFileCriterion);
