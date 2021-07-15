#include "KnownFolder.h"
#include <array>

struct KnownFolderInfo {
    KnownFolder value;
    std::string string;
    KNOWNFOLDERID id;
};

static KnownFolderInfo infos[] = {
    {KnownFolder::Desktop, "Desktop", FOLDERID_Desktop},
 //   {KnownFolder::Music, "Music", FOLDERID_Music},
    {KnownFolder::Music, "Music", FOLDERID_Music},
    {KnownFolder::Videos, "Videos", FOLDERID_Videos},
    {KnownFolder::Documents, "Documents", FOLDERID_Documents},
};

template <typename Predicate>
KnownFolderInfo getInfoImpl(Predicate predicate) {
    const auto begin = infos;
    const auto end = infos + _countof(infos);
    auto it = std::find_if(begin, end, predicate);
    if (it == end) {
        throw std::exception();
    }
    return *it;
}

KnownFolderInfo getInfo(KnownFolder value) {
    return getInfoImpl([&](const KnownFolderInfo &info) {
        return value == info.value;
    });
}

KnownFolderInfo getInfo(const std::string &string) {
    return getInfoImpl([&](const KnownFolderInfo &info) {
        return string == info.string;
    });
}

KnownFolderInfo getInfo(KNOWNFOLDERID id) {
    return getInfoImpl([&](const KnownFolderInfo &info) {
        return id == info.id;
    });
}

KnownFolder KnownFolderHelper::fromString(const std::string &string) {
    return getInfo(string).value;
}

KNOWNFOLDERID KnownFolderHelper::toId(KnownFolder folder) {
    return getInfo(folder).id;
}
