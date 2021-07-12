// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Math/Color.h"
#include "Core/String.h"

namespace Alimer
{
    String Color::ToString() const
    {
        char tempBuffer[kConversionBufferLength];
        sprintf(tempBuffer, "%g %g %g %g", r, g, b, a);
        return String(tempBuffer);
    }

    size_t Color::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, r, g, b, a);
        return hash;
    }
}
