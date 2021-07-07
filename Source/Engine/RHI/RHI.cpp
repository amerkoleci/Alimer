// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"
#include "RHI/RHI.h"

#if defined(ALIMER_RHI_D3D12)
#   include "RHI_D3D12.h"
#endif

namespace Alimer
{
    RHIDevice* GRHIDevice = nullptr;

    bool RHInitialize(RHIValidationMode validationMode)
    {
        if (GRHIDevice != nullptr)
            return true;

#if defined(ALIMER_RHI_D3D12)
        GRHIDevice = new RHIDeviceD3D12(validationMode);
#endif
        return GRHIDevice->Initialize(validationMode);
    }

    void RHIShutdown()
    {
        if (GRHIDevice != nullptr)
        {
            GRHIDevice->Shutdown();
            delete GRHIDevice;
            GRHIDevice = nullptr;
        }
    }
}
