#include "KnownFolderManagerContext.h"
#include "KnownFolderContext.h"
#include "Common/AssertSuccess.h"

KnownFolderManagerContext::KnownFolderManagerContext() {
    assertSuccess(CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&manager)));
}

std::vector<KNOWNFOLDERID> KnownFolderManagerContext::getAllIds() {
    KNOWNFOLDERID *ids;
    UINT count;
    assertSuccess(manager->GetFolderIds(&ids, &count));

    std::vector<KNOWNFOLDERID> result{ids, ids + count};
    CoTaskMemFree(ids);
    return result;
}

KnownFolderContext KnownFolderManagerContext::getFolder(KNOWNFOLDERID id) {
    IKnownFolder *folder = nullptr;
    assertSuccess(manager->GetFolder(id, &folder));
    return KnownFolderContext{folder};
}

void KnownFolderManagerContext::displayAllFolders() {
    for (auto id : getAllIds()) {
        getFolder(id).display();
    }
}
