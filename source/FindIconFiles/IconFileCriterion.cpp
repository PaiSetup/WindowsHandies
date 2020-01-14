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

    // Simple compare
    +[](const IconFileCriterionInput &input) {
        if (StringHelper::compareCaseInsensitive(input.rootDirectoryBaseName, input.fileNameWithoutExtension)) {
            return IconFileCriterionResult::Accept;
        }
        return IconFileCriterionResult::DontKnow;
    },

    // Strip trash from the end (digits, whitespace, hyphens, dots, etc.) and compare
    +[](const IconFileCriterionInput &input) {
        const auto strippedDirectoryName = StringHelper::stripTrashFromTheEnd(input.rootDirectoryBaseName);
        const auto strippedFileName = StringHelper::stripTrashFromTheEnd(input.fileNameWithoutExtension);
        if (StringHelper::compareCaseInsensitive(strippedDirectoryName, strippedFileName)) {
            return IconFileCriterionResult::Accept;
        }
        return IconFileCriterionResult::DontKnow;
    },
};

const unsigned int iconFileFindersCount = sizeof(iconFileFinders) / sizeof(IconFileCriterion);
