// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Signal.h"
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

	protected:
		/// Constructor.
        GameWindow() = default;
	};
}
