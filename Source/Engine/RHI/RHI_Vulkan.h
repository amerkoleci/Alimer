// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "AlimerConfig.h"
#if defined(ALIMER_RHI_VULKAN)
#include "RHI/RHI.h"

namespace Alimer
{
    class RHIDeviceVulkan final : public RHIDevice
    {
    public:
        [[nodiscard]] static bool IsAvailable();

        bool Initialize(RHIValidationMode validationMode) override;
        void Shutdown() override;

        bool BeginFrame() override;
        void EndFrame() override;
    };
}

#endif /* defined(ALIMER_RHI_VULKAN) */
