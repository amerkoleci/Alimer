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
		Vertex = 1 << 0,
		Index = 1 << 1,
		Uniform = 1 << 2,
		ShaderResource = 1 << 3,
		UnorderedAccess = 1 << 4,
		Indirect = 1 << 5,
	};

    struct BufferCreateInfo
    {
        const char* label = nullptr;
        MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
        BufferUsage usage = BufferUsage::None;
        uint64_t size = 0;
        PixelFormat format = PixelFormat::Undefined;
    };

	class ALIMER_API Buffer : public GPUResource, public RefCounted
	{
	public:
        [[nodiscard]] static BufferRef Create(const BufferCreateInfo& createInfo, const void* initialData = nullptr);
		[[nodiscard]] static BufferRef Create(const void* data, BufferUsage usage, uint64_t size, const char* label = nullptr);

		BufferUsage GetUsage() const noexcept { return usage; }
        uint64_t GetSize() const noexcept { return size; }
        MemoryUsage GetMemoryUsage() const noexcept { return memoryUsage; }

        virtual uint8_t* Map() = 0;
        virtual void Unmap() = 0;

	protected:
		/// Constructor.
		Buffer(const BufferCreateInfo& info);

		BufferUsage usage;
        uint64_t size;
        MemoryUsage memoryUsage;
	};
}

ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::BufferUsage);
