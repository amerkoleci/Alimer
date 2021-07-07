// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Platform/Private/GameHost.h"
#include "GameWindow_Win32.h"

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace Alimer
{
    class ALIMER_API GameHostWin32 final : public GameHost
    {
    public:
        GameHostWin32(_In_ Game* game);
        ~GameHostWin32() override;

    private:
        HINSTANCE handle = nullptr;
    };

    GameHostWin32::GameHostWin32(_In_ Game* game)
        : GameHost(game)
    {

    }

    GameHostWin32::~GameHostWin32()
    {

    }

    UniquePtr<GameHost> GameHost::Create(_In_ Game* game)
    {
        return std::make_unique<GameHostWin32>(game);
    }
}
