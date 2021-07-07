// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include <stdexcept>

#if defined(CopyFile)
#   undef CopyFile
#endif

#if defined(LoadLibrary)
#   undef LoadLibrary
#endif

namespace Alimer
{
    // Helper class for COM exceptions
    class COMException : public std::exception
    {
    public:
        COMException(HRESULT hr) noexcept : result(hr) {}

        const char* what() const noexcept override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw COMException(hr);
        }
    }

    template <typename T>
    void SafeRelease(T& resource)
    {
        if (resource)
        {
            resource->Release();
            resource = nullptr;
        }
    }

    struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };
    using ScopedHandle = std::unique_ptr<void, handle_closer>;
}
