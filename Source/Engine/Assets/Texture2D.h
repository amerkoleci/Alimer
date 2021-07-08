// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Assets/Texture.h"

namespace Alimer
{
    /// Defines 2D texture asset.
	class ALIMER_API Texture2D : public Texture
	{
        ALIMER_OBJECT(Texture2D, Texture);

	public:
        /// Constructor.
        Texture2D();

    protected:
	};
}
