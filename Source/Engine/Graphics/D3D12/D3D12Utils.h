// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Log.h"
#include "Graphics/Types.h"
#include "PlatformInclude.h"

#include <wrl/client.h>

#include <dxgi1_6.h>

#include "directx/d3d12.h"
#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include "D3D12MemAlloc.h"

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

namespace Alimer
{
	class D3D12Graphics;
	class D3D12Texture;
	class D3D12CommandBuffer;
	class D3D12SwapChain;
	class D3D12Pipeline;
	class D3D12CommandBuffer;
	class D3D12CommandQueue;

	constexpr const char* ToString(D3D_FEATURE_LEVEL value)
	{
		switch (value)
		{
		case D3D_FEATURE_LEVEL_11_0:
			return "Level 11.0";
		case D3D_FEATURE_LEVEL_11_1:
			return "Level 11.1";
		case D3D_FEATURE_LEVEL_12_0:
			return "Level 12.0";
		case D3D_FEATURE_LEVEL_12_1:
			return "Level 12.1";
		case D3D_FEATURE_LEVEL_12_2:
			return "Level 12.2";
		default:
			return nullptr;
		}
	}

	// Type alias for ComPtr template
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct DxgiFormatMapping
    {
        PixelFormat format;
        DXGI_FORMAT resourceFormat;
        DXGI_FORMAT srvFormat;
        DXGI_FORMAT rtvFormat;
    };

    const DxgiFormatMapping& GetDxgiFormatMapping(PixelFormat format);

	[[nodiscard]] constexpr D3D12_COMMAND_LIST_TYPE ToD3D12(CommandQueueType type)
	{
		switch (type)
		{
		case CommandQueueType::Graphics:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case CommandQueueType::Compute:
			return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		//case CommandQueueType::Copy:
		//	return D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			ALIMER_UNREACHABLE();
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
	}

	[[nodiscard]] constexpr D3D12_COMPARISON_FUNC ToD3D12ComparisonFunc(CompareFunction function)
	{
		switch (function)
		{
		case CompareFunction::Never:
			return D3D12_COMPARISON_FUNC_NEVER;
		case CompareFunction::Less:
			return D3D12_COMPARISON_FUNC_LESS;
		case CompareFunction::Equal:
			return D3D12_COMPARISON_FUNC_EQUAL;
		case CompareFunction::LessEqual:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case CompareFunction::Greater:
			return D3D12_COMPARISON_FUNC_GREATER;
		case CompareFunction::NotEqual:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case CompareFunction::GreaterEqual:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case CompareFunction::Always:
			return D3D12_COMPARISON_FUNC_ALWAYS;

		default:
			ALIMER_UNREACHABLE();
			return static_cast<D3D12_COMPARISON_FUNC>(0);
		}
	}

    [[nodiscard]] constexpr uint32_t D3D12SampleCount(SampleCount count)
    {
        switch (count)
        {
            case SampleCount::Count1:
                return 1;
            case SampleCount::Count2:
                return 2;
            case SampleCount::Count4:
                return 4;
            case SampleCount::Count8:
                return 8;
            case SampleCount::Count16:
                return 16;
            case SampleCount::Count32:
                return 32;

            default:
                ALIMER_UNREACHABLE();
                return 1;
        }
    }

	[[nodiscard]] constexpr DXGI_FORMAT ToDXGISwapChainFormat(PixelFormat format)
	{
		switch (format) {
		case PixelFormat::RGBA16Float:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case PixelFormat::BGRA8UNorm:
		case PixelFormat::BGRA8UNormSrgb:
			return DXGI_FORMAT_B8G8R8A8_UNORM;

		case PixelFormat::RGBA8UNorm:
		case PixelFormat::RGBA8UNormSrgb:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case PixelFormat::RGB10A2UNorm:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
		}

		return DXGI_FORMAT_B8G8R8A8_UNORM;
	}

	class D3D12GpuResource
	{
		friend class D3D12CommandBuffer;

	public:
		D3D12GpuResource() = default;
		D3D12GpuResource(ID3D12Resource* resource_, D3D12_RESOURCE_STATES currentState) 
			: handle(resource_)
			, state(currentState)
			, transitioningState((D3D12_RESOURCE_STATES)-1)
		{
		}

		virtual ~D3D12GpuResource()
		{
			Destroy();
		}

		virtual void Destroy()
		{
            handle = nullptr;
			allocation = nullptr;
			gpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		}

		ID3D12Resource* GetHandle() const { return handle; }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return gpuVirtualAddress; }

	protected:
		ID3D12Resource* handle = nullptr;
		D3D12MA::Allocation* allocation = nullptr;
		mutable D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
		mutable D3D12_RESOURCE_STATES transitioningState = (D3D12_RESOURCE_STATES)-1;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		bool fixedResourceState = false;
	};
}
