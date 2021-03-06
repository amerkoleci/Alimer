// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Object.h"

namespace Alimer
{
	class Stream;

    /// Base class for Assets.
	class ALIMER_API Asset : public Object
	{
        ALIMER_OBJECT(Asset, Object);

	public:
		/// Destructor.
		virtual ~Asset() = default;

    protected:
        /// Constructor.
        Asset() = default;
	};
}
