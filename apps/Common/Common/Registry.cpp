
#include "Common/Registry.h"
#include <Windows.h>

std::wstring RegistryHelper::readString(RegistryHelper::Hive hive, const wchar_t *path, const wchar_t *valueName) {
    DWORD size{};
    LSTATUS result = RegGetValueW(
        HKEY_LOCAL_MACHINE,
        path,
        valueName,
        RRF_RT_REG_SZ,
        nullptr,
        nullptr,
        &size);
    if (result != ERROR_SUCCESS) {
        return L"";
    }

    std::wstring string{};
    string.resize(size / sizeof(wchar_t) - 2);
    result = RegGetValueW(
        HKEY_LOCAL_MACHINE,
        path,
        valueName,
        RRF_RT_REG_SZ,
        nullptr,
        string.data(),
        &size);

    return string;
}
