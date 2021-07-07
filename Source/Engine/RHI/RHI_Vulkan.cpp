// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_VULKAN)
#include "RHI_Vulkan.h"

namespace Alimer
{
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

    bool RHIDeviceVulkan::Initialize(RHIValidationMode validationMode)
    {
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
}

#endif /* defined(ALIMER_RHI_VULKAN) */
