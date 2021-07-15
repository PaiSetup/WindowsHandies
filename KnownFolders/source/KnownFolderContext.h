#pragma once

#include <string>
#include <Shlobj.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class KnownFolderContext {
public:
    KnownFolderContext(IKnownFolder *folder);

    void display();

    void setPath(const std::wstring &path);
    std::wstring getPath();

private:
    ComPtr<IKnownFolder> folder = nullptr;
};
