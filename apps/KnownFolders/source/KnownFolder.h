#pragma once

#include <string>
#include <Shlobj.h>

enum class KnownFolder {
    FIRST,
    BEFORE_FIRST = FIRST - 1,

    Desktop,
    Music,
    Videos,
    Documents,

    COUNT,
    LAST = COUNT - 1,
};

struct KnownFolderHelper {
    static KnownFolder fromString(const std::string &string);
    static KNOWNFOLDERID toId(KnownFolder folder);
};
