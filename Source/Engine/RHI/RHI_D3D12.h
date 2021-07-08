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

#include <mutex>

class RHICommandBufferD3D12 final : public RHICommandBuffer
{
public:
    RHICommandBufferD3D12(_In_ ID3D12Device5* device, RHIQueueType queueType);
    ~RHICommandBufferD3D12() override;

    void Reset(uint32_t frameIndex);
    void PresentSwapChains();
    void FlushQueries();
    void FlushBarriers();

    void BeginRenderPass(const RHISwapChain* swapChain) override;
    void EndRenderPass() override;

    auto GetHandle() const noexcept { return commandList; }

private:
    RHIQueueType queueType;

    ID3D12CommandAllocator* commandAllocators[kRHIMaxFramesInFlight] = {};
    ID3D12GraphicsCommandList6* commandList = nullptr;

    std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
    std::vector<const RHISwapChain*> swapChains;
};

class RHIDeviceD3D12 final : public RHIDevice
{
public:
    [[nodiscard]] static bool IsAvailable();

    RHIDeviceD3D12(RHIValidationMode validationMode);

    bool Initialize(RHIValidationMode validationMode) override;
    void Shutdown() override;

    void WaitForIdle() override;
    bool BeginFrame() override;
    void EndFrame() override;
    void SubmitCommandBuffers();

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

    /* Caps */
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;

    struct CommandQueue
    {
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> handle;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        ID3D12CommandList* submitCommandLists[kRHIMaxFrameCommandBuffers] = {};
        uint32_t submitCount = 0;
    } queues[(uint32_t)RHIQueueType::Count];

    std::atomic_uint32_t commandBuffersCount{ 0 };
    std::unique_ptr<RHICommandBufferD3D12> commandBuffers[kRHIMaxFrameCommandBuffers][(uint32_t)RHIQueueType::Count];
    struct CommandListMetadata
    {
        RHIQueueType queueType = {};
        std::vector<uint32_t> waits;
    } commandBuffersMeta[(uint32_t)RHIQueueType::Count];

    inline RHICommandBufferD3D12* GetCommandBuffer(uint32_t index)
    {
        return commandBuffers[index][(uint32_t)commandBuffersMeta[index].queueType].get();
    }

    Microsoft::WRL::ComPtr<ID3D12Fence> frameFences[kRHIMaxFramesInFlight][(uint32_t)RHIQueueType::Count];

    struct CopyAllocator
    {
        RHIDeviceD3D12* device = nullptr;
        ID3D12CommandQueue* queue = nullptr;
        std::mutex locker;

        struct CopyCMD
        {
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
            Microsoft::WRL::ComPtr<ID3D12Fence> fence;
            //GPUBuffer uploadbuffer;
            //void* data = nullptr;
            //ID3D12Resource* upload_resource = nullptr;
        };
        std::vector<CopyCMD> freelist;

        void Initialize(RHIDeviceD3D12* device);
        void Shutdown();

        CopyCMD Allocate(uint64_t size);
        void Submit(CopyCMD cmd);
    };
    mutable CopyAllocator copyAllocator;
};

#endif /* defined(ALIMER_RHI_D3D12) */
