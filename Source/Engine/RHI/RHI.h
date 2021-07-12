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
        TextureCube = 4,
    };

    enum class RHITextureUsage : uint32_t
    {
        Unknown = 0,
        ShaderRead = 1 << 0,
        ShaderWrite = 1 << 1,
        ShaderReadWrite = ShaderRead | ShaderWrite,
        RenderTarget = 1 << 2,
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
        ShaderRead = 1 << 7,
        ShaderWrite = 1 << 8,
        Indirect = 1 << 9,
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

    struct RHITextureDescriptor
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
        uint32_t sampleCount = 1;

        static inline RHITextureDescriptor Create1D(
            PixelFormat format,
            uint32_t width,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescriptor descriptor;
            descriptor.dimension = RHITextureDimension::Texture1D;
            descriptor.format = format;
            descriptor.usage = usage;
            descriptor.width = width;
            descriptor.depthOrArraySize = arraySize;
            descriptor.mipLevels = mipLevels;
            return descriptor;
        }

        static inline RHITextureDescriptor Create2D(
            PixelFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescriptor descriptor;
            descriptor.format = format;
            descriptor.usage = usage;
            descriptor.width = width;
            descriptor.height = height;
            descriptor.depthOrArraySize = arraySize;
            descriptor.mipLevels = mipLevels;
            return descriptor;
        }

        static RHITextureDescriptor CreateCube(
            PixelFormat format,
            uint32_t width,
            uint32_t arraySize = 1,
            uint32_t mipLevels = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescriptor descriptor;
            descriptor.dimension = RHITextureDimension::TextureCube;
            descriptor.format = format;
            descriptor.usage = usage;
            descriptor.width = width;
            descriptor.height = width;
            descriptor.depthOrArraySize = arraySize;
            descriptor.mipLevels = mipLevels;
            return descriptor;
        }

        static inline RHITextureDescriptor Create3D(
            PixelFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t depth,
            uint16_t mipLevels = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead) noexcept
        {
            RHITextureDescriptor descriptor;
            descriptor.dimension = RHITextureDimension::Texture3D;
            descriptor.format = format;
            descriptor.usage = usage;
            descriptor.width = width;
            descriptor.height = height;
            descriptor.depthOrArraySize = depth;
            descriptor.mipLevels = mipLevels;
            return descriptor;
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

    struct RHISwapChainDescriptor
    {
        RHIExtent2D size = { 0, 0 };
        PixelFormat format = PixelFormat::Undefined;
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
        uint32_t GetSampleCount() const noexcept { return sampleCount; }

    protected:
        RHITexture(const RHITextureDescriptor& descriptor);

        virtual RHITextureView* CreateView(const RHITextureViewDescription& description) const = 0;

        RHITextureDimension dimension;
        PixelFormat format;
        RHITextureUsage usage;
        uint32_t width;
        uint32_t height;
        uint32_t depthOrArraySize;
        uint32_t mipLevels;
        uint32_t sampleCount;
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

    class ALIMER_API RHISampler : public RHIObject
    {

    };

    class ALIMER_API RHISwapChain : public RHIObject
    {
    public:
        RHISwapChain(const RHISwapChainDescriptor& desc);

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

        virtual void BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor) = 0;
        virtual void EndRenderPass() = 0;

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

        [[nodiscard]] virtual RHITextureRef CreateTexture(const RHITextureDescriptor& desc) = 0;
        [[nodiscard]] RHIBufferRef CreateBuffer(const RHIBufferDescription& desc, const void* initialData = nullptr);
        [[nodiscard]] virtual RHISwapChainRef CreateSwapChain(void* window, const RHISwapChainDescriptor& desc) = 0;

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

    ALIMER_API RHITextureRef RHICreateTexture(const RHITextureDescriptor& desc);
    ALIMER_API RHIBufferRef RHICreateBuffer(const RHIBufferDescription& desc, const void* initialData = nullptr);
    ALIMER_API RHISwapChainRef RHICreateSwapChain(void* window, const RHISwapChainDescriptor& desc);

    ALIMER_API bool RHIBeginFrame();
    ALIMER_API void RHIEndFrame();

    ALIMER_API RHICommandBuffer* RHIBeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics);
}

namespace std
{
    /**	Hash value generator for RHITextureViewDescriptor. */
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
