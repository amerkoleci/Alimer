// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Assets/Texture.h"

namespace Alimer
{
    /// Defines texture cube asset.
	class ALIMER_API TextureCube : public Texture
	{
        ALIMER_OBJECT(TextureCube, Texture);

	public:
        /// Constructor.
        TextureCube();

    protected:
	};
}
