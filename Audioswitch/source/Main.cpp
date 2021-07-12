#include "source/Utility/ComLibraryContext.h"
#include "source/Audio/AudioContext.h"
#include "source/Audio/AudioDevice.h"

int calculateNextDeviceIndex(int currentIndex, size_t size) {
    if (currentIndex < 0) {
        return 0;
    }

    return (currentIndex + 1) % size;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hprev, LPSTR cmdline, int show) {
    ComLibraryContext comLibraryContext{};
    AudioContext audioContext{};

    auto devices = audioContext.enumerateActiveDevices();
    if (devices.size() == 0) {
        return 0;
    }

    auto currentDeviceIndex = audioContext.getDefaultDeviceIndex(devices);
    auto newDeviceIndex = calculateNextDeviceIndex(currentDeviceIndex, devices.size());
    if (currentDeviceIndex != newDeviceIndex) {
        audioContext.setAsDefault(devices[newDeviceIndex]);
    }

    return 0;
}
