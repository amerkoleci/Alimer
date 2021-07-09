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

#include <deque>

namespace Alimer
{
    class RHIDeviceD3D12;

    class RHITextureD3D12 final : public RHITexture
    {
    public:
        RHITextureD3D12(RHIDeviceD3D12* device, const RHITextureDescriptor& descriptor, ID3D12Resource* externalResource = nullptr);
        ~RHITextureD3D12() override;
        void Destroy() override;

        ID3D12Resource* GetHandle() const { return handle; }
        D3D12_RESOURCE_STATES GetState() const { return state; }

    private:
        void ApiSetName(const StringView& name) override;
        RHITextureView* CreateView(const RHITextureViewDescriptor& descriptor) const override;

        RHIDeviceD3D12* device = nullptr;
        ID3D12Resource* handle = nullptr;
        D3D12MA::Allocation* allocation = nullptr;
        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
    };

    class RHITextureViewD3D12 final : public RHITextureView
    {
    public:
        RHITextureViewD3D12(RHIDeviceD3D12* device, const RHITextureD3D12* resource, const RHITextureViewDescriptor& descriptor);
        ~RHITextureViewD3D12() override;

        const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV() const { return rtvHandle; }

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE CreateRTV(const RHITextureD3D12* resource) const;

        RHIDeviceD3D12* device;

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
    };

    class RHISwapChainD3D12 final : public RHISwapChain
    {
    public:
        RHISwapChainD3D12(RHIDeviceD3D12* device, void* window, const RHISwapChainDescriptor& descriptor);
        ~RHISwapChainD3D12() override;
        void Destroy() override;

        void Present();

        IDXGISwapChain3* GetHandle() const { return handle; }

    private:
        void AfterReset();
        void ApiSetName(const StringView& name) override;
        RHITextureView* GetCurrentTextureView() const override;

        RHIDeviceD3D12* device;
        IDXGISwapChain3* handle = nullptr;
        std::vector<SharedPtr<RHITextureD3D12>> backBuffers;
        std::vector<RHITextureView*> backBufferViews;
        uint32_t syncInterval = 1;
        uint32_t presentFlags = 0;
    };

    class RHICommandBufferD3D12 final : public RHICommandBuffer
    {
    public:
        RHICommandBufferD3D12(_In_ ID3D12Device5* device, RHIQueueType queueType);
        ~RHICommandBufferD3D12() override;

        void Reset(uint32_t frameIndex);
        void PresentSwapChains();
        void FlushQueries();
        void FlushBarriers();

        void BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor) override;
        void EndRenderPass() override;

        auto GetHandle() const noexcept { return commandList; }

    private:
        RHIQueueType queueType;

        ID3D12CommandAllocator* commandAllocators[kRHIMaxFramesInFlight] = {};
        ID3D12GraphicsCommandList6* commandList = nullptr;

        std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
        std::vector<RHISwapChain*> swapChains;
        const RHITextureD3D12* swapChainTexture = nullptr;
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
        RHITextureRef CreateTexture(const RHITextureDescriptor& descriptor) override;
        RHISwapChainRef CreateSwapChain(void* window, const RHISwapChainDescriptor& descriptor) override;

        D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);
        void FreeDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE handle);
        uint32_t GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;
        void DeferDestroy(IUnknown* resource, D3D12MA::Allocation* allocation = nullptr);

        auto GetDXGIFactory() const noexcept { return dxgiFactory.Get(); }
        bool IsTearingSupported() const noexcept { return tearingSupported; }
        auto GetD3DDevice() const noexcept { return d3dDevice.Get(); }
        auto GetAllocator() const noexcept { return allocator; }
        auto GetGraphicsQueue() const noexcept { return queues[(uint32_t)RHIQueueType::Graphics].handle.Get(); }
        auto GetComputeQueue() const noexcept { return queues[(uint32_t)RHIQueueType::Compute].handle.Get(); }

    private:
        void ProcessDeletionQueue();

        DWORD dxgiFactoryFlags = 0;
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
        bool tearingSupported = false;

        Microsoft::WRL::ComPtr<ID3D12Device5> d3dDevice;
        D3D12MA::Allocator* allocator = nullptr;

        /* Caps */
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;

        bool shuttingDown{ false };
        std::mutex destroyMutex;
        std::deque<std::pair<D3D12MA::Allocation*, uint64_t>> deferredAllocations;
        std::deque<std::pair<IUnknown*, uint64_t>> deferredReleases;
        std::deque<std::pair<int, uint64_t>> destroyerBindlessResource;
        std::deque<std::pair<int, uint64_t>> destroyerBindlessSampler;

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

        struct DescriptorAllocator
        {
            RHIDeviceD3D12* device = nullptr;
            uint32_t descriptorSize = 0;

            std::mutex locker;
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            std::vector<ID3D12DescriptorHeap*> heaps;
            std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> freeList;

            void Initialize(RHIDeviceD3D12* device_, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
            {
                device = device_;
                descriptorSize = device->d3dDevice->GetDescriptorHandleIncrementSize(type);

                desc.Type = type;
                desc.NumDescriptors = numDescriptors;
            }

            void Shutdown()
            {
                for (auto heap : heaps)
                {
                    heap->Release();
                }
                heaps.clear();
            }

            void BlockAllocate()
            {
                heaps.emplace_back();
                ThrowIfFailed(device->d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heaps.back())));

                D3D12_CPU_DESCRIPTOR_HANDLE heap_start = heaps.back()->GetCPUDescriptorHandleForHeapStart();
                for (UINT i = 0; i < desc.NumDescriptors; ++i)
                {
                    D3D12_CPU_DESCRIPTOR_HANDLE handle = heap_start;
                    handle.ptr += i * descriptorSize;
                    freeList.push_back(handle);
                }
            }

            D3D12_CPU_DESCRIPTOR_HANDLE Allocate()
            {
                locker.lock();
                if (freeList.empty())
                {
                    BlockAllocate();
                }

                ALIMER_ASSERT(!freeList.empty());
                D3D12_CPU_DESCRIPTOR_HANDLE handle = freeList.back();
                freeList.pop_back();
                locker.unlock();
                return handle;
            }

            void Free(D3D12_CPU_DESCRIPTOR_HANDLE index)
            {
                locker.lock();
                freeList.push_back(index);
                locker.unlock();
            }
        };
        DescriptorAllocator resourceDescriptorAllocator;
        DescriptorAllocator samplerDescriptorAllocator;
        DescriptorAllocator rtvDescriptorAllocator;
        DescriptorAllocator dsvDescriptorAllocator;

        struct DescriptorHeap
        {
            ID3D12DescriptorHeap* handle = nullptr;
            D3D12_CPU_DESCRIPTOR_HANDLE CPUStart;
            D3D12_GPU_DESCRIPTOR_HANDLE GPUStart;

            // CPU status:
            std::atomic<uint64_t> allocationOffset{ 0 };

            // GPU status:
            ID3D12Fence* fence = nullptr;
            uint64_t fenceValue = 0;
            uint64_t cachedCompletedValue = 0;
        };
        DescriptorHeap resourceHeap;
        DescriptorHeap samplerHeap;

        // Bindless
        static constexpr uint32_t kBindlessResourceCapacity = 500000;
        static constexpr uint32_t kBindlessSamplerCapacity = 256;

        std::vector<uint32_t> freeBindlessResources;
        std::vector<uint32_t> freeBindlessSamplers;
    };
}

#endif /* defined(ALIMER_RHI_D3D12) */
