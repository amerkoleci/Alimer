// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Signal.h"
#include "Core/Main.h"
#include <memory>

namespace Alimer
{
	class ALIMER_API Application
	{
	public:
		/// Occurs when the application is about to eit.
		Signal<int32_t> Exit;

		/// Destructor.
		virtual ~Application();

		/// Gets the current Application instance.
		static Application* GetCurrent();

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
		Application();

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
	};
}

#if !defined(ALIMER_DEFINE_APPLICATION)
#define ALIMER_DEFINE_APPLICATION(className) \
int RunApplication() \
{ \
    std::unique_ptr<className> application= std::make_unique<className>();\
    return application->Run(); \
} \
ALIMER_DEFINE_MAIN(RunApplication());
#endif
