#include "FindDirectories.h"

#include "source/Utility/FileHelper.h"

#include <Windows.h>

std::vector<std::wstring> getSubdirectories(const std::wstring &directory) {
    // Begin search
    WIN32_FIND_DATAW file{};
    HANDLE search_handle = FindFirstFileW((directory + L"\\*").c_str(), &file);
    if (search_handle == INVALID_HANDLE_VALUE) {
        return {};
    }

    // Search directory
    std::vector<std::wstring> subdirectories;
    do {
        if (FileHelper::isCurrentOrParentDirectory(file)) {
            continue;
        }

        const std::wstring fullPath = directory + L"\\" + std::wstring(file.cFileName);
        if (FileHelper::isDirectory(fullPath)) {
            subdirectories.push_back(fullPath);
        }
    } while (FindNextFileW(search_handle, &file));

    // End search
    FindClose(search_handle);
    return subdirectories;
}
