// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_VULKAN)
#include "RHI_Vulkan.h"

namespace Alimer
{
    /* RHICommandBufferVulkan */
    RHICommandBufferVulkan::RHICommandBufferVulkan()
    {

    }

    RHICommandBufferVulkan::~RHICommandBufferVulkan()
    {

    }

    /* RHIDeviceVulkan */
    bool RHIDeviceVulkan::IsAvailable()
    {
        static bool available_initialized = false;
        static bool available = false;

        if (available_initialized) {
            return available;
        }

        available_initialized = true;
        return true;
    }

    RHIDeviceVulkan::RHIDeviceVulkan(RHIValidationMode validationMode)
    {

    }

    RHIDeviceVulkan::~RHIDeviceVulkan()
    {

    }

    bool RHIDeviceVulkan::Initialize(RHIValidationMode validationMode)
    {
        // Create command queue's
        {
            //graphicsQueue.reset(new RHICommandQueueVulkan());
            //computeQueue.reset(new RHICommandQueueVulkan());
        }

        return true;
    }

    void RHIDeviceVulkan::Shutdown()
    {

    }

    bool RHIDeviceVulkan::BeginFrame()
    {
        return true;
    }

    void RHIDeviceVulkan::EndFrame()
    {
    }

    RHICommandBuffer* RHIDeviceVulkan::BeginCommandBuffer(RHIQueueType type)
    {
        return nullptr;
    }
}

#endif /* defined(ALIMER_RHI_VULKAN) */
