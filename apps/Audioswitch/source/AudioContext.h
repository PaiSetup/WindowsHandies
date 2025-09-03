#pragma once

#include "source/IPolicyConfigVista.h"

#include <Objbase.h>
#include <vector>
#include <wrl.h>
#include <mmdeviceapi.h>

class AudioDevice;

class AudioContext {
public:
    AudioContext();
    std::vector<AudioDevice> enumerateActiveDevices();
    int getDefaultDeviceIndex(const std::vector<AudioDevice> &devices);
    void setDevice(const AudioDevice &device);
    void cycleToNextDevice();

private:
    static int calculateNextDeviceIndex(int currentIndex, size_t size);

    Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator{};
    Microsoft::WRL::ComPtr<IPolicyConfigVista> policyConfig{};
};
