// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Math/Vector3.h"
#include "Core/String.h"

namespace Alimer
{
    const Vector3 Vector3::Zero = { 0.0f, 0.0f, 0.0f };
    const Vector3 Vector3::One = { 1.0f, 1.0f, 1.0f };
    const Vector3 Vector3::UnitX = { 1.0f, 0.0f, 0.0f };
    const Vector3 Vector3::UnitY = { 0.0f, 1.0f, 0.0f };
    const Vector3 Vector3::UnitZ = { 0.0f, 0.0f, 1.0f };
    const Vector3 Vector3::Up = { 0.0f, 1.0f, 0.0f };
    const Vector3 Vector3::Down = { 0.0f, -1.0f, 0.0f };
    const Vector3 Vector3::Right = { 1.0f, 0.0f, 0.0f };
    const Vector3 Vector3::Left = { -1.0f, 0.0f, 0.0f };
    const Vector3 Vector3::Forward = { 0.0f, 0.0f, -1.0f };
    const Vector3 Vector3::Backward = { 0.0f, 0.0f, 1.0f };

    String Vector3::ToString() const
    {
        char tempBuffer[kConversionBufferLength];
        sprintf(tempBuffer, "%g %g %g", x, y, z);
        return String(tempBuffer);
    }

    size_t Vector3::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, x, y, z);
        return hash;
    }

    Vector3 Vector3::Add(const Vector3& value1, const Vector3& value2) noexcept
    {
        return Vector3(value1.x + value2.x, value1.y + value2.y, value1.z + value2.z);
    }

    Vector3 Vector3::Subtract(const Vector3& value1, const Vector3& value2) noexcept
    {
        return Vector3(value1.x - value2.x, value1.y - value2.y, value1.z - value2.z);
    }

    Vector3 Vector3::Multiply(const Vector3& value1, const Vector3& value2) noexcept
    {
        return Vector3(value1.x * value2.x, value1.y * value2.y, value1.z * value2.z);
    }

    Vector3 Vector3::Cross(const Vector3& value1, const Vector3& value2) noexcept
    {
        return Vector3(
            (value1.y * value2.z) - (value1.z * value2.y),
            (value1.z * value2.x) - (value1.x * value2.z),
            (value1.x * value2.y) - (value1.y * value2.x)
        );
    }
}
