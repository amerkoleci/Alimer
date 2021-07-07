// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Game.h"
#include "Platform/Private/GameHost.h"

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

    static Game* g_currentGame = nullptr;

    Game::Game()
    {
        //ALIMER_VERIFY_MSG(g_currentGame == nullptr, "Cannot create more than one Application");

        // Initialize host first.
        host = GameHost::Create(this);

        g_currentGame = this;
    }

    Game::~Game()
    {
        // Shutdown modules.
        //PlatformShutdown();
        //gLog().Shutdown();
        g_currentGame = nullptr;
    }

    Game* Game::GetCurrent()
    {
        return g_currentGame;
    }

    void Game::RequestExit(int exitCode)
    {
        exiting = true;
        paused = true;

        if (running)
        {
            OnExit(exitCode);

            running = false;
        }
    }

    int Game::Run()
    {
        if (running)
        {
            return EXIT_SUCCESS;
        }

        running = false;
        return exitCode;
    }

    void Game::Tick()
    {
        
    }

    void Game::Update()
    {
    }

    void Game::Render()
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

    void Game::OnDraw()
    {


    }

    void Game::OnExit(int32_t exitCode)
    {
        //Exit.Emit(exitCode);
    }
}
