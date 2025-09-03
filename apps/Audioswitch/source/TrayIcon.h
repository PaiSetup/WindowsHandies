#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "source/AudioDevice.h"

class AudioContext;

class TrayIcon {
public:
    constexpr static UINT WMAPP_Init = WM_APP + 1;
    constexpr static UINT WMAPP_TrayIconCallback = WM_APP + 2;
    constexpr static UINT WMAPP_CmdQuit = WM_APP + 3;
    constexpr static UINT WMAPP_CmdSelectDeviceMin = WM_APP + 4;
    constexpr static UINT WMAPP_CmdSelectDeviceMax = WMAPP_CmdSelectDeviceMin + 8;

    TrayIcon(HINSTANCE instanceHandle, AudioContext &audioContext);

    static LRESULT windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

    void createNotificationIcon();
    void destroyNotificationIcon();
    void showNotificationContextMenu();

private:
    const HINSTANCE instanceHandle;
    const std::string windowClassName;
    AudioContext &audioContext;
    HWND windowHandle = nullptr;

    std::vector<AudioDevice> contextMenuDevices = {};
};
