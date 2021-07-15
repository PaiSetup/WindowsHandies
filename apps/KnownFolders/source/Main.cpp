#include "KnownFolderContext.h"
#include "KnownFolderManagerContext.h"

#include "Common/ComLibraryContext.h"
#include "Common/AssertSuccess.h"
#include "Common/ArgumentParser.h"

#include <vector>
#include <optional>
#include <iostream>
#include <unordered_map>
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

std::unordered_map<std::wstring, KnownFolder> getStringToKnownFolderMapping() {
    return {
        {L"Music", KnownFolder::Music},
        {L"Videos", KnownFolder::Videos},
        {L"Desktop", KnownFolder::Desktop},
        {L"Documents", KnownFolder::Documents},
        {L"Downloads", KnownFolder::Downloads},
    };
}

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

void printHelp() {
    std::wcout
        << "KnownFolders\n"
        << "\n"
        << "Programatically overrides paths for special Windows folders such as Desktop or\n"
        << "Documents. User has to specify desired path along with the special folder name \n"
        << " to override. Available command-line arguments:\n"
        << "  -f <folderName>  Required. Folder name to set. See below for the complete list of names.\n"
        << "  -p               Required. New path for the folder.\n"
        << "  -m               Optional. Move files from old path to the new path.\n"
        << "  -d               Optional. Displays all special Windows folders and their paths.\n"
        << "  -h               Display this message.\n"
        << "\n"
        << "Available special Windows folder names:\n";
    for (const auto &entry : getStringToKnownFolderMapping()) {
        std::wcout << "  - " << entry.first << '\n';
    }
}

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser parser{argc, argv};
    const auto folderName = parser.getArgumentValue<std::wstring>("-f", L"");
    const auto newPath = parser.getArgumentValue<std::filesystem::path>("-p", L"");
    const auto moveFiles = parser.getArgumentValue<bool>("-m", false);
    const auto displayAllFolders = parser.getArgumentValue<bool>("-d", false);
    const auto help = parser.getArgumentValue<bool>("-h", false);

    // Create context
    ComLibraryContext comLibraryContext{};
    KnownFolderManagerContext manager{};

    // Early-return options
    if (help) {
        printHelp();
        return 0;
    }
    if (displayAllFolders) {
        manager.displayAllFolders();
        return 0;
    }

    // Validate and retrieve known folder
    if (folderName.empty()) {
        std::cerr << "ERROR: You must specify known folder name (see help)\n";
    }
    const auto mapping = getStringToKnownFolderMapping();
    const auto folderIt = mapping.find(folderName);
    if (folderIt == mapping.end()) {
        std::wcerr << "Invalid folder name specified: " << folderName << '\n';
    }
    const KnownFolder folder = folderIt->second;

    // Perform the operation
    setPath(manager, folder, newPath);
}
