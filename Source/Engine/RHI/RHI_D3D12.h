// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "AlimerConfig.h"
#if defined(ALIMER_RHI_D3D12)
#include "RHI/RHI.h"

#include "PlatformInclude.h"
#include <wrl/client.h>
#include <dxgi1_6.h>
#include "directx/d3d12.h"
#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include "D3D12MemAlloc.h"

#ifdef _DEBUG
#   include <dxgidebug.h>
#endif

namespace Alimer
{
    using ID3D12DeviceX = ID3D12Device5;

    class RHIDeviceD3D12 final : public RHIDevice
    {
    public:
        [[nodiscard]] static bool IsAvailable();

        RHIDeviceD3D12(RHIValidationMode validationMode);

        bool Initialize(RHIValidationMode validationMode) override;
        void Shutdown() override;
        bool BeginFrame() override;
        void EndFrame() override;
        bool CreateSwapChain(void* window, const RHUSwapChainDescriptor* descriptor, RHISwapChain* swapChain) const override;

        auto GetDXGIFactory() const noexcept { return dxgiFactory.Get(); }
        auto GetD3DDevice() const noexcept { return d3dDevice.Get(); }
        auto GetAllocator() const noexcept { return allocator; }

    private:
        DWORD dxgiFactoryFlags = 0;
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
        bool tearingSupported = false;

        Microsoft::WRL::ComPtr<ID3D12DeviceX> d3dDevice;
        D3D12MA::Allocator* allocator = nullptr;

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;
    };
}

#endif /* defined(ALIMER_RHI_D3D12) */
