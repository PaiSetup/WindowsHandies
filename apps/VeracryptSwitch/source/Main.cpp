#include "Common/Registry.h"
#include "Common/ArgumentParser.h"
#include "Common/Filesystem.h"
#include <sstream>
#include <iostream>

std::fs::path findVeracryptPath() {
    const std::wstring appId = RegistryHelper::readString(RegistryHelper::Hive::HKLM, L"SOFTWARE\\Classes\\AppID\\VeraCrypt.exe", L"AppId");

    std::wostringstream appPathRegistry{};
    appPathRegistry << L"SOFTWARE\\Classes\\WOW6432Node\\CLSID\\" << appId.c_str() << L"\\LocalServer32";
    std::wstring appPath = RegistryHelper::readString(RegistryHelper::Hive::HKLM, appPathRegistry.str().c_str(), L"");
    appPath = appPath.substr(1, appPath.size() - 2);

    return appPath;
}

bool isMounted(wchar_t driveLetter) {
    std::fs::path drivePath = std::wstring(1, driveLetter) + L":";
    return std::fs::exists(drivePath);
}

void callCommand(const std::wstring &command) {
    std::wcout << "Running command: " << command << '\n';

    int result = _wsystem(command.c_str());
    if (result == 0) {
        std::wcout << L"Success\n";
    } else {
        std::wcout << L"Command failed (_wsystem returned " << result << ")\n";
    }
}

void mount(const std::fs::path veracryptPath, wchar_t driveLetter, const std::fs::path &imagePath) {
    std::wostringstream commandStream{};
    commandStream << L"\""
                  << L"\"" << veracryptPath.c_str() << L"\""
                  << L" /volume " << imagePath
                  << L" /letter " << driveLetter
                  << " /explore /quit"
                  << L"\"";
    std::wcout << "Mounting " << imagePath << " at " << driveLetter << ":\n";
    callCommand(commandStream.str());
}

void unmount(const std::fs::path veracryptPath, wchar_t driveLetter) {
    std::wostringstream commandStream{};
    commandStream << L"\""
                  << L"\"" << veracryptPath.c_str() << L"\""
                  << L""
                  << L" /dismount " << driveLetter
                  << " /quit"
                  << L"\"";
    std::wcout << "Unmounting " << driveLetter << ":\n";
    callCommand(commandStream.str());
}

void printHelp() {
    std::wcout
        << "VeracryptSwitch\n"
        << "\n"
        << "Programatically switches between mounted and unmounted state for a given VeraCrypt image:\n"
        << "  -d <driveLetter>\n"
        << "  -i <imagePath>\n"
        << "\n";
}

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser parser{argc, argv};
    const auto driveLetter = parser.getArgumentValue<wchar_t>("-d", L'\0');
    const auto imagePath = parser.getArgumentValue<std::fs::path>("-i", {});

    // Validate arguments
    if (!isalpha(driveLetter) || !std::fs::exists(imagePath)) {
        printHelp();
        return 1;
    }

    // Find Veracrypt
    const std::fs::path veracryptPath = findVeracryptPath();
    if (!std::fs::is_regular_file(veracryptPath)) {
        std::wcerr << "Cannot find Veracrypt exectuable\n";
        return 1;
    }

    // Execute
    if (isMounted(driveLetter)) {
        unmount(veracryptPath, driveLetter);
    } else {
        mount(veracryptPath, driveLetter, imagePath);
    }
}
