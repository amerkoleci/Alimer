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
    static constexpr uint32_t kRHIMaxFramesInFlight = 2;
    static constexpr uint32_t kRHIMaxBackBufferCount = 3;
    static constexpr uint32_t kRHIMaxViewportsAndScissors = 8;
    static constexpr uint32_t kRHIMaxVertexBufferBindings = 4;
    static constexpr uint32_t kRHIMaxVertexAttributes = 16;
    static constexpr uint32_t kRHIMaxVertexAttributeOffset = 2047u;
    static constexpr uint32_t kRHIMaxVertexBufferStride = 2048u;
    static constexpr uint32_t kRHIMaxFrameCommandBuffers = 32u;
    static constexpr uint32_t kRHICubeMapSlices = 6;
    static constexpr uint32_t kRHIInvalidBindlessIndex = static_cast<uint32_t>(-1);

    struct Shader;
    struct GPUResource;
    struct GPUBuffer;
    struct RHITexture;

    enum SHADERSTAGE
    {
        MS,
        AS,
        VS,
        HS,
        DS,
        GS,
        PS,
        CS,
        LIB,
        SHADERSTAGE_COUNT,
    };
    enum SHADERFORMAT
    {
        SHADERFORMAT_NONE,
        SHADERFORMAT_HLSL5,
        SHADERFORMAT_HLSL6,
        SHADERFORMAT_SPIRV,
    };
    enum SHADERMODEL
    {
        SHADERMODEL_5_0,
        SHADERMODEL_6_0,
        SHADERMODEL_6_1,
        SHADERMODEL_6_2,
        SHADERMODEL_6_3,
        SHADERMODEL_6_4,
        SHADERMODEL_6_5,
    };
    enum PRIMITIVETOPOLOGY
    {
        UNDEFINED,
        TRIANGLELIST,
        TRIANGLESTRIP,
        POINTLIST,
        LINELIST,
        LINESTRIP,
        PATCHLIST,
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
    enum BLEND
    {
        BLEND_ZERO,
        BLEND_ONE,
        BLEND_SRC_COLOR,
        BLEND_INV_SRC_COLOR,
        BLEND_SRC_ALPHA,
        BLEND_INV_SRC_ALPHA,
        BLEND_DEST_ALPHA,
        BLEND_INV_DEST_ALPHA,
        BLEND_DEST_COLOR,
        BLEND_INV_DEST_COLOR,
        BLEND_SRC_ALPHA_SAT,
        BLEND_BLEND_FACTOR,
        BLEND_INV_BLEND_FACTOR,
        BLEND_SRC1_COLOR,
        BLEND_INV_SRC1_COLOR,
        BLEND_SRC1_ALPHA,
        BLEND_INV_SRC1_ALPHA,
    };
    enum COLOR_WRITE_ENABLE
    {
        COLOR_WRITE_DISABLE = 0,
        COLOR_WRITE_ENABLE_RED = 1,
        COLOR_WRITE_ENABLE_GREEN = 2,
        COLOR_WRITE_ENABLE_BLUE = 4,
        COLOR_WRITE_ENABLE_ALPHA = 8,
        COLOR_WRITE_ENABLE_ALL = (((COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN) | COLOR_WRITE_ENABLE_BLUE) | COLOR_WRITE_ENABLE_ALPHA)
    };
    enum BLEND_OP
    {
        BLEND_OP_ADD,
        BLEND_OP_SUBTRACT,
        BLEND_OP_REV_SUBTRACT,
        BLEND_OP_MIN,
        BLEND_OP_MAX,
    };
    enum FILL_MODE
    {
        FILL_WIREFRAME,
        FILL_SOLID,
    };
    enum CULL_MODE
    {
        CULL_NONE,
        CULL_FRONT,
        CULL_BACK,
    };
    enum INPUT_CLASSIFICATION
    {
        INPUT_PER_VERTEX_DATA,
        INPUT_PER_INSTANCE_DATA,
    };
    enum USAGE
    {
        USAGE_DEFAULT,
        USAGE_IMMUTABLE,
        USAGE_DYNAMIC,
        USAGE_STAGING,
    };
    enum TEXTURE_ADDRESS_MODE
    {
        TEXTURE_ADDRESS_WRAP,
        TEXTURE_ADDRESS_MIRROR,
        TEXTURE_ADDRESS_CLAMP,
        TEXTURE_ADDRESS_BORDER,
    };
    enum FILTER
    {
        FILTER_MIN_MAG_MIP_POINT,
        FILTER_MIN_MAG_POINT_MIP_LINEAR,
        FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
        FILTER_MIN_POINT_MAG_MIP_LINEAR,
        FILTER_MIN_LINEAR_MAG_MIP_POINT,
        FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        FILTER_MIN_MAG_LINEAR_MIP_POINT,
        FILTER_MIN_MAG_MIP_LINEAR,
        FILTER_ANISOTROPIC,
        FILTER_COMPARISON_MIN_MAG_MIP_POINT,
        FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
        FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
        FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
        FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
        FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
        FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
        FILTER_COMPARISON_ANISOTROPIC,
        FILTER_MINIMUM_MIN_MAG_MIP_POINT,
        FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
        FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
        FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
        FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
        FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
        FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
        FILTER_MINIMUM_ANISOTROPIC,
        FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
        FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
        FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
        FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
        FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
        FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
        FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
        FILTER_MAXIMUM_ANISOTROPIC,
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
    enum INDEXBUFFER_FORMAT
    {
        INDEXFORMAT_16BIT,
        INDEXFORMAT_32BIT,
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
    enum BUFFER_STATE
    {
        BUFFER_STATE_UNDEFINED,					// invalid state
        BUFFER_STATE_VERTEX_BUFFER,				// vertex buffer, read only
        BUFFER_STATE_INDEX_BUFFER,				// index buffer, read only
        BUFFER_STATE_CONSTANT_BUFFER,			// constant buffer, read only
        BUFFER_STATE_INDIRECT_ARGUMENT,			// argument buffer to DrawIndirect() or DispatchIndirect()
        BUFFER_STATE_SHADER_RESOURCE,			// shader resource, read only
        BUFFER_STATE_SHADER_RESOURCE_COMPUTE,	// shader resource, read only, non-pixel shader
        BUFFER_STATE_UNORDERED_ACCESS,			// shader resource, write enabled
        BUFFER_STATE_COPY_SRC,					// copy from
        BUFFER_STATE_COPY_DST,					// copy to
        BUFFER_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
    };
    enum SHADING_RATE
    {
        SHADING_RATE_1X1,
        SHADING_RATE_1X2,
        SHADING_RATE_2X1,
        SHADING_RATE_2X2,
        SHADING_RATE_2X4,
        SHADING_RATE_4X2,
        SHADING_RATE_4X4,

        SHADING_RATE_INVALID
    };

    // Flags ////////////////////////////////////////////
    enum BIND_FLAG
    {
        BIND_VERTEX_BUFFER = 1 << 0,
        BIND_INDEX_BUFFER = 1 << 1,
        BIND_CONSTANT_BUFFER = 1 << 2,
        BIND_SHADER_RESOURCE = 1 << 3,
        BIND_STREAM_OUTPUT = 1 << 4,
        BIND_RENDER_TARGET = 1 << 5,
        BIND_DEPTH_STENCIL = 1 << 6,
        BIND_UNORDERED_ACCESS = 1 << 7,
    };
    enum CPU_ACCESS
    {
        CPU_ACCESS_WRITE = 1 << 0,
        CPU_ACCESS_READ = 1 << 1,
    };
    enum RESOURCE_MISC_FLAG
    {
        RESOURCE_MISC_SHARED = 1 << 0,
        RESOURCE_MISC_TEXTURECUBE = 1 << 1,
        RESOURCE_MISC_INDIRECT_ARGS = 1 << 2,
        RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS = 1 << 3,
        RESOURCE_MISC_BUFFER_STRUCTURED = 1 << 4,
        RESOURCE_MISC_TILED = 1 << 5,
        RESOURCE_MISC_RAY_TRACING = 1 << 6,
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

    struct Viewport
    {
        float TopLeftX = 0.0f;
        float TopLeftY = 0.0f;
        float Width = 0.0f;
        float Height = 0.0f;
        float MinDepth = 0.0f;
        float MaxDepth = 1.0f;
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
        USAGE Usage = USAGE_DEFAULT;
        uint32_t BindFlags = 0;
        uint32_t CPUAccessFlags = 0;
        uint32_t MiscFlags = 0;
        ClearValue clear = {};
        IMAGE_LAYOUT layout = IMAGE_LAYOUT_SHADER_RESOURCE;
    };
    struct SamplerDesc
    {
        FILTER Filter = FILTER_MIN_MAG_MIP_POINT;
        TEXTURE_ADDRESS_MODE AddressU = TEXTURE_ADDRESS_CLAMP;
        TEXTURE_ADDRESS_MODE AddressV = TEXTURE_ADDRESS_CLAMP;
        TEXTURE_ADDRESS_MODE AddressW = TEXTURE_ADDRESS_CLAMP;
        float MipLODBias = 0.0f;
        uint32_t MaxAnisotropy = 0;
        COMPARISON_FUNC ComparisonFunc = COMPARISON_NEVER;
        float BorderColor[4] = { 0.0f,0.0f,0.0f,0.0f };
        float MinLOD = 0.0f;
        float MaxLOD = FLT_MAX;
    };
    struct RasterizerState
    {
        FILL_MODE FillMode = FILL_SOLID;
        CULL_MODE CullMode = CULL_NONE;
        bool FrontCounterClockwise = false;
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
    struct BlendState
    {
        bool AlphaToCoverageEnable = false;
        bool IndependentBlendEnable = false;

        struct RenderTargetBlendState
        {
            bool BlendEnable = false;
            BLEND SrcBlend = BLEND_SRC_ALPHA;
            BLEND DestBlend = BLEND_INV_SRC_ALPHA;
            BLEND_OP BlendOp = BLEND_OP_ADD;
            BLEND SrcBlendAlpha = BLEND_ONE;
            BLEND DestBlendAlpha = BLEND_ONE;
            BLEND_OP BlendOpAlpha = BLEND_OP_ADD;
            uint8_t RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
        };
        RenderTargetBlendState RenderTarget[8];
    };
    struct GPUBufferDesc
    {
        uint32_t ByteWidth = 0;
        USAGE Usage = USAGE_DEFAULT;
        uint32_t BindFlags = 0;
        uint32_t CPUAccessFlags = 0;
        uint32_t MiscFlags = 0;
        uint32_t StructureByteStride = 0; // needed for typed and structured buffer types!
        FORMAT Format = FORMAT_UNKNOWN; // only needed for typed buffer!
    };
    struct GPUQueryHeapDesc
    {
        GPU_QUERY_TYPE type = GPU_QUERY_TYPE_TIMESTAMP;
        uint32_t queryCount = 0;
    };
    struct PipelineStateDesc
    {
        const Shader* vs = nullptr;
        const Shader* ps = nullptr;
        const Shader* hs = nullptr;
        const Shader* ds = nullptr;
        const Shader* gs = nullptr;
        const Shader* ms = nullptr;
        const Shader* as = nullptr;
        const BlendState* bs = nullptr;
        const RasterizerState* rs = nullptr;
        const DepthStencilState* dss = nullptr;
        const InputLayout* il = nullptr;
        PRIMITIVETOPOLOGY		pt = TRIANGLELIST;
        uint32_t				sampleMask = 0xFFFFFFFF;
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
            BUFFER_STATE state_before;
            BUFFER_STATE state_after;
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
        static GPUBarrier Buffer(const GPUBuffer* buffer, BUFFER_STATE before, BUFFER_STATE after)
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
    struct SwapChainDesc
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t buffercount = 2;
        FORMAT format = FORMAT_R10G10B10A2_UNORM;
        bool fullscreen = false;
        bool vsync = true;
        float clearcolor[4] = { 0,0,0,1 };
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
        SamplerDesc desc;

        const SamplerDesc& GetDesc() const { return desc; }
    };
    struct StaticSampler
    {
        Sampler sampler;
        uint32_t slot = 0;
    };

    struct Shader : public GraphicsDeviceChild
    {
        SHADERSTAGE stage = SHADERSTAGE_COUNT;
        std::vector<StaticSampler> auto_samplers; // ability to set static samplers without explicit root signature
    };

    struct GPUResource : public GraphicsDeviceChild
    {
        enum class GPU_RESOURCE_TYPE
        {
            BUFFER,
            TEXTURE,
            RAYTRACING_ACCELERATION_STRUCTURE,
            UNKNOWN_TYPE,
        } type = GPU_RESOURCE_TYPE::UNKNOWN_TYPE;
        inline bool IsTexture() const { return type == GPU_RESOURCE_TYPE::TEXTURE; }
        inline bool IsBuffer() const { return type == GPU_RESOURCE_TYPE::BUFFER; }
        inline bool IsAccelerationStructure() const { return type == GPU_RESOURCE_TYPE::RAYTRACING_ACCELERATION_STRUCTURE; }
    };

    struct GPUBuffer : public GPUResource
    {
        GPUBufferDesc desc;

        const GPUBufferDesc& GetDesc() const { return desc; }
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
        SwapChainDesc desc;

        const SwapChainDesc& GetDesc() const { return desc; }
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
                    INDEXBUFFER_FORMAT indexFormat = INDEXFORMAT_32BIT;
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
        const Shader* shader = nullptr;
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

    /* Forward declarations */
    class RHIResource;
    class RHIResourceView;
    class RHITextureView;
    class RHIBuffer;
    class RHISwapChain;

    using RHIBufferRef = SharedPtr<RHIBuffer>;
    using RHISwapChainRef = SharedPtr<RHISwapChain>;

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

    enum class RHIBufferUsage : uint32_t
    {
        None = 0,
        MapRead = 1 << 0,
        MapWrite = 1 << 1,
        CopySource = 1 << 2,
        CopyDestination = 1 << 3,
        Index = 1 << 4,
        Vertex = 1 << 5,
        Uniform = 1 << 6,
        Storage = 1 << 7,
        Indirect = 1 << 8,
    };

    enum class RHIShaderStage : uint32_t
    {
        Vertex,
        Hull,
        Domain,
        Geometry,
        Pixel,
        Compute,
        Count,
    };

    enum class RHICompareFunction : uint32_t
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

    enum class RHIPrimitiveTopology : uint32_t
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        Count
    };

    enum class RHIIndexType : uint32_t
    {
        UInt16 = 0,
        UInt32 = 1
    };

    enum class RHISamplerFilter : uint32_t
    {
        Nearest,
        Linear
    };

    enum class RHISamplerAddressMode : uint32_t
    {
        Wrap,
        Mirror,
        Clamp,
        Border,
        MirrorOnce,
    };

    enum class RHISamplerBorderColor : uint32_t
    {
        TransparentBlack,
        OpaqueBlack,
        OpaqueWhite,
    };

    struct RHIExtent2D
    {
        uint32_t width = 1;
        uint32_t height = 1;
    };

    struct RHIExtent3D
    {
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
    };

    struct RHIColor
    {
        float r;
        float g;
        float b;
        float a;
    };

    struct RHIViewport
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

    struct RHIBufferDescription
    {
        StringView name;
        uint32_t size = 0;
        RHIBufferUsage usage = RHIBufferUsage::None;
    };

    struct RHISamplerDescription
    {
        RHISamplerFilter minFilter = RHISamplerFilter::Nearest;
        RHISamplerFilter magFilter = RHISamplerFilter::Nearest;
        RHISamplerFilter mipFilter = RHISamplerFilter::Nearest;
        RHISamplerAddressMode addressModeU = RHISamplerAddressMode::Clamp;
        RHISamplerAddressMode addressModeV = RHISamplerAddressMode::Clamp;
        RHISamplerAddressMode addressModeW = RHISamplerAddressMode::Clamp;
        uint16_t maxAnisotropy = 1;
        RHICompareFunction compareFunction = RHICompareFunction::Never;
        RHISamplerBorderColor borderColor = RHISamplerBorderColor::TransparentBlack;
        float lodMinClamp = 0.0f;
        float lodMaxClamp = FLT_MAX;
    };

    struct RHISwapChainDescription
    {
        RHIExtent2D size = { 0, 0 };
        PixelFormat format = PixelFormat::BGRA8UNorm;
        bool verticalSync = true;
        bool isFullscreen = false;
    };

    class ALIMER_API RHIObject
    {
    public:
        virtual ~RHIObject() = default;

        /// Unconditionally destroy the GPU resource.
        virtual void Destroy() = 0;

        const String& GetName() const { return name; }

        void SetName(const String& newName);
        void SetName(const StringView& newName);

    protected:
        RHIObject() = default;

    private:
        ALIMER_DISABLE_COPY_MOVE(RHIObject);

        virtual void ApiSetName(const StringView& name) = 0;

        String name;
    };

    class ALIMER_API RHIResource : public RHIObject
    {
    public:

    protected:
        uint64_t allocatedSize = 0;

        mutable std::unordered_map<size_t, std::unique_ptr<RHIResourceView>> resourceViewCache;
        mutable std::mutex resourceViewCacheMutex;
    };

    class ALIMER_API RHIResourceView
    {
    public:
        virtual ~RHIResourceView() = default;

        const RHIResource* GetResource() const;

    protected:
        RHIResourceView(const RHIResource* resource);

    private:
        ALIMER_DISABLE_COPY_MOVE(RHIResourceView);

        const RHIResource* resource;
    };

    class ALIMER_API RHITextureView : public RHIResourceView
    {
    protected:
        RHITextureView(const RHITexture* resource, const RHITextureViewDescription& description);

        PixelFormat format;
        uint32_t baseMipLevel;
        uint32_t mipLevels;
        uint32_t baseArrayLayer;
        uint32_t arrayLayers;
    };

    class ALIMER_API RHIBuffer : public RHIResource
    {
    public:
        uint32_t GetSize() const noexcept { return size; }
        RHIBufferUsage GetUsage() const noexcept { return usage; }

    protected:
        RHIBuffer(const RHIBufferDescription& desc);

        uint32_t size;
        RHIBufferUsage usage;
    };

    struct RHIObject2
    {
        std::shared_ptr<void> state;
        inline bool IsValid() const { return state.get() != nullptr; }
    };

    struct RHIShader : public RHIObject2
    {
        RHIShaderStage stage = RHIShaderStage::Count;
    };

    struct RHISampler : public RHIObject2
    {
    };

    class ALIMER_API RHISwapChain : public RHIObject
    {
    public:
        RHISwapChain(const RHISwapChainDescription& desc);

        virtual RHITextureView* GetCurrentTextureView() const = 0;

        const RHIExtent2D& GetSize() const noexcept { return size; }
        uint32_t GetWidth() const noexcept { return size.width; }
        uint32_t GetHeight() const noexcept { return size.height; }

    protected:
        RHIExtent2D size;
        PixelFormat colorFormat;
        bool verticalSync;
        bool isFullscreen;
    };

    class ALIMER_API RHICommandBuffer
    {
    public:
        virtual ~RHICommandBuffer() = default;

        virtual void PushDebugGroup(const StringView& name) = 0;
        virtual void PopDebugGroup() = 0;
        virtual void InsertDebugMarker(const StringView& name) = 0;

        virtual void BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor) = 0;
        virtual void EndRenderPass() = 0;

        virtual void SetViewport(const RHIViewport& viewport) = 0;
        virtual void SetViewports(const RHIViewport* viewports, uint32_t count) = 0;

        //virtual void SetScissorRect(const Rect& rect) = 0;
        //virtual void SetScissorRects(const Rect* rects, uint32_t count) = 0;

        virtual void SetStencilReference(uint32_t value) = 0;
        virtual void SetBlendColor(const RHIColor& color) = 0;
        virtual void SetBlendColor(const float blendColor[4]) = 0;

        void SetIndexBuffer(const RHIBuffer* buffer, RHIIndexType indexType, uint32_t offset = 0);

    private:
        virtual void SetIndexBufferCore(const RHIBuffer* buffer, RHIIndexType indexType, uint32_t offset) = 0;

    protected:
        RHICommandBuffer() = default;

    private:
        ALIMER_DISABLE_COPY_MOVE(RHICommandBuffer);
    };


    uint32_t GetFormatStride(FORMAT value) ;
    bool IsFormatUnorm(FORMAT value) ;
    bool IsFormatBlockCompressed(FORMAT value) ;
    bool IsFormatStencilSupport(FORMAT value) ;

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
        static const uint32_t BUFFERCOUNT = 2;
        bool DEBUGDEVICE = false;
        uint32_t capabilities = 0;
        size_t SHADER_IDENTIFIER_SIZE = 0;
        size_t TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE = 0;
        uint32_t VARIABLE_RATE_SHADING_TILE_SIZE = 0;
        uint64_t TIMESTAMP_FREQUENCY = 0;

    public:
        virtual ~RHIDevice() = default;

        virtual bool Initialize(RHIValidationMode validationMode) = 0;
        virtual void Shutdown() = 0;

        virtual bool CreateSwapChain(const SwapChainDesc* pDesc, void* window, SwapChain* swapChain) const = 0;
        virtual bool CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) const = 0;
        virtual bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, RHITexture* pTexture) const = 0;
        virtual bool CreateShader(SHADERSTAGE stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader) const = 0;
        virtual bool CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) const = 0;
        virtual bool CreateQueryHeap(const GPUQueryHeapDesc* pDesc, GPUQueryHeap* pQueryHeap) const = 0;
        virtual bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) const = 0;
        virtual bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) const = 0;
        virtual bool CreateRaytracingAccelerationStructure(const RaytracingAccelerationStructureDesc* pDesc, RaytracingAccelerationStructure* bvh) const { return false; }
        virtual bool CreateRaytracingPipelineState(const RaytracingPipelineStateDesc* pDesc, RaytracingPipelineState* rtpso) const { return false; }

        virtual int CreateSubresource(RHITexture* texture, SUBRESOURCE_TYPE type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) const = 0;
        virtual int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, uint64_t offset, uint64_t size = ~0) const = 0;

        virtual int GetDescriptorIndex(const GPUResource* resource, SUBRESOURCE_TYPE type, int subresource = -1) const { return -1; };
        virtual int GetDescriptorIndex(const Sampler* sampler) const { return -1; };

        virtual void WriteShadingRateValue(SHADING_RATE rate, void* dest) const {};
        virtual void WriteTopLevelAccelerationStructureInstance(const RaytracingAccelerationStructureDesc::TopLevel::Instance* instance, void* dest) const {}
        virtual void WriteShaderIdentifier(const RaytracingPipelineState* rtpso, uint32_t group_index, void* dest) const {}

        virtual void Map(const GPUResource* resource, Mapping* mapping) const = 0;
        virtual void Unmap(const GPUResource* resource) const = 0;
        virtual void QueryRead(const GPUQueryHeap* heap, uint32_t index, uint32_t count, uint64_t* results) const = 0;

        virtual void SetCommonSampler(const StaticSampler* sam) = 0;

        virtual void SetName(GPUResource* pResource, const StringView& name) = 0;

        // Begin a new command list for GPU command recording.
        //	This will be valid until SubmitCommandLists() is called.
        virtual CommandList BeginCommandList(RHIQueueType queueType = RHIQueueType::Graphics) = 0;
        // Submit all command list that were used with BeginCommandList before this call.
        //	This will make every command list to be in "available" state and restarts them
        virtual void SubmitCommandLists() = 0;

        virtual void WaitForGPU() const = 0;
        virtual void ClearPipelineStateCache() {};

        inline bool CheckCapability(GRAPHICSDEVICE_CAPABILITY capability) const { return capabilities & capability; }


        static constexpr uint32_t GetBufferCount() { return BUFFERCOUNT; }

        constexpr bool IsDebugDevice() const { return DEBUGDEVICE; }

        constexpr size_t GetShaderIdentifierSize() const { return SHADER_IDENTIFIER_SIZE; }
        constexpr size_t GetTopLevelAccelerationStructureInstanceSize() const { return TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE; }
        constexpr uint32_t GetVariableRateShadingTileSize() const { return VARIABLE_RATE_SHADING_TILE_SIZE; }
        constexpr uint64_t GetTimestampFrequency() const { return TIMESTAMP_FREQUENCY; }

        virtual SHADERFORMAT GetShaderFormat() const { return SHADERFORMAT_NONE; }

        virtual RHITexture GetBackBuffer(const SwapChain* swapchain) const = 0;

        ///////////////Thread-sensitive////////////////////////

        virtual void WaitCommandList(CommandList cmd, CommandList wait_for) {}
        virtual void RenderPassBegin(const SwapChain* swapchain, CommandList cmd) = 0;
        virtual void RenderPassBegin(const RenderPass* renderpass, CommandList cmd) = 0;
        virtual void RenderPassEnd(CommandList cmd) = 0;
        virtual void BindScissorRects(uint32_t numRects, const Rect* rects, CommandList cmd) = 0;
        virtual void BindViewports(uint32_t NumViewports, const Viewport* pViewports, CommandList cmd) = 0;
        virtual void BindResource(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, CommandList cmd, int subresource = -1) = 0;
        virtual void BindResources(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd) = 0;
        virtual void BindUAV(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, CommandList cmd, int subresource = -1) = 0;
        virtual void BindUAVs(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd) = 0;
        virtual void UnbindResources(uint32_t slot, uint32_t num, CommandList cmd) = 0;
        virtual void UnbindUAVs(uint32_t slot, uint32_t num, CommandList cmd) = 0;
        virtual void BindSampler(SHADERSTAGE stage, const Sampler* sampler, uint32_t slot, CommandList cmd) = 0;
        virtual void BindConstantBuffer(SHADERSTAGE stage, const GPUBuffer* buffer, uint32_t slot, CommandList cmd) = 0;
        virtual void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets, CommandList cmd) = 0;
        virtual void BindIndexBuffer(const GPUBuffer* indexBuffer, const INDEXBUFFER_FORMAT format, uint32_t offset, CommandList cmd) = 0;
        virtual void BindStencilRef(uint32_t value, CommandList cmd) = 0;
        virtual void BindBlendFactor(float r, float g, float b, float a, CommandList cmd) = 0;
        virtual void BindShadingRate(SHADING_RATE rate, CommandList cmd) {}
        virtual void BindPipelineState(const PipelineState* pso, CommandList cmd) = 0;
        virtual void BindComputeShader(const Shader* cs, CommandList cmd) = 0;
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
        virtual void UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize = -1) = 0;
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

    extern ALIMER_API RHIDevice* GRHIDevice;

    ALIMER_API bool RHInitialize(RHIValidationMode validationMode, RHIBackendType backendType = RHIBackendType::Count);
    ALIMER_API void RHIShutdown();

    //ALIMER_API RHITextureRef RHICreateTexture(const RHITextureDescription& desc);
    //ALIMER_API RHIBufferRef RHICreateBuffer(const RHIBufferDescription& desc, const void* initialData = nullptr);
    //ALIMER_API RHISwapChainRef RHICreateSwapChain(void* window, const RHISwapChainDescription& desc);
    //
    //ALIMER_API bool RHIBeginFrame();
    //ALIMER_API void RHIEndFrame();
    //
    //ALIMER_API RHICommandBuffer* RHIBeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics);
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
ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::RHIBufferUsage);
