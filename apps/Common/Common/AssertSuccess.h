#pragma once

#include <winerror.h>
#include <exception>
#include <iostream>

#define FATAL_ERROR()                                                \
    std::cerr << "ERROR in " << __FILE__ << ":" << __LINE__ << '\n'; \
    throw std::exception {}

#define FATAL_ERROR_IF(condition) \
    if ((condition)) {            \
        FATAL_ERROR();            \
    }

inline void assertSuccess(HRESULT result) {
    if (!SUCCEEDED(result)) {
        FATAL_ERROR();
    }
}
