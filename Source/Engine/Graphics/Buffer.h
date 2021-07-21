// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/RefPtr.h"
#include "Graphics/GPUResource.h"

namespace Alimer
{
	enum class BufferUsage : uint32_t
	{
		None = 0,
        InputAssembly = 1 << 0,
		Constant = 1 << 1,
        ShaderRead = 1 << 2,
        ShaderWrite = 1 << 3,
        ShaderReadWrite = ShaderRead | ShaderWrite,
		Indirect = 1 << 4,
        RayTracingAccelerationStructure = 1 << 5,
	};

    struct BufferDescription
    {
        MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
        BufferUsage usage = BufferUsage::None;
        uint64_t size = 0;
        PixelFormat format = PixelFormat::Unknown;
        const char* label = nullptr;
    };

	class ALIMER_API Buffer : public GPUResource, public RefCounted
	{
	public:
        [[nodiscard]] static BufferRef Create(const BufferDescription& description, const void* initialData = nullptr);
		[[nodiscard]] static BufferRef Create(const void* data, BufferUsage usage, uint64_t size, const char* label = nullptr);

		BufferUsage GetUsage() const noexcept { return usage; }
        uint64_t GetSize() const noexcept { return size; }
        MemoryUsage GetMemoryUsage() const noexcept { return memoryUsage; }

        virtual uint8_t* Map() = 0;
        virtual void Unmap() = 0;

	protected:
		/// Constructor.
		Buffer(const BufferDescription& info);

		BufferUsage usage;
        uint64_t size;
        MemoryUsage memoryUsage;
	};
}

ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::BufferUsage);
