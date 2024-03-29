#include "KnownFolderContext.h"
#include "KnownFolderManagerContext.h"

#include "Common/ComLibraryContext.h"
#include "Common/ArgumentParser.h"

#include <unordered_map>

std::unordered_map<std::wstring, std::vector<KNOWNFOLDERID>> getKnownFolderIds() {
    // clang-format off
    return {
        {L"Music",     {FOLDERID_LocalMusic,     FOLDERID_Music    }},
        {L"Videos",    {FOLDERID_LocalVideos,    FOLDERID_Videos   }},
        {L"Documents", {FOLDERID_LocalDocuments, FOLDERID_Documents}},
        {L"Downloads", {FOLDERID_LocalDownloads, FOLDERID_Downloads}},
        {L"Desktop",   {FOLDERID_Desktop                           }},
    };
    // clang-format on
}

bool migrateFiles(const std::filesystem::path &oldPath, const std::filesystem::path &newPath) {
    FATAL_ERROR_IF(!std::filesystem::is_directory(oldPath));
    FATAL_ERROR_IF(!std::filesystem::is_directory(newPath));

    bool success = true;
    for (const std::filesystem::path &oldFile : std::filesystem::directory_iterator(oldPath)) {
        const std::filesystem::path &newFile = newPath / oldFile.filename();
        FATAL_ERROR_IF(std::filesystem::exists(newFile));

        std::error_code error{};
        std::filesystem::rename(oldFile, newFile, error);
        if (error) {
            std::cerr << "WARNING: moving file " << oldFile << " to " << newPath << " failed (errorCode " << error << ")\n";
            success = false;
        }
    }

    return success;
}

bool setPath(KnownFolderManagerContext &manager,
             const std::vector<KNOWNFOLDERID> &ids,
             const std::filesystem::path &newPath,
             bool performFilesMigration,
             bool removeOldFolder) {
    std::filesystem::create_directories(newPath);

    // Prepare helper structs
    struct SetPathInfo {
        KnownFolderContext folder;
        std::filesystem::path oldPath;
        bool pathChanged;
        bool oldPathExists;
    };
    std::vector<SetPathInfo> infos = {};
    for (const auto &id : ids) {
        infos.emplace_back();
        SetPathInfo &info = infos.back();
        info.folder = manager.getFolder(id);
        info.oldPath = info.folder.getPath();
        info.pathChanged = newPath != info.oldPath;
        info.oldPathExists = std::filesystem::exists(info.oldPath);
    }

    // Perform migrations
    if (performFilesMigration) {
        for (SetPathInfo &info : infos) {
            if (info.pathChanged && info.oldPathExists) {
                if (!migrateFiles(info.oldPath, newPath)) {
                    std::wcerr << "ERROR: failed migrating files from " << info.oldPath << '\n';
                    return false;
                }
            }
        }
    }

    // Change path
    for (SetPathInfo &info : infos) {
        if (info.pathChanged) {
            if (!info.folder.setPath(newPath)) {
                std::wcerr << "ERROR: failed changing known folder\n";
                return false;
            }
        }
    }

    // Remove old folder
    if (removeOldFolder) {
        for (SetPathInfo &info : infos) {
            if (info.pathChanged && info.oldPathExists) {
                std::error_code error{};
                std::filesystem::remove_all(info.oldPath, error);
                if (error) {
                    std::cerr << "WARNING: removing old known folder location (" << info.oldPath << ") failed (errorCode " << error << ")\n";
                }
            }
        }
    }

    return true;
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
        << "  -r               Optional. Removes old directory after the change.\n"
        << "  -h               Display this message.\n"
        << "\n"
        << "Available special Windows folder names:\n";
    for (const auto &entry : getKnownFolderIds()) {
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
    const auto removeOldFolder = parser.getArgumentValue<bool>("-r", false);
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
        return 1;
    }
    const auto mapping = getKnownFolderIds();
    const auto folderIt = mapping.find(folderName);
    if (folderIt == mapping.end()) {
        std::wcerr << "Invalid folder name specified: " << folderName << '\n';
        return 1;
    }
    const std::vector<KNOWNFOLDERID> &folderData = folderIt->second;

    // Perform the operation
    const bool success = setPath(manager, folderData, newPath, moveFiles, removeOldFolder);
    if (success) {
        std::wcerr << "SUCCESS: " << folderName << " is now located at " << newPath << '\n';
        return 0;
    } else {
        std::wcerr << "FAILURE\n";
        return 1;
    }
}
