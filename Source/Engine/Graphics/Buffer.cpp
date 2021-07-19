// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Graphics/Buffer.h"
#include "Graphics/Graphics.h"
#include "Core/Log.h"

namespace Alimer
{
	Buffer::Buffer(const BufferDescription& desc)
		: GPUResource(Type::Buffer)
		, memoryUsage(desc.memoryUsage)
		, usage(desc.usage)
		, size(desc.size)
	{

	}

    BufferRef Buffer::Create(const BufferDescription& description, const void* initialData)
    {
        ALIMER_ASSERT(gGraphics().IsInitialized());

        ALIMER_ASSERT(description.size > 0);

        static constexpr uint64_t kMaxBufferSize = 128u * 1024u * 1024u;

        if (description.size > kMaxBufferSize)
        {
            LOGE("Buffer size too large (size {})", description.size);
            return nullptr;
        }

        return gGraphics().CreateBuffer(description, initialData);
    }

    BufferRef Buffer::Create(const void* data, BufferUsage usage, uint64_t size, const char* label)
	{
        ALIMER_ASSERT(data != nullptr);
		ALIMER_ASSERT(gGraphics().IsInitialized());

        BufferDescription desc;
        desc.memoryUsage = MemoryUsage::GpuOnly;
        desc.usage = usage;
        desc.size = size;
        desc.label = label;
		return gGraphics().CreateBuffer(desc, data);
	}
}
