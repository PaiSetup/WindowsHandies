#pragma once

#include "Common/AssertSuccess.h"

#include <Objbase.h>

class SingleAppInstanceContext {
public:
    SingleAppInstanceContext(const char *name) {
        std::string mutexName = std::string("WindowsHandies") + name;

        hMutex = CreateMutexA(NULL, TRUE, mutexName.c_str());

        if (hMutex == nullptr) {
            return;
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            CloseHandle(hMutex);
            hMutex = nullptr;
        }
    }

    ~SingleAppInstanceContext() {
        CloseHandle(hMutex);
    }

    bool isValid() const {
        return hMutex != NULL;
    }

private:
    HANDLE hMutex = NULL;
};
