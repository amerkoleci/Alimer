// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Types.h"
#include "RHI/PixelFormat.h"

/* Constants */
static constexpr uint32_t kRHIMaxFramesInFlight = 2;
static constexpr uint32_t kRHIMaxBackBufferCount = 3;
static constexpr uint32_t kRHIMaxViewportsAndScissors = 8;
static constexpr uint32_t kRHIMaxVertexBufferBindings = 4;
static constexpr uint32_t kRHIMaxVertexAttributes = 16;
static constexpr uint32_t kRHIMaxVertexAttributeOffset = 2047u;
static constexpr uint32_t kRHIMaxVertexBufferStride = 2048u;

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
    /// Can be used for copy commands.
    Copy,
    Count
};

struct RHISwapChainDescriptor
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t bufferCount = 2;
    RHIPixelFormat format = RHIPixelFormat::BGRA8UNormSrgb;
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

class ALIMER_API RHICommandBuffer
{
public:
    virtual ~RHICommandBuffer() = default;

    RHICommandBuffer(const RHICommandBuffer&) = delete;
    RHICommandBuffer(RHICommandBuffer&&) = delete;
    RHICommandBuffer& operator=(const RHICommandBuffer&) = delete;
    RHICommandBuffer& operator=(RHICommandBuffer&&) = delete;

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

    virtual bool BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual RHICommandBuffer* BeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics) = 0;

    virtual bool CreateSwapChain(void* window, const RHISwapChainDescriptor* descriptor, RHISwapChain* pSwapChain) const = 0;

protected:
    RHIDevice() = default;
};

extern ALIMER_API RHIDevice* GRHIDevice;

#ifdef __cplusplus
extern "C" {
#endif

    ALIMER_API bool RHInitialize(RHIValidationMode validationMode);
    ALIMER_API void RHIShutdown();

    ALIMER_API bool RHIBeginFrame();
    ALIMER_API void RHIEndFrame();

    ALIMER_API RHICommandBuffer* RHIBeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics);

#ifdef __cplusplus
}
#endif
