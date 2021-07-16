// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "GameWindow.h"
#include "Graphics/Graphics.h"

namespace Alimer
{
    GameWindow::GameWindow(const StringView& title)
        : title{ title }
    {

    }

    void GameWindow::SetTitle(const String& title_)
    {
        title = title_;
        ApiSetTitle(title);
    }

    void GameWindow::SetTitle(const StringView& title_)
    {
        title = title_;
        ApiSetTitle(title_);
    }

    void GameWindow::CreateSwapChain(void* windowHandle)
    {
        SwapChainCreateInfo swapChainDesc{};
        //swapChainDesc.colorFormat = PixelFormat::BGRA8UNorm;
        swapChain = SwapChain::Create(windowHandle, swapChainDesc);
        ALIMER_ASSERT(swapChain.IsNotNull());
    }
}
