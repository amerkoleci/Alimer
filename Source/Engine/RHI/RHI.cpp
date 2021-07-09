// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"
#include "Core/Assert.h"
#include "Core/Log.h"
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
    RHITextureView* RHIResource::GetView(const RHITextureViewDescriptor& descriptor) const
    {
        const size_t hash = Hash(descriptor);
        std::lock_guard<std::mutex> guard(resourceViewCacheMutex);
        auto it = resourceViewCache.find(hash);
        if (it == resourceViewCache.end())
        {
            // View not found, create new.
            RHITextureView* view = CreateView(descriptor);
            if (view != nullptr)
            {
                resourceViewCache[hash].reset(view);
                return view;
            }
            else
            {
                LOGE("RHI: Failed to create TextureView");
                return nullptr;
            }
        }

        return static_cast<RHITextureView*>(it->second.get());
    }

    /* RHIResourceView */
    RHIResourceView::RHIResourceView(const RHIResource* resource_)
        : resource(resource_)
    {
        ALIMER_ASSERT(resource != nullptr);
    }

    const RHIResource* RHIResourceView::GetResource() const
    {
        return resource;
    }

    /* RHITexture */
    RHITextureDescriptor RHITextureDescriptor::Create1D(
        PixelFormat format,
        uint32_t width,
        uint16_t mipLevels,
        uint16_t arraySize,
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

    RHITextureDescriptor RHITextureDescriptor::CreateCubemap(
        PixelFormat format,
        uint32_t width,
        uint16_t arraySize,
        uint16_t mipLevels,
        RHITextureUsage usage)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.size.width = width;
        descriptor.size.height = width;
        descriptor.arraySize = kRHICubeMapSlices * arraySize;
        descriptor.mipLevels = mipLevels;
        descriptor.format = format;
        return descriptor;
    }

    RHITextureDescriptor RHITextureDescriptor::Create3D(
        PixelFormat format,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint16_t mipLevels,
        RHITextureUsage usage)
    {
        RHITextureDescriptor descriptor;
        descriptor.usage = usage;
        descriptor.dimension = RHITextureDimension::Dimension3D;
        descriptor.size.width = width;
        descriptor.size.height = height;
        descriptor.size.depth = depth;
        descriptor.mipLevels = mipLevels;
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
    RHITextureView::RHITextureView(const RHITexture* resource, const RHITextureViewDescriptor& descriptor)
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

    /* RHISwapChain */
    RHISwapChain::RHISwapChain(const RHISwapChainDescriptor& descriptor)
        : size(descriptor.size)
        , colorFormat(descriptor.format)
        , verticalSync(descriptor.verticalSync)
        , isFullscreen(descriptor.isFullscreen)
    {

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

    RHITextureRef RHICreateTexture(const RHITextureDescriptor& descriptor)
    {
        return GRHIDevice->CreateTexture(descriptor);
    }

    RHISwapChainRef RHICreateSwapChain(void* window, const RHISwapChainDescriptor& descriptor)
    {
        ALIMER_ASSERT(window);

        return GRHIDevice->CreateSwapChain(window, descriptor);
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
