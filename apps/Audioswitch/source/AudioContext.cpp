#include "AudioContext.h"

#include "source/AudioDevice.h"
#include "Common/AssertSuccess.h"

AudioContext::AudioContext() {
    assertSuccess(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumerator)));
    assertSuccess(CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, IID_PPV_ARGS(&policyConfig)));
}

std::vector<AudioDevice> AudioContext::enumerateActiveDevices() {
    Microsoft::WRL::ComPtr<IMMDeviceCollection> collection{};
    enumerator->EnumAudioEndpoints(::eRender, DEVICE_STATE_ACTIVE, &collection);

    UINT deviceCount;
    collection->GetCount(&deviceCount);
    std::vector<AudioDevice> devices{};

    for (UINT i = 0; i < deviceCount; i++) {
        Microsoft::WRL::ComPtr<IMMDevice> device;
        collection->Item(i, &device);
        devices.emplace_back(device.Get());
    }

    std::vector<LPWSTR> ids{};
    for (auto &d : devices) {
        ids.push_back(d.getId());
    }

    return devices;
}

int AudioContext::getDefaultDeviceIndex(const std::vector<AudioDevice> &devices) {
    LPWSTR defaultDeviceId{};
    Microsoft::WRL::ComPtr<IMMDevice> defaultDevice;
    enumerator->GetDefaultAudioEndpoint(::eRender, ::eMultimedia, &defaultDevice);
    assertSuccess(defaultDevice->GetId(&defaultDeviceId));

    for (size_t deviceIndex = 0; deviceIndex < devices.size(); deviceIndex++) {
        if (lstrcmpW(defaultDeviceId, devices[deviceIndex].getId()) == 0) {
            return static_cast<int>(deviceIndex);
        }
    }
    return -1;
}

void AudioContext::setDevice(const AudioDevice &device) {
    assertSuccess(policyConfig->SetDefaultEndpoint(device.getId(), eMultimedia));
    assertSuccess(policyConfig->SetDefaultEndpoint(device.getId(), eCommunications));
}

void AudioContext::cycleToNextDevice() {
    auto devices = enumerateActiveDevices();
    if (devices.size() == 0) {
        return;
    }

    auto currentDeviceIndex = getDefaultDeviceIndex(devices);
    auto newDeviceIndex = calculateNextDeviceIndex(currentDeviceIndex, devices.size());
    setDevice(devices[newDeviceIndex]);
}

int AudioContext::calculateNextDeviceIndex(int currentIndex, size_t size) {
    if (currentIndex < 0) {
        return 0;
    }

    return (currentIndex + 1) % size;
}
