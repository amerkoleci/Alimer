// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Assert.h"
#include <cmath>
#include <cstdlib>
#include <limits>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4244) // Conversion from 'double' to 'float'
#	pragma warning(disable : 4702) // unreachable code
#endif

namespace Alimer
{
    static constexpr float Pi = 3.1415926535897932f;
    static constexpr float TwoPi = 6.28318530718f;
    static constexpr float PiOver2 = 1.57079632679f;
    static constexpr float PiOver4 = 0.78539816339f;
    static constexpr float HalfPi = PiOver2;

    

    template <typename T> inline T Sign(T v) { return v < T(0) ? T(-1) : (v > T(0) ? T(1) : T(0)); }

    template <typename T> inline T sin(T v) { return std::sin(v); }
    template <typename T> inline T cos(T v) { return std::cos(v); }

    /// Return tangent of an angle in degrees.
    template <class T> inline T Tan(T angle) { return std::tan(angle); }

    template <typename T> inline T Asin(T v) { return std::asin(v); }
    template <typename T> inline T Acos(T v) { return std::acos(v); }
    template <typename T> inline T Atan(T v) { return std::atan(v); }
    template <typename T> inline T Log2(T v) { return std::log2(v); }
    template <typename T> inline T Log10(T v) { return std::log10(v); }
    template <typename T> inline T Ln(T v) { return std::log(v); }
    template <typename T> inline T Exp2(T v) { return std::exp2(v); }
    template <typename T> inline T Exp(T v) { return std::exp(v); }
    template <typename T> inline T Pow(T a, T b) { return std::pow(a, b); }
    template <typename T> inline T ToRadians(T degrees) { return degrees * (Pi / 180.0f); }
    template <typename T> inline T ToDegrees(T radians) { return radians * (180.0f / Pi); }

    /// Linear interpolation between two values.
    template <typename T, typename U> inline T Lerp(T lhs, T rhs, U t) { return lhs * (1.0 - t) + rhs * t; }

    /// Inverse linear interpolation between two values.
    template <typename T> inline T InverseLerp(T lhs, T rhs, T x) { return (x - lhs) / (rhs - lhs); }

    /// Check whether a floating point value is NaN.
    template <typename T> inline bool IsNaN(T value) { return std::isnan(value); }

    /// Check whether a floating point value is positive or negative infinity.
    template <typename T> inline bool IsInf(T value) { return std::isinf(value); }

    template <typename T> inline constexpr bool IsPowerOfTwo(T x)
    {
        static_assert(std::is_integral<T>::value, "IsPowerOfTwo must be called on an integer type.");
        return (x & (x - 1)) == 0 && (x != 0);
    }

    /// Round up to next power of two.
    constexpr uint32_t NextPowerOfTwo(uint32_t value)
    {
        // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
        --value;
        value |= value >> 1u;
        value |= value >> 2u;
        value |= value >> 4u;
        value |= value >> 8u;
        value |= value >> 16u;
        return ++value;
    }

    /// Round up or down to the closest power of two.
    inline uint32_t ClosestPowerOfTwo(uint32_t value)
    {
        const uint32_t next = NextPowerOfTwo(value);
        const uint32_t prev = next >> 1u;
        return (value - prev) > (next - value) ? next : prev;
    }

    constexpr uint32_t AlignTo(uint32_t value, uint32_t alignment)
    {
        ALIMER_ASSERT(alignment > 0);
        return ((value + alignment - 1) / alignment) * alignment;
    }

    constexpr uint64_t AlignTo(uint64_t value, uint64_t alignment)
    {
        ALIMER_ASSERT(alignment > 0);
        return ((value + alignment - 1) / alignment) * alignment;
    }

    constexpr uint32_t AlignUpWithMask(uint32_t value, uint32_t mask)
    {
        return ((value + mask) & ~mask);
    }

    constexpr uint64_t AlignUpWithMask(uint64_t value, uint64_t mask)
    {
        return ((value + mask) & ~mask);
    }

    /// Return a representation of the specified floating-point value as a single format bit layout.
    constexpr unsigned FloatToRawIntBits(float value)
    {
        unsigned u = *((unsigned*)&value);
        return u;
    }

    /// Return fractional part of passed value in range [0, 1).
    template <typename T> inline T Fract(T value) { return value - floor(value); }

    /// Round value down.
    template <typename T> inline T Floor(T x) { return floor(x); }

    /// Round value down to nearest number that can be represented as i*y, where i is integer.
    template <typename T> inline T SnapFloor(T x, T y) { return floor(x / y) * y; }

    /// Round value down. Returns integer value.
    template <typename T> inline int FloorToInt(T x) { return static_cast<int>(floor(x)); }

    /// Round value to nearest integer.
    template <typename T> inline T Round(T x) { return round(x); }

    constexpr inline uint32_t Kilobytes(uint32_t value) { return (value * 1024); }
    constexpr inline uint32_t Megabytes(uint32_t value) { return (value * 1024 * 1024); }
}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif
