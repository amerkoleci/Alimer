// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Application.h"

namespace Alimer
{
    namespace
    {
#ifdef __EMSCRIPTEN__
        void MainLoop(void* arg)
        {
            static_cast<Application*>(arg)->Tick();
        }
#endif
    }

    static Application* s_currentApplication = nullptr;

    Application::Application()
    {
        //ALIMER_VERIFY_MSG(s_currentApplication == nullptr, "Cannot create more than one Application");

        // Initialize platform system first
        //PlatformConstruct();

        s_currentApplication = this;
    }

    Application::~Application()
    {
        // Shutdown modules.
        //PlatformShutdown();
        //gLog().Shutdown();
        s_currentApplication = nullptr;
    }

    Application* Application::GetCurrent()
    {
        return s_currentApplication;
    }

    void Application::RequestExit(int exitCode)
    {
        exiting = true;
        paused = true;

        if (running)
        {
            OnExit(exitCode);

            running = false;
        }
    }

    int Application::Run()
    {
        if (running)
        {
            return EXIT_SUCCESS;
        }

        running = false;
        return exitCode;
    }

    void Application::Tick()
    {
        
    }

    void Application::Update()
    {
    }

    void Application::Render()
    {
        // Don't try to render anything before the first Update or rendering is not allowed
        if (exiting/* ||
            frameCount == 0 ||
            mainWindow->IsMinimized() ||
            gGraphics().IsDeviceLost()*/)
        {
            return;
        }

        // Custom application draw.
        OnDraw();
    }

    void Application::OnDraw()
    {


    }

    void Application::OnExit(int32_t exitCode)
    {
        //Exit.Emit(exitCode);
    }

    Application& gApplication()
    {
        return *s_currentApplication;
    }
}
