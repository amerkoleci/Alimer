// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "GameWindow.h"
#include "Platform_Win32.h"

namespace Alimer
{
    static constexpr wchar_t kWndClass[] = L"AlimerWindow";

	class ALIMER_API GameWindowWin32 final : public GameWindow
	{
	public:
        GameWindowWin32(_In_ HINSTANCE hInstance, const StringView& title, uint32_t width, uint32_t height);
		~GameWindowWin32() override;

        void Show() override;
        bool IsMinimized() const override;

        void HandleActivate(WPARAM wParam);

    private:
        void ApiSetTitle(const StringView& title) override;

        HWND handle = nullptr;
	};
}
