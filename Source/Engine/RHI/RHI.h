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
    class RHISwapChain;

    using RHITextureRef = SharedPtr<RHITexture>;
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
        Dimension1D = 1,
        Dimension2D = 2,
        Dimension3D = 3,
    };

    enum class RHITextureUsage : uint32_t
    {
        Unknown = 0,
        ShaderRead = 1 << 0,
        ShaderWrite = 1 << 1,
        ShaderReadWrite = ShaderRead | ShaderWrite,
        RenderTarget = 1 << 2,
    };
    ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::RHITextureUsage);

    enum class RHITextureViewDimension : uint32_t
    {
        Undefined,
        Dimension1D,
        Dimension2D,
        Dimension2DArray,
        DimensionCube,
        DimensionCubeArray,
        Dimension3D
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

        /// Usage of texture.
        RHITextureUsage usage = RHITextureUsage::ShaderRead;

        /// Dimension of texture.
        RHITextureDimension dimension = RHITextureDimension::Dimension2D;

        /// Size of the texture in pixels.
        RHIExtent3D size;

        /// Pixel format.
        PixelFormat format = PixelFormat::RGBA8UNorm;

        /// Number of array elements (ignored for 3D textures).
        uint32_t arraySize = 1;

        /// Number of mip levels.
        uint32_t mipLevels = 1;

        /// Number of samples.
        uint32_t sampleCount = 1;

        static RHITextureDescriptor Create1D(
            PixelFormat format,
            uint32_t width,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead
        );

        static RHITextureDescriptor Create2D(
            PixelFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels = 1,
            RHITextureUsage usage = RHITextureUsage::ShaderRead
        );

        static RHITextureDescriptor Create2DArray(
            RHITextureUsage usage,
            uint32_t width,
            uint32_t height,
            uint16_t arraySize,
            PixelFormat format);

        static RHITextureDescriptor CreateCubemap(
            RHITextureUsage usage,
            uint32_t width,
            PixelFormat format);

        static RHITextureDescriptor CreateCubemapArray(
            RHITextureUsage usage,
            uint32_t width,
            uint16_t arraySize,
            PixelFormat format);

        static RHITextureDescriptor Create3D(
            RHITextureUsage usage,
            uint32_t width,
            uint32_t height,
            uint32_t depth,
            PixelFormat format);
    };

    struct RHITextureViewDescriptor
    {
        PixelFormat format = PixelFormat::Undefined;
        RHITextureViewDimension dimension = RHITextureViewDimension::Undefined;
        uint32_t baseMipLevel = 0;
        uint32_t mipLevelCount = 0;
        uint32_t baseArrayLayer = 0;
        uint32_t arrayLayerCount = 0;
    };

    struct RHISwapChainDescriptor
    {
        uint32_t width = 0;
        uint32_t height = 0;
        PixelFormat format = PixelFormat::BGRA8UNormSrgb;
        bool fullscreen = false;
        bool verticalSync = true;
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
    protected:
        virtual RHITextureView* CreateView(const RHITextureViewDescriptor& descriptor) = 0;

        mutable std::unordered_map<size_t, std::unique_ptr<RHIResourceView>> resourceViewCache;
        mutable std::mutex resourceViewCacheMutex;
    };

    class ALIMER_API RHIResourceView
    {
    public:
        virtual ~RHIResourceView() = default;

        const RHIResource* GetResource() const;
        RHIResource* GetResource();

    protected:
        RHIResourceView(_In_ RHIResource* resource);

    private:
        ALIMER_DISABLE_COPY_MOVE(RHIResourceView);

        RHIResource* resource;
    };

    class ALIMER_API RHITexture : public RHIResource
    {
    public:
        RHITextureUsage GetUsage() const noexcept { return usage; }
        RHITextureDimension GetDimension() const noexcept { return dimension; }

        uint32_t GetWidth(uint32_t mipLevel = 0) const noexcept { return Max(1u, size.width >> mipLevel); }
        uint32_t GetHeight(uint32_t mipLevel = 0) const noexcept { return Max(1u, size.height >> mipLevel); }
        uint32_t GetDepth(uint32_t mipLevel = 0) const noexcept { return Max(1u, size.depth >> mipLevel); }
        PixelFormat GetFormat() const noexcept { return format; }
        uint32_t GetArraySize() const noexcept { return arraySize; }
        uint32_t GetMipLevels() const noexcept { return mipLevels; }
        uint32_t GetSampleCount() const noexcept { return sampleCount; }

    protected:
        RHITexture(const RHITextureDescriptor& descriptor);

        RHITextureUsage usage;
        RHITextureDimension dimension;
        RHIExtent3D size;
        PixelFormat format;
        uint32_t arraySize{ 1 };
        uint32_t mipLevels{ 1 };
        uint32_t sampleCount{ 1 };
    };

    class ALIMER_API RHITextureView : public RHIResourceView
    {
    protected:
        RHITextureView(_In_ RHITexture* resource, const RHITextureViewDescriptor& descriptor);

        PixelFormat format;
        RHITextureViewDimension dimension;
        uint32_t baseMipLevel;
        uint32_t mipLevelCount;
        uint32_t baseArrayLayer;
        uint32_t arrayLayerCount;
    };

    class ALIMER_API RHIBuffer : public RHIResource
    {

    };

    class ALIMER_API RHISampler : public RHIObject
    {

    };

    class ALIMER_API RHISwapChain : public RHIObject
    {
    public:
        virtual RHITextureView* GetCurrentTextureView() const = 0;

        const RHIExtent2D& GetExtent() const { return extent; }

    protected:
        RHIExtent2D extent;
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

        [[nodiscard]] virtual RHITextureRef CreateTexture(const RHITextureDescriptor& descriptor) = 0;
        [[nodiscard]] virtual RHISwapChainRef CreateSwapChain(void* window, const RHISwapChainDescriptor& descriptor) = 0;

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


    ALIMER_API bool RHInitialize(RHIValidationMode validationMode);
    ALIMER_API void RHIShutdown();

    ALIMER_API bool RHIBeginFrame();
    ALIMER_API void RHIEndFrame();

    ALIMER_API RHICommandBuffer* RHIBeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics);
}
