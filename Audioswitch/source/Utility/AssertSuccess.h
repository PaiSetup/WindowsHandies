#pragma once

#include <winerror.h>
#include <exception>

inline void assertSuccess(HRESULT result) {
    if (!SUCCEEDED(result)) {
        throw std::exception{};
    }
}
