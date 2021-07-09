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
    RHITexture::RHITexture(const RHITextureDescriptor& descriptor)
        : dimension(descriptor.dimension)
        , format(descriptor.format)
        , usage(descriptor.usage)
        , width(descriptor.width)
        , height(descriptor.height)
        , depthOrArraySize(descriptor.depthOrArraySize)
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

        baseMipLevel = descriptor.baseMipLevel;
        mipLevelCount = descriptor.mipLevelCount;
        firstArraySlice = descriptor.firstArraySlice;
        arraySize = descriptor.arraySize;

        if (mipLevelCount == 0) {
            mipLevelCount = resource->GetMipLevels() - baseMipLevel;
        }

        if (arraySize == 0) {
            arraySize = resource->GetArraySize() - firstArraySlice;
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
