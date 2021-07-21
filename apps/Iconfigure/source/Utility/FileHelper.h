#pragma once
#include <Windows.h>
#include <string>

struct FileHelper {
    static bool exists(DWORD attributes) {
        return INVALID_FILE_ATTRIBUTES != attributes;
    }

    static bool isCurrentOrParentDirectory(WIN32_FIND_DATAW file) {
        return (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (!lstrcmpW(file.cFileName, L".") || !lstrcmpW(file.cFileName, L".."));
    }

    static bool isDirectory(const std::wstring &path) {
        DWORD attributes = GetFileAttributesW(path.c_str());
        return exists(attributes) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool isFile(const std::wstring &path) {
        DWORD attributes = GetFileAttributesW(path.c_str());
        return exists(attributes) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool isIconCompatibleExtension(const std::wstring &extension) {
        return (lstrcmpW(extension.c_str(), L".ico") == 0) || (lstrcmpW(extension.c_str(), L".exe") == 0);
    }
};
