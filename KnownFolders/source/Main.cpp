#include "KnownFolderContext.h"
#include "KnownFolderManagerContext.h"

#include "Common/ComLibraryContext.h"
#include "Common/AssertSuccess.h"

#include <vector>
#include <optional>
#include <iostream>
#include <filesystem>

bool createDirectory(const std::filesystem::path &path) {
    const std::filesystem::path parent = path.parent_path();
    if (path == parent) {
        return true;
    }
    const bool parentSuccess = createDirectory(parent);

    bool thisSuccess = CreateDirectoryW(path.c_str(), {});
    if (!thisSuccess && GetLastError() == ERROR_ALREADY_EXISTS) {
        thisSuccess = true;
    }
    return parentSuccess & thisSuccess;
}

enum class KnownFolder {
    Music,
    Videos,
    Desktop,
    Documents,
    Downloads,
};

void setPath(KnownFolderManagerContext &manager, KnownFolder knownFolder, const std::filesystem::path &path) {
    FATAL_ERROR_IF(!createDirectory(path));

    switch (knownFolder) {
    case KnownFolder::Music:
        manager.getFolder(FOLDERID_Music).setPath(path);
        manager.getFolder(FOLDERID_LocalMusic).setPath(path);
        break;
    case KnownFolder::Videos:
        manager.getFolder(FOLDERID_Videos).setPath(path);
        manager.getFolder(FOLDERID_LocalVideos).setPath(path);
        break;
    case KnownFolder::Desktop:
        manager.getFolder(FOLDERID_Desktop).setPath(path);
        break;
    case KnownFolder::Documents:
        manager.getFolder(FOLDERID_Documents).setPath(path);
        manager.getFolder(FOLDERID_LocalDocuments).setPath(path);
        break;
    case KnownFolder::Downloads:
        manager.getFolder(FOLDERID_Downloads).setPath(path);
        manager.getFolder(FOLDERID_LocalDownloads).setPath(path);
        break;
    default:
        FATAL_ERROR();
    }
}

int main() {
    bool success = createDirectory("D:\\a\\b\\c\\d");

    ComLibraryContext comLibraryContext{};
    KnownFolderManagerContext manager{};

    setPath(manager, KnownFolder::Music, L"D:\\Test\\Music");
    manager.displayAllFolders();
}
