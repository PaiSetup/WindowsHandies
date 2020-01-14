#include "FindIconFiles.h"

#include "source/FindIconFiles/IconFileCriterion.h"
#include "source/Utility/StringHelper.h"
#include "source/Utility/FileHelper.h"

#include <Windows.h>

static void findIconFile(const std::wstring &rootDirectory, const std::wstring &directory, std::vector<std::wstring> &outIconFiles) {
    const auto rootDirectoryBaseName = StringHelper::deleteSpaces(StringHelper::basename(rootDirectory));

    // Begin search
    WIN32_FIND_DATAW file{};
    HANDLE search_handle = FindFirstFileW((directory + L"\\*").c_str(), &file);
    if (search_handle == INVALID_HANDLE_VALUE) {
        // return error
    }

    // Search directory
    std::vector<std::wstring> subDirectories{};
    do {
        if (FileHelper::isCurrentOrParentDirectory(file)) {
            continue;
        }

        const std::wstring fileName{file.cFileName};
        const std::wstring fileFullPath = directory + L"\\" + fileName;
        const std::wstring fileExtension = StringHelper::getFileNameExtension(fileName);

        if (FileHelper::isDirectory(fileFullPath)) {
            subDirectories.push_back(fileFullPath);
            continue;
        }

        if (!FileHelper::isFile(fileFullPath)) {
            continue;
        }

        if (!FileHelper::isIconCompatibleExtension(fileExtension)) {
            continue;
        }

        const std::wstring fileBaseName = StringHelper::getFileNameWithoutExtension(fileName);
        IconFileCriterionResult criteriaResult = IconFileCriterionResult::DontKnow;
        for (auto i = 0u; i < iconFileFindersCount; i++) {
            const auto result = iconFileFinders[i](rootDirectoryBaseName, fileBaseName);
            if (result == IconFileCriterionResult::Deny) {
                criteriaResult = IconFileCriterionResult::Deny;
            }
            if (result == IconFileCriterionResult::Accept && criteriaResult != IconFileCriterionResult::Deny) {
                criteriaResult = IconFileCriterionResult::Accept;
            }
        }

        if (criteriaResult == IconFileCriterionResult::Accept) {
            outIconFiles.push_back(fileFullPath);
        }
    } while (FindNextFileW(search_handle, &file));

    // End search
    FindClose(search_handle);

    // If not found, search deeper recursively
    if (outIconFiles.size() == 0) {
        for (const auto &directory : subDirectories) {
            findIconFile(rootDirectory, directory, outIconFiles);
        }
    }
}

std::vector<std::wstring> findIconFile(const std::wstring &directory) {
    std::vector<std::wstring> iconFiles{};
    findIconFile(directory, directory, iconFiles);
    return iconFiles;
}
