#include "Common/AssertSuccess.h"
#include "source/IconEngine.h"
#include "source/IconFileCriterion.h"
#include "source/Utility/StringHelper.h"

#include <iostream>
#include <Shlobj.h>
#include <strsafe.h>

std::vector<std::fs::path> IconEngine::getSubdirectories(const std::vector<std::fs::path> &directories) {
    std::vector<std::fs::path> subdirectories{};
    for (const auto &directory : directories) {
        for (std::fs::path subdirectory : std::fs::directory_iterator(directory)) {
            if (std::fs::is_directory(subdirectory)) {
                subdirectories.push_back(subdirectory);
            }
        }
    }
    return subdirectories;
}

bool IconEngine::hasIcon(const std::fs::path &directory) {
    const DWORD iconPathBufferSize = 4096u;
    WCHAR iconPath[iconPathBufferSize];
    SHFOLDERCUSTOMSETTINGS fcs = {};
    fcs.dwSize = sizeof(SHFOLDERCUSTOMSETTINGS);
    fcs.dwMask = FCSM_ICONFILE;
    fcs.pszIconFile = iconPath;
    fcs.cchIconFile = iconPathBufferSize;
    HRESULT result = SHGetSetFolderCustomSettings(&fcs, directory.c_str(), FCS_READ);
    if (result != S_OK) {
        auto a = GetLastError();
        if (a == ERROR_FILE_NOT_FOUND) {
            return false;
        }
        std::wcerr << "WARNING: failed getting icon for \"" << directory << "\"\n";
        FATAL_ERROR();
    }
    if (wcslen(iconPath) == 0) {
        std::wcerr << "WARNING: invalid icon path was returned for \"" << directory << "\"\n";
        FATAL_ERROR();
    }
    return true;
}

void IconEngine::setIcon(const std::fs::path &directory, const std::fs::path &iconFile, int iconIndex) {
    // Copy icon file path to non-const buffer
    auto iconPath = std::make_unique<wchar_t[]>(iconFile.string().length() + 1);
    StringCchCopyW(iconPath.get(), iconFile.string().length() + 1, iconFile.c_str());

    SHFOLDERCUSTOMSETTINGS fcs = {};
    fcs.dwSize = sizeof(SHFOLDERCUSTOMSETTINGS);
    fcs.dwMask = FCSM_ICONFILE;
    fcs.pszIconFile = iconPath.get();
    fcs.cchIconFile = 0;
    fcs.iIconIndex = iconIndex;
    HRESULT result = SHGetSetFolderCustomSettings(&fcs, directory.c_str(), FCS_FORCEWRITE);
    if (result != S_OK) {
        auto err = GetLastError();
        std::wcerr << "WARNING: failed setting icon for \"" << directory << "\"\n";
    }
}

std::fs::path IconEngine::findIconFile(const std::fs::path &directory, bool recursiveSearch, const std::fs::path &iconLibraryPath) {
    const bool hardcodedSearch = false;

    if (!iconLibraryPath.empty()) {
        std::fs::path icon = findIconFileInIconLibrary(directory, iconLibraryPath);
        if (!icon.empty()) {
            return icon;
        }
    }

    if (hardcodedSearch) {
        std::fs::path icon = findIconFileHardcoded(directory);
        if (!icon.empty()) {
            return icon;
        }
    }

    if (recursiveSearch) {
        std::fs::path icon = findIconFileRecursively(directory, directory);
        if (!icon.empty()) {
            return icon;
        }
    }

    return std::fs::path{};
}

std::fs::path IconEngine::findIconFileInIconLibrary(const std::fs::path &directory, const std::fs::path &iconLibrary) {
    const std::fs::path directoryBaseName = directory.filename();

    for (std::fs::path iconPath : std::fs::directory_iterator(iconLibrary)) {
        const std::fs::path iconName = iconPath.filename().replace_extension();
        if (StringHelper::compareCaseInsensitive(directoryBaseName, iconName)) {
            return iconPath;
        }
    }
    return std::fs::path{};
}

std::fs::path IconEngine::findIconFileHardcoded(const std::fs::path &directory) {
    const static std::vector<std::pair<std::wstring, std::fs::path>> hardcodedMap{
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

    const std::wstring rootDirectoryBaseName = directory.filename();
    std::fs::path iconFile{};
    for (const auto &entry : hardcodedMap) {
        if (StringHelper::compareCaseInsensitive(entry.first, rootDirectoryBaseName)) {
            iconFile = entry.second;
            break;
        }
    }

    if (iconFile.empty()) {
        return std::fs::path{};
    }

    iconFile = directory / iconFile;
    if (!std::fs::is_regular_file(iconFile)) {
        std::wcerr << "WARNING: failed during recursive search - hardcoded path was set incorrectly\n";
        return std::fs::path{};
    }

    return iconFile;
}

std::fs::path IconEngine::findIconFileRecursively(const std::fs::path &rootDirectory, const std::fs::path &directory) {
    std::vector<std::fs::path> subDirectories{};

    for (std::fs::path file : std::fs::directory_iterator(directory)) {
        // Cache subdirectories to step into them later
        if (std::fs::is_directory(file)) {
            subDirectories.push_back(file);
            continue;
        }

        // Prepare names in appriopriate formats
        IconFileCriterionInput criterionInput{};
        criterionInput.rootDirectoryBaseName = rootDirectory.filename();
        criterionInput.fileFullPath = file;
        criterionInput.fileNameWithoutExtension = file.filename().replace_extension();
        criterionInput.fileExtension = file.extension();

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
            return file;
        }
    }

    // If not found, search deeper recursively
    for (const std::fs::path &subDirectory : subDirectories) {
        std::fs::path icon = findIconFileRecursively(rootDirectory, subDirectory);
        if (!icon.empty()) {
            return icon;
        }
    }

    // If still not found, return empty path (meaning failure)
    return std::fs::path{};
}