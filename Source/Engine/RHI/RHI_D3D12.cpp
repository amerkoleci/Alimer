// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_D3D12)
#include "RHI_D3D12.h"
#include "Core/Assert.h"
#include "Core/Log.h"

using Microsoft::WRL::ComPtr;

namespace Alimer
{
    namespace
    {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
        using PFN_CREATE_DXGI_FACTORY2 = decltype(&CreateDXGIFactory2);
        static PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2 = nullptr;

        using PFN_DXGI_GET_DEBUG_INTERFACE1 = decltype(&DXGIGetDebugInterface1);
        static PFN_DXGI_GET_DEBUG_INTERFACE1 DXGIGetDebugInterface1 = nullptr;

        static PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterface = nullptr;
        static PFN_D3D12_CREATE_DEVICE D3D12CreateDevice = nullptr;
        static PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE D3D12SerializeVersionedRootSignature = nullptr;

        //static DxcCreateInstanceProc DxcCreateInstance;
#endif

#ifdef _DEBUG
        // Declare debug guids to avoid linking with "dxguid.lib"
        static constexpr IID DXGI_DEBUG_ALL = { 0xe48ae283, 0xda80, 0x490b, {0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8} };
        static constexpr IID DXGI_DEBUG_DXGI = { 0x25cddaa4, 0xb1c6, 0x47e1, {0xac, 0x3e, 0x98, 0x87, 0x5b, 0x5a, 0x2e, 0x2a} };
#endif
    }

    using IDXGISwapChainX = IDXGISwapChain3;

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

        CreateDXGIFactory2 = (PFN_CREATE_DXGI_FACTORY2)GetProcAddress(dxgiDLL, "CreateDXGIFactory2");
        if (CreateDXGIFactory2 == nullptr)
        {
            return false;
        }

        DXGIGetDebugInterface1 = (PFN_DXGI_GET_DEBUG_INTERFACE1)GetProcAddress(dxgiDLL, "DXGIGetDebugInterface1");

        D3D12GetDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(d3d12DLL, "D3D12GetDebugInterface");
        D3D12CreateDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(d3d12DLL, "D3D12CreateDevice");
        if (!D3D12CreateDevice) {
            return false;
        }

        D3D12SerializeVersionedRootSignature = (PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE)GetProcAddress(d3d12DLL, "D3D12SerializeVersionedRootSignature");
        if (!D3D12SerializeVersionedRootSignature) {
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

        if (SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
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
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
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
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
            {
                dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

                dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
                dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

                DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
                {
                    80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
                };
                DXGI_INFO_QUEUE_FILTER filter = {};
                filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
                filter.DenyList.pIDList = hide;
                dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
            }
#endif
        }

        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf())));

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
                if (SUCCEEDED(D3D12CreateDevice(
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
                ComPtr<ID3D12DebugDevice> debugDevice;
                if (SUCCEEDED(d3dDevice.As(&debugDevice)))
                {
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

        return true;
    }

    void RHIDeviceD3D12::Shutdown()
    {
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

    bool RHIDeviceD3D12::BeginFrame()
    {
        return true;
    }

    void RHIDeviceD3D12::EndFrame()
    {
    }

    bool RHIDeviceD3D12::CreateSwapChain(void* window, const RHUSwapChainDescriptor* descriptor, RHISwapChain* swapChain) const
    {
        return true;
    }
}

#endif /* defined(ALIMER_RHI_D3D12) */
