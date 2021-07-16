// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.
// Based on Wicked Engine RHI: https://github.com/turanszkij/WickedEngine/blob/master/LICENSE.txt

#pragma once

#include "Core/Types.h"
#include "Math/Matrix3x4.h"
#include "Math/Rect.h"
#include "RHI/PixelFormat.h"
#include <unordered_map>
#include <mutex>

// Descriptor binding counts:
#define GPU_RESOURCE_HEAP_CBV_COUNT		15
#define GPU_RESOURCE_HEAP_SRV_COUNT		64
#define GPU_RESOURCE_HEAP_UAV_COUNT		16
#define GPU_SAMPLER_HEAP_COUNT			16

namespace Alimer
{
    /* Constants */
    static constexpr uint32_t kMaxFramesInFlight = 2;
    static constexpr uint32_t kMaxBackBufferCount = 3;
    static constexpr uint32_t kMaxSimultaneousRenderTargets = 8;
    static constexpr uint32_t kMaxViewportsAndScissors = 8;
    static constexpr uint32_t kMaxVertexBufferBindings = 8;
    static constexpr uint32_t kMaxVertexAttributes = 16;
    static constexpr uint32_t kMaxVertexAttributeOffset = 2047;
    static constexpr uint32_t kMaxVertexBufferStride = 2048;
    static constexpr uint32_t kMaxFrameCommandBuffers = 32;
    static constexpr uint32_t kRHICubeMapSlices = 6;
    static constexpr uint32_t kInvalidBindlessIndex = static_cast<uint32_t>(-1);

    /* Forward declarations */
    struct RHIShader;
    struct GPUResource;
    struct GPUBuffer;
    struct RHITexture;

    enum class RHIBackendType : uint32_t
    {
        Direct3D12,
        Vulkan,
        Count
    };

    enum class RHIValidationMode : uint32_t
    {
        /// No validation is enabled.
        Disabled,
        /// Print warnings and errors
        Enabled,
        /// Print all warnings, errors and info messages
        Verbose,
        /// Enable GPU-based validation
        GPU
    };

    enum class RHIQueueType : uint32_t
    {
        /// Can be used for draw, dispatch, or copy commands.
        Graphics,
        /// Can be used for dispatch or copy commands.
        Compute,
        Count
    };

    enum class RHITextureDimension : uint32_t
    {
        Texture1D = 1,
        Texture2D = 2,
        Texture3D = 3,
        TextureCube = 4
    };

    enum class RHITextureUsage : uint32_t
    {
        Unknown = 0,
        ShaderRead = 1 << 0,
        ShaderWrite = 1 << 1,
        ShaderReadWrite = ShaderRead | ShaderWrite,
        RenderTarget = 1 << 2,
    };

    /// Number of MSAA samples to use. 1xMSAA and 4xMSAA are most broadly supported
    enum class RHITextureSampleCount : uint32_t
    {
        Count1,
        Count2,
        Count4,
        Count8,
        Count16,
        Count32,
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

    enum class PrimitiveTopology : uint32_t
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        PatchList,
        Count
    };

    enum class RHIIndexType : uint32_t
    {
        UInt16 = 0,
        UInt32 = 1
    };

    enum class SamplerFilter : uint32_t
    {
        Nearest,
        Linear
    };

    enum class SamplerAddressMode : uint32_t
    {
        Wrap,
        Mirror,
        Clamp,
        Border,
        MirrorOnce,
    };

    enum class SamplerBorderColor : uint32_t
    {
        TransparentBlack,
        OpaqueBlack,
        OpaqueWhite,
    };

    enum class ShaderStage : uint32_t
    {
        Vertex,
        Hull,
        Domain,
        Geometry,
        Pixel,
        Compute,
        Mesh,
        Amplification,
        Library,
        Count,
    };

    enum class ShaderFormat : uint32_t
    {
        DXIL,
        SPIRV,
    };

    enum class ShaderModel
    {
        Model6_0,
        Model6_1,
        Model6_2,
        Model6_3,
        Model6_4,
        Model6_5,
        Model6_6,
        Model6_7,
    };

    enum COMPARISON_FUNC
    {
        COMPARISON_NEVER,
        COMPARISON_LESS,
        COMPARISON_EQUAL,
        COMPARISON_LESS_EQUAL,
        COMPARISON_GREATER,
        COMPARISON_NOT_EQUAL,
        COMPARISON_GREATER_EQUAL,
        COMPARISON_ALWAYS,
    };
    enum DEPTH_WRITE_MASK
    {
        DEPTH_WRITE_MASK_ZERO,
        DEPTH_WRITE_MASK_ALL,
    };
    enum STENCIL_OP
    {
        STENCIL_OP_KEEP,
        STENCIL_OP_ZERO,
        STENCIL_OP_REPLACE,
        STENCIL_OP_INCR_SAT,
        STENCIL_OP_DECR_SAT,
        STENCIL_OP_INVERT,
        STENCIL_OP_INCR,
        STENCIL_OP_DECR,
    };

    enum class BlendFactor : uint32_t
    {
        Zero,
        One,
        SourceColor,
        OneMinusSourceColor,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestinationColor,
        OneMinusDestinationColor,
        DestinationAlpha,
        OneMinusDestinationAlpha,
        SourceAlphaSaturated,
        BlendColor,
        OneMinusBlendColor,
        Source1Color,
        OneMinusSource1Color,
        Source1Alpha,
        OneMinusSource1Alpha,
    };

    enum class ColorWriteMask : uint32_t
    {
        None = 0,
        Red = 0x01,
        Green = 0x02,
        Blue = 0x04,
        Alpha = 0x08,
        All = 0x0F
    };

    enum class BlendOperation : uint32_t
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class FillMode : uint32_t
    {
        Solid,
        Wireframe,
    };

    enum class CullMode : uint32_t
    {
        None,
        Front,
        Back
    };

    enum class FaceWinding : uint32_t
    {
        Clockwise,
        CounterClockwise,
    };

    enum INPUT_CLASSIFICATION
    {
        INPUT_PER_VERTEX_DATA,
        INPUT_PER_INSTANCE_DATA,
    };

    enum class ResourceUsage : uint32_t
    {
        Default,
        Dynamic,
        StagingUpload,
        StagingReadback,
    };

    enum FORMAT
    {
        FORMAT_UNKNOWN,

        FORMAT_R32G32B32A32_FLOAT,
        FORMAT_R32G32B32A32_UINT,
        FORMAT_R32G32B32A32_SINT,

        FORMAT_R32G32B32_FLOAT,
        FORMAT_R32G32B32_UINT,
        FORMAT_R32G32B32_SINT,

        FORMAT_R16G16B16A16_FLOAT,
        FORMAT_R16G16B16A16_UNORM,
        FORMAT_R16G16B16A16_UINT,
        FORMAT_R16G16B16A16_SNORM,
        FORMAT_R16G16B16A16_SINT,

        FORMAT_R32G32_FLOAT,
        FORMAT_R32G32_UINT,
        FORMAT_R32G32_SINT,
        FORMAT_R32G8X24_TYPELESS,		// depth (32-bit) + stencil (8-bit) + shader resource (32-bit)
        FORMAT_D32_FLOAT_S8X24_UINT,	// depth (32-bit) + stencil (8-bit)

        FORMAT_R10G10B10A2_UNORM,
        FORMAT_R10G10B10A2_UINT,
        FORMAT_R11G11B10_FLOAT,
        FORMAT_R8G8B8A8_UNORM,
        FORMAT_R8G8B8A8_UNORM_SRGB,
        FORMAT_R8G8B8A8_UINT,
        FORMAT_R8G8B8A8_SNORM,
        FORMAT_R8G8B8A8_SINT,
        FORMAT_B8G8R8A8_UNORM,
        FORMAT_B8G8R8A8_UNORM_SRGB,
        FORMAT_R16G16_FLOAT,
        FORMAT_R16G16_UNORM,
        FORMAT_R16G16_UINT,
        FORMAT_R16G16_SNORM,
        FORMAT_R16G16_SINT,
        FORMAT_R32_TYPELESS,			// depth (32-bit) + shader resource (32-bit)
        FORMAT_D32_FLOAT,				// depth (32-bit)
        FORMAT_R32_FLOAT,
        FORMAT_R32_UINT,
        FORMAT_R32_SINT,
        FORMAT_R24G8_TYPELESS,			// depth (24-bit) + stencil (8-bit) + shader resource (24-bit)
        FORMAT_D24_UNORM_S8_UINT,		// depth (24-bit) + stencil (8-bit)

        FORMAT_R8G8_UNORM,
        FORMAT_R8G8_UINT,
        FORMAT_R8G8_SNORM,
        FORMAT_R8G8_SINT,
        FORMAT_R16_TYPELESS,			// depth (16-bit) + shader resource (16-bit)
        FORMAT_R16_FLOAT,
        FORMAT_D16_UNORM,				// depth (16-bit)
        FORMAT_R16_UNORM,
        FORMAT_R16_UINT,
        FORMAT_R16_SNORM,
        FORMAT_R16_SINT,

        FORMAT_R8_UNORM,
        FORMAT_R8_UINT,
        FORMAT_R8_SNORM,
        FORMAT_R8_SINT,

        FORMAT_BC1_UNORM,
        FORMAT_BC1_UNORM_SRGB,
        FORMAT_BC2_UNORM,
        FORMAT_BC2_UNORM_SRGB,
        FORMAT_BC3_UNORM,
        FORMAT_BC3_UNORM_SRGB,
        FORMAT_BC4_UNORM,
        FORMAT_BC4_SNORM,
        FORMAT_BC5_UNORM,
        FORMAT_BC5_SNORM,
        FORMAT_BC6H_UF16,
        FORMAT_BC6H_SF16,
        FORMAT_BC7_UNORM,
        FORMAT_BC7_UNORM_SRGB
    };
    enum GPU_QUERY_TYPE
    {
        GPU_QUERY_TYPE_TIMESTAMP,			// retrieve time point of gpu execution
        GPU_QUERY_TYPE_OCCLUSION,			// how many samples passed depth test?
        GPU_QUERY_TYPE_OCCLUSION_BINARY,	// depth test passed or not?
    };

    enum class BufferUsage : uint32_t
    {
        None = 0,
        Vertex = 1 << 0,
        Index = 1 << 1,
        Uniform = 1 << 2,
        ShaderRead = 1 << 3,
        ShaderWrite = 1 << 4,
        ShaderReadWrite = ShaderRead | ShaderWrite,
        Indirect = 1 << 5,
        RayTracingAccelerationStructure = 1 << 6,
    };

    enum class IndexType : uint32_t
    {
        UInt16 = 0,
        UInt32 = 1
    };

    enum SUBRESOURCE_TYPE
    {
        CBV, // constant buffer view
        SRV, // shader resource view
        UAV, // unordered access view
        RTV, // render target view
        DSV, // depth stencil view
    };
    enum IMAGE_LAYOUT
    {
        IMAGE_LAYOUT_UNDEFINED,					// invalid state
        IMAGE_LAYOUT_RENDERTARGET,				// render target, write enabled
        IMAGE_LAYOUT_DEPTHSTENCIL,				// depth stencil, write enabled
        IMAGE_LAYOUT_DEPTHSTENCIL_READONLY,		// depth stencil, read only
        IMAGE_LAYOUT_SHADER_RESOURCE,			// shader resource, read only
        IMAGE_LAYOUT_SHADER_RESOURCE_COMPUTE,	// shader resource, read only, non-pixel shader
        IMAGE_LAYOUT_UNORDERED_ACCESS,			// shader resource, write enabled
        IMAGE_LAYOUT_COPY_SRC,					// copy from
        IMAGE_LAYOUT_COPY_DST,					// copy to
        IMAGE_LAYOUT_SHADING_RATE_SOURCE,		// shading rate control per tile
    };

    enum class BufferState : uint32_t
    {
        Undefined,			// invalid state
        Vertex,				// vertex buffer, read only
        Index,				// index buffer, read only
        Constant,			// constant buffer, read only
        IndirectArgument,	// argument buffer to DrawIndirect() or DispatchIndirect()
        ShaderRead,			// shader resource, read only
        ShaderReadCompute,	// shader resource, read only, non-pixel shader
        ShaderWrite,		// shader resource, write enabled
        CopySrc,			// copy from
        CopyDst,			// copy to
        RayTracingAccelerationStructure,
    };

    enum class ShadingRate : uint32_t
    {
        Rate1x1,
        Rate1x2,
        Rate2x1,
        Rate2x2,
        Rate2x4,
        Rate4x2,
        Rate4x4,
        Invalid
    };

    // Flags ////////////////////////////////////////////
    enum BIND_FLAG
    {
        BIND_SHADER_RESOURCE = 1 << 0,
        BIND_RENDER_TARGET = 1 << 1,
        BIND_DEPTH_STENCIL = 1 << 2,
        BIND_UNORDERED_ACCESS = 1 << 3,
    };

    enum RESOURCE_MISC_FLAG
    {
        RESOURCE_MISC_TEXTURECUBE = 1 << 0
    };

    enum GRAPHICSDEVICE_CAPABILITY
    {
        GRAPHICSDEVICE_CAPABILITY_TESSELLATION = 1 << 0,
        GRAPHICSDEVICE_CAPABILITY_CONSERVATIVE_RASTERIZATION = 1 << 1,
        GRAPHICSDEVICE_CAPABILITY_RASTERIZER_ORDERED_VIEWS = 1 << 2,
        GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_COMMON = 1 << 3, // eg: R16G16B16A16_FLOAT, R8G8B8A8_UNORM and more common ones
        GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_R11G11B10_FLOAT = 1 << 4,
        GRAPHICSDEVICE_CAPABILITY_RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS = 1 << 5,
        GRAPHICSDEVICE_CAPABILITY_RAYTRACING_PIPELINE = 1 << 6,
        GRAPHICSDEVICE_CAPABILITY_RAYTRACING_INLINE = 1 << 7,
        GRAPHICSDEVICE_CAPABILITY_RAYTRACING_GEOMETRYINDEX = 1 << 8,
        GRAPHICSDEVICE_CAPABILITY_VARIABLE_RATE_SHADING = 1 << 9,
        GRAPHICSDEVICE_CAPABILITY_VARIABLE_RATE_SHADING_TIER2 = 1 << 10,
        GRAPHICSDEVICE_CAPABILITY_MESH_SHADER = 1 << 11,
        GRAPHICSDEVICE_CAPABILITY_BINDLESS_DESCRIPTORS = 1 << 12,


        // helper query for full raytracing support:
        GRAPHICSDEVICE_CAPABILITY_RAYTRACING = GRAPHICSDEVICE_CAPABILITY_RAYTRACING_PIPELINE | GRAPHICSDEVICE_CAPABILITY_RAYTRACING_INLINE | GRAPHICSDEVICE_CAPABILITY_RAYTRACING_GEOMETRYINDEX,
    };

    // Descriptor structs:
    struct Extent2D
    {
        uint32_t width = 1;
        uint32_t height = 1;
    };

    struct Extent3D
    {
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
    };

    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    struct RHITextureDescription
    {
        StringView name;

        /// Dimension of texture.
        RHITextureDimension dimension = RHITextureDimension::Texture2D;

        /// Pixel format.
        PixelFormat format = PixelFormat::RGBA8UNorm;

        /// Usage of texture.
        RHITextureUsage usage = RHITextureUsage::ShaderRead;

        /// Width of the texture in pixels.
        uint32_t width = 1;

        /// Height of the texture in pixels.
        uint32_t height = 1;

        /// Depth or array size of the texture in pixels.
        uint32_t depthOrArraySize = 1;

        /// Number of mip levels.
        uint32_t mipLevels = 1;

        /// Number of samples.
        RHITextureSampleCount sampleCount = RHITextureSampleCount::Count1;

        static inline RHITextureDescription Texture1D(
            PixelFormat format,
            uint32_t width,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescription desc;
            desc.dimension = RHITextureDimension::Texture1D;
            desc.format = format;
            desc.usage = usage;
            desc.width = width;
            desc.depthOrArraySize = arraySize;
            desc.mipLevels = mipLevels;
            return desc;
        }

        static inline RHITextureDescription Texture2D(
            PixelFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescription desc;
            desc.format = format;
            desc.usage = usage;
            desc.width = width;
            desc.height = height;
            desc.depthOrArraySize = arraySize;
            desc.mipLevels = mipLevels;
            return desc;
        }

        static inline RHITextureDescription Texture3D(
            PixelFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t depth,
            uint16_t mipLevels = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescription desc;
            desc.dimension = RHITextureDimension::Texture3D;
            desc.format = format;
            desc.usage = usage;
            desc.width = width;
            desc.height = height;
            desc.depthOrArraySize = depth;
            desc.mipLevels = mipLevels;
            return desc;
        }

        static RHITextureDescription TextureCube(
            PixelFormat format,
            uint32_t width,
            uint32_t arraySize = 1,
            uint32_t mipLevels = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescription desc;
            desc.dimension = RHITextureDimension::TextureCube;
            desc.format = format;
            desc.usage = usage;
            desc.width = width;
            desc.height = width;
            desc.depthOrArraySize = arraySize;
            desc.mipLevels = mipLevels;
            return desc;
        }
    };

    struct RHITextureViewDescription
    {
        PixelFormat format = PixelFormat::Undefined;
        /// The base mip level visible in the view. Must be less than texture mip levels.
        uint32_t baseMipLevel = 0;
        /// The number of mip levels visible in the view.
        uint32_t mipLevels = 0;
        /// The base array layer visible in the view.
        uint32_t baseArrayLayer = 0;
        ///  The number of array layers visible in the view.
        uint32_t arrayLayers = 0;
    };

    struct RHISamplerDescriptor
    {
        SamplerFilter minFilter = SamplerFilter::Nearest;
        SamplerFilter magFilter = SamplerFilter::Nearest;
        SamplerFilter mipFilter = SamplerFilter::Nearest;
        SamplerAddressMode addressModeU = SamplerAddressMode::Clamp;
        SamplerAddressMode addressModeV = SamplerAddressMode::Clamp;
        SamplerAddressMode addressModeW = SamplerAddressMode::Clamp;
        uint16_t maxAnisotropy = 1;
        CompareFunction compareFunction = CompareFunction::Never;
        SamplerBorderColor borderColor = SamplerBorderColor::TransparentBlack;
        float lodMinClamp = 0.0f;
        float lodMaxClamp = FLT_MAX;
    };

    struct InputLayout
    {
        static const uint32_t APPEND_ALIGNED_ELEMENT = 0xffffffff; // automatically figure out AlignedByteOffset depending on Format

        struct Element
        {
            std::string SemanticName;
            uint32_t SemanticIndex = 0;
            FORMAT Format = FORMAT_UNKNOWN;
            uint32_t InputSlot = 0;
            uint32_t AlignedByteOffset = APPEND_ALIGNED_ELEMENT;
            INPUT_CLASSIFICATION InputSlotClass = INPUT_CLASSIFICATION::INPUT_PER_VERTEX_DATA;
        };
        std::vector<Element> elements;
    };
    union ClearValue
    {
        float color[4];
        struct ClearDepthStencil
        {
            float depth;
            uint32_t stencil;
        } depthstencil;
    };

    struct TextureDesc
    {
        enum TEXTURE_TYPE
        {
            TEXTURE_1D,
            TEXTURE_2D,
            TEXTURE_3D,
        } type = TEXTURE_2D;
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 0;
        uint32_t ArraySize = 1;
        uint32_t MipLevels = 1;
        FORMAT Format = FORMAT_UNKNOWN;
        uint32_t SampleCount = 1;
        ResourceUsage resourceUsage = ResourceUsage::Default;
        uint32_t BindFlags = 0;
        uint32_t MiscFlags = 0;
        ClearValue clear = {};
        IMAGE_LAYOUT layout = IMAGE_LAYOUT_SHADER_RESOURCE;
    };

    struct RasterizerState
    {
        CullMode cullMode = CullMode::Back;
        FaceWinding frontFace = FaceWinding::Clockwise;
        FillMode fillMode = FillMode::Solid;
        int32_t DepthBias = 0;
        float DepthBiasClamp = 0.0f;
        float SlopeScaledDepthBias = 0.0f;
        bool DepthClipEnable = false;
        bool MultisampleEnable = false;
        bool AntialiasedLineEnable = false;
        bool ConservativeRasterizationEnable = false;
        uint32_t ForcedSampleCount = 0;
    };
    struct DepthStencilState
    {
        bool DepthEnable = false;
        DEPTH_WRITE_MASK DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
        COMPARISON_FUNC DepthFunc = COMPARISON_NEVER;
        bool StencilEnable = false;
        uint8_t StencilReadMask = 0xff;
        uint8_t StencilWriteMask = 0xff;

        struct DepthStencilOp
        {
            STENCIL_OP StencilFailOp = STENCIL_OP_KEEP;
            STENCIL_OP StencilDepthFailOp = STENCIL_OP_KEEP;
            STENCIL_OP StencilPassOp = STENCIL_OP_KEEP;
            COMPARISON_FUNC StencilFunc = COMPARISON_NEVER;
        };
        DepthStencilOp FrontFace;
        DepthStencilOp BackFace;
    };

    struct RenderTargetBlendState
    {
        bool blendEnable = false;
        BlendFactor srcColorBlendFactor = BlendFactor::One;
        BlendFactor dstColorBlendFactor = BlendFactor::Zero;
        BlendOperation colorBlendOperation = BlendOperation::Add;
        BlendFactor srcAlphaBlendFactor = BlendFactor::One;
        BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
        BlendOperation alphaBlendOperation{ BlendOperation::Add };
        ColorWriteMask writeMask = ColorWriteMask::All;
    };

    struct BlendState
    {
        bool alphaToCoverageEnable = false;
        bool independentBlendEnable = false;
        RenderTargetBlendState renderTarget[kMaxSimultaneousRenderTargets];
    };

    struct BufferDescriptor
    {
        const char* label = nullptr;
        uint64_t size = 0;
        BufferUsage usage = BufferUsage::None;
        ResourceUsage resourceUsage = ResourceUsage::Default;
        uint32_t StructureByteStride = 0; // needed for untyped structured buffer types!
        FORMAT Format = FORMAT_UNKNOWN; // only needed for typed buffer!
    };

    struct GPUQueryHeapDesc
    {
        GPU_QUERY_TYPE type = GPU_QUERY_TYPE_TIMESTAMP;
        uint32_t queryCount = 0;
    };
    struct PipelineStateDesc
    {
        const RHIShader* vertex = nullptr;
        const RHIShader* hull = nullptr;
        const RHIShader* domain = nullptr;
        const RHIShader* geometry = nullptr;
        const RHIShader* pixel = nullptr;
        const RHIShader* mesh = nullptr;
        const RHIShader* amplification = nullptr;
        const BlendState* bs = nullptr;
        const RasterizerState* rs = nullptr;
        const DepthStencilState* dss = nullptr;
        const InputLayout* il = nullptr;
        PrimitiveTopology   primitiveTopology = PrimitiveTopology::TriangleList;
        uint32_t			sampleMask = 0xFFFFFFFF;
    };

    struct GPUBarrier
    {
        enum TYPE
        {
            MEMORY_BARRIER,		// UAV accesses
            IMAGE_BARRIER,		// image layout transition
            BUFFER_BARRIER,		// buffer state transition
        } type = MEMORY_BARRIER;

        struct Memory
        {
            const GPUResource* resource;
        };
        struct Image
        {
            const RHITexture* texture;
            IMAGE_LAYOUT layout_before;
            IMAGE_LAYOUT layout_after;
            int mip;
            int slice;
        };
        struct Buffer
        {
            const GPUBuffer* buffer;
            BufferState state_before;
            BufferState state_after;
        };
        union
        {
            Memory memory;
            Image image;
            Buffer buffer;
        };

        static GPUBarrier Memory(const GPUResource* resource = nullptr)
        {
            GPUBarrier barrier;
            barrier.type = MEMORY_BARRIER;
            barrier.memory.resource = resource;
            return barrier;
        }
        static GPUBarrier Image(const RHITexture* texture, IMAGE_LAYOUT before, IMAGE_LAYOUT after,
            int mip = -1, int slice = -1)
        {
            GPUBarrier barrier;
            barrier.type = IMAGE_BARRIER;
            barrier.image.texture = texture;
            barrier.image.layout_before = before;
            barrier.image.layout_after = after;
            barrier.image.mip = mip;
            barrier.image.slice = slice;
            return barrier;
        }
        static GPUBarrier Buffer(const GPUBuffer* buffer, BufferState before, BufferState after)
        {
            GPUBarrier barrier;
            barrier.type = BUFFER_BARRIER;
            barrier.buffer.buffer = buffer;
            barrier.buffer.state_before = before;
            barrier.buffer.state_after = after;
            return barrier;
        }
    };
    struct RenderPassAttachment
    {
        enum TYPE
        {
            RENDERTARGET,
            DEPTH_STENCIL,
            RESOLVE,
            SHADING_RATE_SOURCE
        } type = RENDERTARGET;
        enum LOAD_OPERATION
        {
            LOADOP_LOAD,
            LOADOP_CLEAR,
            LOADOP_DONTCARE,
        } loadop = LOADOP_LOAD;
        const RHITexture* texture = nullptr;
        int subresource = -1;
        enum STORE_OPERATION
        {
            STOREOP_STORE,
            STOREOP_DONTCARE,
        } storeop = STOREOP_STORE;
        IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_UNDEFINED;	// layout before the render pass
        IMAGE_LAYOUT subpass_layout = IMAGE_LAYOUT_UNDEFINED;	// layout within the render pass
        IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_UNDEFINED;		// layout after the render pass

        static RenderPassAttachment RenderTarget(
            const RHITexture* resource = nullptr,
            LOAD_OPERATION load_op = LOADOP_LOAD,
            STORE_OPERATION store_op = STOREOP_STORE,
            IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_SHADER_RESOURCE,
            IMAGE_LAYOUT subpass_layout = IMAGE_LAYOUT_RENDERTARGET,
            IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_SHADER_RESOURCE
        )
        {
            RenderPassAttachment attachment;
            attachment.type = RENDERTARGET;
            attachment.texture = resource;
            attachment.loadop = load_op;
            attachment.storeop = store_op;
            attachment.initial_layout = initial_layout;
            attachment.subpass_layout = subpass_layout;
            attachment.final_layout = final_layout;
            return attachment;
        }

        static RenderPassAttachment DepthStencil(
            const RHITexture* resource = nullptr,
            LOAD_OPERATION load_op = LOADOP_LOAD,
            STORE_OPERATION store_op = STOREOP_STORE,
            IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_DEPTHSTENCIL,
            IMAGE_LAYOUT subpass_layout = IMAGE_LAYOUT_DEPTHSTENCIL,
            IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_DEPTHSTENCIL
        )
        {
            RenderPassAttachment attachment;
            attachment.type = DEPTH_STENCIL;
            attachment.texture = resource;
            attachment.loadop = load_op;
            attachment.storeop = store_op;
            attachment.initial_layout = initial_layout;
            attachment.subpass_layout = subpass_layout;
            attachment.final_layout = final_layout;
            return attachment;
        }

        static RenderPassAttachment Resolve(
            const RHITexture* resource = nullptr,
            IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_SHADER_RESOURCE,
            IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_SHADER_RESOURCE
        )
        {
            RenderPassAttachment attachment;
            attachment.type = RESOLVE;
            attachment.texture = resource;
            attachment.initial_layout = initial_layout;
            attachment.final_layout = final_layout;
            return attachment;
        }

        static RenderPassAttachment ShadingRateSource(
            const RHITexture* resource = nullptr,
            IMAGE_LAYOUT initial_layout = IMAGE_LAYOUT_SHADING_RATE_SOURCE,
            IMAGE_LAYOUT final_layout = IMAGE_LAYOUT_SHADING_RATE_SOURCE
        )
        {
            RenderPassAttachment attachment;
            attachment.type = SHADING_RATE_SOURCE;
            attachment.texture = resource;
            attachment.initial_layout = initial_layout;
            attachment.subpass_layout = IMAGE_LAYOUT_SHADING_RATE_SOURCE;
            attachment.final_layout = final_layout;
            return attachment;
        }
    };
    struct RenderPassDesc
    {
        enum FLAGS
        {
            FLAG_EMPTY = 0,
            FLAG_ALLOW_UAV_WRITES = 1 << 0,
        };
        uint32_t _flags = FLAG_EMPTY;
        std::vector<RenderPassAttachment> attachments;
    };

    struct SwapChainDescriptor
    {
        uint32_t width = 0;
        uint32_t height = 0;
        PixelFormat format = PixelFormat::BGRA8UNorm;
        bool verticalSync = true;
        bool isFullscreen = false;
    };

    struct IndirectDrawArgsInstanced
    {
        uint32_t VertexCountPerInstance = 0;
        uint32_t InstanceCount = 0;
        uint32_t StartVertexLocation = 0;
        uint32_t StartInstanceLocation = 0;
    };
    struct IndirectDrawArgsIndexedInstanced
    {
        uint32_t IndexCountPerInstance = 0;
        uint32_t InstanceCount = 0;
        uint32_t StartIndexLocation = 0;
        int32_t BaseVertexLocation = 0;
        uint32_t StartInstanceLocation = 0;
    };
    struct IndirectDispatchArgs
    {
        uint32_t ThreadGroupCountX = 0;
        uint32_t ThreadGroupCountY = 0;
        uint32_t ThreadGroupCountZ = 0;
    };
    struct SubresourceData
    {
        const void* pSysMem = nullptr;
        uint32_t SysMemPitch = 0;
        uint32_t SysMemSlicePitch = 0;
    };

    struct Mapping
    {
        enum FLAGS
        {
            FLAG_EMPTY = 0,
            FLAG_READ = 1 << 0,
            FLAG_WRITE = 1 << 1,
        };
        uint32_t _flags = FLAG_EMPTY;
        size_t offset = 0;
        size_t size = 0;
        uint32_t rowpitch = 0;	// output
        void* data = nullptr;	// output
    };


    // Resources:

    struct GraphicsDeviceChild
    {
        std::shared_ptr<void> internal_state;
        inline bool IsValid() const { return internal_state.get() != nullptr; }
    };

    struct Sampler : public GraphicsDeviceChild
    {
        //RHISamplerDescriptor desc;

        //const RHISamplerDescriptor& GetDesc() const { return desc; }
    };

    struct StaticSampler
    {
        Sampler sampler;
        uint32_t slot = 0;
    };

    struct RHIShader : public GraphicsDeviceChild
    {
        ShaderStage stage = ShaderStage::Count;
        std::vector<StaticSampler> auto_samplers; // ability to set static samplers without explicit root signature
    };

    enum class RHIResourceType : uint32_t
    {
        Unknown,
        Buffer,
        Texture,
        RayTracingAccelerationStructure,
    };

    struct GPUResource : public GraphicsDeviceChild
    {
        uint64_t allocatedSize = 0;
        RHIResourceType type = RHIResourceType::Unknown;

        inline bool IsBuffer() const { return type == RHIResourceType::Buffer; }
        inline bool IsTexture() const { return type == RHIResourceType::Texture; }
        inline bool IsAccelerationStructure() const { return type == RHIResourceType::RayTracingAccelerationStructure; }
    };

    struct GPUBuffer : public GPUResource
    {
        BufferDescriptor desc;

        const BufferDescriptor& GetDesc() const { return desc; }
    };

    struct RHITexture : public GPUResource
    {
        TextureDesc	desc;

        const TextureDesc& GetDesc() const { return desc; }
    };

    struct GPUQueryHeap : public GraphicsDeviceChild
    {
        GPUQueryHeapDesc desc;

        const GPUQueryHeapDesc& GetDesc() const { return desc; }
    };

    struct PipelineState : public GraphicsDeviceChild
    {
        size_t hash = 0;
        PipelineStateDesc desc;

        const PipelineStateDesc& GetDesc() const { return desc; }
    };

    struct RenderPass : public GraphicsDeviceChild
    {
        size_t hash = 0;
        RenderPassDesc desc;

        const RenderPassDesc& GetDesc() const { return desc; }
    };

    struct SwapChain : public GraphicsDeviceChild
    {
        Extent2D extent;
        bool verticalSync = true;
        bool isFullscreen = false;

        const Extent2D& GetExtent() const { return extent; }
    };

    struct RaytracingAccelerationStructureDesc
    {
        enum FLAGS
        {
            FLAG_EMPTY = 0,
            FLAG_ALLOW_UPDATE = 1 << 0,
            FLAG_ALLOW_COMPACTION = 1 << 1,
            FLAG_PREFER_FAST_TRACE = 1 << 2,
            FLAG_PREFER_FAST_BUILD = 1 << 3,
            FLAG_MINIMIZE_MEMORY = 1 << 4,
        };
        uint32_t _flags = FLAG_EMPTY;

        enum TYPE
        {
            BOTTOMLEVEL,
            TOPLEVEL,
        } type = BOTTOMLEVEL;

        struct BottomLevel
        {
            struct Geometry
            {
                enum FLAGS
                {
                    FLAG_EMPTY = 0,
                    FLAG_OPAQUE = 1 << 0,
                    FLAG_NO_DUPLICATE_ANYHIT_INVOCATION = 1 << 1,
                    FLAG_USE_TRANSFORM = 1 << 2,
                };
                uint32_t _flags = FLAG_EMPTY;

                enum TYPE
                {
                    TRIANGLES,
                    PROCEDURAL_AABBS,
                } type = TRIANGLES;

                struct Triangles
                {
                    GPUBuffer vertexBuffer;
                    GPUBuffer indexBuffer;
                    uint32_t indexCount = 0;
                    uint32_t indexOffset = 0;
                    uint32_t vertexCount = 0;
                    uint32_t vertexByteOffset = 0;
                    uint32_t vertexStride = 0;
                    IndexType indexFormat = IndexType::UInt32;
                    FORMAT vertexFormat = FORMAT_R32G32B32_FLOAT;
                    GPUBuffer transform3x4Buffer;
                    uint32_t transform3x4BufferOffset = 0;
                } triangles;
                struct Procedural_AABBs
                {
                    GPUBuffer aabbBuffer;
                    uint32_t offset = 0;
                    uint32_t count = 0;
                    uint32_t stride = 0;
                } aabbs;

            };
            std::vector<Geometry> geometries;
        } bottomlevel;

        struct TopLevel
        {
            struct Instance
            {
                enum FLAGS
                {
                    FLAG_EMPTY = 0,
                    FLAG_TRIANGLE_CULL_DISABLE = 1 << 0,
                    FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE = 1 << 1,
                    FLAG_FORCE_OPAQUE = 1 << 2,
                    FLAG_FORCE_NON_OPAQUE = 1 << 3,
                };

                Matrix3x4 transform;
                uint32_t InstanceID : 24;
                uint32_t InstanceMask : 8;
                uint32_t InstanceContributionToHitGroupIndex : 24;
                uint32_t Flags : 8;
                GPUResource bottomlevel;
            };
            GPUBuffer instanceBuffer;
            uint32_t offset = 0;
            uint32_t count = 0;
        } toplevel;
    };
    struct RaytracingAccelerationStructure : public GPUResource
    {
        RaytracingAccelerationStructureDesc desc;

        const RaytracingAccelerationStructureDesc& GetDesc() const { return desc; }
    };

    struct ShaderLibrary
    {
        enum TYPE
        {
            RAYGENERATION,
            MISS,
            CLOSESTHIT,
            ANYHIT,
            INTERSECTION,
        } type = RAYGENERATION;
        const RHIShader* shader = nullptr;
        std::string function_name;
    };
    struct ShaderHitGroup
    {
        enum TYPE
        {
            GENERAL, // raygen or miss
            TRIANGLES,
            PROCEDURAL,
        } type = TRIANGLES;
        std::string name;
        uint32_t general_shader = ~0;
        uint32_t closesthit_shader = ~0;
        uint32_t anyhit_shader = ~0;
        uint32_t intersection_shader = ~0;
    };
    struct RaytracingPipelineStateDesc
    {
        std::vector<ShaderLibrary> shaderlibraries;
        std::vector<ShaderHitGroup> hitgroups;
        uint32_t max_trace_recursion_depth = 1;
        uint32_t max_attribute_size_in_bytes = 0;
        uint32_t max_payload_size_in_bytes = 0;
    };
    struct RaytracingPipelineState : public GraphicsDeviceChild
    {
        RaytracingPipelineStateDesc desc;

        const RaytracingPipelineStateDesc& GetDesc() const { return desc; }
    };

    struct ShaderTable
    {
        const GPUBuffer* buffer = nullptr;
        uint64_t offset = 0;
        uint64_t size = 0;
        uint64_t stride = 0;
    };
    struct DispatchRaysDesc
    {
        ShaderTable raygeneration;
        ShaderTable miss;
        ShaderTable hitgroup;
        ShaderTable callable;
        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t Depth = 1;
    };

    class ALIMER_API RHICommandBuffer
    {
    public:
        virtual ~RHICommandBuffer() = default;

        virtual void PushDebugGroup(const StringView& name) = 0;
        virtual void PopDebugGroup() = 0;
        virtual void InsertDebugMarker(const StringView& name) = 0;

        //virtual void BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor) = 0;
        //virtual void EndRenderPass() = 0;

        virtual void SetViewport(const Viewport& viewport) = 0;
        virtual void SetViewports(const Viewport* viewports, uint32_t count) = 0;

        //virtual void SetScissorRect(const Rect& rect) = 0;
        //virtual void SetScissorRects(const Rect* rects, uint32_t count) = 0;

        virtual void SetStencilReference(uint32_t value) = 0;
        virtual void SetBlendColor(const float blendColor[4]) = 0;

        void SetIndexBuffer(const GPUBuffer* buffer, RHIIndexType indexType, uint32_t offset = 0);

    private:
        virtual void SetIndexBufferCore(const GPUBuffer* buffer, RHIIndexType indexType, uint32_t offset) = 0;

    protected:
        RHICommandBuffer() = default;

    private:
        ALIMER_DISABLE_COPY_MOVE(RHICommandBuffer);
    };


    uint32_t GetFormatStride(FORMAT value);
    bool IsFormatUnorm(FORMAT value);
    bool IsFormatBlockCompressed(FORMAT value);
    bool IsFormatStencilSupport(FORMAT value);

    using CommandList = uint8_t;
    static const CommandList COMMANDLIST_COUNT = 32;
    static const CommandList INVALID_COMMANDLIST = COMMANDLIST_COUNT;


    struct GPUAllocation
    {
        void* data = nullptr;				// application can write to this. Reads might be not supported or slow. The offset is already applied
        const GPUBuffer* buffer = nullptr;	// application can bind it to the GPU
        uint32_t offset = 0;				// allocation's offset from the GPUbuffer's beginning

        // Returns true if the allocation was successful
        inline bool IsValid() const { return data != nullptr && buffer != nullptr; }
    };

    class ALIMER_API RHIDevice
    {
    protected:
        RHIValidationMode validationMode = RHIValidationMode::Disabled;
        uint32_t capabilities = 0;
        size_t SHADER_IDENTIFIER_SIZE = 0;
        size_t TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE = 0;
        uint32_t VARIABLE_RATE_SHADING_TILE_SIZE = 0;
        uint64_t timestampFrequency = 0;

    public:
        virtual ~RHIDevice() = default;

        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual bool CreateSwapChain(const SwapChainDescriptor* descriptor, void* window, SwapChain* swapChain) const = 0;
        virtual bool CreateBuffer(const BufferDescriptor* descriptor, const void* initialData, GPUBuffer* pBuffer) const = 0;
        virtual bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, RHITexture* pTexture) const = 0;
        virtual bool CreateShader(ShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, RHIShader* pShader) const = 0;
        virtual bool CreateSampler(const RHISamplerDescriptor* descriptor, Sampler* pSamplerState) const = 0;
        virtual bool CreateQueryHeap(const GPUQueryHeapDesc* pDesc, GPUQueryHeap* pQueryHeap) const = 0;
        virtual bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) const = 0;
        virtual bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) const = 0;
        virtual bool CreateRaytracingAccelerationStructure(const RaytracingAccelerationStructureDesc* pDesc, RaytracingAccelerationStructure* bvh) const { return false; }
        virtual bool CreateRaytracingPipelineState(const RaytracingPipelineStateDesc* pDesc, RaytracingPipelineState* rtpso) const { return false; }

        virtual int CreateSubresource(RHITexture* texture, SUBRESOURCE_TYPE type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) const = 0;
        virtual int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, uint64_t offset, uint64_t size = ~0) const = 0;

        virtual int GetDescriptorIndex(const GPUResource* resource, SUBRESOURCE_TYPE type, int subresource = -1) const { return -1; };
        virtual int GetDescriptorIndex(const Sampler* sampler) const { return -1; };

        virtual void WriteShadingRateValue(ShadingRate rate, void* dest) const = 0;
        virtual void WriteTopLevelAccelerationStructureInstance(const RaytracingAccelerationStructureDesc::TopLevel::Instance* instance, void* dest) const {}
        virtual void WriteShaderIdentifier(const RaytracingPipelineState* rtpso, uint32_t group_index, void* dest) const {}

        virtual void Map(const GPUResource* resource, Mapping* mapping) const = 0;
        virtual void Unmap(const GPUResource* resource) const = 0;
        virtual void QueryRead(const GPUQueryHeap* heap, uint32_t index, uint32_t count, uint64_t* results) const = 0;

        virtual void SetCommonSampler(const StaticSampler* sam) = 0;

        virtual void SetName(const GPUResource* pResource, const StringView& name) const = 0;

        // Begin a new command list for GPU command recording.
        //	This will be valid until SubmitCommandLists() is called.
        virtual CommandList BeginCommandList(RHIQueueType queueType = RHIQueueType::Graphics) = 0;
        // Submit all command list that were used with BeginCommandList before this call.
        //	This will make every command list to be in "available" state and restarts them
        virtual void SubmitCommandLists() = 0;

        virtual void WaitForGPU() const = 0;
        virtual void ClearPipelineStateCache() {};

        inline bool CheckCapability(GRAPHICSDEVICE_CAPABILITY capability) const { return capabilities & capability; }

        constexpr RHIValidationMode GetValidationMode() const { return validationMode; }

        constexpr size_t GetShaderIdentifierSize() const { return SHADER_IDENTIFIER_SIZE; }
        constexpr size_t GetTopLevelAccelerationStructureInstanceSize() const { return TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE; }
        constexpr uint32_t GetVariableRateShadingTileSize() const { return VARIABLE_RATE_SHADING_TILE_SIZE; }
        constexpr uint64_t GetTimestampFrequency() const { return timestampFrequency; }

        virtual ShaderFormat GetShaderFormat() const = 0;

        virtual RHITexture GetBackBuffer(const SwapChain* swapchain) const = 0;

        ///////////////Thread-sensitive////////////////////////

        virtual void WaitCommandList(CommandList cmd, CommandList wait_for) {}
        virtual void BeginRenderPass(CommandList commandList, const SwapChain* swapchain, const float clearColor[4]) = 0;
        virtual void BeginRenderPass(CommandList commandList, const RenderPass* renderpass) = 0;
        virtual void EndRenderPass(CommandList commandList) = 0;
        virtual void BindScissorRects(CommandList commandList, uint32_t scissorCount, const Rect* pScissorRects) = 0;
        virtual void BindViewport(CommandList commandList, const Viewport& viewport) = 0;
        virtual void BindViewports(CommandList commandList, uint32_t viewportCount, const Viewport* pViewports) = 0;
        virtual void BindResource(ShaderStage stage, const GPUResource* resource, uint32_t slot, CommandList cmd, int subresource = -1) = 0;
        virtual void BindResources(ShaderStage stage, const GPUResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd) = 0;
        virtual void BindUAV(ShaderStage stage, const GPUResource* resource, uint32_t slot, CommandList cmd, int subresource = -1) = 0;
        virtual void BindUAVs(ShaderStage stage, const GPUResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd) = 0;
        virtual void BindSampler(CommandList commandList, uint32_t slot, const Sampler* sampler) = 0;
        virtual void BindConstantBuffer(ShaderStage stage, const GPUBuffer* buffer, uint32_t slot, CommandList cmd) = 0;
        virtual void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets, CommandList cmd) = 0;
        virtual void BindIndexBuffer(CommandList commandList, const GPUBuffer* indexBuffer, uint64_t offset, IndexType indexType) = 0;
        virtual void BindStencilRef(CommandList commandList, uint32_t value) = 0;
        virtual void BindBlendFactor(float r, float g, float b, float a, CommandList cmd) = 0;
        virtual void BindShadingRate(CommandList commandList, ShadingRate rate) = 0;
        virtual void BindPipelineState(const PipelineState* pso, CommandList cmd) = 0;
        virtual void BindComputeShader(CommandList commandList, const RHIShader* shader) = 0;
        virtual void Draw(uint32_t vertexCount, uint32_t startVertexLocation, CommandList cmd) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, CommandList cmd) = 0;
        virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation, CommandList cmd) = 0;
        virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation, CommandList cmd) = 0;
        virtual void DrawInstancedIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) = 0;
        virtual void DrawIndexedInstancedIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) = 0;
        virtual void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ, CommandList cmd) = 0;
        virtual void DispatchIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) = 0;
        virtual void DispatchMesh(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ, CommandList cmd) {}
        virtual void DispatchMeshIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) {}
        virtual void CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd) = 0;
        virtual void UpdateBuffer(CommandList commandList, const GPUBuffer* buffer, const void* data, uint64_t size = 0) = 0;
        virtual void QueryBegin(const GPUQueryHeap* heap, uint32_t index, CommandList cmd) = 0;
        virtual void QueryEnd(const GPUQueryHeap* heap, uint32_t index, CommandList cmd) = 0;
        virtual void QueryResolve(const GPUQueryHeap* heap, uint32_t index, uint32_t count, CommandList cmd) {}
        virtual void Barrier(const GPUBarrier* barriers, uint32_t numBarriers, CommandList cmd) = 0;
        virtual void BuildRaytracingAccelerationStructure(const RaytracingAccelerationStructure* dst, CommandList cmd, const RaytracingAccelerationStructure* src = nullptr) {}
        virtual void BindRaytracingPipelineState(const RaytracingPipelineState* rtpso, CommandList cmd) {}
        virtual void DispatchRays(const DispatchRaysDesc* desc, CommandList cmd) {}
        virtual void PushConstants(const void* data, uint32_t size, CommandList cmd) {}

        // Allocates temporary memory that the CPU can write and GPU can read. 
        //	It is only alive for one frame and automatically invalidated after that.
        //	The CPU pointer gets invalidated as soon as there is a Draw() or Dispatch() event on the same thread
        //	This allocation can be used to provide temporary vertex buffer, index buffer or raw buffer data to shaders
        virtual GPUAllocation AllocateGPU(size_t dataSize, CommandList cmd) = 0;

        virtual void EventBegin(const StringView& name, CommandList cmd) = 0;
        virtual void EventEnd(CommandList cmd) = 0;
        virtual void SetMarker(const StringView& name, CommandList cmd) = 0;

        constexpr uint64_t GetFrameCount() const { return frameCount; }
        constexpr uint32_t GetFrameIndex() const { return frameIndex; }

    protected:
        RHIDevice() = default;

        uint64_t frameCount = 0;
        uint32_t frameIndex = 0;

    private:
        ALIMER_DISABLE_COPY_MOVE(RHIDevice);
    };

    extern ALIMER_API RHIDevice* GDevice;

    ALIMER_API bool RHIInitialize(RHIValidationMode validationMode, RHIBackendType backendType = RHIBackendType::Count);
    ALIMER_API void RHIShutdown();
    ALIMER_API void RHIWaitForGPU();
}

namespace std
{
    template<>
    struct hash<Alimer::RHITextureViewDescription>
    {
        size_t operator()(const Alimer::RHITextureViewDescription& desc) const
        {
            size_t hash = 0;
            Alimer::HashCombine(hash, (uint32_t)desc.format);
            Alimer::HashCombine(hash, desc.baseMipLevel);
            Alimer::HashCombine(hash, desc.mipLevels);
            Alimer::HashCombine(hash, desc.baseArrayLayer);
            Alimer::HashCombine(hash, desc.arrayLayers);
            return hash;
        }
    };
}

ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::RHITextureUsage);
ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::BufferUsage);
ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::ColorWriteMask);
