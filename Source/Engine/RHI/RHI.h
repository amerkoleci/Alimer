// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Types.h"
#include "RHI/PixelFormat.h"

namespace Alimer
{
    /* Constants */
    static constexpr uint32_t kMaxFramesInFlight = 2;
    static constexpr uint32_t kMaxViewportsAndScissors = 8;
    static constexpr uint32_t kMaxVertexBufferBindings = 4;
    static constexpr uint32_t kMaxVertexAttributes = 16;
    static constexpr uint32_t kMaxVertexAttributeOffset = 2047u;
    static constexpr uint32_t kMaxVertexBufferStride = 2048u;

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

    struct RHUSwapChainDescriptor
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t bufferCount = 2;
        PixelFormat format = PixelFormat::BGRA8UNormSrgb;
        bool fullscreen = false;
        bool verticalSync = true;
    };

    struct ALIMER_API RHIObject
    {
        std::shared_ptr<void> state;
        inline bool IsValid() const { return state.get() != nullptr; }
    };

    struct ALIMER_API RHIResource : public RHIObject
    {

    };

    struct ALIMER_API RHITexture : public RHIResource
    {

    };

    struct ALIMER_API RHIBuffer : public RHIResource
    {

    };

    struct ALIMER_API RHISampler : public RHIObject
    {

    };

    struct ALIMER_API RHISwapChain : public RHIObject
    {

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

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual bool CreateSwapChain(void* window, const RHUSwapChainDescriptor* descriptor, RHISwapChain* swapChain) const = 0;

    protected:
        /// Constructor.
        RHIDevice() = default;
    };

    extern ALIMER_API RHIDevice* GRHIDevice;

    ALIMER_API bool RHInitialize(RHIValidationMode validationMode);
    ALIMER_API void RHIShutdown();

    ALIMER_FORCE_INLINE bool RHIBeginFrame()
    {
        return GRHIDevice->BeginFrame();
    }

    ALIMER_FORCE_INLINE void RHIEndFrame()
    {
        GRHIDevice->EndFrame();
    }
}
