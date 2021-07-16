// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "D3D12Sampler.h"
#include "D3D12Graphics.h"

namespace Alimer
{
    namespace
    {
        [[nodiscard]] constexpr D3D12_FILTER_TYPE ToD3D12FilterType(SamplerFilter value)
        {
            switch (value)
            {
                case SamplerFilter::Nearest:
                    return D3D12_FILTER_TYPE_POINT;
                case SamplerFilter::Linear:
                    return D3D12_FILTER_TYPE_LINEAR;
                default:
                    ALIMER_UNREACHABLE();
                    return D3D12_FILTER_TYPE_POINT;
            }
        }

        [[nodiscard]] constexpr D3D12_TEXTURE_ADDRESS_MODE ToD3D12AddressMode(SamplerAddressMode mode)
        {
            switch (mode) {
                case SamplerAddressMode::Wrap:
                    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                case SamplerAddressMode::Mirror:
                    return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
                case SamplerAddressMode::Clamp:
                    return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
                case SamplerAddressMode::Border:
                    return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                case SamplerAddressMode::MirrorOnce:
                    return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
                default:
                    ALIMER_UNREACHABLE();
                    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            }
        }
    }

    D3D12Sampler::D3D12Sampler(D3D12Graphics& device_, const SamplerDescription& desc)
        : device(device_)
    {
        const D3D12_FILTER_TYPE minFilter = ToD3D12FilterType(desc.minFilter);
        const D3D12_FILTER_TYPE magFilter = ToD3D12FilterType(desc.magFilter);
        const D3D12_FILTER_TYPE mipFilter = ToD3D12FilterType(desc.mipFilter);

        D3D12_FILTER_REDUCTION_TYPE reduction = desc.compareFunction == CompareFunction::Never
            ? D3D12_FILTER_REDUCTION_TYPE_STANDARD
            : D3D12_FILTER_REDUCTION_TYPE_COMPARISON;

        D3D12_SAMPLER_DESC samplerDesc{};

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_sampler_desc
        if (desc.maxAnisotropy > 1)
        {
            samplerDesc.Filter = D3D12_ENCODE_ANISOTROPIC_FILTER(reduction);
        }
        else
        {
            samplerDesc.Filter = D3D12_ENCODE_BASIC_FILTER(minFilter, magFilter, mipFilter, reduction);
        }

        samplerDesc.AddressU = ToD3D12AddressMode(desc.addressModeU);
        samplerDesc.AddressV = ToD3D12AddressMode(desc.addressModeV);
        samplerDesc.AddressW = ToD3D12AddressMode(desc.addressModeW);
        samplerDesc.MipLODBias = 0.f;
        samplerDesc.MaxAnisotropy = Min<UINT>(desc.maxAnisotropy, 16u);
        if (desc.compareFunction != CompareFunction::Never)
        {
            samplerDesc.ComparisonFunc = ToD3D12ComparisonFunc(desc.compareFunction);
        }
        else
        {
            samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        }

        switch (desc.borderColor)
        {
            case SamplerBorderColor::OpaqueBlack:
                samplerDesc.BorderColor[0] = 0.0f;
                samplerDesc.BorderColor[1] = 0.0f;
                samplerDesc.BorderColor[2] = 0.0f;
                samplerDesc.BorderColor[3] = 1.0f;
                break;

            case SamplerBorderColor::OpaqueWhite:
                samplerDesc.BorderColor[0] = 1.0f;
                samplerDesc.BorderColor[1] = 1.0f;
                samplerDesc.BorderColor[2] = 1.0f;
                samplerDesc.BorderColor[3] = 1.0f;
                break;
            default:
                samplerDesc.BorderColor[0] = 0.0f;
                samplerDesc.BorderColor[1] = 0.0f;
                samplerDesc.BorderColor[2] = 0.0f;
                samplerDesc.BorderColor[3] = 0.0f;
                break;
        }

        samplerDesc.MinLOD = desc.lodMinClamp;
        samplerDesc.MaxLOD = desc.lodMaxClamp;

        handle = device.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        device.GetHandle()->CreateSampler(&samplerDesc, handle);
        bindlessIndex = device.AllocateBindlessSampler(handle);
    }

    D3D12Sampler::~D3D12Sampler()
    {
        Destroy();
    }

    void D3D12Sampler::Destroy()
    {
        if (handle.ptr != 0)
        {
            device.FreeDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, handle);
        }

        device.FreeBindlessSampler(bindlessIndex);
    }
}
