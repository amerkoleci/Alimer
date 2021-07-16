// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "GameWindow.h"
#include "Core/Main.h"

namespace Alimer
{
    class GameHost;

    struct GameConfig
    {
        String title = "Alimer";
        uint32_t width = 1200;
        uint32_t height = 800;

        RHIBackendType backendType = RHIBackendType::Count;
#if defined(_DEBUG)
        RHIValidationMode validationMode = RHIValidationMode::Disabled;
#else
        RHIValidationMode validationMode = RHIValidationMode::Enabled;
#endif
    };

	/// Class that provides graphics initialization, game logic, and rendering code.
	class ALIMER_API Game
	{
	public:
		/// Occurs when the game is about to exit.
		Signal<int32_t> Exiting;

		/// Destructor.
		virtual ~Game();

		/// Gets the current Game instance.
		static Game* GetCurrent();

		/// Setups all subsystem and run's platform main loop.
        int32_t Run();

		void Tick();

		/// Request the game to exit.
		void Exit();

        // Gets the config data used to run the application
        const GameConfig& GetConfig() const { return config; }

		/// Checks whether exit was requested.
		[[nodiscard]] bool IsExitRequested() const noexcept { return exiting; }

        [[nodiscard]] GameWindow* GetWindow() const;

	protected:
		/// Constructor.
        Game();

		virtual void Initialize() {}
		virtual void Update();
		virtual void OnDraw([[maybe_unused]] CommandList& commandBuffer);

        virtual void BeginRun() {}
        virtual void EndRun() {}

        virtual bool BeginDraw();
        virtual void EndDraw();

    private:
        void HostReady();
        void HostExiting(int32_t exitCode);

        void InitializeBeforeRun();
        void Render();

	protected:
        GameConfig config{};
        bool headless{ false };
		bool running{ false };
		bool paused{ false };
        bool exiting{ false };
        int32_t exitCode{ 0 };
        bool endRunRequired{ false };

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
