#include "TrayIcon.h"

#include <commctrl.h>
#include "source/AudioContext.h"
#include "Common/AssertSuccess.h"
#include "resources/resource.h"

TrayIcon::TrayIcon(HINSTANCE instanceHandle, AudioContext &audioContext)
    : instanceHandle(instanceHandle),
      windowClassName("Audioswitch window class"),
      audioContext(audioContext) {
    WNDCLASSEXA data = {sizeof(data)};
    data.style = CS_HREDRAW | CS_VREDRAW;
    data.lpfnWndProc = windowProc;
    data.hInstance = instanceHandle;
    data.hIcon = LoadIcon(instanceHandle, MAKEINTRESOURCEA(IDI_ICON));
    data.hCursor = LoadCursor(NULL, IDC_ARROW);
    data.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    data.lpszMenuName = NULL;
    data.lpszClassName = windowClassName.c_str();
    ATOM retVal = RegisterClassExA(&data);
    FATAL_ERROR_IF(retVal == 0);

    windowHandle = CreateWindowExA(
        0u,
        windowClassName.c_str(),
        "Audioswitch window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        250,
        200,
        NULL,
        NULL,
        instanceHandle,
        this);
    FATAL_ERROR_IF(windowHandle == nullptr);
}

LRESULT TrayIcon::windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    // Get window instance
    TrayIcon *trayIcon = {};
    if (message == WM_CREATE) {
        trayIcon = reinterpret_cast<TrayIcon *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(trayIcon));
        PostMessage(windowHandle, WMAPP_Init, 0, 0); // Post a new message to do further init. WM_CREATE would be too early.
    } else {
        trayIcon = reinterpret_cast<TrayIcon *>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
    }

    // Handle destruction
    if (message == WM_DESTROY) {
        trayIcon->destroyNotificationIcon();
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, 0);
        ::PostQuitMessage(0);
    }

    // If there is no window, it's been destroyed, ignore
    if (trayIcon == nullptr) {
        return ::DefWindowProcW(windowHandle, message, wParam, lParam);
    }

    // Handle events
    switch (message) {
    case WMAPP_Init:
        // Handle late init of the window
        trayIcon->createNotificationIcon();
        return 0;

    case WMAPP_TrayIconCallback:
        // Handle click on the tray icon
        switch (LOWORD(lParam)) {
        case NIN_SELECT:
            trayIcon->audioContext.cycleToNextDevice();
            return 0;
        case WM_CONTEXTMENU:
            trayIcon->showNotificationContextMenu();
            return 0;
        }
        break;

    case WM_COMMAND: {
        // Handle commands from the tray icon such as context menu clicks
        const auto commandId = LOWORD(wParam);
        if (commandId == WMAPP_CmdQuit) {
            PostMessage(windowHandle, WM_CLOSE, 0, 0);
            return 0;
        }
        if (commandId >= WMAPP_CmdSelectDeviceMin && commandId < WMAPP_CmdSelectDeviceMax) {
            const auto deviceIdx = commandId - WMAPP_CmdSelectDeviceMin;
            if (deviceIdx < trayIcon->contextMenuDevices.size()) {
                trayIcon->audioContext.setDevice(trayIcon->contextMenuDevices[deviceIdx]);
            }
            return 0;
        }
        break;
    }
    }

    return ::DefWindowProcW(windowHandle, message, wParam, lParam);
}

void TrayIcon::createNotificationIcon() {
    NOTIFYICONDATAA nid = {};
    nid.cbSize = sizeof(nid);
    nid.uID = 0u;
    nid.hWnd = windowHandle;
    nid.uFlags = NIF_ICON | NIF_MESSAGE;
    nid.uCallbackMessage = WMAPP_TrayIconCallback;
    strcpy_s(nid.szTip, "Audioswitch tip");
    strcpy_s(nid.szInfoTitle, "Audioswitch title");
    LoadIconMetric(instanceHandle, MAKEINTRESOURCEW(IDI_ICON), LIM_LARGE, &nid.hIcon);

    BOOL success = Shell_NotifyIconA(NIM_ADD, &nid);
    FATAL_ERROR_IF(!success);

    nid.uVersion = NOTIFYICON_VERSION_4;
    success = Shell_NotifyIconA(NIM_SETVERSION, &nid);
    FATAL_ERROR_IF(!success);
}

void TrayIcon::destroyNotificationIcon() {
    NOTIFYICONDATAA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = windowHandle;
    nid.uID = 0u;
    nid.uFlags = NIF_MESSAGE | NIF_ICON;
    nid.uCallbackMessage = WMAPP_TrayIconCallback;
    BOOL success = Shell_NotifyIconA(NIM_DELETE, &nid);
    FATAL_ERROR_IF(!success);
}

void TrayIcon::showNotificationContextMenu() {
    // Create a new popup menu
    HMENU hSubMenu = CreatePopupMenu();
    if (!hSubMenu) {
        return;
    }

    contextMenuDevices = audioContext.enumerateActiveDevices();
    const int defaultDeviceIndex = audioContext.getDefaultDeviceIndex(contextMenuDevices);
    UINT id = WMAPP_CmdSelectDeviceMin;
    for (size_t i = 0; i < contextMenuDevices.size() && id < WMAPP_CmdSelectDeviceMax; i++) {
        std::wstring name = contextMenuDevices[i].getFriendlyName();
        if (i == defaultDeviceIndex) {
            name = L">>>" + name + L"<<<";
        }
        AppendMenuW(hSubMenu, MF_STRING, id++, name.c_str());
    }
    AppendMenuA(hSubMenu, MF_STRING, WMAPP_CmdQuit, "Quit");

    // Our window must be foreground before calling TrackPopupMenu
    SetForegroundWindow(windowHandle);

    // Respect menu drop alignment
    UINT uFlags = TPM_RIGHTBUTTON;
    if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) {
        uFlags |= TPM_RIGHTALIGN;
    } else {
        uFlags |= TPM_LEFTALIGN;
    }

    // The mouse coordinates are passed via lParam in WM_CONTEXTMENU, not wParam
    POINT pt;
    GetCursorPos(&pt);

    // Show the menu
    TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, windowHandle, NULL);

    // Clean up
    DestroyMenu(hSubMenu);
}
