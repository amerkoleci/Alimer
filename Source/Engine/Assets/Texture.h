// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Assets/Asset.h"
#include "RHI/RHI.h"

namespace Alimer
{
    /// Base class for Texture assets.
	class ALIMER_API Texture : public Asset
	{
        ALIMER_OBJECT(Texture, Asset);

	public:
        const RHITexture* GetRHITexture() const { return rhiTexture.get(); }

    protected:
        /// Constructor.
        Texture() = default;

        SharedPtr<RHITexture> rhiTexture;
	};
}
