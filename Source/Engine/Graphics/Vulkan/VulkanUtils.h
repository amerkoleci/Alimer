// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Log.h"
#include "Graphics/Types.h"
#define NOMINMAX
#include "volk.h"
#include "vk_mem_alloc.h"
#include <array>
#include <deque>
#include <set>

// Default fence timeout in nanoseconds
#define VK_DEFAULT_FENCE_TIMEOUT 100000000000

namespace Alimer
{
	class VulkanGraphics;
	class VulkanBuffer;
	class VulkanTexture;
	class VulkanTextureView;
	class VulkanSwapChain;
	class VulkanShader;
	class VulkanDescriptorSetLayout;
	class VulkanPipelineLayout;
	class VulkanPipeline;
	class VulkanCommandBuffer;
	class VulkanCommandQueue;

	static constexpr uint32_t kVulkanBindingShift_CBV = 0;
	static constexpr uint32_t kVulkanBindingShift_SRV = 1000;
	static constexpr uint32_t kVulkanBindingShift_UAV = 2000;
	static constexpr uint32_t kVulkanBindingShift_Sampler = 3000;

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

    [[nodiscard]] constexpr VkFormat ToVulkanFormat(PixelFormat format)
    {
        switch (format)
        {
            // 8-bit formats
        case PixelFormat::R8UNorm:  return VK_FORMAT_R8_UNORM;
        case PixelFormat::R8SNorm:  return VK_FORMAT_R8_SNORM;
        case PixelFormat::R8UInt:   return VK_FORMAT_R8_UINT;
        case PixelFormat::R8SInt:   return VK_FORMAT_R8_SINT;
            // 16-bit formats
        case PixelFormat::R16UNorm:     return VK_FORMAT_R16_UNORM;
        case PixelFormat::R16SNorm:     return VK_FORMAT_R16_SNORM;
        case PixelFormat::R16UInt:      return VK_FORMAT_R16_UINT;
        case PixelFormat::R16SInt:      return VK_FORMAT_R16_SINT;
        case PixelFormat::R16Float:     return VK_FORMAT_R16_SFLOAT;
        case PixelFormat::RG8UNorm:     return VK_FORMAT_R8G8_UNORM;
        case PixelFormat::RG8SNorm:     return VK_FORMAT_R8G8_SNORM;
        case PixelFormat::RG8UInt:      return VK_FORMAT_R8G8_UINT;
        case PixelFormat::RG8SInt:      return VK_FORMAT_R8G8_SINT;
            // 32-bit formats
        case PixelFormat::R32UInt:          return VK_FORMAT_R32_UINT;
        case PixelFormat::R32SInt:          return VK_FORMAT_R32_SINT;
        case PixelFormat::R32Float:         return VK_FORMAT_R32_SFLOAT;
        case PixelFormat::RG16UNorm:        return VK_FORMAT_R16G16_UNORM;
        case PixelFormat::RG16SNorm:        return VK_FORMAT_R16G16_SNORM;
        case PixelFormat::RG16UInt:         return VK_FORMAT_R16G16_UINT;
        case PixelFormat::RG16SInt:         return VK_FORMAT_R16G16_SINT;
        case PixelFormat::RG16Float:        return VK_FORMAT_R16G16_SFLOAT;
        case PixelFormat::RGBA8UNorm:       return VK_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::RGBA8UNormSrgb:   return VK_FORMAT_R8G8B8A8_SRGB;
        case PixelFormat::RGBA8SNorm:       return VK_FORMAT_R8G8B8A8_SNORM;
        case PixelFormat::RGBA8UInt:        return VK_FORMAT_R8G8B8A8_UINT;
        case PixelFormat::RGBA8SInt:        return VK_FORMAT_R8G8B8A8_SINT;
        case PixelFormat::BGRA8UNorm:       return VK_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::BGRA8UNormSrgb:   return VK_FORMAT_B8G8R8A8_SRGB;
            // Packed 32-Bit formats
        case PixelFormat::RGB10A2UNorm:     return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case PixelFormat::RG11B10Float:     return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case PixelFormat::RGB9E5Float:      return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
            // 64-Bit formats
        case PixelFormat::RG32UInt:         return VK_FORMAT_R32G32_UINT;
        case PixelFormat::RG32SInt:         return VK_FORMAT_R32G32_SINT;
        case PixelFormat::RG32Float:        return VK_FORMAT_R32G32_SFLOAT;
        case PixelFormat::RGBA16UNorm:      return VK_FORMAT_R16G16B16A16_UNORM;
        case PixelFormat::RGBA16SNorm:      return VK_FORMAT_R16G16B16A16_SNORM;
        case PixelFormat::RGBA16UInt:       return VK_FORMAT_R16G16B16A16_UINT;
        case PixelFormat::RGBA16SInt:       return VK_FORMAT_R16G16B16A16_SINT;
        case PixelFormat::RGBA16Float:      return VK_FORMAT_R16G16B16A16_SFLOAT;
            // 128-Bit formats
        case PixelFormat::RGBA32UInt:       return VK_FORMAT_R32G32B32A32_UINT;
        case PixelFormat::RGBA32SInt:       return VK_FORMAT_R32G32B32A32_SINT;
        case PixelFormat::RGBA32Float:      return VK_FORMAT_R32G32B32A32_SFLOAT;
            // Depth-stencil formats
        case PixelFormat::Depth16UNorm:     return VK_FORMAT_D16_UNORM;
        case PixelFormat::Depth32Float:     return VK_FORMAT_D32_SFLOAT;
        case PixelFormat::Depth24UNormStencil8: return VK_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat::Depth32FloatStencil8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
            // Compressed BC formats
        case PixelFormat::BC1RGBAUNorm:         return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case PixelFormat::BC1RGBAUNormSrgb:     return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        case PixelFormat::BC2RGBAUNorm:         return VK_FORMAT_BC2_UNORM_BLOCK;
        case PixelFormat::BC2RGBAUNormSrgb:     return VK_FORMAT_BC2_SRGB_BLOCK;
        case PixelFormat::BC3RGBAUNorm:         return VK_FORMAT_BC3_UNORM_BLOCK;
        case PixelFormat::BC3RGBAUNormSrgb:     return VK_FORMAT_BC3_SRGB_BLOCK;
        case PixelFormat::BC4RSNorm:            return VK_FORMAT_BC4_SNORM_BLOCK;
        case PixelFormat::BC4RUNorm:            return VK_FORMAT_BC4_UNORM_BLOCK;
        case PixelFormat::BC5RGSNorm:           return VK_FORMAT_BC5_SNORM_BLOCK;
        case PixelFormat::BC5RGUNorm:           return VK_FORMAT_BC5_UNORM_BLOCK;
        case PixelFormat::BC6HRGBFloat:         return VK_FORMAT_BC6H_SFLOAT_BLOCK;
        case PixelFormat::BC6HRGBUFloat:        return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case PixelFormat::BC7RGBAUNorm:         return VK_FORMAT_BC7_UNORM_BLOCK;
        case PixelFormat::BC7RGBAUNormSrgb:     return VK_FORMAT_BC7_SRGB_BLOCK;

        default:
            ALIMER_UNREACHABLE();
            return VK_FORMAT_UNDEFINED;
        }
    }

    [[nodiscard]] constexpr PixelFormat FromVulkanFormat(VkFormat format)
    {
        switch (format)
        {
            // 8-bit formats
        case VK_FORMAT_R8_UNORM:	return PixelFormat::R8UNorm;
        case VK_FORMAT_R8_SNORM:	return PixelFormat::R8SNorm;
        case VK_FORMAT_R8_UINT:		return PixelFormat::R8UInt;
        case VK_FORMAT_R8_SINT:		return PixelFormat::R8SInt;
            // 16-bit formats
        case VK_FORMAT_R16_UNORM:	return PixelFormat::R16UNorm;
        case VK_FORMAT_R16_SNORM:	return PixelFormat::R16SNorm;
        case VK_FORMAT_R16_UINT:	return PixelFormat::R16UInt;
        case VK_FORMAT_R16_SINT:	return PixelFormat::R16SInt;
        case VK_FORMAT_R16_SFLOAT:	return PixelFormat::R16Float;
        case VK_FORMAT_R8G8_UNORM:	return PixelFormat::RG8UNorm;
        case VK_FORMAT_R8G8_SNORM:	return PixelFormat::RG8SNorm;
        case VK_FORMAT_R8G8_UINT:	return PixelFormat::RG8UInt;
        case VK_FORMAT_R8G8_SINT:	return PixelFormat::RG8SInt;
            // 32-bit formats
        case VK_FORMAT_R32_UINT:        return PixelFormat::R32UInt;
        case VK_FORMAT_R32_SINT:        return PixelFormat::R32SInt;
        case VK_FORMAT_R32_SFLOAT:      return PixelFormat::R32Float;
        case VK_FORMAT_R16G16_UNORM:	return PixelFormat::RG16UNorm;
        case VK_FORMAT_R16G16_SNORM:	return PixelFormat::RG16SNorm;
        case VK_FORMAT_R16G16_UINT:		return PixelFormat::RG16UInt;
        case VK_FORMAT_R16G16_SINT:		return PixelFormat::RG16SInt;
        case VK_FORMAT_R16G16_SFLOAT:	return PixelFormat::RG16Float;
        case VK_FORMAT_R8G8B8A8_UNORM:	return PixelFormat::RGBA8UNorm;
        case VK_FORMAT_R8G8B8A8_SRGB:	return PixelFormat::RGBA8UNormSrgb;
        case VK_FORMAT_R8G8B8A8_SNORM:	return PixelFormat::RGBA8SNorm;
        case VK_FORMAT_R8G8B8A8_UINT:	return PixelFormat::RGBA8UInt;
        case VK_FORMAT_R8G8B8A8_SINT:	return PixelFormat::RGBA8SInt;
        case VK_FORMAT_B8G8R8A8_UNORM:	return PixelFormat::BGRA8UNorm;
        case VK_FORMAT_B8G8R8A8_SRGB:   return PixelFormat::BGRA8UNormSrgb;
            // Packed 32-Bit formats
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:	return PixelFormat::RGB10A2UNorm;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:     return PixelFormat::RG11B10Float;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:      return PixelFormat::RGB9E5Float;
            // 64-Bit formats
        case VK_FORMAT_R32G32_UINT:			return PixelFormat::RG32UInt;
        case VK_FORMAT_R32G32_SINT:         return PixelFormat::RG32SInt;
        case VK_FORMAT_R32G32_SFLOAT:       return PixelFormat::RG32Float;
        case VK_FORMAT_R16G16B16A16_UNORM:  return PixelFormat::RGBA16UNorm;
        case VK_FORMAT_R16G16B16A16_SNORM:  return PixelFormat::RGBA16SNorm;
        case VK_FORMAT_R16G16B16A16_UINT:   return PixelFormat::RGBA16UInt;
        case VK_FORMAT_R16G16B16A16_SINT:   return PixelFormat::RGBA16SInt;
        case VK_FORMAT_R16G16B16A16_SFLOAT: return PixelFormat::RGBA16Float;
            // 128-Bit formats
        case VK_FORMAT_R32G32B32A32_UINT:   return PixelFormat::RGBA32UInt;
        case VK_FORMAT_R32G32B32A32_SINT:   return PixelFormat::RGBA32SInt;
        case VK_FORMAT_R32G32B32A32_SFLOAT: return PixelFormat::RGBA32Float;
            // Depth-stencil formats
        case VK_FORMAT_D16_UNORM:			return PixelFormat::Depth16UNorm;
        case VK_FORMAT_D32_SFLOAT:			return PixelFormat::Depth32Float;
        case VK_FORMAT_D24_UNORM_S8_UINT:	return PixelFormat::Depth24UNormStencil8;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:	return PixelFormat::Depth32FloatStencil8;
            // Compressed BC formats
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:	return PixelFormat::BC1RGBAUNorm;
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:		return PixelFormat::BC1RGBAUNormSrgb;
        case VK_FORMAT_BC2_UNORM_BLOCK:			return PixelFormat::BC2RGBAUNorm;
        case VK_FORMAT_BC2_SRGB_BLOCK:			return PixelFormat::BC2RGBAUNormSrgb;
        case VK_FORMAT_BC3_UNORM_BLOCK:			return PixelFormat::BC3RGBAUNorm;
        case VK_FORMAT_BC3_SRGB_BLOCK:			return PixelFormat::BC3RGBAUNormSrgb;
        case VK_FORMAT_BC4_SNORM_BLOCK:			return PixelFormat::BC4RSNorm;
        case VK_FORMAT_BC4_UNORM_BLOCK:			return PixelFormat::BC4RUNorm;
        case VK_FORMAT_BC5_SNORM_BLOCK:			return PixelFormat::BC5RGSNorm;
        case VK_FORMAT_BC5_UNORM_BLOCK:			return PixelFormat::BC5RGUNorm;
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:		return PixelFormat::BC6HRGBFloat;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:		return PixelFormat::BC6HRGBUFloat;
        case VK_FORMAT_BC7_UNORM_BLOCK:			return PixelFormat::BC7RGBAUNorm;
        case VK_FORMAT_BC7_SRGB_BLOCK:			return PixelFormat::BC7RGBAUNormSrgb;

        default:
            ALIMER_UNREACHABLE();
            return PixelFormat::Undefined;
        }
    }

	[[nodiscard]] constexpr VkCompareOp ToVkCompareOp(CompareFunction function)
	{
		switch (function)
		{
		case CompareFunction::Never:
			return VK_COMPARE_OP_NEVER;
		case CompareFunction::Less:
			return VK_COMPARE_OP_LESS;
		case CompareFunction::Equal:
			return VK_COMPARE_OP_EQUAL;
		case CompareFunction::LessEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case CompareFunction::Greater:
			return VK_COMPARE_OP_GREATER;
		case CompareFunction::NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;
		case CompareFunction::GreaterEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case CompareFunction::Always:
			return VK_COMPARE_OP_ALWAYS;

		default:
			ALIMER_UNREACHABLE();
			return VK_COMPARE_OP_MAX_ENUM;
		}
	}

    [[nodiscard]] constexpr VkSampleCountFlagBits VulkanSampleCount(SampleCount count)
    {
        switch (count)
        {
            case SampleCount::Count1:
                return VK_SAMPLE_COUNT_1_BIT;
            case SampleCount::Count2:
                return VK_SAMPLE_COUNT_2_BIT;
            case SampleCount::Count4:
                return VK_SAMPLE_COUNT_4_BIT;
            case SampleCount::Count8:
                return VK_SAMPLE_COUNT_8_BIT;
            case SampleCount::Count16:
                return VK_SAMPLE_COUNT_16_BIT;
            case SampleCount::Count32:
                return VK_SAMPLE_COUNT_32_BIT;

            default:
                ALIMER_UNREACHABLE();
                return VK_SAMPLE_COUNT_1_BIT;
        }
    }

	[[nodiscard]] constexpr VkAttachmentLoadOp ToVulkan(LoadAction action)
	{
		switch (action) {
		case LoadAction::Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case LoadAction::Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case LoadAction::Discard:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		default:
			ALIMER_UNREACHABLE();
			return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
		}
	}

	[[nodiscard]] constexpr VkAttachmentStoreOp ToVulkan(StoreAction action)
	{
		switch (action) {
		case StoreAction::Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		case StoreAction::Discard:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		default:
			ALIMER_UNREACHABLE();
			return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
		}
	}

	[[nodiscard]] inline VkShaderStageFlags ToVulkan(ShaderStage stage)
	{
		switch (stage) {
		case ShaderStage::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderStage::Pixel:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderStage::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		default:
			ALIMER_UNREACHABLE();
			return 0;
		}
	}

	[[nodiscard]] inline VkShaderStageFlags ToVulkan(ShaderStages stage)
	{
		if (stage == ShaderStages::All)
		{
			return VK_SHADER_STAGE_ALL;
		}

		VkShaderStageFlags vkStage = 0u;
		
		if (Any(stage, ShaderStages::Vertex))
		{
			vkStage |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (Any(stage, ShaderStages::Pixel))
		{
			vkStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (Any(stage, ShaderStages::Compute))
		{
			vkStage |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		return vkStage;
	}

	constexpr VkImageAspectFlags GetVkImageAspectFlags(PixelFormat format, bool ignoreStencil = false)
	{
		VkImageAspectFlags flags = VK_IMAGE_ASPECT_COLOR_BIT;
		if (IsDepthStencilFormat(format))
		{
			flags = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (ignoreStencil == false && IsStencilFormat(format))
			{
				flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}

		return flags;
	}

	[[nodiscard]] constexpr bool IsDynamicBuffer(VkDescriptorType type)
	{
		return
			type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC ||
			type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	}

	[[nodiscard]] constexpr bool IsBuffer(VkDescriptorType type)
	{
		return
			type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
			type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
			IsDynamicBuffer(type);
	}

	enum class VulkanBufferState : uint32_t
	{
		Undefined = 0,
		Vertex = 1 << 0,
		Index = 1 << 1,
		Uniform = 1 << 2,
		ShaderRead = 1 << 3,
		ShaderWrite = 1 << 4,
		IndirectArgument = 1 << 5,
		CopyDest = 1 << 6,
		CopySource = 1 << 7,
		AccelerationStructureRead = 1 << 8,
		AccelerationStructureWrite = 1 << 9,
	};

	struct VulkanAttachmentDescription
	{
		PixelFormat format;
		LoadAction loadAction;
		StoreAction storeAction;
	};

	struct VulkanRenderPassKey
	{
        uint32_t colorAttachmentCount = 0;
		VulkanAttachmentDescription colorAttachments[kMaxSimultaneousRenderTargets] = {};
		VulkanAttachmentDescription depthStencilAttachment = {};
		SampleCount sampleCount = SampleCount::Count1;

		size_t GetHash() const
		{
			if (hash == 0)
			{
				HashCombine(hash, colorAttachmentCount, (uint32_t)sampleCount);
				HashCombine(hash, depthStencilAttachment.format, depthStencilAttachment.loadAction, depthStencilAttachment.storeAction);

				for (uint32_t i = 0; i < colorAttachmentCount; ++i)
				{
					HashCombine(hash, colorAttachments[i].format, colorAttachments[i].loadAction, colorAttachments[i].storeAction);
				}
			}

			return hash;
		}

	private:
		mutable size_t hash = 0;
	};

	struct VulkanFboKey
	{
		VkRenderPass renderPass;
		uint32_t attachmentCount = 0;
		VkImageView attachments[kMaxSimultaneousRenderTargets + 1] = {};
		uint32_t width;
		uint32_t height;
		uint32_t layers;
	};
}

ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::VulkanBufferState);

/// Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x) \
	do \
	{ \
		VkResult err = x; \
		if (err) \
		{ \
			LOGE("Detected Vulkan error: {}", Alimer::ToString(err)); \
		} \
	} while (0)

#define VK_LOG_ERROR(result, message) LOGE("Vulkan: {}, error: {}", message, Alimer::ToString(result));
