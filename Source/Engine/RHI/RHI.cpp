// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"
#include "RHI/RHI.h"

#if defined(ALIMER_RHI_D3D12)
#   include "RHI_D3D12.h"
#endif

#if defined(ALIMER_RHI_VULKAN)
#   include "RHI_Vulkan.h"
#endif

namespace Alimer
{
    /* RHIObject */
    void RHIObject::SetName(const String& newName)
    {
        name = newName;
        ApiSetName(name);
    }

    void RHIObject::SetName(const StringView& newName)
    {
        name = newName;
        ApiSetName(newName);
    }

    /* RHIResourceView */
    RHIResourceView::RHIResourceView(_In_ RHIResource* resource_)
        : resource(resource_)
    {
        ALIMER_ASSERT(resource != nullptr);
    }

    const RHIResource* RHIResourceView::GetResource() const
    {
        return resource;
    }

    RHIResource* RHIResourceView::GetResource()
    {
        return resource;
    }

    /* RHITexture */
    RHITextureDescriptor RHITextureDescriptor::Create1D(
        PixelFormat format,
        uint32_t width,
        uint32_t mipLevels,
        uint32_t arraySize,
        RHITextureUsage usage)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.dimension = RHITextureDimension::Dimension1D;
        descriptor.size.width = width;
        descriptor.format = format;
        descriptor.mipLevels = mipLevels;
        descriptor.arraySize = arraySize;
        return descriptor;
    }

    RHITextureDescriptor RHITextureDescriptor::Create2D(
        PixelFormat format,
        uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        RHITextureUsage usage)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.size.width = width;
        descriptor.size.height = height;
        descriptor.format = format;
        descriptor.mipLevels = mipLevels;
        return descriptor;
    }

    RHITextureDescriptor RHITextureDescriptor::Create2DArray(
        RHITextureUsage usage,
        uint32_t width,
        uint32_t height,
        uint16_t arraySize,
        PixelFormat format)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.size.width = width;
        descriptor.size.height = height;
        descriptor.arraySize = arraySize;
        descriptor.format = format;
        return descriptor;
    }

    RHITextureDescriptor RHITextureDescriptor::CreateCubemap(
        RHITextureUsage usage,
        uint32_t width,
        PixelFormat format)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.size.width = width;
        descriptor.size.height = width;
        descriptor.arraySize = kRHICubeMapSlices;
        descriptor.format = format;
        return descriptor;
    }

    RHITextureDescriptor RHITextureDescriptor::CreateCubemapArray(
        RHITextureUsage usage,
        uint32_t width,
        uint16_t arraySize,
        PixelFormat format)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.size.width = width;
        descriptor.size.height = width;
        descriptor.arraySize = kRHICubeMapSlices * arraySize;
        descriptor.format = format;
        return descriptor;
    }

    RHITextureDescriptor RHITextureDescriptor::Create3D(
        RHITextureUsage usage,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        PixelFormat format)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.dimension = RHITextureDimension::Dimension3D;
        descriptor.size.width = width;
        descriptor.size.height = height;
        descriptor.size.depth = depth;
        descriptor.format = format;
        return descriptor;
    }

    RHITexture::RHITexture(const RHITextureDescriptor& descriptor)
        : usage(descriptor.usage)
        , dimension(descriptor.dimension)
        , size(descriptor.size)
        , format(descriptor.format)
        , arraySize(descriptor.arraySize)
        , mipLevels(descriptor.mipLevels)
        , sampleCount(descriptor.sampleCount)
    {

    }

    /* RHITextureView */
    RHITextureView::RHITextureView(_In_ RHITexture* resource, const RHITextureViewDescriptor& descriptor)
        : RHIResourceView(resource)
    {
        if (descriptor.format == PixelFormat::Undefined)
        {
            format = resource->GetFormat();
        }
        else
        {
            format = descriptor.format;
        }

        if (descriptor.dimension == RHITextureViewDimension::Undefined)
        {
            switch (resource->GetDimension())
            {
                case RHITextureDimension::Dimension1D:
                    dimension = RHITextureViewDimension::Dimension1D;
                    break;
                case RHITextureDimension::Dimension2D:
                    if (resource->GetArraySize() > 1u && descriptor.arrayLayerCount == 0)
                    {
                        dimension = RHITextureViewDimension::Dimension2DArray;
                    }
                    else
                    {
                        dimension = RHITextureViewDimension::Dimension2D;
                    }
                    break;
                case RHITextureDimension::Dimension3D:
                    dimension = RHITextureViewDimension::Dimension3D;
                    break;
            }
        }
        else
        {
            dimension = descriptor.dimension;
        }

        baseMipLevel = descriptor.baseMipLevel;
        mipLevelCount = descriptor.mipLevelCount;
        baseArrayLayer = descriptor.baseArrayLayer;
        arrayLayerCount = descriptor.arrayLayerCount;

        if (mipLevelCount == 0) {
            mipLevelCount = resource->GetMipLevels() - baseMipLevel;
        }

        if (arrayLayerCount == 0) {
            arrayLayerCount = resource->GetArraySize() - baseArrayLayer;
        }
    }

    /* RHIDevice */
    RHIDevice* GRHIDevice = nullptr;

    bool RHInitialize(RHIValidationMode validationMode)
    {
        if (GRHIDevice != nullptr)
            return true;

#if defined(ALIMER_RHI_D3D12)
        GRHIDevice = new RHIDeviceD3D12(validationMode);
#endif

#if defined(ALIMER_RHI_VULKAN)
        //GRHIDevice = new RHIDeviceVulkan(validationMode);
#endif

        return GRHIDevice->Initialize(validationMode);
    }

    void RHIShutdown()
    {
        if (GRHIDevice != nullptr)
        {
            GRHIDevice->Shutdown();
            delete GRHIDevice;
            GRHIDevice = nullptr;
        }
    }

    bool RHIBeginFrame()
    {
        return GRHIDevice->BeginFrame();
    }

    void RHIEndFrame()
    {
        GRHIDevice->EndFrame();
    }

    RHICommandBuffer* RHIBeginCommandBuffer(RHIQueueType type)
    {
        return GRHIDevice->BeginCommandBuffer(type);
    }
}
