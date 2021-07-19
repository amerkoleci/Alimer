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

    [[nodiscard]] constexpr DXGI_FORMAT ToDXGIFormat(PixelFormat format)
    {
        switch (format)
        {
            // 8-bit formats
        case PixelFormat::R8UNorm:  return DXGI_FORMAT_R8_UNORM;
        case PixelFormat::R8SNorm:  return DXGI_FORMAT_R8_SNORM;
        case PixelFormat::R8UInt:   return DXGI_FORMAT_R8_UINT;
        case PixelFormat::R8SInt:   return DXGI_FORMAT_R8_SINT;
            // 16-bit formats
        case PixelFormat::R16UNorm:     return DXGI_FORMAT_R16_UNORM;
        case PixelFormat::R16SNorm:     return DXGI_FORMAT_R16_SNORM;
        case PixelFormat::R16UInt:      return DXGI_FORMAT_R16_UINT;
        case PixelFormat::R16SInt:      return DXGI_FORMAT_R16_SINT;
        case PixelFormat::R16Float:     return DXGI_FORMAT_R16_FLOAT;
        case PixelFormat::RG8UNorm:     return DXGI_FORMAT_R8G8_UNORM;
        case PixelFormat::RG8SNorm:     return DXGI_FORMAT_R8G8_SNORM;
        case PixelFormat::RG8UInt:      return DXGI_FORMAT_R8G8_UINT;
        case PixelFormat::RG8SInt:      return DXGI_FORMAT_R8G8_SINT;
            // 32-bit formats
        case PixelFormat::R32UInt:          return DXGI_FORMAT_R32_UINT;
        case PixelFormat::R32SInt:          return DXGI_FORMAT_R32_SINT;
        case PixelFormat::R32Float:         return DXGI_FORMAT_R32_FLOAT;
        case PixelFormat::RG16UNorm:        return DXGI_FORMAT_R16G16_UNORM;
        case PixelFormat::RG16SNorm:        return DXGI_FORMAT_R16G16_SNORM;
        case PixelFormat::RG16UInt:         return DXGI_FORMAT_R16G16_UINT;
        case PixelFormat::RG16SInt:         return DXGI_FORMAT_R16G16_SINT;
        case PixelFormat::RG16Float:        return DXGI_FORMAT_R16G16_FLOAT;
        case PixelFormat::RGBA8UNorm:       return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::RGBA8UNormSrgb:   return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case PixelFormat::RGBA8SNorm:       return DXGI_FORMAT_R8G8B8A8_SNORM;
        case PixelFormat::RGBA8UInt:        return DXGI_FORMAT_R8G8B8A8_UINT;
        case PixelFormat::RGBA8SInt:        return DXGI_FORMAT_R8G8B8A8_SINT;
        case PixelFormat::BGRA8UNorm:       return DXGI_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::BGRA8UNormSrgb:   return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            // Packed 32-Bit formats
        case PixelFormat::RGB10A2UNorm:     return DXGI_FORMAT_R10G10B10A2_UNORM;
        case PixelFormat::RG11B10Float:     return DXGI_FORMAT_R11G11B10_FLOAT;
        case PixelFormat::RGB9E5Float:      return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
            // 64-Bit formats
        case PixelFormat::RG32UInt:         return DXGI_FORMAT_R32G32_UINT;
        case PixelFormat::RG32SInt:         return DXGI_FORMAT_R32G32_SINT;
        case PixelFormat::RG32Float:        return DXGI_FORMAT_R32G32_FLOAT;
        case PixelFormat::RGBA16UNorm:      return DXGI_FORMAT_R16G16B16A16_UNORM;
        case PixelFormat::RGBA16SNorm:      return DXGI_FORMAT_R16G16B16A16_SNORM;
        case PixelFormat::RGBA16UInt:       return DXGI_FORMAT_R16G16B16A16_UINT;
        case PixelFormat::RGBA16SInt:       return DXGI_FORMAT_R16G16B16A16_SINT;
        case PixelFormat::RGBA16Float:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
            // 128-Bit formats
        case PixelFormat::RGBA32UInt:       return DXGI_FORMAT_R32G32B32A32_UINT;
        case PixelFormat::RGBA32SInt:       return DXGI_FORMAT_R32G32B32A32_SINT;
        case PixelFormat::RGBA32Float:      return DXGI_FORMAT_R32G32B32A32_FLOAT;
            // Depth-stencil formats
        case PixelFormat::Depth16UNorm:			return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::Depth32Float:			return DXGI_FORMAT_D32_FLOAT;
        case PixelFormat::Depth24UNormStencil8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat::Depth32FloatStencil8: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            // Compressed BC formats
        case PixelFormat::BC1RGBAUNorm:         return DXGI_FORMAT_BC1_UNORM;
        case PixelFormat::BC1RGBAUNormSrgb:     return DXGI_FORMAT_BC1_UNORM_SRGB;
        case PixelFormat::BC2RGBAUNorm:         return DXGI_FORMAT_BC2_UNORM;
        case PixelFormat::BC2RGBAUNormSrgb:     return DXGI_FORMAT_BC2_UNORM_SRGB;
        case PixelFormat::BC3RGBAUNorm:         return DXGI_FORMAT_BC3_UNORM;
        case PixelFormat::BC3RGBAUNormSrgb:     return DXGI_FORMAT_BC3_UNORM_SRGB;
        case PixelFormat::BC4RSNorm:            return DXGI_FORMAT_BC4_SNORM;
        case PixelFormat::BC4RUNorm:            return DXGI_FORMAT_BC4_UNORM;
        case PixelFormat::BC5RGSNorm:           return DXGI_FORMAT_BC5_SNORM;
        case PixelFormat::BC5RGUNorm:           return DXGI_FORMAT_BC5_UNORM;
        case PixelFormat::BC6HRGBFloat:         return DXGI_FORMAT_BC6H_SF16;
        case PixelFormat::BC6HRGBUFloat:        return DXGI_FORMAT_BC6H_UF16;
        case PixelFormat::BC7RGBAUNorm:         return DXGI_FORMAT_BC7_UNORM;
        case PixelFormat::BC7RGBAUNormSrgb:     return DXGI_FORMAT_BC7_UNORM_SRGB;

        default:
            ALIMER_UNREACHABLE();
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    [[nodiscard]] constexpr PixelFormat FromDXGIFormat(DXGI_FORMAT format)
    {
        switch (format)
        {
            // 8-bit formats
        case DXGI_FORMAT_R8_UNORM:	return PixelFormat::R8UNorm;
        case DXGI_FORMAT_R8_SNORM:	return PixelFormat::R8SNorm;
        case DXGI_FORMAT_R8_UINT:	return PixelFormat::R8UInt;
        case DXGI_FORMAT_R8_SINT:	return PixelFormat::R8SInt;
            // 16-bit formats
        case DXGI_FORMAT_R16_UNORM:		return PixelFormat::R16UNorm;
        case DXGI_FORMAT_R16_SNORM:		return PixelFormat::R16SNorm;
        case DXGI_FORMAT_R16_UINT:		return PixelFormat::R16UInt;
        case DXGI_FORMAT_R16_SINT:		return PixelFormat::R16SInt;
        case DXGI_FORMAT_R16_FLOAT:		return PixelFormat::R16Float;
        case DXGI_FORMAT_R8G8_UNORM:	return PixelFormat::RG8UNorm;
        case DXGI_FORMAT_R8G8_SNORM:	return PixelFormat::RG8SNorm;
        case DXGI_FORMAT_R8G8_UINT:		return PixelFormat::RG8UInt;
        case DXGI_FORMAT_R8G8_SINT:		return PixelFormat::RG8SInt;
            // 32-bit formats
        case DXGI_FORMAT_R32_UINT:				return PixelFormat::R32UInt;
        case DXGI_FORMAT_R32_SINT:				return PixelFormat::R32SInt;
        case DXGI_FORMAT_R32_FLOAT:				return PixelFormat::R32Float;
        case DXGI_FORMAT_R16G16_UNORM:			return PixelFormat::RG16UNorm;
        case DXGI_FORMAT_R16G16_SNORM:			return PixelFormat::RG16SNorm;
        case DXGI_FORMAT_R16G16_UINT:			return PixelFormat::RG16UInt;
        case DXGI_FORMAT_R16G16_SINT:			return PixelFormat::RG16SInt;
        case DXGI_FORMAT_R16G16_FLOAT:			return PixelFormat::RG16Float;
        case DXGI_FORMAT_R8G8B8A8_UNORM:		return PixelFormat::RGBA8UNorm;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	return PixelFormat::RGBA8UNormSrgb;
        case DXGI_FORMAT_R8G8B8A8_SNORM:		return PixelFormat::RGBA8SNorm;
        case DXGI_FORMAT_R8G8B8A8_UINT:			return PixelFormat::RGBA8UInt;
        case DXGI_FORMAT_R8G8B8A8_SINT:			return PixelFormat::RGBA8SInt;
        case DXGI_FORMAT_B8G8R8A8_UNORM:		return PixelFormat::BGRA8UNorm;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:	return PixelFormat::BGRA8UNormSrgb;
            // Packed 32-Bit formats
        case DXGI_FORMAT_R10G10B10A2_UNORM:		return PixelFormat::RGB10A2UNorm;
        case DXGI_FORMAT_R11G11B10_FLOAT:		return PixelFormat::RG11B10Float;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:    return PixelFormat::RGB9E5Float;
            // 64-Bit formats
        case DXGI_FORMAT_R32G32_UINT:			return PixelFormat::RG32UInt;
        case DXGI_FORMAT_R32G32_SINT:			return PixelFormat::RG32SInt;
        case DXGI_FORMAT_R32G32_FLOAT:			return PixelFormat::RG32Float;
        case DXGI_FORMAT_R16G16B16A16_UNORM:	return PixelFormat::RGBA16UNorm;
        case DXGI_FORMAT_R16G16B16A16_SNORM:	return PixelFormat::RGBA16SNorm;
        case DXGI_FORMAT_R16G16B16A16_UINT:		return PixelFormat::RGBA16UInt;
        case DXGI_FORMAT_R16G16B16A16_SINT:		return PixelFormat::RGBA16SInt;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:	return PixelFormat::RGBA16Float;
            // 128-Bit formats
        case DXGI_FORMAT_R32G32B32A32_UINT:		return PixelFormat::RGBA32UInt;
        case DXGI_FORMAT_R32G32B32A32_SINT:		return PixelFormat::RGBA32SInt;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:	return PixelFormat::RGBA32Float;
            // Depth-stencil formats
        case DXGI_FORMAT_D16_UNORM:				return PixelFormat::Depth16UNorm;
        case DXGI_FORMAT_D32_FLOAT:				return PixelFormat::Depth32Float;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:		return PixelFormat::Depth24UNormStencil8;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:	return PixelFormat::Depth32FloatStencil8;
            // Compressed BC formats
        case DXGI_FORMAT_BC1_UNORM:			return PixelFormat::BC1RGBAUNorm;
        case DXGI_FORMAT_BC1_UNORM_SRGB:	return PixelFormat::BC1RGBAUNormSrgb;
        case DXGI_FORMAT_BC2_UNORM:			return PixelFormat::BC2RGBAUNorm;
        case DXGI_FORMAT_BC2_UNORM_SRGB:	return PixelFormat::BC2RGBAUNormSrgb;
        case DXGI_FORMAT_BC3_UNORM:			return PixelFormat::BC3RGBAUNorm;
        case DXGI_FORMAT_BC3_UNORM_SRGB:	return PixelFormat::BC3RGBAUNormSrgb;
        case DXGI_FORMAT_BC4_SNORM:			return PixelFormat::BC4RSNorm;
        case DXGI_FORMAT_BC4_UNORM:			return PixelFormat::BC4RUNorm;
        case DXGI_FORMAT_BC5_SNORM:			return PixelFormat::BC5RGSNorm;
        case DXGI_FORMAT_BC5_UNORM:			return PixelFormat::BC5RGUNorm;
        case DXGI_FORMAT_BC6H_SF16:			return PixelFormat::BC6HRGBFloat;
        case DXGI_FORMAT_BC6H_UF16:			return PixelFormat::BC6HRGBUFloat;
        case DXGI_FORMAT_BC7_UNORM:			return PixelFormat::BC7RGBAUNorm;
        case DXGI_FORMAT_BC7_UNORM_SRGB:	return PixelFormat::BC7RGBAUNormSrgb;

        default:
            ALIMER_UNREACHABLE();
            return PixelFormat::Undefined;
        }
    }

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
