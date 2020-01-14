#include "FindIconFiles.h"

#include "source/FindIconFiles/IconFileCriterion.h"
#include "source/Utility/StringHelper.h"
#include "source/Utility/FileHelper.h"

#include <Windows.h>

static void findIconFileHardcoded(const std::wstring &directory, std::vector<std::wstring> &outIconFiles) {
    const static std::vector<std::pair<std::wstring, std::wstring>> hardcodedMap{
        {L"Bandicut", L"bdcut.exe"},
        {L"BeyondCompare", L"BCompare.exe"},
        {L"cmake", L"bin\\cmake-gui.exe"},
        {L"GPA", L"GpaMonitor.exe"},
        {L"IntelliJ", L"bin\\idea.exe"},
        {L"irfanview", L"i_view64.exe"},
        {L"MikTex", L"miktex\\bin\\x64\\mf.exe"},
    };

    const std::wstring rootDirectoryBaseName = StringHelper::deleteSpaces(StringHelper::basename(directory));
    std::wstring fileName{};
    for (auto i = 0u; i < hardcodedMap.size(); i++) {
        const auto &names = hardcodedMap[i];
        if (StringHelper::compareCaseInsensitive(names.first, rootDirectoryBaseName)) {
            fileName = names.second;
        }
    }

    if (fileName.empty()) {
        return;
    }

    const std::wstring fileFullPath = directory + L"\\" + fileName;
    if (!FileHelper::isFile(fileFullPath)) {
        throw 1; // TODO incorrect hardcoding, handle this better
    }

    outIconFiles.push_back(fileFullPath);
}

static void findIconFileRecursively(const std::wstring &rootDirectory, const std::wstring &directory, std::vector<std::wstring> &outIconFiles) {
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

        const std::wstring rootDirectoryBaseName = StringHelper::deleteSpaces(StringHelper::basename(rootDirectory));
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
            findIconFileRecursively(rootDirectory, directory, outIconFiles);
        }
    }
}

std::vector<std::wstring> findIconFile(const std::wstring &directory) {
    std::vector<std::wstring> iconFiles{};

    findIconFileHardcoded(directory, iconFiles);
    if (!iconFiles.empty()) {
        iconFiles;
    }

    findIconFileRecursively(directory, directory, iconFiles);
    return iconFiles;
}
