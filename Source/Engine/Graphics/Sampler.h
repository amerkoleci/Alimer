// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/RefPtr.h"
#include "Graphics/GPUResource.h"

namespace Alimer
{
    enum class SamplerFilter : uint32_t
    {
        Nearest,
        Linear
    };

    enum class SamplerAddressMode : uint32_t
    {
        Wrap,
        Mirror,
        Clamp,
        Border,
        MirrorOnce,
    };

    enum class SamplerBorderColor : uint32_t
    {
        TransparentBlack,
        OpaqueBlack,
        OpaqueWhite,
    };

    struct SamplerDescription
    {
        SamplerFilter minFilter = SamplerFilter::Nearest;
        SamplerFilter magFilter = SamplerFilter::Nearest;
        SamplerFilter mipFilter = SamplerFilter::Nearest;
        SamplerAddressMode addressModeU = SamplerAddressMode::Clamp;
        SamplerAddressMode addressModeV = SamplerAddressMode::Clamp;
        SamplerAddressMode addressModeW = SamplerAddressMode::Clamp;
        uint16_t maxAnisotropy = 1;
        CompareFunction compareFunction = CompareFunction::Never;
        SamplerBorderColor borderColor = SamplerBorderColor::TransparentBlack;
        float lodMinClamp = 0.0f;
        float lodMaxClamp = FLT_MAX;
        const char* label = nullptr;
    };

	class ALIMER_API Sampler : public GPUObject, public RefCounted
	{
	public:
		static SamplerRef Create(const SamplerDescription& description);

        uint32_t GetBindlessIndex() const noexcept { return bindlessIndex; }

	protected:
		/// Constructor.
		Sampler();

        uint32_t bindlessIndex = kInvalidBindlessIndex;
	};
}
