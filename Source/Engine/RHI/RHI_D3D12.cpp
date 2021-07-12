// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_D3D12)
#include "RHI_D3D12.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "Math/MathHelper.h"
#include "directx/d3dx12.h"
#include <pix.h>

namespace Alimer
{
    using Microsoft::WRL::ComPtr;

    namespace
    {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
        using PFN_CREATE_DXGI_FACTORY2 = decltype(&CreateDXGIFactory2);
        using PFN_DXGI_GET_DEBUG_INTERFACE1 = decltype(&DXGIGetDebugInterface1);

        static PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2 = nullptr;
        static PFN_DXGI_GET_DEBUG_INTERFACE1 DXGIGetDebugInterface1 = nullptr;

        static PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterface = nullptr;
        static PFN_D3D12_CREATE_DEVICE D3D12CreateDevice = nullptr;
        static PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE D3D12SerializeVersionedRootSignature = nullptr;

        //static DxcCreateInstanceProc DxcCreateInstance;
#endif

#ifdef _DEBUG
    // Declare debug guids to avoid linking with "dxguid.lib"
        static constexpr IID RHI_DXGI_DEBUG_ALL = { 0xe48ae283, 0xda80, 0x490b, {0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8} };
        static constexpr IID RHI_DXGI_DEBUG_DXGI = { 0x25cddaa4, 0xb1c6, 0x47e1, {0xac, 0x3e, 0x98, 0x87, 0x5b, 0x5a, 0x2e, 0x2a} };
#endif
    }

    namespace
    {
        static_assert(sizeof(RHIViewport) == sizeof(D3D12_VIEWPORT), "Size mismatch");
        static_assert(offsetof(RHIViewport, x) == offsetof(D3D12_VIEWPORT, TopLeftX), "Layout mismatch");
        static_assert(offsetof(RHIViewport, y) == offsetof(D3D12_VIEWPORT, TopLeftY), "Layout mismatch");
        static_assert(offsetof(RHIViewport, width) == offsetof(D3D12_VIEWPORT, Width), "Layout mismatch");
        static_assert(offsetof(RHIViewport, height) == offsetof(D3D12_VIEWPORT, Height), "Layout mismatch");
        static_assert(offsetof(RHIViewport, minDepth) == offsetof(D3D12_VIEWPORT, MinDepth), "Layout mismatch");
        static_assert(offsetof(RHIViewport, maxDepth) == offsetof(D3D12_VIEWPORT, MaxDepth), "Layout mismatch");

        [[nodiscard]] constexpr DXGI_FORMAT ToDXGIFormat(PixelFormat format)
        {
            switch (format)
            {
                // 8-bit formats
                case PixelFormat::R8Unorm:  return DXGI_FORMAT_R8_UNORM;
                case PixelFormat::R8Snorm:  return DXGI_FORMAT_R8_SNORM;
                case PixelFormat::R8Uint:   return DXGI_FORMAT_R8_UINT;
                case PixelFormat::R8Sint:   return DXGI_FORMAT_R8_SINT;
                    // 16-bit formats
                case PixelFormat::R16Unorm:     return DXGI_FORMAT_R16_UNORM;
                case PixelFormat::R16Snorm:     return DXGI_FORMAT_R16_SNORM;
                case PixelFormat::R16Uint:      return DXGI_FORMAT_R16_UINT;
                case PixelFormat::R16Sint:      return DXGI_FORMAT_R16_SINT;
                case PixelFormat::R16Float:     return DXGI_FORMAT_R16_FLOAT;
                case PixelFormat::RG8Unorm:     return DXGI_FORMAT_R8G8_UNORM;
                case PixelFormat::RG8Snorm:     return DXGI_FORMAT_R8G8_SNORM;
                case PixelFormat::RG8Uint:      return DXGI_FORMAT_R8G8_UINT;
                case PixelFormat::RG8Sint:      return DXGI_FORMAT_R8G8_SINT;
                    // 32-bit formats
                case PixelFormat::R32Uint:          return DXGI_FORMAT_R32_UINT;
                case PixelFormat::R32Sint:          return DXGI_FORMAT_R32_SINT;
                case PixelFormat::R32Float:         return DXGI_FORMAT_R32_FLOAT;
                case PixelFormat::RG16Unorm:        return DXGI_FORMAT_R16G16_UNORM;
                case PixelFormat::RG16Snorm:        return DXGI_FORMAT_R16G16_SNORM;
                case PixelFormat::RG16Uint:         return DXGI_FORMAT_R16G16_UINT;
                case PixelFormat::RG16Sint:         return DXGI_FORMAT_R16G16_SINT;
                case PixelFormat::RG16Float:        return DXGI_FORMAT_R16G16_FLOAT;
                case PixelFormat::RGBA8UNorm:       return DXGI_FORMAT_R8G8B8A8_UNORM;
                case PixelFormat::RGBA8UNormSrgb:   return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                case PixelFormat::RGBA8SNorm:       return DXGI_FORMAT_R8G8B8A8_SNORM;
                case PixelFormat::RGBA8Uint:        return DXGI_FORMAT_R8G8B8A8_UINT;
                case PixelFormat::RGBA8Sint:        return DXGI_FORMAT_R8G8B8A8_SINT;
                case PixelFormat::BGRA8UNorm:       return DXGI_FORMAT_B8G8R8A8_UNORM;
                case PixelFormat::BGRA8UNormSrgb:   return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
                    // Packed 32-Bit formats
                case PixelFormat::RGB10A2Unorm:     return DXGI_FORMAT_R10G10B10A2_UNORM;
                case PixelFormat::RG11B10Float:     return DXGI_FORMAT_R11G11B10_FLOAT;
                case PixelFormat::RGB9E5Float:      return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
                    // 64-Bit formats
                case PixelFormat::RG32Uint:         return DXGI_FORMAT_R32G32_UINT;
                case PixelFormat::RG32Sint:         return DXGI_FORMAT_R32G32_SINT;
                case PixelFormat::RG32Float:        return DXGI_FORMAT_R32G32_FLOAT;
                case PixelFormat::RGBA16Unorm:      return DXGI_FORMAT_R16G16B16A16_UNORM;
                case PixelFormat::RGBA16Snorm:      return DXGI_FORMAT_R16G16B16A16_SNORM;
                case PixelFormat::RGBA16Uint:       return DXGI_FORMAT_R16G16B16A16_UINT;
                case PixelFormat::RGBA16Sint:       return DXGI_FORMAT_R16G16B16A16_SINT;
                case PixelFormat::RGBA16Float:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
                    // 128-Bit formats
                case PixelFormat::RGBA32Uint:       return DXGI_FORMAT_R32G32B32A32_UINT;
                case PixelFormat::RGBA32Sint:       return DXGI_FORMAT_R32G32B32A32_SINT;
                case PixelFormat::RGBA32Float:      return DXGI_FORMAT_R32G32B32A32_FLOAT;
                    // Depth-stencil formats
                case PixelFormat::Depth16Unorm:			return DXGI_FORMAT_D16_UNORM;
                case PixelFormat::Depth32Float:			return DXGI_FORMAT_D32_FLOAT;
                case PixelFormat::Depth24UnormStencil8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
                case PixelFormat::Depth32FloatStencil8: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
                    // Compressed BC formats
                case PixelFormat::BC1RGBAUnorm:         return DXGI_FORMAT_BC1_UNORM;
                case PixelFormat::BC1RGBAUnormSrgb:     return DXGI_FORMAT_BC1_UNORM_SRGB;
                case PixelFormat::BC2RGBAUnorm:         return DXGI_FORMAT_BC2_UNORM;
                case PixelFormat::BC2RGBAUnormSrgb:     return DXGI_FORMAT_BC2_UNORM_SRGB;
                case PixelFormat::BC3RGBAUnorm:         return DXGI_FORMAT_BC3_UNORM;
                case PixelFormat::BC3RGBAUnormSrgb:     return DXGI_FORMAT_BC3_UNORM_SRGB;
                case PixelFormat::BC4RSnorm:            return DXGI_FORMAT_BC4_SNORM;
                case PixelFormat::BC4RUnorm:            return DXGI_FORMAT_BC4_UNORM;
                case PixelFormat::BC5RGSnorm:           return DXGI_FORMAT_BC5_SNORM;
                case PixelFormat::BC5RGUnorm:           return DXGI_FORMAT_BC5_UNORM;
                case PixelFormat::BC6HRGBFloat:         return DXGI_FORMAT_BC6H_SF16;
                case PixelFormat::BC6HRGBUfloat:        return DXGI_FORMAT_BC6H_UF16;
                case PixelFormat::BC7RGBAUnorm:         return DXGI_FORMAT_BC7_UNORM;
                case PixelFormat::BC7RGBAUnormSrgb:     return DXGI_FORMAT_BC7_UNORM_SRGB;

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
                case DXGI_FORMAT_R8_UNORM:	return PixelFormat::R8Unorm;
                case DXGI_FORMAT_R8_SNORM:	return PixelFormat::R8Snorm;
                case DXGI_FORMAT_R8_UINT:	return PixelFormat::R8Uint;
                case DXGI_FORMAT_R8_SINT:	return PixelFormat::R8Sint;
                    // 16-bit formats
                case DXGI_FORMAT_R16_UNORM:		return PixelFormat::R16Unorm;
                case DXGI_FORMAT_R16_SNORM:		return PixelFormat::R16Snorm;
                case DXGI_FORMAT_R16_UINT:		return PixelFormat::R16Uint;
                case DXGI_FORMAT_R16_SINT:		return PixelFormat::R16Sint;
                case DXGI_FORMAT_R16_FLOAT:		return PixelFormat::R16Float;
                case DXGI_FORMAT_R8G8_UNORM:	return PixelFormat::RG8Unorm;
                case DXGI_FORMAT_R8G8_SNORM:	return PixelFormat::RG8Snorm;
                case DXGI_FORMAT_R8G8_UINT:		return PixelFormat::RG8Uint;
                case DXGI_FORMAT_R8G8_SINT:		return PixelFormat::RG8Sint;
                    // 32-bit formats
                case DXGI_FORMAT_R32_UINT:				return PixelFormat::R32Uint;
                case DXGI_FORMAT_R32_SINT:				return PixelFormat::R32Sint;
                case DXGI_FORMAT_R32_FLOAT:				return PixelFormat::R32Float;
                case DXGI_FORMAT_R16G16_UNORM:			return PixelFormat::RG16Unorm;
                case DXGI_FORMAT_R16G16_SNORM:			return PixelFormat::RG16Snorm;
                case DXGI_FORMAT_R16G16_UINT:			return PixelFormat::RG16Uint;
                case DXGI_FORMAT_R16G16_SINT:			return PixelFormat::RG16Sint;
                case DXGI_FORMAT_R16G16_FLOAT:			return PixelFormat::RG16Float;
                case DXGI_FORMAT_R8G8B8A8_UNORM:		return PixelFormat::RGBA8UNorm;
                case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	return PixelFormat::RGBA8UNormSrgb;
                case DXGI_FORMAT_R8G8B8A8_SNORM:		return PixelFormat::RGBA8SNorm;
                case DXGI_FORMAT_R8G8B8A8_UINT:			return PixelFormat::RGBA8Uint;
                case DXGI_FORMAT_R8G8B8A8_SINT:			return PixelFormat::RGBA8Sint;
                case DXGI_FORMAT_B8G8R8A8_UNORM:		return PixelFormat::BGRA8UNorm;
                case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:	return PixelFormat::BGRA8UNormSrgb;
                    // Packed 32-Bit formats
                case DXGI_FORMAT_R10G10B10A2_UNORM:		return PixelFormat::RGB10A2Unorm;
                case DXGI_FORMAT_R11G11B10_FLOAT:		return PixelFormat::RG11B10Float;
                case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:    return PixelFormat::RGB9E5Float;
                    // 64-Bit formats
                case DXGI_FORMAT_R32G32_UINT:			return PixelFormat::RG32Uint;
                case DXGI_FORMAT_R32G32_SINT:			return PixelFormat::RG32Sint;
                case DXGI_FORMAT_R32G32_FLOAT:			return PixelFormat::RG32Float;
                case DXGI_FORMAT_R16G16B16A16_UNORM:	return PixelFormat::RGBA16Unorm;
                case DXGI_FORMAT_R16G16B16A16_SNORM:	return PixelFormat::RGBA16Snorm;
                case DXGI_FORMAT_R16G16B16A16_UINT:		return PixelFormat::RGBA16Uint;
                case DXGI_FORMAT_R16G16B16A16_SINT:		return PixelFormat::RGBA16Sint;
                case DXGI_FORMAT_R16G16B16A16_FLOAT:	return PixelFormat::RGBA16Float;
                    // 128-Bit formats
                case DXGI_FORMAT_R32G32B32A32_UINT:		return PixelFormat::RGBA32Uint;
                case DXGI_FORMAT_R32G32B32A32_SINT:		return PixelFormat::RGBA32Sint;
                case DXGI_FORMAT_R32G32B32A32_FLOAT:	return PixelFormat::RGBA32Float;
                    // Depth-stencil formats
                case DXGI_FORMAT_D16_UNORM:				return PixelFormat::Depth16Unorm;
                case DXGI_FORMAT_D32_FLOAT:				return PixelFormat::Depth32Float;
                case DXGI_FORMAT_D24_UNORM_S8_UINT:		return PixelFormat::Depth24UnormStencil8;
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:	return PixelFormat::Depth32FloatStencil8;
                    // Compressed BC formats
                case DXGI_FORMAT_BC1_UNORM:			return PixelFormat::BC1RGBAUnorm;
                case DXGI_FORMAT_BC1_UNORM_SRGB:	return PixelFormat::BC1RGBAUnormSrgb;
                case DXGI_FORMAT_BC2_UNORM:			return PixelFormat::BC2RGBAUnorm;
                case DXGI_FORMAT_BC2_UNORM_SRGB:	return PixelFormat::BC2RGBAUnormSrgb;
                case DXGI_FORMAT_BC3_UNORM:			return PixelFormat::BC3RGBAUnorm;
                case DXGI_FORMAT_BC3_UNORM_SRGB:	return PixelFormat::BC3RGBAUnormSrgb;
                case DXGI_FORMAT_BC4_SNORM:			return PixelFormat::BC4RSnorm;
                case DXGI_FORMAT_BC4_UNORM:			return PixelFormat::BC4RUnorm;
                case DXGI_FORMAT_BC5_SNORM:			return PixelFormat::BC5RGSnorm;
                case DXGI_FORMAT_BC5_UNORM:			return PixelFormat::BC5RGUnorm;
                case DXGI_FORMAT_BC6H_SF16:			return PixelFormat::BC6HRGBFloat;
                case DXGI_FORMAT_BC6H_UF16:			return PixelFormat::BC6HRGBUfloat;
                case DXGI_FORMAT_BC7_UNORM:			return PixelFormat::BC7RGBAUnorm;
                case DXGI_FORMAT_BC7_UNORM_SRGB:	return PixelFormat::BC7RGBAUnormSrgb;

                default:
                    ALIMER_UNREACHABLE();
                    return PixelFormat::Undefined;
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

                case PixelFormat::RGB10A2Unorm:
                    return DXGI_FORMAT_R10G10B10A2_UNORM;
            }

            return DXGI_FORMAT_B8G8R8A8_UNORM;
        }

        [[nodiscard]] constexpr DXGI_FORMAT GetTypelessFormatFromDepthFormat(PixelFormat format)
        {
            switch (format)
            {
                case PixelFormat::Depth16Unorm:
                    return DXGI_FORMAT_R16_TYPELESS;
                case PixelFormat::Depth32Float:
                    return DXGI_FORMAT_R32_TYPELESS;
                case PixelFormat::Depth24UnormStencil8:
                    return DXGI_FORMAT_R24G8_TYPELESS;
                case PixelFormat::Depth32FloatStencil8:
                    return DXGI_FORMAT_R32G8X24_TYPELESS;

                default:
                    ALIMER_ASSERT(IsDepthFormat(format) == false);
                    return ToDXGIFormat(format);
            }
        }

        [[nodiscard]] constexpr D3D12_RESOURCE_DIMENSION ToD3D12(RHITextureDimension dimension)
        {
            switch (dimension)
            {
                case RHITextureDimension::Texture1D:
                    return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
                case RHITextureDimension::Texture2D:
                case RHITextureDimension::TextureCube:
                    return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                case RHITextureDimension::Texture3D:
                    return D3D12_RESOURCE_DIMENSION_TEXTURE3D;

                default:
                    ALIMER_UNREACHABLE();
                    return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            }
        }

        [[nodiscard]] constexpr uint32_t D3D12SampleCount(RHITextureSampleCount count)
        {
            switch (count)
            {
                case RHITextureSampleCount::Count1:
                    return 1;
                case RHITextureSampleCount::Count2:
                    return 2;
                case RHITextureSampleCount::Count4:
                    return 4;
                case RHITextureSampleCount::Count8:
                    return 8;
                case RHITextureSampleCount::Count16:
                    return 16;
                case RHITextureSampleCount::Count32:
                    return 32;

                default:
                    ALIMER_UNREACHABLE();
                    return 1;
            }
        }
    }

    /* RHITextureD3D12 */
    RHITextureD3D12::RHITextureD3D12(RHIDeviceD3D12* device_, const RHITextureDescription& descriptor, ID3D12Resource* externalResource)
        : RHITexture(descriptor)
        , device(device_)
        , handle(externalResource)
    {
        if (externalResource == nullptr)
        {
            D3D12MA::ALLOCATION_DESC allocationDesc{};
            allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

            const bool isDepthStencil = IsDepthStencilFormat(format);

            D3D12_RESOURCE_DESC resourceDesc;
            resourceDesc.Dimension = ToD3D12(dimension);
            resourceDesc.Alignment = 0;
            resourceDesc.Width = width;
            resourceDesc.Height = height;
            if (dimension == RHITextureDimension::Texture3D)
            {
                resourceDesc.DepthOrArraySize = static_cast<UINT16>(depthOrArraySize);
            }
            else if (dimension == RHITextureDimension::TextureCube)
            {
                resourceDesc.DepthOrArraySize = static_cast<UINT16>(depthOrArraySize * kRHICubeMapSlices);
            }
            else
            {
                resourceDesc.DepthOrArraySize = static_cast<UINT16>(depthOrArraySize);
            }
            resourceDesc.MipLevels = static_cast<UINT16>(mipLevels);
            resourceDesc.Format = ToDXGIFormat(format);
            resourceDesc.SampleDesc.Count = D3D12SampleCount(sampleCount);
            resourceDesc.SampleDesc.Quality = 0;
            if (sampleCount > RHITextureSampleCount::Count1)
            {
                resourceDesc.SampleDesc.Quality = 0xFFFFFFFF;
            }
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            D3D12_CLEAR_VALUE clearValue = {};
            D3D12_CLEAR_VALUE* pClearValue = nullptr;

            if (Any(usage, RHITextureUsage::RenderTarget))
            {
                // Render targets and Depth/Stencil targets are always committed resources
                allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_COMMITTED;

                clearValue.Format = resourceDesc.Format;

                if (isDepthStencil)
                {
                    state |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
                    resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
                    if (!Any(usage, RHITextureUsage::ShaderRead))
                    {
                        resourceDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
                    }

                    clearValue.DepthStencil.Depth = 1.0f;
                }
                else
                {
                    state |= D3D12_RESOURCE_STATE_RENDER_TARGET;
                    resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                }

                pClearValue = &clearValue;
            }

            if (Any(usage, RHITextureUsage::ShaderWrite))
            {
                // Remove unsupported flags.
                resourceDesc.Flags &= ~D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
                resourceDesc.Flags &= ~D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

                // Remove unsupported states.
                state &= ~D3D12_RESOURCE_STATE_DEPTH_READ;
                state &= ~D3D12_RESOURCE_STATE_DEPTH_WRITE;
                state &= ~D3D12_RESOURCE_STATE_RENDER_TARGET;

                state |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            }

            // If depth and either ua or sr, set to typeless
            if (IsDepthFormat(format)
                && Any(usage, RHITextureUsage::ShaderReadWrite))
            {
                resourceDesc.Format = GetTypelessFormatFromDepthFormat(format);
                pClearValue = nullptr;
            }

            HRESULT result = device->GetAllocator()->CreateResource(&allocationDesc,
                &resourceDesc,
                state,
                pClearValue,
                &allocation,
                IID_PPV_ARGS(&handle)
            );

            if (FAILED(result))
            {
                LOGE("Direct3D12: Failed to create texture: {}", std::to_string(result));
                return;
            }
        }
        else
        {
            handle->AddRef();
        }

        if (!descriptor.name.empty())
        {
            SetName(descriptor.name);
        }
    }

    RHITextureD3D12::~RHITextureD3D12()
    {
        Destroy();
    }

    void RHITextureD3D12::Destroy()
    {
        device->DeferDestroy(handle, allocation);
    }

    void RHITextureD3D12::ApiSetName(const StringView& name)
    {
        auto wName = ToUtf16(name);
        ThrowIfFailed(handle->SetName(wName.c_str()));
    }

    RHITextureView* RHITextureD3D12::CreateView(const RHITextureViewDescription& description) const
    {
        return new RHITextureViewD3D12(device, this, description);
    }

    /* RHITextureViewD3D12 */
    RHITextureViewD3D12::RHITextureViewD3D12(RHIDeviceD3D12* device_, const RHITextureD3D12* resource, const RHITextureViewDescription& desc)
        : RHITextureView(resource, desc)
        , device(device_)
    {
        const RHITextureUsage usage = resource->GetUsage();

        if (Any(usage, RHITextureUsage::RenderTarget))
        {
            if (IsDepthStencilFormat(format))
            {
                dsvHandle = device->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                device->GetD3DDevice()->CreateDepthStencilView(resource->GetHandle(), nullptr, dsvHandle);
            }
            else
            {
                rtvHandle = CreateRTV(resource);
            }
        }
    }

    RHITextureViewD3D12::~RHITextureViewD3D12()
    {
        if (rtvHandle.ptr != 0)
        {
            device->FreeDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvHandle);
        }

        if (dsvHandle.ptr != 0)
        {
            device->FreeDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, dsvHandle);
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RHITextureViewD3D12::CreateRTV(const RHITextureD3D12* resource) const
    {
        const uint32_t arrayMultiplier = (resource->GetDimension() == RHITextureDimension::TextureCube) ? kRHICubeMapSlices : 1;

        D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
        viewDesc.Format = ToDXGIFormat(format);

        switch (resource->GetDimension())
        {
            case RHITextureDimension::Texture1D:
                if (resource->GetArrayLayers() > 1)
                {
                    viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                    viewDesc.Texture1DArray.MipSlice = baseMipLevel;
                    viewDesc.Texture1DArray.FirstArraySlice = baseArrayLayer;
                    viewDesc.Texture1DArray.ArraySize = arrayLayers;

                }
                else
                {
                    viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                    viewDesc.Texture1D.MipSlice = baseMipLevel;
                }
                break;
            case RHITextureDimension::Texture2D:
            case RHITextureDimension::TextureCube:
                if (resource->GetSampleCount() > RHITextureSampleCount::Count1)
                {
                    if (resource->GetArrayLayers() > 1)
                    {
                        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        viewDesc.Texture2DMSArray.FirstArraySlice = baseArrayLayer;
                        viewDesc.Texture2DMSArray.ArraySize = arrayLayers;
                    }
                    else
                    {
                        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
                    }
                }
                else
                {
                    if (resource->GetArrayLayers() * arrayMultiplier > 1)
                    {
                        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                        viewDesc.Texture2DArray.MipSlice = baseMipLevel;
                        viewDesc.Texture2DArray.FirstArraySlice = baseArrayLayer;
                        viewDesc.Texture2DArray.ArraySize = arrayLayers;
                        viewDesc.Texture2DArray.PlaneSlice = 0;
                    }
                    else
                    {

                        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                        viewDesc.Texture2D.MipSlice = baseMipLevel;
                        viewDesc.Texture2D.PlaneSlice = 0;
                    }
                }
                break;
            case RHITextureDimension::Texture3D:
                viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                viewDesc.Texture3D.MipSlice = baseMipLevel;
                viewDesc.Texture3D.FirstWSlice = 0;
                viewDesc.Texture3D.WSize = resource->GetDepth(baseMipLevel);
                break;
            default:
                ALIMER_UNREACHABLE();
                break;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE handle = device->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        device->GetD3DDevice()->CreateRenderTargetView(resource->GetHandle(), &viewDesc, handle);
        return handle;
    }

    /* RHIBufferD3D12 */
    RHIBufferD3D12::RHIBufferD3D12(RHIDeviceD3D12* device, const RHIBufferDescription& desc, const void* initialData)
        : RHIBuffer(desc)
        , device{ device }
    {
        UINT64 alignedSize = desc.size;
        if (Any(desc.usage, RHIBufferUsage::Uniform))
        {
            alignedSize = AlignTo(alignedSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        }

        D3D12_RESOURCE_DESC resourceDesc;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = alignedSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        if (Any(desc.usage, RHIBufferUsage::Storage))
        {
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        D3D12MA::ALLOCATION_DESC allocationDesc = {};
        if (Any(desc.usage, RHIBufferUsage::MapRead))
        {
            allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
            state = D3D12_RESOURCE_STATE_COPY_DEST;
        }
        else if (Any(desc.usage, RHIBufferUsage::MapWrite))
        {
            allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
            state = D3D12_RESOURCE_STATE_GENERIC_READ;
        }
        else
        {
            allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
            state = D3D12_RESOURCE_STATE_COMMON;
        }

        HRESULT result = device->GetAllocator()->CreateResource(
            &allocationDesc, &resourceDesc,
            state,
            nullptr,
            &allocation, IID_PPV_ARGS(&handle)
        );

        if (FAILED(result))
        {
            LOGE("Direct3D12: Failed to create buffer: {}", std::to_string(result));
            return;
        }

        if (!desc.name.empty())
        {
            SetName(desc.name);
        }


        gpuVirtualAddress = handle->GetGPUVirtualAddress();
        device->GetD3DDevice()->GetCopyableFootprints(&resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &allocatedSize);

        // Issue data copy on request:
        if (initialData != nullptr)
        {
            RHIUploadContextD3D12 context = device->Allocate(desc.size);
            memcpy(context.data, initialData, desc.size);

            context.commandList->CopyBufferRegion(
                handle,
                0,
                context.uploadBuffer,
                0,
                desc.size
            );

            device->Submit(context);
        }
    }

    RHIBufferD3D12::~RHIBufferD3D12()
    {
        Destroy();
    }

    void RHIBufferD3D12::Destroy()
    {
        device->DeferDestroy(handle, allocation);
    }

    void RHIBufferD3D12::ApiSetName(const StringView& name)
    {
        auto wName = ToUtf16(name);
        ThrowIfFailed(handle->SetName(wName.c_str()));
    }

    /* RHISwapChainD3D12 */
    RHISwapChainD3D12::RHISwapChainD3D12(RHIDeviceD3D12* device_, void* window, const RHISwapChainDescription& desc)
        : RHISwapChain(desc)
        , device(device_)
    {
        if (!desc.verticalSync)
        {
            syncInterval = 0;
            if (device->IsTearingSupported())
            {
                presentFlags = DXGI_PRESENT_ALLOW_TEARING;
            }
        }

        // We use DXGI_FORMAT_B8G8R8A8_UNORM as default on D3D12
        if (desc.format == PixelFormat::Undefined)
        {
            colorFormat = PixelFormat::BGRA8UNorm;
        }

        HRESULT hr;

        ComPtr<IDXGISwapChain1> tempSwapChain;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = size.width;
        swapChainDesc.Height = size.height;
        swapChainDesc.Format = ToDXGISwapChainFormat(colorFormat);
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = kRHIMaxBackBufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = (device->IsTearingSupported()) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = !isFullscreen;

        hr = device->GetDXGIFactory()->CreateSwapChainForHwnd(
            device->GetGraphicsQueue(),
            (HWND)window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            tempSwapChain.GetAddressOf()
        );

        // Prevent DXGI from responding to the ALT+ENTER shortcut
        ThrowIfFailed(device->GetDXGIFactory()->MakeWindowAssociation((HWND)window, DXGI_MWA_NO_ALT_ENTER));
#else
        swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

        hr = dxgiFactory->CreateSwapChainForCoreWindow(
            device->GetGraphicsQueue(),
            static_cast<IUnknown*>(winrt::get_abi(*window)),
            &swapChainDesc,
            nullptr,
            tempSwapChain.GetAddressOf()
        );
#endif

        if (FAILED(hr))
        {
            return;
        }

        hr = tempSwapChain->QueryInterface(&handle);
        if (FAILED(hr))
        {
            return;
        }

        AfterReset();
    }

    RHISwapChainD3D12::~RHISwapChainD3D12()
    {
        Destroy();
    }

    void RHISwapChainD3D12::Destroy()
    {
        SafeRelease(handle);
    }

    void RHISwapChainD3D12::AfterReset()
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ThrowIfFailed(handle->GetDesc1(&swapChainDesc));

        size.width = swapChainDesc.Width;
        size.height = swapChainDesc.Height;

        backBuffers.resize(swapChainDesc.BufferCount);
        backBufferViews.resize(swapChainDesc.BufferCount);

        RHITextureDescription textureDesc = RHITextureDescription::Texture2D(
            PixelFormat::BGRA8UNorm,
            size.width, size.height,
            1, 1,
            RHITextureUsage::RenderTarget);

        RHITextureViewDescription textureViewDesc = {};
        textureViewDesc.format = colorFormat;

        for (uint32_t i = 0; i < swapChainDesc.BufferCount; ++i)
        {
            ComPtr<ID3D12Resource> backbuffer;
            ThrowIfFailed(handle->GetBuffer(i, IID_PPV_ARGS(&backbuffer)));

            backBuffers[i] = std::make_shared<RHITextureD3D12>(device, textureDesc, backbuffer.Get());
            backBufferViews[i] = backBuffers[i]->GetView(textureViewDesc);
        }
    }

    void RHISwapChainD3D12::Present()
    {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        // Note this will fail if in true 'fullscreen' mode.

        HRESULT hr = handle->Present(syncInterval, presentFlags);

        // If the device was reset we must completely reinitialize the renderer.
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
#ifdef _DEBUG
            //char buff[64] = {};
            //sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
            //    static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr));
            //OutputDebugStringA(buff);
#endif
            //HandleDeviceLost();
        }
        else
        {
            ThrowIfFailed(hr);
        }
    }


    void RHISwapChainD3D12::ApiSetName(const StringView& name)
    {

    }

    RHITextureView* RHISwapChainD3D12::GetCurrentTextureView() const
    {
        uint32_t backbufferIndex = handle->GetCurrentBackBufferIndex();
        return backBufferViews[backbufferIndex];
    }

    [[nodiscard]] inline const RHIBufferD3D12* ToInternal(const RHIBuffer* resource)
    {
        return static_cast<const RHIBufferD3D12*>(resource);
    }

    [[nodiscard]] inline RHISwapChainD3D12* ToInternal(RHISwapChain* resource)
    {
        return static_cast<RHISwapChainD3D12*>(resource);
    }

    /* RHICommandBufferD3D12 */
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
        swapChainTexture = nullptr;
    }

    void RHICommandBufferD3D12::PresentSwapChains()
    {
        for (auto& swapChain : swapChains)
        {
            ToInternal(swapChain)->Present();
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

    void RHICommandBufferD3D12::PushDebugGroup(const StringView& name)
    {
        auto wideName = ToUtf16(name);
        PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, wideName.c_str());
    }

    void RHICommandBufferD3D12::PopDebugGroup()
    {
        PIXEndEvent(commandList);
    }

    void RHICommandBufferD3D12::InsertDebugMarker(const StringView& name)
    {
        auto wideName = ToUtf16(name);
        PIXSetMarker(commandList, PIX_COLOR_DEFAULT, wideName.c_str());
    }

    void RHICommandBufferD3D12::BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor)
    {
        swapChains.push_back(swapChain);

        RHITextureViewD3D12* view = static_cast<RHITextureViewD3D12*>(swapChain->GetCurrentTextureView());
        swapChainTexture = static_cast<const RHITextureD3D12*>(view->GetResource());

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = swapChainTexture->GetHandle();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resourceBarriers.push_back(barrier);
        FlushBarriers();

        D3D12_RENDER_PASS_RENDER_TARGET_DESC RTV = {};
        RTV.cpuDescriptor = view->GetRTV();
        RTV.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        RTV.BeginningAccess.Clear.ClearValue.Color[0] = clearColor.r;
        RTV.BeginningAccess.Clear.ClearValue.Color[1] = clearColor.g;
        RTV.BeginningAccess.Clear.ClearValue.Color[2] = clearColor.b;
        RTV.BeginningAccess.Clear.ClearValue.Color[3] = clearColor.a;
        RTV.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        commandList->BeginRenderPass(1, &RTV, nullptr, D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES);
    }

    void RHICommandBufferD3D12::EndRenderPass()
    {
        commandList->EndRenderPass();

        if (swapChainTexture != nullptr)
        {
            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = swapChainTexture->GetHandle();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            resourceBarriers.push_back(barrier);
        }
    }

    void RHICommandBufferD3D12::SetViewport(const RHIViewport& viewport)
    {
        commandList->RSSetViewports(1, (D3D12_VIEWPORT*)&viewport);
    }

    void RHICommandBufferD3D12::SetViewports(const RHIViewport* viewports, uint32_t count)
    {
        commandList->RSSetViewports(count, (D3D12_VIEWPORT*)viewports);
    }

    void RHICommandBufferD3D12::SetStencilReference(uint32_t value)
    {
        commandList->OMSetStencilRef(value);
    }

    void RHICommandBufferD3D12::SetBlendColor(const RHIColor& color)
    {
        commandList->OMSetBlendFactor(&color.r);
    }

    void RHICommandBufferD3D12::SetBlendColor(const float blendColor[4])
    {
        commandList->OMSetBlendFactor(blendColor);
    }

    void RHICommandBufferD3D12::SetIndexBufferCore(const RHIBuffer* buffer, RHIIndexType indexType, uint32_t offset)
    {
        D3D12_INDEX_BUFFER_VIEW view = {};
        view.BufferLocation = ToInternal(buffer)->GetGpuVirtualAddress() + (D3D12_GPU_VIRTUAL_ADDRESS)offset;
        view.SizeInBytes = (UINT)(buffer->GetSize() - offset);
        view.Format = (indexType == RHIIndexType::UInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);
        commandList->IASetIndexBuffer(&view);
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

        locker.lock();
        for (size_t i = 0, count = freeList.size(); i < count; ++i)
        {
            SafeRelease(freeList[i].commandAllocator);
            SafeRelease(freeList[i].commandList);
            SafeRelease(freeList[i].fence);
            SafeRelease(freeList[i].uploadBufferAllocation);
            SafeRelease(freeList[i].uploadBuffer);
        }
        freeList.clear();
        locker.unlock();
        SafeRelease(queue);
    }

    RHIUploadContextD3D12 RHIDeviceD3D12::CopyAllocator::Allocate(uint32_t size)
    {
        locker.lock();

        // Create a new command list if there are none free.
        if (freeList.empty())
        {
            RHIUploadContextD3D12 context;

            ThrowIfFailed(
                device->d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&context.commandAllocator))
            );
            ThrowIfFailed(
                device->d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, context.commandAllocator, nullptr, IID_PPV_ARGS(&context.commandList))
            );

            ThrowIfFailed(context.commandList->Close());
            ThrowIfFailed(device->d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&context.fence)));

            freeList.push_back(context);
        }

        RHIUploadContextD3D12 context = freeList.back();
        if (context.uploadBuffer == nullptr
            || context.uploadBuffer->GetDesc().Width < size)
        {
            // Try to search for a staging buffer that can fit the request:
            for (size_t i = 0; i < freeList.size(); ++i)
            {
                if (freeList[i].uploadBuffer != nullptr
                    && freeList[i].uploadBuffer->GetDesc().Width >= size)
                {
                    context = freeList[i];
                    std::swap(freeList[i], freeList.back());
                    break;
                }
            }
        }
        freeList.pop_back();
        locker.unlock();

        // If no buffer was found that fits the data, create one:
        if (context.uploadBuffer == nullptr
            || context.uploadBuffer->GetDesc().Width < size)
        {
            D3D12MA::ALLOCATION_DESC allocationDesc = {};
            allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

            auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(NextPowerOfTwo(size));

            HRESULT hr = device->GetAllocator()->CreateResource(
                &allocationDesc, &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                &context.uploadBufferAllocation, IID_PPV_ARGS(&context.uploadBuffer)
            );

            ThrowIfFailed(hr);
            ThrowIfFailed(context.uploadBuffer->Map(0, nullptr, &context.data));
        }

        // begin command list in valid state:
        ThrowIfFailed(context.commandAllocator->Reset());
        ThrowIfFailed(context.commandList->Reset(context.commandAllocator, nullptr));

        return context;
    }

    void RHIDeviceD3D12::CopyAllocator::Submit(RHIUploadContextD3D12 context)
    {
        ThrowIfFailed(context.commandList->Close());

        ID3D12CommandList* commandlists[] = {
            context.commandList
        };
        queue->ExecuteCommandLists(1, commandlists);
        ThrowIfFailed(queue->Signal(context.fence, 1));
        ThrowIfFailed(context.fence->SetEventOnCompletion(1, nullptr));
        ThrowIfFailed(context.fence->Signal(0));

        locker.lock();
        freeList.push_back(context);
        locker.unlock();
    }

    /* RHIDeviceD3D12 */
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

        if (SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
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

        // Descriptor Allocators
        {
            resourceDescriptorAllocator.Initialize(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024);
            samplerDescriptorAllocator.Initialize(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024);
            rtvDescriptorAllocator.Initialize(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1024);
            dsvDescriptorAllocator.Initialize(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 256);
        }

        // Shader visible descriptor resource heap and bindless.
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            heapDesc.NumDescriptors = 1000000; // tier 1 limit
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&resourceHeap.handle)));
            resourceHeap.CPUStart = resourceHeap.handle->GetCPUDescriptorHandleForHeapStart();
            resourceHeap.GPUStart = resourceHeap.handle->GetGPUDescriptorHandleForHeapStart();

            ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&resourceHeap.fence)));
            resourceHeap.fenceValue = resourceHeap.fence->GetCompletedValue();

            for (uint32_t i = 0; i < kBindlessResourceCapacity; ++i)
            {
                freeBindlessResources.push_back(kBindlessResourceCapacity - i - 1);
            }
        }

        // Shader visible descriptor sampler heap and bindless.
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            heapDesc.NumDescriptors = 2048; // tier 1 limit
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&samplerHeap.handle)));
            samplerHeap.CPUStart = samplerHeap.handle->GetCPUDescriptorHandleForHeapStart();
            samplerHeap.GPUStart = samplerHeap.handle->GetGPUDescriptorHandleForHeapStart();

            ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&samplerHeap.fence)));
            samplerHeap.fenceValue = samplerHeap.fence->GetCompletedValue();

            for (uint32_t i = 0; i < kBindlessSamplerCapacity; ++i)
            {
                freeBindlessSamplers.push_back(kBindlessSamplerCapacity - i - 1);
            }
        }

        return true;
    }

    void RHIDeviceD3D12::Shutdown()
    {
        WaitForIdle();
        copyAllocator.Shutdown();

        shuttingDown = true;

        // CPU Descriptor Heaps
        rtvDescriptorAllocator.Shutdown();
        dsvDescriptorAllocator.Shutdown();
        resourceDescriptorAllocator.Shutdown();
        samplerDescriptorAllocator.Shutdown();

        // GPU Descriptor Heaps
        SafeRelease(resourceHeap.handle);
        SafeRelease(resourceHeap.fence);
        SafeRelease(samplerHeap.handle);
        SafeRelease(samplerHeap.fence);

        frameCount = UINT64_MAX;
        ProcessDeletionQueue();
        frameCount = 0;

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

        // Descriptor heaps' progress is recorded by the GPU:
        resourceHeap.fenceValue = resourceHeap.allocationOffset.load();
        ThrowIfFailed(
            queues[(uint32_t)RHIQueueType::Graphics].handle->Signal(resourceHeap.fence, resourceHeap.fenceValue)
        );
        resourceHeap.cachedCompletedValue = resourceHeap.fence->GetCompletedValue();

        samplerHeap.fenceValue = samplerHeap.allocationOffset.load();
        ThrowIfFailed(
            queues[(uint32_t)RHIQueueType::Graphics].handle->Signal(samplerHeap.fence, samplerHeap.fenceValue)
        );
        samplerHeap.cachedCompletedValue = samplerHeap.fence->GetCompletedValue();

        // Delete pending resources.
        ProcessDeletionQueue();
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

    void RHIDeviceD3D12::ProcessDeletionQueue()
    {
        std::lock_guard<std::mutex> guard(destroyMutex);

        while (!deferredAllocations.empty())
        {
            if (deferredAllocations.front().second + kRHIMaxFramesInFlight < frameCount)
            {
                auto item = deferredAllocations.front();
                deferredAllocations.pop_front();
                item.first->Release();
            }
            else
            {
                break;
            }
        }

        while (!deferredReleases.empty())
        {
            if (deferredReleases.front().second + kRHIMaxFramesInFlight < frameCount)
            {
                auto item = deferredReleases.front();
                deferredReleases.pop_front();
                item.first->Release();
            }
            else
            {
                break;
            }
        }

        while (!destroyerBindlessResource.empty())
        {
            if (destroyerBindlessResource.front().second + kRHIMaxFramesInFlight < frameCount)
            {
                int index = destroyerBindlessResource.front().first;
                destroyerBindlessResource.pop_front();
                freeBindlessResources.push_back(index);
            }
            else
            {
                break;
            }
        }

        while (!destroyerBindlessSampler.empty())
        {
            if (destroyerBindlessSampler.front().second + kRHIMaxFramesInFlight < frameCount)
            {
                int index = destroyerBindlessSampler.front().first;
                destroyerBindlessSampler.pop_front();
                freeBindlessSamplers.push_back(index);
            }
            else
            {
                break;
            }
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

    RHITextureRef RHIDeviceD3D12::CreateTexture(const RHITextureDescription& desc)
    {
        auto result = new RHITextureD3D12(this, desc);

        if (result->GetHandle())
            return RHITextureRef(result);

        delete result;
        return nullptr;
    }

    RHIBufferRef RHIDeviceD3D12::CreateBufferCore(const RHIBufferDescription& desc, const void* initialData)
    {
        auto result = new RHIBufferD3D12(this, desc, initialData);

        if (result->GetHandle())
            return RHIBufferRef(result);

        delete result;
        return nullptr;
    }

    RHISwapChainRef RHIDeviceD3D12::CreateSwapChain(void* window, const RHISwapChainDescription& desc)
    {
        auto result = new RHISwapChainD3D12(this, window, desc);

        if (result->GetHandle())
            return RHISwapChainRef(result);

        delete result;
        return nullptr;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RHIDeviceD3D12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
        switch (type)
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
                return resourceDescriptorAllocator.Allocate();
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
                return samplerDescriptorAllocator.Allocate();
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
                return rtvDescriptorAllocator.Allocate();
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
                return dsvDescriptorAllocator.Allocate();
            default:
                ALIMER_UNREACHABLE();
                return {};
        }
    }

    void RHIDeviceD3D12::FreeDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE handle)
    {
        switch (type)
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
                resourceDescriptorAllocator.Free(handle);
                break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
                samplerDescriptorAllocator.Free(handle);
                break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
                rtvDescriptorAllocator.Free(handle);
                break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
                dsvDescriptorAllocator.Free(handle);
                break;
            default:
                ALIMER_UNREACHABLE();
                break;
        }
    }

    uint32_t RHIDeviceD3D12::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
    {
        switch (type)
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
                return resourceDescriptorAllocator.descriptorSize;
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
                return samplerDescriptorAllocator.descriptorSize;
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
                return rtvDescriptorAllocator.descriptorSize;
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
                return dsvDescriptorAllocator.descriptorSize;
            default:
                ALIMER_UNREACHABLE();
        }
    }


    RHIUploadContextD3D12 RHIDeviceD3D12::Allocate(uint32_t size)
    {
        return copyAllocator.Allocate(size);
    }

    void RHIDeviceD3D12::Submit(RHIUploadContextD3D12 context)
    {
        copyAllocator.Submit(context);
    }

    void RHIDeviceD3D12::DeferDestroy(IUnknown* resource, D3D12MA::Allocation* allocation)
    {
        if (!resource && !allocation)
            return;

        std::lock_guard<std::mutex> guard(destroyMutex);

        if (shuttingDown)
        {
            resource->Release();
            SafeRelease(allocation);
            return;
        }

        deferredReleases.push_back(std::make_pair(resource, frameCount));
        if (allocation != nullptr)
        {
            deferredAllocations.push_back(std::make_pair(allocation, frameCount));
        }
    }
}

#endif /* defined(ALIMER_RHI_D3D12) */
