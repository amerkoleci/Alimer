// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/RefPtr.h"
#include "Math/Color.h"
#include "Graphics/PixelFormat.h"
#include <array>

namespace Alimer
{
	/* Constants */
	static constexpr uint32_t kMaxFramesInFlight = 2;
	static constexpr uint32_t kMaxSimultaneousRenderTargets = 8;
	static constexpr uint32_t kMaxFrameCommandBuffers = 16;
	static constexpr uint32_t kMaxViewportsAndScissors = 8;
	static constexpr uint32_t kMaxVertexBufferBindings = 4;
	static constexpr uint32_t kMaxVertexAttributes = 16;
	static constexpr uint32_t kMaxVertexAttributeOffset = 2047u;
	static constexpr uint32_t kMaxVertexBufferStride = 2048u;
	static constexpr uint32_t kMaxDescriptorSets = 4;
	static constexpr uint32_t kMaxDescriptorBindings = 32;
	static constexpr uint32_t kMaxPushConstantSize = 128;
	static constexpr uint32_t kMaxUniformBufferSize = 16 * 1024;

    static constexpr uint32_t kInvalidBindlessIndex = static_cast<uint32_t>(-1);

	static constexpr uint32_t KnownVendorId_AMD = 0x1002;
	static constexpr uint32_t KnownVendorId_Intel = 0x8086;
	static constexpr uint32_t KnownVendorId_Nvidia = 0x10DE;
	static constexpr uint32_t KnownVendorId_Microsoft = 0x1414;
	static constexpr uint32_t KnownVendorId_ARM = 0x13B5;
	static constexpr uint32_t KnownVendorId_ImgTec = 0x1010;
	static constexpr uint32_t KnownVendorId_Qualcomm = 0x5143;

	/* Forward declarations */
	class Buffer;
	class Texture;
    class TextureView;
	class Sampler;
	class Shader;
	class Pipeline;
	class SwapChain;

    using BufferRef = RefPtr<Buffer>;
    using TextureRef = RefPtr<Texture>;
    using SamplerRef = RefPtr<Sampler>;
    using ShaderRef = RefPtr<Shader>;
    using PipelineRef = RefPtr<Pipeline>;
    using SwapChainRef = RefPtr<SwapChain>;

	enum class GPUBackendType
	{
		Vulkan,
		Direct3D12,
		Count
	};

	enum class GPUDebugFlags
	{
		None,
		DebugLayers = 1 << 0,
		GPUBasedValidation = 1 << 1,
		RenderDoc = 1 << 2,
	};

	enum class GPUAdapterType
	{
		DiscreteGPU,
		IntegratedGPU,
		CPU,
		Unknown
	};

	enum class CommandQueueType
	{
		Graphics,
		Compute,
		Count
	};

	enum class MemoryUsage : uint32_t
	{
		GpuOnly,
		CpuOnly,
		CpuToGpu,
		GpuToCpu
	};

    /// Number of MSAA samples to use. 1xMSAA and 4xMSAA are most broadly supported
    enum class SampleCount : uint32_t
    {
        Count1,
        Count2,
        Count4,
        Count8,
        Count16,
        Count32,
    };

	enum class LoadAction : uint32_t
    {
		Load,
		Clear,
		Discard,
	};

	enum class StoreAction : uint32_t
    {
		Store,
		Discard,
        Clear
	};

	enum class PrimitiveTopology : uint32_t
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip, 
		Count
	};

	enum class CompareFunction : uint32_t
    {
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,
	};

	enum class IndexType : uint32_t
    {
		UInt16 = 0,
		UInt32 = 1
	};

	enum class VertexFormat : uint32_t 
	{
		Invalid = 0,
		UChar2,
		UChar4,
		Char2,
		Char4,
		UChar2Norm,
		UChar4Norm,
		Char2Norm,
		Char4Norm,
		UShort2,
		UShort4,
		Short2,
		Short4,
		UShort2Norm,
		UShort4Norm,
		Short2Norm,
		Short4Norm,
		Half2,
		Half4,
		Float,
		Float2,
		Float3,
		Float4,
		UInt,
		UInt2,
		UInt3,
		UInt4,
		Int,
		Int2,
		Int3,
		Int4,
		RGB10A2Unorm
	};

	struct RenderPassColorAttachment
	{
        TextureView* view = nullptr;
        TextureView* resolveView = nullptr;
		LoadAction loadAction = LoadAction::Discard;
		StoreAction storeAction = StoreAction::Store;
		Color clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

	struct RenderPassDepthStencilAttachment
    {
        TextureView* view = nullptr;
		LoadAction depthLoadAction = LoadAction::Clear;
		StoreAction depthStoreAction = StoreAction::Discard;
		float clearDepth = 1.0;
		bool depthReadOnly = false;
		LoadAction stencilLoadAction = LoadAction::Discard;
		StoreAction stencilStoreAction = StoreAction::Discard;
		uint8_t clearStencil = 0;
		bool stencilReadOnly = false;
	};

	struct RenderPassInfo
	{
		RenderPassColorAttachment colorAttachments[kMaxSimultaneousRenderTargets] = {};
		RenderPassDepthStencilAttachment depthStencilAttachment;
	};

	enum class ShaderStage : uint32_t
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute,
		Count,
	};

	/* Shader reflection */
	enum class ShaderStages : uint32_t
	{
		None,
		Vertex = 1 << 0,
		Hull = 1 << 1,
		Domain = 1 << 2,
		Geometry = 1 << 3,
		Pixel = 1 << 4,
		Compute = 1 << 5,
		All = 0x7FFFFFFF,
	};

	enum class ShaderResourceType : uint32_t
	{
		Invalid,
		Input,
		Output,
        SampledTexture,
		StorageTexture,
		Sampler,
		UniformBuffer,
        StorageBuffer,
		PushConstant,
		All
	};

	enum class ShaderBlobType
	{
		DXIL,
		SPIRV
	};

	enum class PixelFormatFeatures
	{
		None = 0,
		Sampled = 1 << 0,
		RenderTarget = 1 << 1,
		DepthStencil = 1 << 2,
        RenderTargetBlend = 1 << 3,
		Filter = 1 << 4,
		Storage = 1 << 5,
        StorageAtomic = 1 << 6,
		Blit = 1 << 7,
	};

	struct ShaderResource
	{
		ShaderStages stages = ShaderStages::None;
		ShaderResourceType type = ShaderResourceType::Invalid;
		std::string name;

		uint32_t set;
		uint32_t binding;
		uint32_t arraySize;

		uint32_t offset;
		uint32_t size;
	};

	struct GraphicsFeatures {
		bool independentBlend;
		bool computeShader;
		bool multiViewport;
		bool indexUInt32;
		bool multiDrawIndirect;
		bool fillModeNonSolid;
		bool samplerAnisotropy;
		bool textureCompressionETC2;
		bool textureCompressionASTC_LDR;
		bool textureCompressionBC;
		bool textureCubeArray;
		bool bindlessDescriptors = false;
		bool raytracing = false;
		bool raytracingInline = false;
		bool variableRateShading = false;
		bool variableRateShadingExtended = false;
		bool meshShader = false;
	};

	struct GraphicsLimits {
		uint32_t        maxVertexAttributes;
		uint32_t        maxVertexBindings;
		uint32_t        maxVertexAttributeOffset;
		uint32_t        maxVertexBindingStride;
        uint32_t        maxTextureDimension1D;
		uint32_t        maxTextureDimension2D;
		uint32_t        maxTextureDimension3D;
		uint32_t        maxTextureDimensionCube;
		uint32_t        maxTextureArrayLayers;
		uint32_t        maxColorAttachments;
		uint32_t        maxUniformBufferSize;
		uint32_t        maxStorageBufferSize;
		uint64_t        minUniformBufferOffsetAlignment;
		uint64_t        minStorageBufferOffsetAlignment;
		uint32_t        maxSamplerAnisotropy;
		uint32_t        maxViewports;
		uint32_t        maxViewportWidth;
		uint32_t        maxViewportHeight;
		uint32_t        maxTessellationPatchSize;
		uint32_t        maxComputeSharedMemorySize;
		uint32_t        maxComputeWorkGroupCountX;
		uint32_t        maxComputeWorkGroupCountY;
		uint32_t        maxComputeWorkGroupCountZ;
		uint32_t        maxComputeWorkGroupInvocations;
		uint32_t        maxComputeWorkGroupSizeX;
		uint32_t        maxComputeWorkGroupSizeY;
		uint32_t        maxComputeWorkGroupSizeZ;
	};

	struct PixelFormatProperties {
		PixelFormatFeatures features;
	};

	struct GraphicsDeviceCaps {
		GPUBackendType			backendType;
		uint32_t				vendorId;
		uint32_t				adapterId;
		GPUAdapterType			adapterType;
		String					adapterName;
		GraphicsFeatures		features;
		GraphicsLimits			limits;
		PixelFormat defaultDepthFormat = PixelFormat::Undefined;
		PixelFormat defaultDepthStencilFormat = PixelFormat::Undefined;
		ShaderBlobType blobType = ShaderBlobType::DXIL;
		PixelFormatProperties formatProperties[ecast(PixelFormat::Count)] = {};
	};

	inline const char* GetVendorName(uint32_t vendorId)
	{
		switch (vendorId)
		{
		case KnownVendorId_AMD:
			return "AMD";
		case KnownVendorId_ImgTec:
			return "IMAGINATION";
		case KnownVendorId_Nvidia:
			return "Nvidia";
		case KnownVendorId_ARM:
			return "ARM";
		case KnownVendorId_Qualcomm:
			return "Qualcom";
		case KnownVendorId_Intel:
			return "Intel";
		default:
			return "Unknown";
		}
	}

	ALIMER_API uint32_t GetVertexFormatNumComponents(VertexFormat format);
	ALIMER_API uint32_t GetVertexFormatComponentSize(VertexFormat format);
	ALIMER_API uint32_t GetVertexFormatSize(VertexFormat format);
}

ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::GPUDebugFlags);
ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::ShaderStages);
ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::PixelFormatFeatures);
