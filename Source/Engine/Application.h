// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "AlimerConfig.h"
#include "Core/Types.h"
#include <memory>

namespace Alimer
{
	class ALIMER_API Application
	{
	public:
		/// Occurs when the application is about to eit.
		//Signal<int32_t> Exit;

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

#if defined(_MSC_VER) && !defined(ALIMER_WIN32_CONSOLE)
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define ALIMER_DEFINE_APPLICATION(className) \
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) \
{ \
	UNREFERENCED_PARAMETER(hPrevInstance);\
	UNREFERENCED_PARAMETER(lpCmdLine);\
	UNREFERENCED_PARAMETER(nCmdShow);\
	Alimer::ParseArguments(GetCommandLineW());\
	std::unique_ptr<className> application = std::make_unique<className>();\
	application->Run(); \
	return 0; \
}
#else
#define ALIMER_DEFINE_APPLICATION(className) \
int main(int argc, const char* argv[]) \
{ \
	Alimer::Platform::ParseArguments(argc, argv); \
	std::unique_ptr<className> application = std::make_unique<className>();\
	application->Run(argc, argv); \
	return 0; \
}
#endif
