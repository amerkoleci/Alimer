// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "RHI/PixelFormat.h"

const PixelFormatDesc kFormatDesc[] =
{
    {RHIPixelFormat::Undefined, "Invalid", PixelFormatType::Unknown, 0, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}},
    // 8-bit formats
    {RHIPixelFormat::R8Unorm, "R8Unorm", PixelFormatType::UNorm, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
    {RHIPixelFormat::R8Snorm, "R8Snorm", PixelFormatType::SNorm, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
    {RHIPixelFormat::R8Uint, "R8Uint", PixelFormatType::Uint, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
    {RHIPixelFormat::R8Sint, "R8Sint", PixelFormatType::Sint, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
    // 16-bit formats
    {RHIPixelFormat::R16Unorm, "R16Unorm", PixelFormatType::UNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
    {RHIPixelFormat::R16Snorm, "R16Snorm", PixelFormatType::SNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
    {RHIPixelFormat::R16Uint, "R16Uint", PixelFormatType::Uint, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
    {RHIPixelFormat::R16Sint, "R16Sint", PixelFormatType::Sint, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
    {RHIPixelFormat::R16Float, "R16Float", PixelFormatType::Float, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
    {RHIPixelFormat::RG8Unorm, "RG8Unorm", PixelFormatType::UNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
    {RHIPixelFormat::RG8Snorm, "RG8Snorm", PixelFormatType::SNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
    {RHIPixelFormat::RG8Uint, "RG8Uint", PixelFormatType::Uint, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
    {RHIPixelFormat::RG8Sint, "RG8Sint", PixelFormatType::Sint, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
    // 32-bit formats
    {RHIPixelFormat::R32Uint, "R32Uint", PixelFormatType::Uint, 32, {1, 1, 4, 1, 1}, {0, 0, 32, 0, 0, 0}},
    {RHIPixelFormat::R32Sint, "R32Sint", PixelFormatType::Sint, 32, {1, 1, 4, 1, 1}, {0, 0, 32, 0, 0, 0}},
    {RHIPixelFormat::R32Float, "R32Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 32, 0, 0, 0}},
    {RHIPixelFormat::RG16Unorm, "RG16Unorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
    {RHIPixelFormat::RG16Snorm, "RG16Snorm", PixelFormatType::SNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
    {RHIPixelFormat::RG16Uint, "RG16Uint", PixelFormatType::Uint, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
    {RHIPixelFormat::RG16Sint, "RG16Sint", PixelFormatType::Sint, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
    {RHIPixelFormat::RG16Float, "RG16Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
    {RHIPixelFormat::RGBA8UNorm, "RGBA8UNorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
    {RHIPixelFormat::RGBA8UNormSrgb, "RGBA8UNormSrgb", PixelFormatType::UnormSrgb, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
    {RHIPixelFormat::RGBA8SNorm, "RGBA8SNorm", PixelFormatType::SNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
    {RHIPixelFormat::RGBA8Uint, "RGBA8Uint", PixelFormatType::Uint, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
    {RHIPixelFormat::RGBA8Sint, "RGBA8Sint", PixelFormatType::Sint, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
    {RHIPixelFormat::BGRA8UNorm, "BGRA8UNorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
    {RHIPixelFormat::BGRA8UNormSrgb, "BGRA8UNormSrgb", PixelFormatType::UnormSrgb, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
    // Packed 32-Bit formats
    {RHIPixelFormat::RGB10A2Unorm, "RGB10A2Unorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 10, 10, 10, 2}},
    {RHIPixelFormat::RG11B10Float, "RG11B10Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 11, 11, 10, 0}},
    {RHIPixelFormat::RGB9E5Float, "RGB9E5Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 9, 9, 9, 5}},
    // 64-Bit formats
    {RHIPixelFormat::RG32Uint, "RG32Uint", PixelFormatType::Uint, 64, {1, 1, 8, 1, 1}, {0, 0, 32, 32, 0, 0}},
    {RHIPixelFormat::RG32Sint, "RG32Sint", PixelFormatType::Sint, 64, {1, 1, 8, 1, 1}, {0, 0, 32, 32, 0, 0}},
    {RHIPixelFormat::RG32Float, "RG32Float", PixelFormatType::Float, 64, {1, 1, 8, 1, 1}, {0, 0, 32, 32, 0, 0}},
    {RHIPixelFormat::RGBA16Unorm, "RGBA16Unorm", PixelFormatType::UNorm, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
    {RHIPixelFormat::RGBA16Snorm, "RGBA16Snorm", PixelFormatType::SNorm, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
    {RHIPixelFormat::RGBA16Uint, "RGBA16Uint", PixelFormatType::Uint, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
    {RHIPixelFormat::RGBA16Sint, "RGBA16Sint", PixelFormatType::Sint, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
    {RHIPixelFormat::RGBA16Float, "Rgba16Float", PixelFormatType::Float, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
    // 128-Bit formats
    {RHIPixelFormat::RGBA32Uint, "RGBA32Uint", PixelFormatType::Uint, 128, {1, 1, 16, 1, 1}, {0, 0, 32, 32, 32, 32}},
    {RHIPixelFormat::RGBA32Sint, "RGBA32Sint", PixelFormatType::Sint, 128, {1, 1, 16, 1, 1}, {0, 0, 32, 32, 32, 32}},
    {RHIPixelFormat::RGBA32Float, "RGBA32Float", PixelFormatType::Float, 128, {1, 1, 16, 1, 1},{0, 0, 32, 32, 32, 32}},
    // Depth-stencil formats
    {RHIPixelFormat::Depth16Unorm, "Depth16Unorm", PixelFormatType::UNorm, 16, {1, 1, 2, 1, 1}, {16, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::Depth32Float, "Depth32Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {32, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::Depth24UnormStencil8, "Depth24UnormStencil8", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {24, 8, 0, 0, 0, 0}},
    {RHIPixelFormat::Depth32FloatStencil8, "Depth32FloatStencil8", PixelFormatType::Float, 48, {1, 1, 4, 1, 1}, {32, 8, 0, 0, 0, 0}},
    // Compressed BC formats
    {RHIPixelFormat::BC1RGBAUnorm, "BC1RGBAUnorm", PixelFormatType::UNorm, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC1RGBAUnormSrgb, "BC1RGBAUnormSrgb", PixelFormatType::UnormSrgb, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC2RGBAUnorm, "BC2RGBAUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC2RGBAUnormSrgb, "BC2RGBAUnormSrgb", PixelFormatType::UnormSrgb, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC3RGBAUnorm, "BC3RGBAUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC3RGBAUnormSrgb, "BC3RGBAUnormSrgb", PixelFormatType::UnormSrgb, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC4RUnorm, "BC4RUnorm", PixelFormatType::UNorm, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC4RSnorm, "BC4RSnorm", PixelFormatType::SNorm, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC5RGUnorm, "BC5RGUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC5RGSnorm, "BC5RGSnorm", PixelFormatType::SNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC6HRGBUfloat, "BC6HRGBUfloat", PixelFormatType::Float, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC6HRGBFloat, "BC6HRGBFloat", PixelFormatType::Float, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC7RGBAUnorm, "BC7RGBAUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
    {RHIPixelFormat::BC7RGBAUnormSrgb, "BC7RGBAUnormSrgb", PixelFormatType::UnormSrgb, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
};

const char* ToString(RHIPixelFormat format)
{
    ALIMER_ASSERT(kFormatDesc[(uint32_t)format].format == format);
    return kFormatDesc[(uint32_t)format].name;
}
