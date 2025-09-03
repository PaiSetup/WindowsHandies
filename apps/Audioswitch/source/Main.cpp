#include "Common/ComLibraryContext.h"
#include "Common/SingleAppInstanceContext.h"
#include "source/AudioContext.h"
#include "source/TrayIcon.h"

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hprev, LPSTR cmdline, int show) {
    SingleAppInstanceContext singleAppInstanceContext("Audioswitch");
    if (!singleAppInstanceContext.isValid()) {
        std::cout << "Another instance is already running.\n";
        return 0;
    }

    ComLibraryContext comLibraryContext{};
    AudioContext audioContext{};

    TrayIcon trayIcon(hInstance, audioContext);
    MSG message{};
    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}
