// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "VulkanSampler.h"
#include "VulkanGraphics.h"

namespace Alimer
{
    namespace
    {
        [[nodiscard]] constexpr VkFilter VulkanSamplerFilter(SamplerFilter mode)
        {
            switch (mode)
            {
                case SamplerFilter::Nearest:
                    return VK_FILTER_NEAREST;
                case SamplerFilter::Linear:
                    return VK_FILTER_LINEAR;
                default:
                    ALIMER_UNREACHABLE();
                    return VK_FILTER_MAX_ENUM;
            }
        }

        [[nodiscard]] constexpr VkSamplerMipmapMode VulkanMipMapMode(SamplerFilter mode)
        {
            switch (mode)
            {
                case SamplerFilter::Nearest:
                    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
                case SamplerFilter::Linear:
                    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
                default:
                    ALIMER_UNREACHABLE();
                    return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
            }
        }

        [[nodiscard]] constexpr VkSamplerAddressMode VulkanSamplerAddressMode(SamplerAddressMode mode)
        {
            switch (mode)
            {
                case SamplerAddressMode::Wrap:
                    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
                case SamplerAddressMode::Mirror:
                    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                case SamplerAddressMode::Clamp:
                    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                case SamplerAddressMode::Border:
                    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
                case SamplerAddressMode::MirrorOnce:
                    return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
                default:
                    ALIMER_UNREACHABLE();
                    return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
            }
        }

        [[nodiscard]] constexpr VkBorderColor VulkanBorderColor(SamplerBorderColor value)
        {
            switch (value)
            {
                case SamplerBorderColor::TransparentBlack:
                    return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
                case SamplerBorderColor::OpaqueBlack:
                    return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
                case SamplerBorderColor::OpaqueWhite:
                    return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
                default:
                    ALIMER_UNREACHABLE();
                    return VK_BORDER_COLOR_MAX_ENUM;
            }
        }
    }

	VulkanSampler::VulkanSampler(VulkanGraphics& device, const SamplerDescription& desc)
		: Sampler()
        , device{ device }
	{
        VkSamplerCreateInfo createInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        createInfo.magFilter = VulkanSamplerFilter(desc.magFilter);
        createInfo.minFilter = VulkanSamplerFilter(desc.minFilter);
        createInfo.mipmapMode = VulkanMipMapMode(desc.mipFilter);
        createInfo.addressModeU = VulkanSamplerAddressMode(desc.addressModeU);
        createInfo.addressModeV = VulkanSamplerAddressMode(desc.addressModeV);
        createInfo.addressModeW = VulkanSamplerAddressMode(desc.addressModeW);
        createInfo.mipLodBias = 0.0f;

        // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkSamplerCreateInfo.html
        if (device.GetCaps().features.samplerAnisotropy && desc.maxAnisotropy > 1)
        {
            createInfo.anisotropyEnable = VK_TRUE;
            createInfo.maxAnisotropy = Min(
                static_cast<float>(desc.maxAnisotropy),
                static_cast<float>(device.GetCaps().limits.maxSamplerAnisotropy)
            );
        }
        else
        {
            createInfo.anisotropyEnable = VK_FALSE;
            createInfo.maxAnisotropy = 1;
        }

        if (desc.compareFunction != CompareFunction::Never)
        {
            createInfo.compareOp = ToVkCompareOp(desc.compareFunction);
            createInfo.compareEnable = VK_TRUE;
        }
        else {
            createInfo.compareOp = VK_COMPARE_OP_NEVER;
            createInfo.compareEnable = VK_FALSE;
        }

        createInfo.minLod = desc.lodMinClamp;
        createInfo.maxLod = desc.lodMaxClamp;
        createInfo.unnormalizedCoordinates = VK_FALSE;
        createInfo.borderColor = VulkanBorderColor(desc.borderColor);

		VkResult result = vkCreateSampler(device.GetHandle(), &createInfo, nullptr, &handle);

		if (result != VK_SUCCESS)
		{
			VK_LOG_ERROR(result, "Failed to create sampler.");
			return;
		}

        if (desc.label != nullptr)
        {
            device.SetObjectName(VK_OBJECT_TYPE_SAMPLER, (uint64_t)handle, desc.label);
        }

        OnCreated();
	}

	VulkanSampler::~VulkanSampler()
	{
		Destroy();
	}

	void VulkanSampler::Destroy()
	{
		if (handle != VK_NULL_HANDLE)
		{
			device.DeferDestroy(handle);
		}

		handle = VK_NULL_HANDLE;
        OnDestroyed();
	}
}
