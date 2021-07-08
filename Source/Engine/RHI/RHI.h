// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Types.h"
#include "RHI/PixelFormat.h"

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

    enum class TextureUsage : uint32_t
    {
        Unknown = 0,
        ShaderRead = 1 << 0,
        ShaderWrite = 1 << 1,
        ShaderReadWrite = ShaderRead | ShaderWrite,
        RenderTarget = 1 << 2,
    };

    struct RHIExtent2D
    {
        uint32_t width;
        uint32_t height;
    };

    struct RHIExtent3D
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    };

    struct RHIColor
    {
        float r;
        float g;
        float b;
        float a;
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

        RHIObject(const RHIObject&) = delete;
        RHIObject(RHIObject&&) = delete;
        RHIObject& operator=(const RHIObject&) = delete;
        RHIObject& operator=(RHIObject&&) = delete;

        const String& GetName() const { return name; }

        void SetName(const String& newName);
        void SetName(const StringView& newName);

    protected:
        RHIObject() = default;

    private:
        virtual void ApiSetName(const StringView& name) = 0;

        String name;
    };

    class ALIMER_API RHIResource : public RHIObject
    {

    };

    class ALIMER_API RHIResourceView
    {
    public:
        virtual ~RHIResourceView() = default;

        RHIResourceView(const RHIResourceView&) = delete;
        RHIResourceView(RHIResourceView&&) = delete;
        RHIResourceView& operator=(const RHIResourceView&) = delete;
        RHIResourceView& operator=(RHIResourceView&&) = delete;

    protected:
        RHIResourceView() = default;
    };

    class ALIMER_API RHITexture : public RHIResource
    {

    };

    class ALIMER_API RHITextureView : public RHIResourceView
    {

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
        const RHIExtent2D& GetExtent() const { return extent; }

    protected:
        RHIExtent2D extent;
    };

    class ALIMER_API RHICommandBuffer
    {
    public:
        virtual ~RHICommandBuffer() = default;

        RHICommandBuffer(const RHICommandBuffer&) = delete;
        RHICommandBuffer(RHICommandBuffer&&) = delete;
        RHICommandBuffer& operator=(const RHICommandBuffer&) = delete;
        RHICommandBuffer& operator=(RHICommandBuffer&&) = delete;

        virtual void BeginRenderPass(RHISwapChain* swapChain, const RHIColor& clearColor) = 0;
        virtual void EndRenderPass() = 0;

    protected:
        RHICommandBuffer() = default;
    };

    class ALIMER_API RHIDevice
    {
    public:
        virtual ~RHIDevice() = default;

        RHIDevice(const RHIDevice&) = delete;
        RHIDevice(RHIDevice&&) = delete;
        RHIDevice& operator=(const RHIDevice&) = delete;
        RHIDevice& operator=(RHIDevice&&) = delete;

        virtual bool Initialize(RHIValidationMode validationMode) = 0;
        virtual void Shutdown() = 0;

        virtual void WaitForIdle() = 0;
        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;

        [[nodiscard]] virtual RHICommandBuffer* BeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics) = 0;

        [[nodiscard]] virtual SharedPtr<RHISwapChain> CreateSwapChain(void* window, const RHISwapChainDescriptor* descriptor) = 0;

        constexpr uint64_t GetFrameCount() const { return frameCount; }
        constexpr uint32_t GetFrameIndex() const { return frameIndex; }

    protected:
        RHIDevice() = default;

        uint64_t frameCount = 0;
        uint32_t frameIndex = 0;
    };

    extern ALIMER_API RHIDevice* GRHIDevice;


    ALIMER_API bool RHInitialize(RHIValidationMode validationMode);
    ALIMER_API void RHIShutdown();

    ALIMER_API bool RHIBeginFrame();
    ALIMER_API void RHIEndFrame();

    ALIMER_API RHICommandBuffer* RHIBeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics);
}
