// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Signal.h"
#include <memory>

namespace Alimer
{
	class ALIMER_API RHIDevice
	{
	public:
        virtual ~RHIDevice() = default;

        RHIDevice(const RHIDevice&) = delete;
        RHIDevice(RHIDevice&&) = delete;
        RHIDevice& operator=(const RHIDevice&) = delete;
        RHIDevice& operator=(RHIDevice&&) = delete;

	protected:
		/// Constructor.
        RHIDevice() = default;
	};

    extern ALIMER_API RHIDevice* GRHIDevice;
}
