#include "source/FindIconFiles/FindIconFiles.h"
#include "source/FindDirectories/FindDirectories.h"

//#include <Shlobj.h>
#include <iostream>
#include <string>

int main() {

    //auto a = findIconFile(L"E:\\Programs\\Free Video Cutter");

    auto subdirectories = getSubdirectories(L"E:\\Programs");
    for (const auto &directory : subdirectories) {
        std::wcout << directory << "\t";
        auto iconFiles = findIconFile(directory);
        for (const auto &iconFile : iconFiles) {
            std::wcout << iconFile << " ";
        }
        std::wcout << "\n";
    }

    return 0;

    /*wchar_t icon[] = L"D:\\Desktop\\icons.ico";
    auto err = GetLastError();
    SHFOLDERCUSTOMSETTINGS fcs = {0};
    fcs.dwSize = sizeof(SHFOLDERCUSTOMSETTINGS);
    fcs.dwMask = FCSM_ICONFILE;
    fcs.pszIconFile = icon;
    fcs.cchIconFile = 0;
    fcs.iIconIndex = 0;
    SHGetSetFolderCustomSettings(&fcs, L"D:\\Desktop\\TestDir2", FCS_FORCEWRITE);
    err = GetLastError();

    return 0;*/
}
