// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Math/Quaternion.h"
#include "Core/String.h"

namespace Alimer
{
    const Quaternion Quaternion::Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
    const Quaternion Quaternion::One = { 1.0f, 1.0f, 1.0f, 1.0f };
    const Quaternion Quaternion::Identity = { 0.0f, 0.0f, 0.0f, 1.0f };

    std::string Quaternion::ToString() const
    {
        char tempBuffer[kConversionBufferLength];
        sprintf(tempBuffer, "%g %g %g %g", x, y, z, w);
        return String(tempBuffer);
    }

    size_t Quaternion::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, x, y, z, w);
        return hash;
    }
}
