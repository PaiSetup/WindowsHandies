#include "IconFileCriterion.h"

#include "source/Utility/StringHelper.h"
#include "source/Utility/FileHelper.h"

#include <unordered_map>

const IconFileCriterion iconFileFinders[] = {
    // Deny non-files
    +[](const IconFileCriterionInput &input) {
        if (!FileHelper::isFile(input.fileFullPath)) {
            return IconFileCriterionResult::Deny;
        }
        return IconFileCriterionResult::DontKnow;
    },

    // Deny no-icon files
    +[](const IconFileCriterionInput &input) {
        // TODO check if file actually has icon
        if (!FileHelper::isIconCompatibleExtension(input.fileExtension)) {
            return IconFileCriterionResult::Deny;
        }
        return IconFileCriterionResult::DontKnow;
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
