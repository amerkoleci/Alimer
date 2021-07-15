// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Game.h"
#include "RHI/RHI.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "Math/Color.h"
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
        ALIMER_VERIFY_MSG(g_currentGame == nullptr, "Cannot create more than one Application");

        // Init log first.
        gLog().Start();

        // Initialize host.
        host = GameHost::Create(this);
        host->Ready.ConnectMember(this, &Game::HostReady);
        host->Exiting.ConnectMember(this, &Game::HostExiting);

        g_currentGame = this;
    }

    Game::~Game()
    {
        // Shutdown modules.
        host.reset();
        RHIWaitForGPU();
        RHIShutdown();
        gLog().Shutdown();
        g_currentGame = nullptr;
    }

    Game* Game::GetCurrent()
    {
        return g_currentGame;
    }

    GameWindow* Game::GetWindow() const
    {
        return host->GetMainWindow();
    }

    int Game::Run()
    {
        if (running)
        {
            return EXIT_SUCCESS;
        }

#if !defined(__GNUC__) || __EXCEPTIONS
        try
        {
#endif
            host->Run();

            if (host->IsBlockingRun())
            {
                // If the previous call was blocking, then we can call Endrun
                EndRun();
            }
            else
            {
                // EndRun will be executed on Exit
                endRunRequired = true;
            }

#if !defined(__GNUC__) || __EXCEPTIONS
        }
        catch (std::bad_alloc&)
        {
            //ErrorDialog(GetTypeName(), "An out-of-memory error occurred. The application will now exit.");
            return EXIT_FAILURE;
        }
#endif

        if (!endRunRequired)
        {
            running = false;
        }

        return exitCode;
    }

    void Game::Tick()
    {
        Render();
    }

    void Game::Update()
    {
    }

    void Game::HostReady()
    {
        InitializeBeforeRun();
    }

    void Game::HostExiting(int32_t exitCode_)
    {
        exitCode = exitCode_;
        Exiting.Emit(exitCode_);
    }

    void Game::InitializeBeforeRun()
    {
        // Platform logic has been setup and main window has been created.

        // Init RHI
        ValidationMode validationMode = ValidationMode::Disabled;

#if defined(_DEBUG)
        validationMode = ValidationMode::Enabled;
#endif

        if (!RHIInitialize(validationMode, BackendType::Count))
        {
            headless = true;
        }
        else
        {

        }

        Initialize();

        // Show main window.
        host->GetMainWindow()->Show();
    }

    void Game::Render()
    {
        // Don't try to render anything before the first Update or rendering is not allowed
        if (!exiting &&
            //frameCount > 0 &&
            !host->GetMainWindow()->IsMinimized() &&
            BeginDraw())
        {
            // Custom application draw.
            CommandList commandBuffer = GDevice->BeginCommandList();

            GDevice->BeginRenderPass(commandBuffer, host->GetMainWindow()->GetRHISwapChain(), Colors::CornflowerBlue);
            OnDraw(commandBuffer);
            GDevice->EndRenderPass(commandBuffer);
            EndDraw();
        }
    }

    bool Game::BeginDraw()
    {
        return true; // RHIBeginFrame();
    }

    void Game::EndDraw()
    {
        GDevice->SubmitCommandLists();
        //RHIEndFrame();
    }

    void Game::OnDraw(_In_ CommandList& commandBuffer)
    {


    }

    void Game::Exit()
    {
        exiting = true;
        host->Exit();
        if (running && endRunRequired)
        {
            EndRun();
            running = false;
        }
    }
}
