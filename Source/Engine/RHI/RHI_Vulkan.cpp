// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_VULKAN) && defined(TODO)
#include "RHI_Vulkan.h"
#include "Core/Assert.h"
#include "Core/Log.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace Alimer
{

    constexpr const char* ToString(VkResult result)
    {
        switch (result)
        {
#define STR(r)   \
	case VK_##r: \
		return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
#undef STR
            default:
                return "UNKNOWN_ERROR";
        }
    }

    namespace
    {
        VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {
            std::string messageTypeStr = "General";

            if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
                messageTypeStr = "Validation";
            else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
                messageTypeStr = "Performance";

            // Log debug messge
            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            {
                LOGW("Vulkan - {}: {}", messageTypeStr.c_str(), pCallbackData->pMessage);
            }
            else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            {
                LOGE("Vulkan - {}: {}", messageTypeStr.c_str(), pCallbackData->pMessage);
            }

            return VK_FALSE;
        }

        bool ValidateLayers(const std::vector<const char*>& required,
            const std::vector<VkLayerProperties>& available)
        {
            for (auto layer : required)
            {
                bool found = false;
                for (auto& available_layer : available)
                {
                    if (strcmp(available_layer.layerName, layer) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    LOGW("Validation Layer '{}' not found", layer);
                    return false;
                }
            }

            return true;
        }

        std::vector<const char*> GetOptimalValidationLayers(const std::vector<VkLayerProperties>& supported_instance_layers)
        {
            std::vector<std::vector<const char*>> validation_layer_priority_list =
            {
                // The preferred validation layer is "VK_LAYER_KHRONOS_validation"
                {"VK_LAYER_KHRONOS_validation"},

                // Otherwise we fallback to using the LunarG meta layer
                {"VK_LAYER_LUNARG_standard_validation"},

                // Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
                {
                    "VK_LAYER_GOOGLE_threading",
                    "VK_LAYER_LUNARG_parameter_validation",
                    "VK_LAYER_LUNARG_object_tracker",
                    "VK_LAYER_LUNARG_core_validation",
                    "VK_LAYER_GOOGLE_unique_objects",
                },

                // Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
                {"VK_LAYER_LUNARG_core_validation"}
            };

            for (auto& validation_layers : validation_layer_priority_list)
            {
                if (ValidateLayers(validation_layers, supported_instance_layers))
                {
                    return validation_layers;
                }

                LOGW("Couldn't enable validation layers (see log for error) - falling back");
            }

            // Else return nothing
            return {};
        }
    }

    namespace
    {
        static_assert(sizeof(RHIViewport) == sizeof(VkViewport), "Size mismatch");
        static_assert(offsetof(RHIViewport, x) == offsetof(VkViewport, x), "Layout mismatch");
        static_assert(offsetof(RHIViewport, y) == offsetof(VkViewport, y), "Layout mismatch");
        static_assert(offsetof(RHIViewport, width) == offsetof(VkViewport, width), "Layout mismatch");
        static_assert(offsetof(RHIViewport, height) == offsetof(VkViewport, height), "Layout mismatch");
        static_assert(offsetof(RHIViewport, minDepth) == offsetof(VkViewport, minDepth), "Layout mismatch");
        static_assert(offsetof(RHIViewport, maxDepth) == offsetof(VkViewport, maxDepth), "Layout mismatch");
    }

    /// Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x) \
	do \
	{ \
		VkResult err = x; \
		if (err) \
		{ \
			LOGE("Detected Vulkan error: {}", ToString(err)); \
		} \
	} while (0)

#define VK_LOG_ERROR(result, message) LOGE("Vulkan: {}, error: {}", message, ToString(result));

    /* RHICommandBufferVulkan */
    RHICommandBufferVulkan::RHICommandBufferVulkan(_In_ RHIDeviceVulkan* device, VkCommandPool commandPool)
        : device{ device }
        , commandPool{ commandPool }
    {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(device->GetHandle(), &allocateInfo, &handle);

        if (result != VK_SUCCESS)
        {
            VK_LOG_ERROR(result, "Failed to allocate command buffer");
        }
    }

    RHICommandBufferVulkan::~RHICommandBufferVulkan()
    {
        if (handle != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(device->GetHandle(), commandPool, 1, &handle);
            handle = VK_NULL_HANDLE;
        }
    }

    void RHICommandBufferVulkan::PushDebugGroup(const StringView& name)
    {
        if (!debugUtilsSupported)
            return;

        VkDebugUtilsLabelEXT label = { VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
        label.pLabelName = name.data();
        label.color[0] = 0.0f;
        label.color[1] = 0.0f;
        label.color[2] = 0.0f;
        label.color[3] = 1.0f;
        vkCmdBeginDebugUtilsLabelEXT(handle, &label);
    }

    void RHICommandBufferVulkan::PopDebugGroup()
    {
        if (!debugUtilsSupported)
            return;

        vkCmdEndDebugUtilsLabelEXT(handle);
    }

    void RHICommandBufferVulkan::InsertDebugMarker(const StringView& name)
    {
        if (!debugUtilsSupported)
            return;

        VkDebugUtilsLabelEXT label = { VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
        label.pLabelName = name.data();
        label.color[0] = 0.0f;
        label.color[1] = 0.0f;
        label.color[2] = 0.0f;
        label.color[3] = 1.0f;
        vkCmdInsertDebugUtilsLabelEXT(handle, &label);
    }

    void RHICommandBufferVulkan::BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor)
    {
        
    }

    void RHICommandBufferVulkan::EndRenderPass()
    {
        
    }

    void RHICommandBufferVulkan::SetViewport(const RHIViewport& viewport)
    {
        // Flip viewport to match DirectX coordinate system
        VkViewport vkViewport;
        vkViewport.x = viewport.x;
        vkViewport.y = viewport.y + viewport.height;
        vkViewport.width = viewport.width;
        vkViewport.height = -viewport.height;
        vkViewport.minDepth = viewport.minDepth;
        vkViewport.maxDepth = viewport.maxDepth;

        vkCmdSetViewport(handle, 0, 1, &vkViewport);
    }

    void RHICommandBufferVulkan::SetViewports(const RHIViewport* viewports, uint32_t count)
    {
        // Flip viewport to match DirectX coordinate system
        VkViewport vkViewports[kRHIMaxViewportsAndScissors];

        for (uint32_t i = 0; i < count; ++i)
        {
            vkViewports[i].x = viewports[i].x;
            vkViewports[i].y = viewports[i].y + viewports[i].height;
            vkViewports[i].width = viewports[i].width;
            vkViewports[i].height = -viewports[i].height;
            vkViewports[i].minDepth = viewports[i].minDepth;
            vkViewports[i].maxDepth = viewports[i].maxDepth;
        }

        vkCmdSetViewport(handle, 0, count, vkViewports);
    }

    void RHICommandBufferVulkan::SetStencilReference(uint32_t value)
    {
        vkCmdSetStencilReference(handle, VK_STENCIL_FRONT_AND_BACK, value);
    }

    void RHICommandBufferVulkan::SetBlendColor(const RHIColor& color)
    {
        vkCmdSetBlendConstants(handle, &color.r);
    }

    void RHICommandBufferVulkan::SetBlendColor(const float blendColor[4])
    {
        vkCmdSetBlendConstants(handle, blendColor);
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

        VkResult result = volkInitialize();
        if (result != VK_SUCCESS)
        {
            return false;
        }

        const uint32_t instanceVersion = volkGetInstanceVersion();
        if (instanceVersion < VK_API_VERSION_1_2)
        {
            return false;
        }

        available = true;
        return true;
    }

    RHIDeviceVulkan::RHIDeviceVulkan(RHIValidationMode validationMode)
    {
        ALIMER_VERIFY(IsAvailable());

        // Create instance and debug utils first.
        uint32_t instanceExtensionCount;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr));
        std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data()));

        uint32_t instanceLayerCount;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));
        std::vector<VkLayerProperties> availableInstanceLayers(instanceLayerCount);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, availableInstanceLayers.data()));

        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

        // Enable surface extensions depending on os
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
        instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_DIRECT2DISPLAY)
        instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_DIRECTFB_EXT)
        instanceExtensions.push_back(VK_EXT_DIRECTFB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
        instanceExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_HEADLESS_EXT)
        instanceExtensions.push_back(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
#endif

        if (validationMode != RHIValidationMode::Disabled)
        {
            // Determine the optimal validation layers to enable that are necessary for useful debugging
            std::vector<const char*> optimalValidationLyers = GetOptimalValidationLayers(availableInstanceLayers);
            instanceLayers.insert(instanceLayers.end(), optimalValidationLyers.begin(), optimalValidationLyers.end());
        }
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
        //return new RHICommandBufferVulkan(this, VK_NULL_HANDLE);
    }
}

#endif /* defined(ALIMER_RHI_VULKAN) */
