#pragma once

#include <Shlobj.h>

#include <vector>
#include <wrl.h>

using Microsoft::WRL::ComPtr;
class KnownFolderContext;

class KnownFolderManagerContext {
public:
    KnownFolderManagerContext();

    std::vector<KNOWNFOLDERID> getAllIds();
    KnownFolderContext getFolder(KNOWNFOLDERID id);
    void displayAllFolders();

private:
    ComPtr<IKnownFolderManager> manager = nullptr;
};
