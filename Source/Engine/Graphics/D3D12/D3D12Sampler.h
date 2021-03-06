// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Graphics/Sampler.h"
#include "D3D12Utils.h"

namespace Alimer
{
	class D3D12Sampler final : public Sampler
	{
	public:
		D3D12Sampler(D3D12Graphics& device, const SamplerDescription& desc);
		~D3D12Sampler() override;
		void Destroy() override;

        const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle() const { return handle; }

	private:
        D3D12Graphics& device;
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
	};

	constexpr const D3D12Sampler* ToD3D12(const Sampler* resource)
	{
		return static_cast<const D3D12Sampler*>(resource);
	}
}

