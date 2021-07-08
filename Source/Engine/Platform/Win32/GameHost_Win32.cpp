// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Platform/Private/GameHost.h"
#include "GameWindow_Win32.h"
#include <combaseapi.h>
#include <system_error>

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace Alimer
{
    
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    class ALIMER_API GameHostWin32 final : public GameHost
    {
    public:
        GameHostWin32(_In_ Game * game);
        ~GameHostWin32() override;

        void Run() override;
        void Exit() override;
        GameWindow* GetMainWindow() const override;
        bool IsBlockingRun() const override;

    private:
        HINSTANCE hInstance{ nullptr };
        UniquePtr<GameWindowWin32> mainWindow;
        bool exitRequested{ false };
    };

    GameHostWin32::GameHostWin32(_In_ Game* game)
        : GameHost(game)
    {
        HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
        if (FAILED(hr))
            return;

        hInstance = GetModuleHandleW(nullptr);
        if (!hInstance)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");

        // Register class
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIconW(hInstance, L"IDI_APPLICATION");
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszClassName = kWndClass;
        wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassExW(&wcex))
            return;

        // Create main window.
        mainWindow = std::make_unique<GameWindowWin32>(hInstance, "Alimer", 1200, 800);
    }

    GameHostWin32::~GameHostWin32()
    {
        CoUninitialize();
    }

    void GameHostWin32::Run()
    {
        Ready.Emit();

        // Main message loop
        MSG msg = {};
        while (WM_QUIT != msg.message)
        {
            if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            else
            {
                game->Tick();
            }
        }

        // Call exit callback
        Exiting.Emit(static_cast<int>(msg.wParam));
    }

    void GameHostWin32::Exit()
    {
        exitRequested = true;
    }

    GameWindow* GameHostWin32::GetMainWindow() const
    {
        return mainWindow.get();
    }

    bool GameHostWin32::IsBlockingRun() const
    {
        return true;
    }

    // Windows procedure
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto window = reinterpret_cast<GameWindowWin32*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

        switch (message)
        {
            case WM_DESTROY:
                PostQuitMessage(0);
                break;

            case WM_PAINT:
                //if (s_in_sizemove && game)
                //{
                //    game->Tick();
                //}
                //else
                {
                    PAINTSTRUCT ps;
                    (void)BeginPaint(hWnd, &ps);
                    EndPaint(hWnd, &ps);
                }
                break;
            case WM_GETMINMAXINFO:
                if (lParam)
                {
                    auto info = reinterpret_cast<MINMAXINFO*>(lParam);
                    info->ptMinTrackSize.x = 320;
                    info->ptMinTrackSize.y = 200;
                }
                break;

            case WM_ACTIVATEAPP:
                if (window)
                {
                    window->HandleActivate(wParam);
                //    if (wParam)
                //    {
                //        game->OnActivated();
                //    }
                //    else
                //    {
                //        game->OnDeactivated();
                //    }
                }
                break;

            case WM_MENUCHAR:
                // A menu is active and the user presses a key that does not correspond
                // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
                return MAKELRESULT(0, MNC_CLOSE);
        }

        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    UniquePtr<GameHost> GameHost::Create(_In_ Game* game)
    {
        return std::make_unique<GameHostWin32>(game);
    }
}
