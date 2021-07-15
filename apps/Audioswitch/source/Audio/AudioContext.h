#pragma once

#include "source/Utility/IPolicyConfigVista.h"

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
    void setAsDefault(const AudioDevice &device);

private:
    Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator{};
    Microsoft::WRL::ComPtr<IPolicyConfigVista> policyConfig{};
};
