#include <string>

struct RegistryHelper {
    enum class Hive {
        HKLM,
        HKCU,
    };

    static std::wstring readString(Hive hive, const wchar_t *path, const wchar_t *valueName);
};
