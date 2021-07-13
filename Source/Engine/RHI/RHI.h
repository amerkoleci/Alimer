// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Types.h"
#include "RHI/PixelFormat.h"
#include <unordered_map>
#include <mutex>

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

    /* Forward declarations */
    class RHIResource;
    class RHIResourceView;
    class RHITexture;
    class RHITextureView;
    class RHIBuffer;
    class RHISwapChain;

    using RHITextureRef = SharedPtr<RHITexture>;
    using RHIBufferRef = SharedPtr<RHIBuffer>;
    using RHISwapChainRef = SharedPtr<RHISwapChain>;

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

    class ALIMER_API RHITexture : public RHIResource
    {
    public:
        [[nodiscard]] RHITextureView* GetView(const RHITextureViewDescription& description) const;

        RHITextureDimension GetDimension() const noexcept { return dimension; }
        PixelFormat GetFormat() const noexcept { return format; }
        RHITextureUsage GetUsage() const noexcept { return usage; }

        uint32_t GetWidth(uint32_t mipLevel = 0) const noexcept { return Max(1u, width >> mipLevel); }
        uint32_t GetHeight(uint32_t mipLevel = 0) const noexcept { return Max(1u, height >> mipLevel); }
        uint32_t GetDepth(uint32_t mipLevel = 0) const noexcept { return (dimension == RHITextureDimension::Texture3D) ? Max(1u, depthOrArraySize >> mipLevel) : 1; }
        uint32_t GetArrayLayers() const noexcept { return (dimension == RHITextureDimension::Texture3D) ? 1 : depthOrArraySize; }
        uint32_t GetMipLevels() const noexcept { return mipLevels; }
        RHITextureSampleCount GetSampleCount() const noexcept { return sampleCount; }

    protected:
        RHITexture(const RHITextureDescription& desc);

        virtual RHITextureView* CreateView(const RHITextureViewDescription& description) const = 0;

        RHITextureDimension dimension;
        PixelFormat format;
        RHITextureUsage usage;
        uint32_t width;
        uint32_t height;
        uint32_t depthOrArraySize;
        uint32_t mipLevels;
        RHITextureSampleCount sampleCount;
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

    class ALIMER_API RHIDevice
    {
    public:
        virtual ~RHIDevice() = default;

        virtual bool Initialize(RHIValidationMode validationMode) = 0;
        virtual void Shutdown() = 0;

        virtual void WaitForIdle() = 0;
        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;

        [[nodiscard]] virtual RHICommandBuffer* BeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics) = 0;

        [[nodiscard]] virtual RHITextureRef CreateTexture(const RHITextureDescription& desc) = 0;
        [[nodiscard]] RHIBufferRef CreateBuffer(const RHIBufferDescription& desc, const void* initialData = nullptr);
        [[nodiscard]] virtual RHISwapChainRef CreateSwapChain(void* window, const RHISwapChainDescription& desc) = 0;

        virtual bool CreateSampler(const RHISamplerDescription* desc, RHISampler* pSampler) const = 0;

        constexpr uint64_t GetFrameCount() const { return frameCount; }
        constexpr uint32_t GetFrameIndex() const { return frameIndex; }

    private:
        virtual RHIBufferRef CreateBufferCore(const RHIBufferDescription& desc, const void* initialData) = 0;

    protected:
        RHIDevice() = default;

        uint64_t frameCount = 0;
        uint32_t frameIndex = 0;

    private:
        ALIMER_DISABLE_COPY_MOVE(RHIDevice);
    };

    extern ALIMER_API RHIDevice* GRHIDevice;

    ALIMER_API bool RHInitialize(RHIValidationMode validationMode);
    ALIMER_API void RHIShutdown();

    ALIMER_API RHITextureRef RHICreateTexture(const RHITextureDescription& desc);
    ALIMER_API RHIBufferRef RHICreateBuffer(const RHIBufferDescription& desc, const void* initialData = nullptr);
    ALIMER_API RHISwapChainRef RHICreateSwapChain(void* window, const RHISwapChainDescription& desc);

    ALIMER_API bool RHIBeginFrame();
    ALIMER_API void RHIEndFrame();

    ALIMER_API RHICommandBuffer* RHIBeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics);
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
