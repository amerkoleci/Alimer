// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "GameWindow.h"
#include "Platform_Win32.h"

namespace Alimer
{
	class ALIMER_API GameWindowWin32 final : public GameWindow
	{
	public:
		~GameWindowWin32() override;

    private:
        HWND handle = nullptr;
	};
}
