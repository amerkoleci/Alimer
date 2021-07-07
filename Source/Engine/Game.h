// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "GameWindow.h"
#include "Core/Main.h"

namespace Alimer
{
    class GameHost;

	/// Class that provides graphics initialization, game logic, and rendering code.
	class ALIMER_API Game
	{
	public:
		/// Occurs when the game is about to eit.
		Signal<int32_t> Exit;

		/// Destructor.
		virtual ~Game();

		/// Gets the current Game instance.
		static Game* GetCurrent();

		/// Setups all subsystem and run's platform main loop.
		int Run();

		void Tick();

		/// Request the application to exit.
		void RequestExit(int exitCode = 0);

		/// Checks whether exit was requested.
		[[nodiscard]] bool IsExitRequested() const noexcept { return exiting; }

		//[[nodiscard]] Window* GetMainWindow() const { return mainWindow; }

	protected:
		/// Constructor.
        Game();

		virtual void Initialize() {}
		virtual void Update();
		virtual void OnDraw();

		virtual void OnExit(int32_t exitCode);

    private:
        void Render();

	protected:
		bool running{ false };
		bool paused{ false };
		bool exiting = false;
		bool headless = false;
        int exitCode{ 0 };

    private:
        /// OS game host.
        UniquePtr<GameHost> host;
	};
}

#if !defined(ALIMER_DEFINE_GAME)
#define ALIMER_DEFINE_GAME(className) \
int RunGame() \
{ \
    std::unique_ptr<className> game = std::make_unique<className>();\
    return game->Run(); \
} \
ALIMER_DEFINE_MAIN(RunGame());
#endif
