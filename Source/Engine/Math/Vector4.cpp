// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Math/Vector4.h"
#include "Core/String.h"

namespace Alimer
{
    const Vector4 Vector4::Zero = { 0.f, 0.f, 0.f, 0.f };
    const Vector4 Vector4::One = { 1.f, 1.f, 1.f, 1.f };
    const Vector4 Vector4::UnitX = { 1.f, 0.f, 0.f, 0.f };
    const Vector4 Vector4::UnitY = { 0.f, 1.f, 0.f, 0.f };
    const Vector4 Vector4::UnitZ = { 0.f, 0.f, 1.f, 0.f };
    const Vector4 Vector4::UnitW = { 0.f, 0.f, 0.f, 1.f };

    String Vector4::ToString() const
    {
        char tempBuffer[kConversionBufferLength];
        sprintf(tempBuffer, "%g %g %g %g", x, y, z, w);
        return String(tempBuffer);
    }

    size_t Vector4::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, x, y, z, w);
        return hash;
    }
}
