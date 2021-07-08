// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "AlimerConfig.h"
#include "Core/Types.h"
#include "Core/Assert.h"

/// Defines pixel format.
enum class RHIPixelFormat : uint32_t
{
    Undefined = 0,
    // 8-bit formats
    R8Unorm,
    R8Snorm,
    R8Uint,
    R8Sint,
    // 16-bit formats
    R16Unorm,
    R16Snorm,
    R16Uint,
    R16Sint,
    R16Float,
    RG8Unorm,
    RG8Snorm,
    RG8Uint,
    RG8Sint,
    // 32-bit formats
    R32Uint,
    R32Sint,
    R32Float,
    RG16Unorm,
    RG16Snorm,
    RG16Uint,
    RG16Sint,
    RG16Float,
    RGBA8UNorm,
    RGBA8UNormSrgb,
    RGBA8SNorm,
    RGBA8Uint,
    RGBA8Sint,
    BGRA8UNorm,
    BGRA8UNormSrgb,
    // Packed 32-Bit formats
    RGB10A2Unorm,
    RG11B10Float,
    RGB9E5Float,
    // 64-Bit formats
    RG32Uint,
    RG32Sint,
    RG32Float,
    RGBA16Unorm,
    RGBA16Snorm,
    RGBA16Uint,
    RGBA16Sint,
    RGBA16Float,
    // 128-Bit formats
    RGBA32Uint,
    RGBA32Sint,
    RGBA32Float,
    // Depth-stencil formats
    Depth16Unorm,
    Depth32Float,
    Depth24UnormStencil8,
    Depth32FloatStencil8,
    // Compressed BC formats
    BC1RGBAUnorm,
    BC1RGBAUnormSrgb,
    BC2RGBAUnorm,
    BC2RGBAUnormSrgb,
    BC3RGBAUnorm,
    BC3RGBAUnormSrgb,
    BC4RUnorm,
    BC4RSnorm,
    BC5RGUnorm,
    BC5RGSnorm,
    BC6HRGBUfloat,
    BC6HRGBFloat,
    BC7RGBAUnorm,
    BC7RGBAUnormSrgb,
    Count
};

/// Pixel format Type
enum class PixelFormatType
{
    /// Unknown format Type.
    Unknown = 0,
    /// Floating-point formats.
    Float,
    /// Unsigned normalized formats.
    UNorm,
    /// Unsigned normalized SRGB formats
    UnormSrgb,
    /// Signed normalized formats.
    SNorm,
    /// Unsigned integer formats.
    Uint,
    /// Signed integer formats.
    Sint
};

struct PixelFormatDesc
{
    RHIPixelFormat format;
    const char* name;
    PixelFormatType type;
    uint8_t bitsPerPixel;
    struct
    {
        uint8_t blockWidth;
        uint8_t blockHeight;
        uint8_t blockSize;
        uint8_t minBlockX;
        uint8_t minBlockY;
    } compression;

    struct
    {
        uint8_t depth;
        uint8_t stencil;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    } bits;
};

ALIMER_API extern const PixelFormatDesc kFormatDesc[];

/// Get the number of bits per format.
constexpr uint32_t GetFormatBitsPerPixel(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].bitsPerPixel;
}

constexpr uint32_t GetFormatBlockSize(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].compression.blockSize;
}

/// Check if the format has a depth component
constexpr bool IsDepthFormat(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].bits.depth > 0;
}

/// Check if the format has a stencil component
constexpr bool IsStencilFormat(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].bits.stencil > 0;
}

/// Check if the format has depth or stencil components
constexpr bool IsDepthStencilFormat(RHIPixelFormat format)
{
    return IsDepthFormat(format) || IsStencilFormat(format);
}

/// Check if the format is a compressed format
constexpr bool IsBlockCompressedFormat(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    switch (format)
    {
        case RHIPixelFormat::BC1RGBAUnorm:
        case RHIPixelFormat::BC1RGBAUnormSrgb:
        case RHIPixelFormat::BC2RGBAUnorm:
        case RHIPixelFormat::BC2RGBAUnormSrgb:
        case RHIPixelFormat::BC3RGBAUnorm:
        case RHIPixelFormat::BC3RGBAUnormSrgb:
        case RHIPixelFormat::BC4RUnorm:
        case RHIPixelFormat::BC4RSnorm:
        case RHIPixelFormat::BC5RGUnorm:
        case RHIPixelFormat::BC5RGSnorm:
        case RHIPixelFormat::BC6HRGBUfloat:
        case RHIPixelFormat::BC6HRGBFloat:
        case RHIPixelFormat::BC7RGBAUnorm:
        case RHIPixelFormat::BC7RGBAUnormSrgb:
            return true;
    }

    return false;
}

/// Get the format compression ration along the x-axis
constexpr uint32_t GetFormatBlockWidth(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].compression.blockWidth;
}

/// Get the format compression ration along the y-axis
constexpr uint32_t GetFormatBlockHeight(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].compression.blockHeight;
}

/// Get the format Type
constexpr PixelFormatType GetFormatType(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].type;
}

ALIMER_API const char* ToString(RHIPixelFormat format);

/// Check if a format represents sRGB color space.
constexpr bool IsSrgbFormat(RHIPixelFormat format) { return (GetFormatType(format) == PixelFormatType::UnormSrgb); }

/// Convert a SRGB format to linear. If the format is already linear no conversion will be made.
constexpr RHIPixelFormat SRGBToLinearFormat(RHIPixelFormat format)
{
    switch (format)
    {
        case RHIPixelFormat::BC1RGBAUnormSrgb:
            return RHIPixelFormat::BC1RGBAUnorm;
        case RHIPixelFormat::BC2RGBAUnormSrgb:
            return RHIPixelFormat::BC2RGBAUnorm;
        case RHIPixelFormat::BC3RGBAUnormSrgb:
            return RHIPixelFormat::BC3RGBAUnorm;
        case RHIPixelFormat::BGRA8UNormSrgb:
            return RHIPixelFormat::BGRA8UNorm;
        case RHIPixelFormat::RGBA8UNormSrgb:
            return RHIPixelFormat::RGBA8UNorm;
        case RHIPixelFormat::BC7RGBAUnormSrgb:
            return RHIPixelFormat::BC7RGBAUnorm;
        default:
            ALIMER_ASSERT(IsSrgbFormat(format) == false);
            return format;
    }
}

/// Convert an linear format to sRGB. If the format doesn't have a matching sRGB format no conversion will be made.
constexpr RHIPixelFormat LinearToSRGBFormat(RHIPixelFormat format)
{
    switch (format)
    {
        case RHIPixelFormat::BC1RGBAUnorm:
            return RHIPixelFormat::BC1RGBAUnormSrgb;
        case RHIPixelFormat::BC2RGBAUnorm:
            return RHIPixelFormat::BC2RGBAUnormSrgb;
        case RHIPixelFormat::BC3RGBAUnorm:
            return RHIPixelFormat::BC3RGBAUnormSrgb;
        case RHIPixelFormat::BGRA8UNorm:
            return RHIPixelFormat::BGRA8UNormSrgb;
        case RHIPixelFormat::RGBA8UNorm:
            return RHIPixelFormat::RGBA8UNormSrgb;
        case RHIPixelFormat::BC7RGBAUnorm:
            return RHIPixelFormat::BC7RGBAUnormSrgb;
        default:
            return format;
    }
}
