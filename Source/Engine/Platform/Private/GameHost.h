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
        [[nodiscard]] static UniquePtr<GameHost> Create(_In_ Game* game);

        virtual ~GameHost() = default;

        GameHost(const GameHost&) = delete;
        GameHost(GameHost&&) = delete;
        GameHost& operator=(const GameHost&) = delete;
        GameHost& operator=(GameHost&&) = delete;

        // Events
        Signal<> Ready;
        Signal<int32_t> Exiting;

        Signal<> Activated;
        Signal<> Deactivated;

        /// Run main loop and return exit code.
        virtual void Run() = 0;
        virtual void Exit() = 0;

        [[nodiscard]] virtual GameWindow* GetMainWindow() const = 0;
        [[nodiscard]] virtual bool IsBlockingRun() const = 0;

	protected:
		/// Constructor.
        GameHost(_In_ Game* game);

        Game* game;
	};
}
