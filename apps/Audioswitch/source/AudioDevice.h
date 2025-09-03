#pragma once

#include <mmdeviceapi.h>

class AudioDevice {
public:
    AudioDevice(IMMDevice *device);

    LPWSTR getId() const;
    LPWSTR getFriendlyName() const;

private:
    LPWSTR id;
    PROPVARIANT friendlyName;
};
