#pragma once
#include <intsafe.h>

#include "debug_console.h"

#define ComPtrIDAddr(type) IID_PPV_ARGS(&type)
#define ComPtrAddr(type) (&type)
#define ComPtrVoid(type) (void**)(&type)


extern std::array<wchar_t, 1000> errorTextBuf;

inline bool DXSuccess(const HRESULT& result)
{
    if(result < 0)
    {
        int msgLen = FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            result,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)errorTextBuf.data(),
            1000, NULL);
        Debug::log << "DX Error : " << std::wstring(errorTextBuf.begin(), errorTextBuf.begin() + msgLen);
    }
    return result >= 0;
}
inline void DXAssert(const HRESULT& result)
{
    if (result < 0)
    {
        int msgLen = FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            result,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)errorTextBuf.data(),
            1000, NULL);
        throw std::exception((std::string("DX Error : ") + std::to_string(std::wstring(errorTextBuf.begin(), errorTextBuf.begin() + msgLen))).data());
    }
}