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
