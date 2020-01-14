#include "source/FindIconFiles/FindIconFiles.h"
#include "source/FindDirectories/FindDirectories.h"
#include "source/Utility/ArgumentParser.h"
#include "source/Utility/FileHelper.h"

#include <Shlobj.h>
#include <iostream>
#include <strsafe.h>
#include <string>
#include <iomanip>

void setIcon(const std::wstring &directory, const std::wstring &iconFile, int iconIndex) {
    // Copy icon file path to non-const buffer
    auto iconPath = std::make_unique<wchar_t[]>(iconFile.size() + 1);
    StringCchCopyW(iconPath.get(), iconFile.size() + 1, iconFile.data());

    SHFOLDERCUSTOMSETTINGS fcs = {};
    fcs.dwSize = sizeof(SHFOLDERCUSTOMSETTINGS);
    fcs.dwMask = FCSM_ICONFILE;
    fcs.pszIconFile = iconPath.get();
    fcs.cchIconFile = 0;
    fcs.iIconIndex = iconIndex;
    HRESULT result = SHGetSetFolderCustomSettings(&fcs, directory.c_str(), FCS_FORCEWRITE);
    if (result != S_OK) {
        auto err = GetLastError();
        throw 1; // TODO handle
    }
}

struct UserInput {
    struct Boolean {
    };

    // library or recursive or both
    // if (library || both) - library path
    // ask before?
};

void printHelp() {
    std::wcout << "IconSetter\n"
               << "\n"
               << "Iterates over input directories, selects the most appriopriate icon files for them\n"
               << "and sets the icons using Windows API.\n"
               << "\n"
               << "Options\n"
               << "\t-d\tDirectory - directory, whose children will be updated. Required.\n"
               << "\t-r\tRecursive search - search recursively each directory for icon files to set\n"
               << "\t-l <path>\tIcon library path - path to directory containing icons chosen using filenames\n"
               << "\t-f\tForce action - do not prompt user before updating icons\n"
               << "\t-h\tHelp\n";
}

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser parser{argc, argv};
    const auto help = parser.getArgumentValue<bool>("-h", false);
    const auto directory = parser.getArgumentValue<std::wstring>("-d", L"");
    const auto recursiveSearch = parser.getArgumentValue<bool>("-r", false);
    const auto iconLibraryPath = parser.getArgumentValue<std::wstring>("-l", L"");
    const auto forceAction = parser.getArgumentValue<bool>("-f", false);

    if (help) {
        printHelp();
        return 0;
    }

    if (directory.empty()) {
        std::wcerr << "ERROR: no input\n";
        printHelp();
        return 1;
    }

    if (!FileHelper::isDirectory(directory.c_str())) {
        std::wcerr << "ERROR: invalid directory specified\n";
        printHelp();
        return 1;
    }

    if (!recursiveSearch && iconLibraryPath.empty()) {
        std::wcerr << "ERROR: no icon search algorithm chosen\n";
        printHelp();
        return 1;
    }

    if (!iconLibraryPath.empty() && !FileHelper::isDirectory(iconLibraryPath.c_str())) {
        std::wcerr << "ERROR: invalid icon library path specified\n";
        printHelp();
        return 1;
    }

    // Find icon files
    std::cout << "Looking for icons...\n";
    const auto subdirectories = getSubdirectories(directory);
    struct DirectoryWithIcons {
        std::wstring directory;
        std::wstring iconFile;
    };
    std::vector<DirectoryWithIcons> directoriesWithIcons{};
    for (const auto &directory : subdirectories) {
        std::wcout << "\t" << std::left << std::setw(30) << directory; // TODO magic number

        const auto iconFiles = findIconFile(directory, recursiveSearch, iconLibraryPath);
        const auto iconFile = (iconFiles.size() > 0) ? iconFiles[0] : L"NOT FOUND";

        directoriesWithIcons.push_back({directory, iconFile});
        std::wcout << "\t" << iconFile << "\n";
    }

    // Prompt user
    if (!forceAction) {
        std::wcout << "Do you want to proceed? (Y/N): ";
        wchar_t input{};
        std::wcin >> input;
        if (!std::wcin || input == 'n' || input == 'N') {
            std::wcout << "Aborting...\n";
            return 0;
        }
    }

    // Execute
    for (const auto &entry : directoriesWithIcons) {
        setIcon(entry.directory, entry.iconFile, 0);
    }

    return 0;
}
