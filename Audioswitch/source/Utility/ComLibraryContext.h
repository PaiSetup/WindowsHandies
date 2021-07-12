#pragma once

#include "source/Utility/AssertSuccess.h"

#include <Objbase.h>

struct ComLibraryContext {
    ComLibraryContext() {
        assertSuccess(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
    }

    ~ComLibraryContext() {
        CoUninitialize();
    }
};
