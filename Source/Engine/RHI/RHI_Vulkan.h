// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "AlimerConfig.h"
#if defined(ALIMER_RHI_VULKAN)
#include "RHI/RHI.h"

class RHICommandBufferVulkan final : public RHICommandBuffer
{
private:

public:
    RHICommandBufferVulkan();
    ~RHICommandBufferVulkan() override;
};

class RHIDeviceVulkan final : public RHIDevice
{
public:
    [[nodiscard]] static bool IsAvailable();

    RHIDeviceVulkan(RHIValidationMode validationMode);
    ~RHIDeviceVulkan() override;

    bool Initialize(RHIValidationMode validationMode) override;
    void Shutdown() override;
    bool BeginFrame() override;
    void EndFrame() override;

    RHICommandBuffer* BeginCommandBuffer(RHIQueueType type = RHIQueueType::Graphics) override;

private:
};

#endif /* defined(ALIMER_RHI_VULKAN) */
