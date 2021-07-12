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
    class RHIDeviceVulkan;

    class RHICommandBufferVulkan final : public RHICommandBuffer
    {
    public:
        RHICommandBufferVulkan(_In_ RHIDeviceVulkan* device, VkCommandPool commandPool);
        ~RHICommandBufferVulkan() override;

        void Reset(uint32_t frameIndex);

        void PushDebugGroup(const StringView& name) override;
        void PopDebugGroup() override;
        void InsertDebugMarker(const StringView& name) override;

        void BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor) override;
        void EndRenderPass() override;

        void SetViewport(const RHIViewport& viewport) override;
        void SetViewports(const RHIViewport* viewports, uint32_t count) override;

        void SetStencilReference(uint32_t value) override;
        void SetBlendColor(const RHIColor& color) override;
        void SetBlendColor(const float blendColor[4]) override;

        VkCommandBuffer GetHandle() const noexcept { return handle; }

    private:
        RHIDeviceVulkan* device;
        VkCommandPool commandPool;
        bool debugUtilsSupported{ false };

        VkCommandBuffer handle{ VK_NULL_HANDLE };
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

        bool DebugUtilsSupported() const noexcept { return debugUtils; }
        VkInstance GetInstance() const noexcept { return instance; }
        VkPhysicalDevice GetPhysicalDevice() const noexcept { return physicalDevice; }

        VkDevice GetHandle() const noexcept { return handle; }

    private:
        bool debugUtils = false;
        VkInstance instance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;

        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkDevice handle{ VK_NULL_HANDLE };
    };
}

#endif /* defined(ALIMER_RHI_VULKAN) */
