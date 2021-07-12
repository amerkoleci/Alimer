// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Math/Vector2.h"
#include "Core/String.h"

namespace Alimer
{
    const Vector2 Vector2::Zero = { 0.0f, 0.0f };
    const Vector2 Vector2::Left = { -1.0f, 0.0f };
    const Vector2 Vector2::Right = { 1.0f, 0.0f };
    const Vector2 Vector2::Up = { 0.0f, 1.0f };
    const Vector2 Vector2::Down = { 0.0f, -1.0f };
    const Vector2 Vector2::One = { 1.0f, 1.0f };

    const Int2 Int2::Zero = { 0, 0 };
    const Int2 Int2::Left = { -1, 0 };
    const Int2 Int2::Right = { 1, 0 };
    const Int2 Int2::Up = { 0, 1 };
    const Int2 Int2::Down = { 0, -1 };
    const Int2 Int2::One = { 1, 1 };

    String Vector2::ToString() const
    {
        char tempBuffer[kConversionBufferLength];
        sprintf(tempBuffer, "%g %g", x, y);
        return String(tempBuffer);
    }

    size_t Vector2::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, x, y);
        return hash;
    }

    String Int2::ToString() const
    {
        char tempBuffer[kConversionBufferLength];
        sprintf(tempBuffer, "%d %d", x, y);
        return String(tempBuffer);
    }

    size_t Int2::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, x, y);
        return hash;
    }
}
