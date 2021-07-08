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

class RHICommandBufferD3D12 final : public RHICommandBuffer
{
private:
    const D3D12_COMMAND_LIST_TYPE& type;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[kRHIMaxFramesInFlight];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList;

public:
    RHICommandBufferD3D12(_In_ ID3D12Device5* device, D3D12_COMMAND_LIST_TYPE type);
    ~RHICommandBufferD3D12() override;
};

class RHIDeviceD3D12 final : public RHIDevice
{
public:
    [[nodiscard]] static bool IsAvailable();

    RHIDeviceD3D12(RHIValidationMode validationMode);

    bool Initialize(RHIValidationMode validationMode) override;
    void Shutdown() override;
    bool BeginFrame() override;
    void EndFrame() override;

    RHICommandBuffer* BeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics) override;
    bool CreateSwapChain(void* window, const RHISwapChainDescriptor* descriptor, RHISwapChain* pSwapChain) const override;

    auto GetDXGIFactory() const noexcept { return dxgiFactory.Get(); }
    auto GetD3DDevice() const noexcept { return d3dDevice.Get(); }
    auto GetAllocator() const noexcept { return allocator; }

private:
    DWORD dxgiFactoryFlags = 0;
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    bool tearingSupported = false;

    Microsoft::WRL::ComPtr<ID3D12Device5> d3dDevice;
    D3D12MA::Allocator* allocator = nullptr;

    struct CommandQueue
    {
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> handle;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        //ID3D12CommandList* submit_cmds[COMMANDLIST_COUNT] = {};
        //uint32_t submit_count = 0;
    } queues[(uint32_t)RHIQueueType::Count];

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;
};

#endif /* defined(ALIMER_RHI_D3D12) */
