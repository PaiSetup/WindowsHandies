#pragma once

#include <Shlobj.h>
#include <wrl.h>
#include <filesystem>

using Microsoft::WRL::ComPtr;

class KnownFolderContext {
public:
    KnownFolderContext() = default;
    KnownFolderContext(IKnownFolder *folder);
    KnownFolderContext(const KnownFolderContext &) = default;
    KnownFolderContext &operator=(const KnownFolderContext &) = default;
    KnownFolderContext(KnownFolderContext &&) = default;
    KnownFolderContext &operator=(KnownFolderContext &&) = default;

    void display();

    bool setPath(const std::filesystem::path &path);
    std::filesystem::path getPath();

private:
    ComPtr<IKnownFolder> folder = nullptr;
};
