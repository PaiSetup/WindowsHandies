#pragma once

#include <Shlobj.h>
#include <wrl.h>
#include <filesystem>

using Microsoft::WRL::ComPtr;

class KnownFolderContext {
public:
    KnownFolderContext(IKnownFolder *folder);

    void display();

    void setPath(const std::filesystem::path &path);
    std::filesystem::path getPath();

private:
    ComPtr<IKnownFolder> folder = nullptr;
};
