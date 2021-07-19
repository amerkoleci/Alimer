// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Graphics/PixelFormat.h"

namespace Alimer
{
	const PixelFormatDesc kFormatDesc[] =
	{
		{PixelFormat::Undefined, "Invalid", PixelFormatType::Unknown, 0, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}},
		// 8-bit formats
		{PixelFormat::R8UNorm, "R8UNorm", PixelFormatType::UNorm, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
		{PixelFormat::R8SNorm, "R8SNorm", PixelFormatType::SNorm, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
		{PixelFormat::R8UInt, "R8UInt", PixelFormatType::Uint, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
		{PixelFormat::R8SInt, "R8SInt", PixelFormatType::Sint, 8, {1, 1, 1, 1, 1}, {0, 0, 8, 0, 0, 0}},
		// 16-bit formats
		{PixelFormat::R16UNorm, "R16UNorm", PixelFormatType::UNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
		{PixelFormat::R16SNorm, "R16SNorm", PixelFormatType::SNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
		{PixelFormat::R16UInt, "R16UInt", PixelFormatType::Uint, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
		{PixelFormat::R16SInt, "R16SInt", PixelFormatType::Sint, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
		{PixelFormat::R16Float, "R16Float", PixelFormatType::Float, 16, {1, 1, 2, 1, 1}, {0, 0, 16, 0, 0, 0}},
		{PixelFormat::RG8UNorm, "RG8UNorm", PixelFormatType::UNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
		{PixelFormat::RG8SNorm, "RG8SNorm", PixelFormatType::SNorm, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
		{PixelFormat::RG8UInt, "RG8UInt", PixelFormatType::Uint, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
		{PixelFormat::RG8SInt, "RG8SInt", PixelFormatType::Sint, 16, {1, 1, 2, 1, 1}, {0, 0, 8, 8, 0, 0}},
		// 32-bit formats
		{PixelFormat::R32UInt, "R32UInt", PixelFormatType::Uint, 32, {1, 1, 4, 1, 1}, {0, 0, 32, 0, 0, 0}},
		{PixelFormat::R32SInt, "R32SInt", PixelFormatType::Sint, 32, {1, 1, 4, 1, 1}, {0, 0, 32, 0, 0, 0}},
		{PixelFormat::R32Float, "R32Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 32, 0, 0, 0}},
		{PixelFormat::RG16UNorm, "RG16UNorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
		{PixelFormat::RG16SNorm, "RG16SNorm", PixelFormatType::SNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
		{PixelFormat::RG16UInt, "RG16UInt", PixelFormatType::Uint, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
		{PixelFormat::RG16SInt, "RG16SInt", PixelFormatType::Sint, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
		{PixelFormat::RG16Float, "RG16Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 16, 16, 0, 0}},
		{PixelFormat::RGBA8UNorm, "RGBA8UNorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
		{PixelFormat::RGBA8UNormSrgb, "RGBA8UNormSrgb", PixelFormatType::UnormSrgb, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
		{PixelFormat::RGBA8SNorm, "RGBA8SNorm", PixelFormatType::SNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
		{PixelFormat::RGBA8UInt, "RGBA8UInt", PixelFormatType::Uint, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
		{PixelFormat::RGBA8SInt, "RGBA8SInt", PixelFormatType::Sint, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
		{PixelFormat::BGRA8UNorm, "BGRA8UNorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
		{PixelFormat::BGRA8UNormSrgb, "BGRA8UNormSrgb", PixelFormatType::UnormSrgb, 32, {1, 1, 4, 1, 1}, {0, 0, 8, 8, 8, 8}},
		// Packed 32-Bit formats
		{PixelFormat::RGB10A2UNorm, "RGB10A2UNorm", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {0, 0, 10, 10, 10, 2}},
		{PixelFormat::RG11B10Float, "RG11B10Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 11, 11, 10, 0}},
		{PixelFormat::RGB9E5Float, "RGB9E5Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {0, 0, 9, 9, 9, 5}},
		// 64-Bit formats
		{PixelFormat::RG32UInt, "RG32UInt", PixelFormatType::Uint, 64, {1, 1, 8, 1, 1}, {0, 0, 32, 32, 0, 0}},
		{PixelFormat::RG32SInt, "RG32SInt", PixelFormatType::Sint, 64, {1, 1, 8, 1, 1}, {0, 0, 32, 32, 0, 0}},
		{PixelFormat::RG32Float, "RG32Float", PixelFormatType::Float, 64, {1, 1, 8, 1, 1}, {0, 0, 32, 32, 0, 0}},
		{PixelFormat::RGBA16UNorm, "RGBA16UNorm", PixelFormatType::UNorm, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
		{PixelFormat::RGBA16SNorm, "RGBA16SNorm", PixelFormatType::SNorm, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
		{PixelFormat::RGBA16UInt, "RGBA16UInt", PixelFormatType::Uint, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
		{PixelFormat::RGBA16SInt, "RGBA16SInt", PixelFormatType::Sint, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
		{PixelFormat::RGBA16Float, "Rgba16Float", PixelFormatType::Float, 64, {1, 1, 8, 1, 1}, {0, 0, 16, 16, 16, 16}},
		// 128-Bit formats
		{PixelFormat::RGBA32UInt, "RGBA32UInt", PixelFormatType::Uint, 128, {1, 1, 16, 1, 1}, {0, 0, 32, 32, 32, 32}},
		{PixelFormat::RGBA32SInt, "RGBA32SInt", PixelFormatType::Sint, 128, {1, 1, 16, 1, 1}, {0, 0, 32, 32, 32, 32}},
		{PixelFormat::RGBA32Float, "RGBA32Float", PixelFormatType::Float, 128, {1, 1, 16, 1, 1},{0, 0, 32, 32, 32, 32}},
		// Depth-stencil formats
		{PixelFormat::Depth16UNorm, "Depth16UNorm", PixelFormatType::UNorm, 16, {1, 1, 2, 1, 1}, {16, 0, 0, 0, 0, 0}},
		{PixelFormat::Depth32Float, "Depth32Float", PixelFormatType::Float, 32, {1, 1, 4, 1, 1}, {32, 0, 0, 0, 0, 0}},
		{PixelFormat::Depth24UNormStencil8, "Depth24UNormStencil8", PixelFormatType::UNorm, 32, {1, 1, 4, 1, 1}, {24, 8, 0, 0, 0, 0}},
		{PixelFormat::Depth32FloatStencil8, "Depth32FloatStencil8", PixelFormatType::Float, 48, {1, 1, 4, 1, 1}, {32, 8, 0, 0, 0, 0}},
		// Compressed BC formats
		{PixelFormat::BC1RGBAUnorm, "BC1RGBAUnorm", PixelFormatType::UNorm, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC1RGBAUnormSrgb, "BC1RGBAUnormSrgb", PixelFormatType::UnormSrgb, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC2RGBAUnorm, "BC2RGBAUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC2RGBAUnormSrgb, "BC2RGBAUnormSrgb", PixelFormatType::UnormSrgb, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC3RGBAUnorm, "BC3RGBAUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC3RGBAUnormSrgb, "BC3RGBAUnormSrgb", PixelFormatType::UnormSrgb, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC4RUnorm, "BC4RUnorm", PixelFormatType::UNorm, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC4RSnorm, "BC4RSnorm", PixelFormatType::SNorm, 4, {4, 4, 8, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC5RGUnorm, "BC5RGUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC5RGSnorm, "BC5RGSnorm", PixelFormatType::SNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC6HRGBUfloat, "BC6HRGBUfloat", PixelFormatType::Float, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC6HRGBFloat, "BC6HRGBFloat", PixelFormatType::Float, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC7RGBAUnorm, "BC7RGBAUnorm", PixelFormatType::UNorm, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
		{PixelFormat::BC7RGBAUnormSrgb, "BC7RGBAUnormSrgb", PixelFormatType::UnormSrgb, 8, {4, 4, 16, 1, 1}, {0, 0, 0, 0, 0, 0}},
	};
}
