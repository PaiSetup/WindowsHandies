#include "FindIconFiles.h"

#include "source/FindIconFiles/IconFileCriterion.h"
#include "source/Utility/StringHelper.h"
#include "source/Utility/FileHelper.h"

#include <Windows.h>
#include <iostream>

static void findIconFileInIconLibrary(const std::wstring &iconLibrary, const std::wstring &directory, std::vector<std::wstring> &outIconFiles) {
    // Begin search
    WIN32_FIND_DATAW file{};
    HANDLE search_handle = FindFirstFileW((iconLibrary + L"\\*.ico").c_str(), &file);
    if (search_handle == INVALID_HANDLE_VALUE) {
        std::wcerr << "WARNING: Failed during icon library search\n";
        return;
    }

    // Search directory
    do {
        const std::wstring directoryBaseName = StringHelper::basename(directory);
        const std::wstring fileName = std::wstring{file.cFileName};
        const std::wstring fileNameExtension = StringHelper::getFileNameExtension(fileName);
        const std::wstring fileNameWithoutExtension = StringHelper::getFileNameWithoutExtension(fileName);

        if (StringHelper::compareCaseInsensitive(directoryBaseName, fileNameWithoutExtension)) {
            const std::wstring fileFullPath = iconLibrary + L"\\" + fileName;
            outIconFiles.push_back(fileFullPath);
        }
    } while (FindNextFileW(search_handle, &file));

    // End search
    FindClose(search_handle);
}

static void findIconFileHardcoded(const std::wstring &directory, std::vector<std::wstring> &outIconFiles) {
    const static std::vector<std::pair<std::wstring, std::wstring>> hardcodedMap{
        {L"Bandicut", L"bdcut.exe"},
        {L"BeyondCompare", L"BCompare.exe"},
        {L"cmake", L"bin\\cmake-gui.exe"},
        {L"Git", L"git-cmd.exe"},
        {L"GPA", L"GpaMonitor.exe"},
        {L"IntelliJ", L"bin\\idea.exe"},
        {L"irfanview", L"i_view64.exe"},
        {L"Jenkins", L"war\\favicon.ico"},
        {L"MikTex", L"miktex\\bin\\x64\\mf.exe"},
        {L"MicrosoftOffice2013", L"Office15\\FIRSTRUN.exe"},
        {L"Octave", L"share\\octave\\4.2.2\\imagelib\\octave-logo.ico"},
        {L"Unity3D", L"Unity Hub\\Unity Hub.exe"},
        {L"VisualStudioCode", L"Code.exe"},
        {L"7zip", L"7zFM.exe"},
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
        std::wcerr << "WARNING: failed during recursive search - hardcoded path was set incorrectly\n";
        return;
    }

    outIconFiles.push_back(fileFullPath);
}

static void findIconFileRecursively(const std::wstring &rootDirectory, const std::wstring &directory, std::vector<std::wstring> &outIconFiles) {
    // Begin search
    WIN32_FIND_DATAW file{};
    HANDLE search_handle = FindFirstFileW((directory + L"\\*").c_str(), &file);
    if (search_handle == INVALID_HANDLE_VALUE) {
        std::wcerr << "WARNING: Failed during recursive search\n";
    }

    // Search directory
    std::vector<std::wstring> subDirectories{};
    do {
        // Prepare names in appriopriate formats
        const std::wstring fileName = std::wstring{file.cFileName};
        IconFileCriterionInput criterionInput{};
        criterionInput.rootDirectoryBaseName = StringHelper::deleteSpaces(StringHelper::basename(rootDirectory));
        criterionInput.fileFullPath = directory + L"\\" + fileName;
        criterionInput.fileNameWithoutExtension = StringHelper::getFileNameWithoutExtension(fileName);
        criterionInput.fileExtension = StringHelper::getFileNameExtension(fileName);

        // Ignore pseudo-files
        if (FileHelper::isCurrentOrParentDirectory(file)) {
            continue;
        }

        // Cache subdirectories to step into them later
        if (FileHelper::isDirectory(criterionInput.fileFullPath)) {
            subDirectories.push_back(criterionInput.fileFullPath);
            continue;
        }

        // Iterate over available criteria
        IconFileCriterionResult criteriaResult = IconFileCriterionResult::DontKnow;
        for (auto i = 0u; i < iconFileFindersCount; i++) {
            const auto result = iconFileFinders[i](criterionInput);
            if (result == IconFileCriterionResult::Deny) {
                criteriaResult = IconFileCriterionResult::Deny;
                break;
            }
            if (result == IconFileCriterionResult::Accept && criteriaResult != IconFileCriterionResult::Deny) {
                criteriaResult = IconFileCriterionResult::Accept;
            }
        }

        // Push result if accepted acording to criteria
        if (criteriaResult == IconFileCriterionResult::Accept) {
            outIconFiles.push_back(criterionInput.fileFullPath);
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

std::vector<std::wstring> findIconFile(const std::wstring &directory, bool recursiveSearch, std::wstring iconLibraryPath) {
    std::vector<std::wstring> iconFiles{};

    if (!iconLibraryPath.empty()) {
        findIconFileInIconLibrary(iconLibraryPath, directory, iconFiles);
        if (!iconFiles.empty()) {
            return iconFiles;
        }
    }

    if (recursiveSearch) {
        findIconFileHardcoded(directory, iconFiles);
        if (!iconFiles.empty()) {
            return iconFiles;
        }
        findIconFileRecursively(directory, directory, iconFiles);
    }

    return iconFiles;
}
