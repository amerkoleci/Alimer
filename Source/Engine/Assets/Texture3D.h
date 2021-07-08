// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Assets/Texture.h"

namespace Alimer
{
    /// Defines 4D texture asset.
	class ALIMER_API Texture3D : public Texture
	{
        ALIMER_OBJECT(Texture3D, Texture);

	public:
        /// Constructor.
        Texture3D();

    protected:
	};
}
