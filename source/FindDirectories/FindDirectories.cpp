#include "FindDirectories.h"

#include "source/Utility/FileHelper.h"

#include <Windows.h>

inline void getSubdirectories(const std::wstring &directory, std::vector<std::wstring> &outSubdirectories) {
    // Begin search
    WIN32_FIND_DATAW file{};
    HANDLE search_handle = FindFirstFileW((directory + L"\\*").c_str(), &file);
    if (search_handle == INVALID_HANDLE_VALUE) {
        return;
    }

    // Search directory
    do {
        if (FileHelper::isCurrentOrParentDirectory(file)) {
            continue;
        }

        const std::wstring fullPath = directory + L"\\" + std::wstring(file.cFileName);
        if (FileHelper::isDirectory(fullPath)) {
            outSubdirectories.push_back(fullPath);
        }
    } while (FindNextFileW(search_handle, &file));

    // End search
    FindClose(search_handle);
}

std::vector<std::wstring> getSubdirectories(const std::vector<std::wstring> &directories) {
    std::vector<std::wstring> subdirectories{};
    for (const auto &directory : directories) {
        getSubdirectories(directory, subdirectories);
    }
    return subdirectories;
}
