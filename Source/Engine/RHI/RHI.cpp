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

    RHITextureView* RHITexture::GetView(const RHITextureViewDescription& description) const
    {
        const size_t hash = Hash(description);
        std::lock_guard<std::mutex> guard(resourceViewCacheMutex);
        auto it = resourceViewCache.find(hash);
        if (it == resourceViewCache.end())
        {
            // View not found, create new.
            RHITextureView* view = CreateView(description);
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

    /* RHITextureView */
    RHITextureView::RHITextureView(const RHITexture* resource, const RHITextureViewDescription& description)
        : RHIResourceView(resource)
    {
        if (description.format == PixelFormat::Undefined)
        {
            format = resource->GetFormat();
        }
        else
        {
            format = description.format;
        }

        baseMipLevel = description.baseMipLevel;
        mipLevels = description.mipLevels;
        baseArrayLayer = description.baseArrayLayer;
        arrayLayers = description.arrayLayers;

        if (mipLevels == 0) {
            mipLevels = resource->GetMipLevels() - baseMipLevel;
        }

        if (arrayLayers == 0) {
            arrayLayers = resource->GetArrayLayers() - baseArrayLayer;
        }
    }

    /* RHIBuffer */
    RHIBuffer::RHIBuffer(const RHIBufferDescription& desc)
        : size(desc.size)
        , usage(desc.usage)
    {

    }

    /* RHISwapChain */
    RHISwapChain::RHISwapChain(const RHISwapChainDescriptor& desc)
        : size(desc.size)
        , colorFormat(desc.format)
        , verticalSync(desc.verticalSync)
        , isFullscreen(desc.isFullscreen)
    {

    }

    /* RHIDevice */
    RHIDevice* GRHIDevice = nullptr;

    RHIBufferRef RHIDevice::CreateBuffer(const RHIBufferDescription& desc, const void* initialData)
    {
        ALIMER_ASSERT(desc.size > 0);

        static constexpr uint64_t kMaxBufferSize = 128u * 1024u * 1024u;

        if (desc.size > kMaxBufferSize)
        {
            LOGE("Buffer size too large (size {})", desc.size);
            return nullptr;
        }

        return CreateBufferCore(desc, initialData);
    }

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

    RHITextureRef RHICreateTexture(const RHITextureDescriptor& desc)
    {
        return GRHIDevice->CreateTexture(desc);
    }

    RHIBufferRef RHICreateBuffer(const RHIBufferDescription& desc, const void* initialData)
    {
        return GRHIDevice->CreateBuffer(desc, initialData);
    }

    RHISwapChainRef RHICreateSwapChain(void* window, const RHISwapChainDescriptor& desc)
    {
        ALIMER_ASSERT(window);

        return GRHIDevice->CreateSwapChain(window, desc);
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
