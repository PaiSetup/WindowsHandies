#include "KnownFolderContext.h"
#include "Common/AssertSuccess.h"

#include <iostream>

KnownFolderContext::KnownFolderContext(IKnownFolder *folder)
    : folder(folder) {}

void KnownFolderContext::display() {
    KNOWNFOLDER_DEFINITION definition;
    assertSuccess(folder->GetFolderDefinition(&definition));
    if (definition.category != KF_CATEGORY_COMMON && definition.category != KF_CATEGORY_PERUSER) {
        return;
    }

    const std::wstring path = getPath();
    if (path != L"") {
        std::wcout << definition.pszName << ":    " << path << "\n";
    }
}

void KnownFolderContext::setPath(const std::wstring &path) {
    assertSuccess(folder->SetPath(0, path.c_str()));
}

std::wstring KnownFolderContext::getPath() {
    wchar_t *path = nullptr;
    if (FAILED(folder->GetPath(0, &path))) {
        return L"";
    }
    std::wstring result{path};
    CoTaskMemFree(path);
    return result;
}
