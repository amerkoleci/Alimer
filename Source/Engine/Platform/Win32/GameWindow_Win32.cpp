// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "GameWindow_Win32.h"
#include <system_error>

namespace Alimer
{
    GameWindowWin32::GameWindowWin32(_In_ HINSTANCE hInstance, const StringView& title, uint32_t width, uint32_t height)
        : GameWindow(title)
    {
        RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        auto wTitle = ToUtf16(title);
        handle = CreateWindowExW(0, kWndClass,
            wTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rc.right - rc.left, rc.bottom - rc.top,
            nullptr,
            nullptr,
            hInstance,
            nullptr);

        if (!handle)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to create window");

        SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    GameWindowWin32::~GameWindowWin32()
    {
        //RHIWaitForGPU();
        swapChain.Reset();

        if (handle)
        {
            DestroyWindow(handle);
        }
    }

    void GameWindowWin32::Show()
    {
        if (swapChain.IsNull())
        {
            CreateSwapChain(handle);
        }

        ShowWindow(handle, SW_SHOW);
    }

    bool GameWindowWin32::IsMinimized() const
    {
        return ::IsIconic(handle);
    }

    void GameWindowWin32::ApiSetTitle(const StringView& title)
    {
        auto wTitle = ToUtf16(title);
        SetWindowTextW(handle, wTitle.c_str());
    }

    void GameWindowWin32::HandleActivate(WPARAM wParam)
    {
        bool active = wParam != 0;
    }
}
