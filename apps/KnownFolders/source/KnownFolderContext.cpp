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

    const std::filesystem::path path = getPath();
    if (!path.empty()) {
        std::wcout << definition.pszName << ":    " << path << "\n";
    }
}

void KnownFolderContext::setPath(const std::filesystem::path &path) {
    assertSuccess(folder->SetPath(0, path.c_str()));
}

std::filesystem::path KnownFolderContext::getPath() {
    wchar_t *path = nullptr;
    if (FAILED(folder->GetPath(0, &path))) {
        return L"";
    }
    std::filesystem::path result{path};
    CoTaskMemFree(path);
    return result;
}
