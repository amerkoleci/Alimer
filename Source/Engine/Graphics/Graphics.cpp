// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Graphics/Texture.h"
#include "Graphics/Buffer.h"
#include "Graphics/Pipeline.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Graphics.h"
#include "Core/Log.h"

#if defined(ALIMER_RHI_D3D12)
#include "Graphics/D3D12/D3D12Graphics.h"
#endif

#if defined(ALIMER_RHI_VULKAN)
#include "Graphics/Vulkan/VulkanGraphics.h"
#endif

namespace Alimer
{
    void Graphics::Destroy()
    {
        // Stock Samplers
        {
            //pointClampSampler.Reset();
            //pointWrapSampler.Reset();
            //linearClampSampler.Reset();
            //linearWrapSampler.Reset();
        }

        // Destroy pending resources that still exist.
        {
            std::lock_guard<std::mutex> lock(objectsMutex);

            for (GPUObject* resource : objects)
            {
                resource->Destroy();
            }

            objects.clear();
        }
    }

    bool Graphics::Initialize(GPUValidationMode validationMode, GPUBackendType backendType)
    {
        if (gGraphics().IsInitialized())
            return true;

        if (backendType == GPUBackendType::Count)
        {
#if defined(ALIMER_RHI_D3D12)
            if (D3D12Graphics::IsAvailable())
                backendType = GPUBackendType::Direct3D12;
#endif

            if (backendType == GPUBackendType::Count)
            {
#if defined(ALIMER_RHI_VULKAN)
                if (VulkanGraphics::IsAvailable())
                    backendType = GPUBackendType::Vulkan;
#endif
            }
        }

        switch (backendType)
        {
#if defined(ALIMER_RHI_D3D12)
            case GPUBackendType::Direct3D12:
                gGraphics().Start<D3D12Graphics>(validationMode);
                break;
#endif

#if defined(ALIMER_RHI_VULKAN)
            case GPUBackendType::Vulkan:
                gGraphics().Start<VulkanGraphics>(validationMode);
                break;
#endif

            default:
                LOGE("RHI: Cannot detect supported backend");
                return false;
        }

        return gGraphics().IsInitialized();
    }

	void Graphics::AddGPUObject(GPUObject* resource)
	{
        std::lock_guard<std::mutex> lock(objectsMutex);
        auto it = std::find(objects.begin(), objects.end(), resource);
        if (it == objects.end())
        {
            objects.push_back(resource);
        }
        else
        {
            LOGD("GPUObject already tracked");
        }
	}

	void Graphics::RemoveGPUObject(GPUObject* resource)
	{
		std::lock_guard<std::mutex> lock(objectsMutex);
		auto it = std::remove(objects.begin(), objects.end(), resource);
		if (it != objects.end())
		{
            objects.erase(it);
		}
	}

    CommandBuffer* Graphics::BeginCommandBuffer(CommandQueueType queueType)
    {
        return GetQueue(queueType).GetCommandBuffer();
    }

	Graphics& gGraphics()
	{
		return Graphics::Instance();
	}
}
