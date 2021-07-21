#include "Common/ArgumentParser.h"
#include "Common/AssertSuccess.h"
#include "source/FindIconFiles/FindIconFiles.h"
#include "source/FindDirectories/FindDirectories.h"
#include "source/Utility/FileHelper.h"

#include <Shlobj.h>
#include <iostream>
#include <strsafe.h>
#include <string>
#include <iomanip>

bool hasIcon(const std::fs::path &directory) {
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

void setIcon(const std::fs::path &directory, const std::fs::path &iconFile, int iconIndex) {
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

void printHelp() {
    std::wcout << "Iconfigure\n"
                  "\n"
                  "Iterates over input directories, selects the most appriopriate icon files for them and sets \n"
                  "the icons using Windows API. Path to folder with directories to set icon for and at least \n"
                  "one icon search method \n"
                  "\n"
                  "Example usages:\n"
                  "  Iconfigure.exe -d E:\\Programs -r                            setup all subdirectories of \n"
                  "                                                              E:\\Programs by looking for \n"
                  "                                                              icons in themrecursively.\n"
                  "\n"
                  "  Iconfigure.exe -d E:\\Programs -d E:\\Games -l E:\\Icons       setup all subdirectories of \n"
                  "                                                              E:\\Programs and E:\\Games by \n"
                  "                                                              looking for icons in E:\\Icons.\n"
                  "\n"
                  "Options\n"
                  "  -d <path>  Directory - directory, whose children will be updated. Required one or more.\n"
                  "  -r         Recursive search - enabled recursive search within each directory for icon files\n"
                  "  -l <path>  Icon library path - enables icon library search - select icons from within the\n"
                  "             library directory based on file names.\n"
                  "  -y         Do not prompt user before updating the icons\n"
                  "  -f         Force set icon. Existing icons for selected directories will be overwritten\n"
                  "  -h         Display this help message\n";
}

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser parser{argc, argv};
    const auto help = parser.getArgumentValue<bool>("-h", false);
    const auto directories = parser.getArgumentValues<std::fs::path>("-d");
    const auto recursiveSearch = parser.getArgumentValue<bool>("-r", false);
    const auto iconLibraryPath = parser.getArgumentValue<std::fs::path>("-l", L"");
    const auto skipPrompt = parser.getArgumentValue<bool>("-y", false);
    const auto forceSetIcon = parser.getArgumentValue<bool>("-f", false);

    if (help) {
        printHelp();
        return 0;
    }

    if (directories.empty()) {
        printHelp();
        std::wcerr << "ERROR: no input\n";
        return 1;
    }

    for (const std::fs::path &directory : directories) {
        if (!std::fs::is_directory(directory)) {
            printHelp();
            std::wcerr << "ERROR: invalid directory specified - \"" << directory << "\"\n";
            return 1;
        }
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
    const auto subdirectories = getSubdirectories(directories);
    struct DirectoryWithIcons {
        std::fs::path directory;
        std::fs::path iconFile;
    };
    std::vector<DirectoryWithIcons> directoriesWithIcons{};
    for (const std::fs::path &directory : subdirectories) {
        if (!forceSetIcon && hasIcon(directory)) {
            continue;
        }

        std::wcout << "\t" << std::left << std::setw(30) << directory; // TODO magic number

        const std::fs::path iconFile = findIconFile(directory, recursiveSearch, iconLibraryPath);

        directoriesWithIcons.push_back({directory, iconFile});
        std::wcout << "\t" << iconFile << "\n";
    }

    // Prompt user
    if (!skipPrompt) {
        std::wcout << "Do you want to proceed? (Y/N): ";
        wchar_t input{};
        std::wcin >> input;
        if (!std::wcin || input == 'n' || input == 'N') {
            std::wcout << "Aborting...\n";
            return 0;
        }
    }

    // Execute
    if (directoriesWithIcons.empty()) {
        std::cout << "No directories for icon setting found\n";
        return 0;
    }
    std::wcout << "Setting icons...\n";
    for (const auto &entry : directoriesWithIcons) {
        if (!entry.iconFile.empty()) {
            setIcon(entry.directory, entry.iconFile, 0);
        }
    }

    std::wcout << "Done\n";
    return 0;
}
