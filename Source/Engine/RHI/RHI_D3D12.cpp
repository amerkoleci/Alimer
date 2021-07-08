// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_D3D12)
#include "RHI_D3D12.h"
#include "Core/Assert.h"
#include "Core/Log.h"

using Microsoft::WRL::ComPtr;
using namespace Alimer;

using PFN_CREATE_DXGI_FACTORY2 = decltype(&CreateDXGIFactory2);
using PFN_DXGI_GET_DEBUG_INTERFACE1 = decltype(&DXGIGetDebugInterface1);

namespace Internal
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    static PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2 = nullptr;
    static PFN_DXGI_GET_DEBUG_INTERFACE1 DXGIGetDebugInterface1 = nullptr;

    static PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterface = nullptr;
    static PFN_D3D12_CREATE_DEVICE D3D12CreateDevice = nullptr;
    static PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE D3D12SerializeVersionedRootSignature = nullptr;

    //static DxcCreateInstanceProc DxcCreateInstance;
#endif
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#define RHI_CreateDXGIFactory2 Internal::CreateDXGIFactory2
#define RHI_DXGIGetDebugInterface1 Internal::DXGIGetDebugInterface1
#define RHI_D3D12GetDebugInterface Internal::D3D12GetDebugInterface
#define RHI_D3D12CreateDevice Internal::D3D12CreateDevice
#define RHI_D3D12SerializeVersionedRootSignature Internal::D3D12SerializeVersionedRootSignature
#else
#define RHI_CreateDXGIFactory2 CreateDXGIFactory2
#define RHI_DXGIGetDebugInterface1 DXGIGetDebugInterface1
#define RHI_D3D12GetDebugInterface D3D12GetDebugInterface
#define RHI_D3D12CreateDevice D3D12CreateDevice
#define RHI_D3D12SerializeVersionedRootSignature D3D12SerializeVersionedRootSignature
#endif

#ifdef _DEBUG
// Declare debug guids to avoid linking with "dxguid.lib"
static constexpr IID RHI_DXGI_DEBUG_ALL = { 0xe48ae283, 0xda80, 0x490b, {0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8} };
static constexpr IID RHI_DXGI_DEBUG_DXGI = { 0x25cddaa4, 0xb1c6, 0x47e1, {0xac, 0x3e, 0x98, 0x87, 0x5b, 0x5a, 0x2e, 0x2a} };
#endif

[[nodiscard]] constexpr DXGI_FORMAT ToDXGISwapChainFormat(RHIPixelFormat format)
{
    switch (format) {
        case RHIPixelFormat::RGBA16Float:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case RHIPixelFormat::BGRA8UNorm:
        case RHIPixelFormat::BGRA8UNormSrgb:
            return DXGI_FORMAT_B8G8R8A8_UNORM;

        case RHIPixelFormat::RGBA8UNorm:
        case RHIPixelFormat::RGBA8UNormSrgb:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case RHIPixelFormat::RGB10A2Unorm:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
    }

    return DXGI_FORMAT_B8G8R8A8_UNORM;
}


struct SwapChain_D3D12
{
    ComPtr<IDXGISwapChain3> handle;

    ~SwapChain_D3D12()
    {
    }
};

[[nodiscard]] inline SwapChain_D3D12* ToInternal(const RHISwapChain* resource)
{
    return static_cast<SwapChain_D3D12*>(resource->state.get());
}

RHICommandBufferD3D12::RHICommandBufferD3D12(_In_ ID3D12Device5* device, RHIQueueType queueType)
    : queueType{ queueType }
{
    D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
    switch (queueType)
    {
        case RHIQueueType::Compute:
            commandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            break;

        default:
            commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
            break;
    }

    for (uint32_t i = 0; i < kRHIMaxFramesInFlight; ++i)
    {
        ThrowIfFailed(device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&commandAllocators[i])));
    }

    ThrowIfFailed(device->CreateCommandList1(0, commandListType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&commandList)));
}

RHICommandBufferD3D12::~RHICommandBufferD3D12()
{
    for (uint32_t i = 0; i < kRHIMaxFramesInFlight; ++i)
    {
        SafeRelease(commandAllocators[i]);
    }

    SafeRelease(commandList);
}

void RHICommandBufferD3D12::Reset(uint32_t frameIndex)
{
    // Start the command list in a default state:
    ThrowIfFailed(commandAllocators[frameIndex]->Reset());
    ThrowIfFailed(commandList->Reset(commandAllocators[frameIndex], nullptr));

    // Reset states
    resourceBarriers.clear();
    swapChains.clear();
}

void RHICommandBufferD3D12::PresentSwapChains()
{
    for (auto& swapChain : swapChains)
    {
        ToInternal(swapChain)->handle->Present(1, 0);
    }
}

void RHICommandBufferD3D12::FlushQueries()
{
}

void RHICommandBufferD3D12::FlushBarriers()
{
    if (resourceBarriers.empty())
        return;

    for (size_t i = 0; i < resourceBarriers.size(); ++i)
    {
        auto& barrier = resourceBarriers[i];
        if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION
            && queueType != RHIQueueType::Graphics)
        {
            // Only graphics queue can do pixel shader state:
            barrier.Transition.StateBefore &= ~D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            barrier.Transition.StateAfter &= ~D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        }
        if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION &&
            barrier.Transition.StateBefore == barrier.Transition.StateAfter)
        {
            // Remove NOP barriers:
            barrier = resourceBarriers.back();
            resourceBarriers.pop_back();
            i--;
        }
    }

    if (!resourceBarriers.empty())
    {
        commandList->ResourceBarrier((UINT)resourceBarriers.size(), resourceBarriers.data());
        resourceBarriers.clear();
    }
}

void RHICommandBufferD3D12::BeginRenderPass(const RHISwapChain* swapChain)
{
    swapChains.push_back(swapChain);
    auto internalState = ToInternal(swapChain);

    //D3D12_RESOURCE_BARRIER barrier = {};
    //barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    //barrier.Transition.pResource = internalState->backBuffers[internalState->handle->GetCurrentBackBufferIndex()].Get();
    //barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    //barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    //barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    //barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    //resourceBarriers.push_back(barrier);
    //FlushBarriers();

    //D3D12_RENDER_PASS_RENDER_TARGET_DESC RTV = {};
    //RTV.cpuDescriptor = internal_state->backbufferRTV[internal_state->swapChain->GetCurrentBackBufferIndex()];
    //RTV.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
    //RTV.BeginningAccess.Clear.ClearValue.Color[0] = swapchain->desc.clearcolor[0];
    //RTV.BeginningAccess.Clear.ClearValue.Color[1] = swapchain->desc.clearcolor[1];
    //RTV.BeginningAccess.Clear.ClearValue.Color[2] = swapchain->desc.clearcolor[2];
    //RTV.BeginningAccess.Clear.ClearValue.Color[3] = swapchain->desc.clearcolor[3];
    //RTV.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    //commandList->BeginRenderPass(1, &RTV, nullptr, D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES);
}

void RHICommandBufferD3D12::EndRenderPass()
{
    //commandList->EndRenderPass();
}

/* RHIDeviceD3D12 */
void RHIDeviceD3D12::CopyAllocator::Initialize(RHIDeviceD3D12* device_)
{
    device = device_;

    D3D12_COMMAND_QUEUE_DESC queueDesc;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    ThrowIfFailed(device->d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue)));
}

void RHIDeviceD3D12::CopyAllocator::Shutdown()
{
    ComPtr<ID3D12Fence> fence;
    ThrowIfFailed(device->d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    ThrowIfFailed(queue->Signal(fence.Get(), 1));
    ThrowIfFailed(fence->SetEventOnCompletion(1, nullptr));

    SafeRelease(queue);
}

bool RHIDeviceD3D12::IsAvailable()
{
    static bool available_initialized = false;
    static bool available = false;

    if (available_initialized) {
        return available;
    }

    available_initialized = true;

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    HMODULE dxgiDLL = LoadLibraryExW(L"dxgi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE d3d12DLL = LoadLibraryExW(L"d3d12.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    //HMODULE dxcompiler = LoadLibraryW(L"dxcompiler.dll");

    if (dxgiDLL == nullptr ||
        d3d12DLL == nullptr)
    {
        return false;
    }

    Internal::CreateDXGIFactory2 = (PFN_CREATE_DXGI_FACTORY2)GetProcAddress(dxgiDLL, "CreateDXGIFactory2");
    if (Internal::CreateDXGIFactory2 == nullptr)
    {
        return false;
    }

    Internal::DXGIGetDebugInterface1 = (PFN_DXGI_GET_DEBUG_INTERFACE1)GetProcAddress(dxgiDLL, "DXGIGetDebugInterface1");

    Internal::D3D12GetDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(d3d12DLL, "D3D12GetDebugInterface");
    Internal::D3D12CreateDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(d3d12DLL, "D3D12CreateDevice");
    if (!Internal::D3D12CreateDevice) {
        return false;
    }

    Internal::D3D12SerializeVersionedRootSignature = (PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE)GetProcAddress(d3d12DLL, "D3D12SerializeVersionedRootSignature");
    if (!Internal::D3D12SerializeVersionedRootSignature) {
        return false;
    }
#else
    //HMODULE dxcompiler = LoadPackagedLibrary(L"dxcompiler.dll", 0);
#endif

    //if (dxcompiler)
    //{
    //    DxcCreateInstance = (DxcCreateInstanceProc)GetProcAddress(dxcompiler, "DxcCreateInstance");
    //    ALIMER_ASSERT(DxcCreateInstance != nullptr);
    //}

    if (SUCCEEDED(RHI_D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
    {
        available = true;
        return true;
    }

    return false;
}

RHIDeviceD3D12::RHIDeviceD3D12(RHIValidationMode validationMode)
{
    ALIMER_VERIFY(IsAvailable());

    if (validationMode != RHIValidationMode::Disabled)
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(RHI_D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
        {
            debugController->EnableDebugLayer();

            if (validationMode == RHIValidationMode::GPU)
            {
                ComPtr<ID3D12Debug1> debugController1;
                if (SUCCEEDED(debugController.As(&debugController1)))
                {
                    debugController1->SetEnableGPUBasedValidation(TRUE);
                    debugController1->SetEnableSynchronizedCommandQueueValidation(TRUE);
                }

                ComPtr<ID3D12Debug2> debugController2;
                if (SUCCEEDED(debugController.As(&debugController2)))
                {
                    debugController2->SetGPUBasedValidationFlags(D3D12_GPU_BASED_VALIDATION_FLAGS_NONE);
                }
            }
        }
        else
        {
            OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
        }

#if defined(_DEBUG)
        ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
        if (SUCCEEDED(RHI_DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
        {
            dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

            dxgiInfoQueue->SetBreakOnSeverity(RHI_DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
            dxgiInfoQueue->SetBreakOnSeverity(RHI_DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

            DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
            {
                80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
            };
            DXGI_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
            filter.DenyList.pIDList = hide;
            dxgiInfoQueue->AddStorageFilterEntries(RHI_DXGI_DEBUG_DXGI, &filter);
        }
#endif
    }

    ThrowIfFailed(RHI_CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf())));

    // Determines whether tearing support is available for fullscreen borderless windows.
    {
        BOOL allowTearing = FALSE;

        ComPtr<IDXGIFactory5> dxgiFactory5;
        HRESULT hr = dxgiFactory.As(&dxgiFactory5);
        if (SUCCEEDED(hr))
        {
            hr = dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
        }

        if (FAILED(hr) || !allowTearing)
        {
            tearingSupported = false;
#ifdef _DEBUG
            LOGW("Direct3D12: Variable refresh rate displays not supported");
#endif
        }
        else
        {
            tearingSupported = true;
        }
    }
}

bool RHIDeviceD3D12::Initialize(RHIValidationMode validationMode)
{
    // Enum adapter and create device.
    {
        ComPtr<IDXGIFactory6> dxgiFactory6;
        const bool enumByPreference = SUCCEEDED(dxgiFactory.As(&dxgiFactory6));

        auto NextAdapter = [&](uint32_t index, IDXGIAdapter1** ppAdapter)
        {
            if (enumByPreference)
                return dxgiFactory6->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(ppAdapter));
            else
                return dxgiFactory->EnumAdapters1(index, ppAdapter);
        };

        ComPtr<IDXGIAdapter1> adapter;
        for (uint32_t adapterIndex = 0;
            DXGI_ERROR_NOT_FOUND != NextAdapter(adapterIndex, adapter.ReleaseAndGetAddressOf());
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            ThrowIfFailed(adapter->GetDesc1(&desc));

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                continue;
            }

            // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
            if (SUCCEEDED(RHI_D3D12CreateDevice(
                adapter.Get(),
                D3D_FEATURE_LEVEL_12_0,
                IID_PPV_ARGS(d3dDevice.ReleaseAndGetAddressOf()))))
            {
#ifdef _DEBUG
                wchar_t buff[256] = {};
                swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
                OutputDebugStringW(buff);
#endif
                break;
            }
        }

        d3dDevice->SetName(L"AlimerDevice");

        if (validationMode != RHIValidationMode::Disabled)
        {
            ComPtr<ID3D12DebugDevice2> debugDevice;
            if (SUCCEEDED(d3dDevice.As(&debugDevice)))
            {
                // TODO: Replace with SetDebugParameter as SetFeatureMask is deprecated
                debugDevice->SetFeatureMask(D3D12_DEBUG_FEATURE_ALLOW_BEHAVIOR_CHANGING_DEBUG_AIDS | D3D12_DEBUG_FEATURE_CONSERVATIVE_RESOURCE_STATE_TRACKING);
            }

            ComPtr<ID3D12InfoQueue> d3d12InfoQueue;
            if (SUCCEEDED(d3dDevice.As(&d3d12InfoQueue)))
            {
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

                std::vector<D3D12_MESSAGE_SEVERITY> enabledSeverities;

                // These severities should be seen all the time
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_CORRUPTION);
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_ERROR);
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_WARNING);
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_MESSAGE);

                if (validationMode == RHIValidationMode::Verbose)
                {
                    // Verbose only filters
                    enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_INFO);
                }

                D3D12_MESSAGE_ID hide[] =
                {
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
                    D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,

                    D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                    D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
                    //D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE
                };
                D3D12_INFO_QUEUE_FILTER filter = {};
                filter.AllowList.NumSeverities = static_cast<UINT>(enabledSeverities.size());
                filter.AllowList.pSeverityList = enabledSeverities.data();
                filter.DenyList.NumIDs = _countof(hide);
                filter.DenyList.pIDList = hide;
                ThrowIfFailed(d3d12InfoQueue->AddStorageFilterEntries(&filter));

                ThrowIfFailed(d3d12InfoQueue->AddApplicationMessage(D3D12_MESSAGE_SEVERITY_MESSAGE, "D3D12 Debug Filters setup"));
            }
        }

        // Create memory allocator
        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = d3dDevice.Get();
        allocatorDesc.pAdapter = adapter.Get();

        if (FAILED(D3D12MA::CreateAllocator(&allocatorDesc, &allocator)))
        {
            return false;
        }

        // Init capabilities.
        DXGI_ADAPTER_DESC1 adapterDesc;
        ThrowIfFailed(adapter->GetDesc1(&adapterDesc));

        // Determine maximum supported feature level for this device
        static const D3D_FEATURE_LEVEL s_featureLevels[] =
        {
    #if defined(NTDDI_WIN10_FE) || defined(USING_D3D12_AGILITY_SDK)
            D3D_FEATURE_LEVEL_12_2,
    #endif
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
        {
            static_cast<UINT>(std::size(s_featureLevels)), s_featureLevels, D3D_FEATURE_LEVEL_11_0
        };

        HRESULT hr = d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
        if (SUCCEEDED(hr))
        {
            featureLevel = featLevels.MaxSupportedFeatureLevel;
        }
        else
        {
            featureLevel = D3D_FEATURE_LEVEL_12_0;
        }
    }

    // Create command queue's
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 0;
        ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queues[(uint32_t)RHIQueueType::Graphics].handle)));
        ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&queues[(uint32_t)RHIQueueType::Graphics].fence)));
        queues[(uint32_t)RHIQueueType::Graphics].handle->SetName(L"Graphics Command Queue");
        queues[(uint32_t)RHIQueueType::Graphics].handle->SetName(L"Graphics Command Queue Fence");

        // Compute
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queues[(uint32_t)RHIQueueType::Compute].handle)));
        ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&queues[(uint32_t)RHIQueueType::Compute].fence)));
        queues[(uint32_t)RHIQueueType::Compute].handle->SetName(L"Compute Command Queue");
        queues[(uint32_t)RHIQueueType::Compute].handle->SetName(L"Compute Command Queue Fence");
    }

    // Create frame-resident resources:
    for (uint32_t i = 0; i < kRHIMaxFramesInFlight; ++i)
    {
        for (uint32_t queue = 0; queue < (uint32_t)RHIQueueType::Count; ++queue)
        {
            ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFences[i][queue])));
        }
    }

    copyAllocator.Initialize(this);

    return true;
}

void RHIDeviceD3D12::Shutdown()
{
    WaitForIdle();
    copyAllocator.Shutdown();

    // Allocator.
    if (allocator != nullptr)
    {
        D3D12MA::Stats stats;
        allocator->CalculateStats(&stats);

        if (stats.Total.UsedBytes > 0)
        {
            LOGI("Total device memory leaked: {} bytes.", stats.Total.UsedBytes);
        }

        SafeRelease(allocator);
    }
}

void RHIDeviceD3D12::WaitForIdle()
{
    ComPtr<ID3D12Fence> fence;
    ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    for (auto& queue : queues)
    {
        ThrowIfFailed(queue.handle->Signal(fence.Get(), 1));
        if (fence->GetCompletedValue() < 1)
        {
            ThrowIfFailed(fence->SetEventOnCompletion(1, nullptr));
        }
        fence->Signal(0);
    }
}

bool RHIDeviceD3D12::BeginFrame()
{
    commandBuffersCount.store(0);

    return true;
}

void RHIDeviceD3D12::EndFrame()
{
    // Submit current command buffers
    SubmitCommandBuffers();

    // From here, we begin a new frame, this affects GetFrameResources()!
    frameCount++;
    frameIndex = frameCount % kRHIMaxFramesInFlight;

    // Begin new frame.
    // Initiate stalling CPU when GPU is not yet finished with next frame:
    for (uint32_t queue = 0; queue < (uint32_t)RHIQueueType::Count; ++queue)
    {
        if (frameCount >= kRHIMaxFramesInFlight && frameFences[frameIndex][queue]->GetCompletedValue() < 1)
        {
            // NULL event handle will simply wait immediately:
            //	https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12fence-seteventoncompletion#remarks
            ThrowIfFailed(frameFences[frameIndex][queue]->SetEventOnCompletion(1, nullptr));
        }

        ThrowIfFailed(frameFences[frameIndex][queue]->Signal(0));
    }

#if TODO
    // Descriptor heaps' progress is recorded by the GPU:
    descriptorheap_res.fenceValue = descriptorheap_res.allocationOffset.load();
    hr = queues[QUEUE_GRAPHICS].queue->Signal(descriptorheap_res.fence.Get(), descriptorheap_res.fenceValue);
    assert(SUCCEEDED(hr));
    descriptorheap_res.cached_completedValue = descriptorheap_res.fence->GetCompletedValue();
    descriptorheap_sam.fenceValue = descriptorheap_sam.allocationOffset.load();
    hr = queues[QUEUE_GRAPHICS].queue->Signal(descriptorheap_sam.fence.Get(), descriptorheap_sam.fenceValue);
    assert(SUCCEEDED(hr));
    descriptorheap_sam.cached_completedValue = descriptorheap_sam.fence->GetCompletedValue();
    allocationhandler->Update(FRAMECOUNT, BUFFERCOUNT);
#endif // TODO
}

void RHIDeviceD3D12::SubmitCommandBuffers()
{
    RHIQueueType submitQueueType = RHIQueueType::Count;

    uint32_t cmd_last = commandBuffersCount.load();
    commandBuffersCount.store(0);
    for (uint32_t cmd = 0; cmd < cmd_last; ++cmd)
    {
        RHICommandBufferD3D12* commandBuffer = GetCommandBuffer(cmd);

        commandBuffer->FlushQueries();
        commandBuffer->FlushBarriers();

        ThrowIfFailed(commandBuffer->GetHandle()->Close());

        const CommandListMetadata& meta = commandBuffersMeta[cmd];
        if (submitQueueType == RHIQueueType::Count)
        {
            submitQueueType = meta.queueType;
        }

        // new queue type or wait breaks submit batch
        if (meta.queueType != submitQueueType
            || !meta.waits.empty())
        {
            // submit previous cmd batch:
            if (queues[(uint32_t)submitQueueType].submitCount > 0)
            {
                queues[(uint32_t)submitQueueType].handle->ExecuteCommandLists(
                    queues[(uint32_t)submitQueueType].submitCount,
                    queues[(uint32_t)submitQueueType].submitCommandLists
                );

                queues[(uint32_t)submitQueueType].submitCount = 0;
            }

            // Signal status in case any future waits needed.
            HRESULT hr = queues[(uint32_t)submitQueueType].handle->Signal(
                queues[(uint32_t)submitQueueType].fence.Get(),
                frameCount * kRHIMaxFrameCommandBuffers + (uint64_t)cmd
            );

            ThrowIfFailed(hr);

            submitQueueType = meta.queueType;

            for (auto& wait : meta.waits)
            {
                // record wait for signal on a previous submit:
                const CommandListMetadata& waitMeta = commandBuffersMeta[wait];
                hr = queues[(uint32_t)submitQueueType].handle->Wait(
                    queues[(uint32_t)waitMeta.queueType].fence.Get(),
                    frameCount * kRHIMaxFrameCommandBuffers + (uint64_t)wait
                );

                ThrowIfFailed(hr);
            }
        }

        ALIMER_ASSERT(submitQueueType < RHIQueueType::Count);
        queues[(uint32_t)submitQueueType].submitCommandLists[queues[(uint32_t)submitQueueType].submitCount++] = commandBuffer->GetHandle();
    }

    // submit last cmd batch:
    ALIMER_ASSERT(submitQueueType < RHIQueueType::Count);
    ALIMER_ASSERT(queues[(uint32_t)submitQueueType].submitCount > 0);
    queues[(uint32_t)submitQueueType].handle->ExecuteCommandLists(
        queues[(uint32_t)submitQueueType].submitCount,
        queues[(uint32_t)submitQueueType].submitCommandLists
    );
    queues[(uint32_t)submitQueueType].submitCount = 0;

    // Mark the completion of queues for this frame:
    for (uint32_t queue = 0; queue < (uint32_t)RHIQueueType::Count; ++queue)
    {
        ThrowIfFailed(queues[queue].handle->Signal(frameFences[GetFrameIndex()][queue].Get(), 1));
    }

    for (uint32_t cmd = 0; cmd < cmd_last; ++cmd)
    {
        GetCommandBuffer(cmd)->PresentSwapChains();
    }
}

RHICommandBuffer* RHIDeviceD3D12::BeginCommandBuffer(RHIQueueType type)
{
    uint32_t index = commandBuffersCount.fetch_add(1);
    ALIMER_ASSERT(index < kRHIMaxFrameCommandBuffers);
    commandBuffersMeta[index].queueType = type;
    commandBuffersMeta[index].waits.clear();

    if (GetCommandBuffer(index) == nullptr)
    {
        // We need to create one more command buffer.
        commandBuffers[index][(uint32_t)type].reset(new RHICommandBufferD3D12(d3dDevice.Get(), type));
    }

    GetCommandBuffer(index)->Reset(frameIndex);

    return GetCommandBuffer(index);
}

bool RHIDeviceD3D12::CreateSwapChain(void* window, const RHISwapChainDescriptor* descriptor, RHISwapChain* pSwapChain) const
{
    ALIMER_ASSERT(descriptor);

    auto internalState = std::make_shared<SwapChain_D3D12>();

    HRESULT hr;

    ComPtr<IDXGISwapChain1> tempSwapChain;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = descriptor->width;
    swapChainDesc.Height = descriptor->height;
    swapChainDesc.Format = ToDXGISwapChainFormat(descriptor->format);
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = Max(descriptor->bufferCount, kRHIMaxBackBufferCount);
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swapChainDesc.Flags = (tearingSupported) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
    fsSwapChainDesc.Windowed = !descriptor->fullscreen;

    hr = dxgiFactory->CreateSwapChainForHwnd(
        queues[(uint32_t)RHIQueueType::Graphics].handle.Get(),
        (HWND)window,
        &swapChainDesc,
        &fsSwapChainDesc,
        nullptr,
        tempSwapChain.GetAddressOf()
    );

    // Prevent DXGI from responding to the ALT+ENTER shortcut
    ThrowIfFailed(dxgiFactory->MakeWindowAssociation((HWND)window, DXGI_MWA_NO_ALT_ENTER));
#else
    swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

    hr = dxgiFactory->CreateSwapChainForCoreWindow(
        queues[(uint32_t)RHIQueueType::Graphics].handle.Get(),
        static_cast<IUnknown*>(winrt::get_abi(*window)),
        &swapChainDesc,
        nullptr,
        tempSwapChain.GetAddressOf()
    );
#endif

    if (FAILED(hr))
    {
        return false;
    }

    hr = tempSwapChain.As(&internalState->handle);
    if (FAILED(hr))
    {
        return false;
    }

    pSwapChain->state = internalState;
    return true;
}

#endif /* defined(ALIMER_RHI_D3D12) */
