// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Signal.h"
#include "Graphics/SwapChain.h"
#include <memory>

namespace Alimer
{
	/// Defines OS game window.
	class ALIMER_API GameWindow
	{
	public:
        virtual ~GameWindow() = default;

        GameWindow(const GameWindow&) = delete;
        GameWindow(GameWindow&&) = delete;
        GameWindow& operator=(const GameWindow&) = delete;
        GameWindow& operator=(GameWindow&&) = delete;

        String GetTitle() const noexcept { return title; }
        void SetTitle(const String& title);
        void SetTitle(const StringView& title);

        virtual void Show() = 0;
        virtual bool IsMinimized() const = 0;

        const SwapChain* GetSwapChain() const { return swapChain.Get(); }

	protected:
		/// Constructor.
        GameWindow(const StringView& title);

        virtual void ApiSetTitle(const StringView& title) = 0;

        void CreateSwapChain(void* windowHandle);

        String title;
        SwapChainRef swapChain;
	};
}
