// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Game.h"

namespace Alimer
{
    class Game;

	/// Defines OS game host which manages main loop and window creation.
	class ALIMER_API GameHost
	{
	public:
        virtual ~GameHost() = default;

        GameHost(const GameHost&) = delete;
        GameHost(GameHost&&) = delete;
        GameHost& operator=(const GameHost&) = delete;
        GameHost& operator=(GameHost&&) = delete;

        [[nodiscard]] static UniquePtr<GameHost> Create(_In_ Game* game);

	protected:
		/// Constructor.
        GameHost(_In_ Game* game);

        Game* game;
	};
}
