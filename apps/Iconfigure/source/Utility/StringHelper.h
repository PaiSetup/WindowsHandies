#pragma once

#include <algorithm>
#include <string>
#include <cwctype>

struct StringHelper {
    static bool compareCaseInsensitive(const std::wstring &left, const std::wstring &right) {
        const auto size = left.size();
        if (size != right.size()) {
            return false;
        }

        for (auto i = 0u; i < size; i++) {
            if (::tolower(left[i]) != ::tolower(right[i])) {
                return false;
            }
        }

        return true;
    }

    static std::wstring stripTrashFromFilename(const std::wstring &str) {
        // Strip trash from the end
        const static std::wstring trashChars = L"0123456789-_ \t.";
        auto trailingTrash = 0u;
        for (auto it = str.rbegin(); it < str.rend(); it++) {
            if (trashChars.find(*it) == std::wstring::npos) {
                break;
            }
            trailingTrash++;
        }
        std::wstring result = str.substr(0, str.size() - trailingTrash);

        // Delete whitespace from the middle
        result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
        return result;
    }

    static std::wstring toLower(const std::wstring &str) {
        std::wstring result(str.size(), ' ');
        std::transform(str.begin(), str.end(), result.begin(), [](wchar_t c) { return std::towlower(c); });
        return result;
    }

    static bool isSubstringCaseInsensitive(const std::wstring &str, const std::wstring &pattern) {
        const auto strLower = toLower(str);
        const auto patternLower = toLower(pattern);
        return strLower.find(patternLower) != std::wstring::npos;
    }
};
