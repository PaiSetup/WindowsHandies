#include "AudioDevice.h"

#include "source/Utility/AssertSuccess.h"

#include <wrl.h>
#include <Functiondiscoverykeys_devpkey.h>

AudioDevice::AudioDevice(IMMDevice *device) {
    assertSuccess(device->GetId(&id));

    Microsoft::WRL::ComPtr<IPropertyStore> pPropertyStore;
    assertSuccess(device->OpenPropertyStore(STGM_READ, &pPropertyStore));
    assertSuccess(pPropertyStore->GetValue(::PKEY_Device_FriendlyName, &friendlyName));
}

LPWSTR AudioDevice::getId() const {
    return id;
}

LPWSTR AudioDevice::getFriendlyName() const {
    return friendlyName.pwszVal;
}
