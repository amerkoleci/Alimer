// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_VULKAN)
#include "RHI/RHI.h"

#include "PlatformInclude.h"
#include "volk.h"
#include "vk_mem_alloc.h"

namespace Alimer
{
    class RHICommandBufferVulkan final : public RHICommandBuffer
    {
    private:

    public:
        RHICommandBufferVulkan();
        ~RHICommandBufferVulkan() override;
    };

    class RHIDeviceVulkan final : public RHIDevice
    {
    public:
        [[nodiscard]] static bool IsAvailable();

        RHIDeviceVulkan(RHIValidationMode validationMode);
        ~RHIDeviceVulkan() override;

        bool Initialize(RHIValidationMode validationMode) override;
        void Shutdown() override;
        bool BeginFrame() override;
        void EndFrame() override;

        RHICommandBuffer* BeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics) override;

    private:
        bool debugUtils = false;
        VkInstance instance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;
    };
}

#endif /* defined(ALIMER_RHI_VULKAN) */
