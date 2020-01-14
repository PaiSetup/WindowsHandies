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
               << "and sets the icons using Windows API. Path to folder with directories to set icon for\n"
               << "and at least one icon search method \n"
               << "\n"
               << "Usage\n"
               << "  IconSetter.exe -d E:\\Programs -r -f\n"
               << "  IconSetter.exe -d D: -l D:\\Pictures\\Icons -f\n"
               << "\n"
               << "Options\n"
               << "  -d <path>  Directory - directory, whose children will be updated. Required.\n"
               << "  -r         Recursive search - enabled recursive search within each directory for icon files\n"
               << "  -l <path>  Icon library path - enables icon library search - select icons from within the\n"
               << "             library directory based on file names.\n"
               << "  -f         Force action - do not prompt user before updating the icons\n"
               << "  -h         Display this help message\n";
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
        printHelp();
        std::wcerr << "ERROR: no input\n";
        return 1;
    }

    if (!FileHelper::isDirectory(directory.c_str())) {
        printHelp();
        std::wcerr << "ERROR: invalid directory specified\n";
        return 1;
    }

    if (!recursiveSearch && iconLibraryPath.empty()) {
        printHelp();
        std::wcerr << "ERROR: no icon search algorithm chosen\n";
        return 1;
    }

    if (!iconLibraryPath.empty() && !FileHelper::isDirectory(iconLibraryPath.c_str())) {
        printHelp();
        std::wcerr << "ERROR: invalid icon library path specified\n";
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
        const auto iconFile = (iconFiles.size() > 0) ? iconFiles[0] : L"";

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
    std::wcout << "Setting icons...\n";
    for (const auto &entry : directoriesWithIcons) {
        if (!entry.iconFile.empty()) {
            setIcon(entry.directory, entry.iconFile, 0);
        }
    }

    std::wcout << "Done\n";
    return 0;
}
