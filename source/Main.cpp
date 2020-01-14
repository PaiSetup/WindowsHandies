#include "source/FindIconFiles/FindIconFiles.h"
#include "source/FindDirectories/FindDirectories.h"

#include <Shlobj.h>
#include <iostream>
#include <strsafe.h>
#include <string>

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

int main() {
    auto subdirectories = getSubdirectories(L"E:\\Programs");
    for (const auto &directory : subdirectories) {
        std::wcout << directory << "\t\t\t";
        auto iconFiles = findIconFile(directory);
        for (const auto &iconFile : iconFiles) {
            std::wcout << iconFile << " ";
        }
        std::wcout << "\n";

        if (iconFiles.size() > 0) {
             setIcon(directory, iconFiles[0], 0);
        }
    }

    return 0;
}
