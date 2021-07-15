// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.
// Based on Wicked Engine RHI: https://github.com/turanszkij/WickedEngine/blob/master/LICENSE.txt

#include "AlimerConfig.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "RHI/RHI.h"

#if defined(ALIMER_RHI_D3D12)
#   include "RHI_D3D12.h"
#endif

#if defined(ALIMER_RHI_VULKAN) 
#   include "RHI_Vulkan.h"
#endif

namespace Alimer
{
    /* RHICommandBuffer */
    void RHICommandBuffer::SetIndexBuffer(const GPUBuffer* buffer, RHIIndexType indexType, uint32_t offset)
    {
        ALIMER_ASSERT(buffer != nullptr);
        //ALIMER_ASSERT_MSG(Any(buffer->GetUsage(), RHIBufferUsage::Index), "Buffer created without Index usage");

        SetIndexBufferCore(buffer, indexType, offset);
    }

    uint32_t GetFormatStride(FORMAT value)
    {
        switch (value)
        {

            case FORMAT_R32G32B32A32_FLOAT:
            case FORMAT_R32G32B32A32_UINT:
            case FORMAT_R32G32B32A32_SINT:
            case FORMAT_BC1_UNORM:
            case FORMAT_BC1_UNORM_SRGB:
            case FORMAT_BC2_UNORM:
            case FORMAT_BC2_UNORM_SRGB:
            case FORMAT_BC3_UNORM:
            case FORMAT_BC3_UNORM_SRGB:
            case FORMAT_BC4_SNORM:
            case FORMAT_BC4_UNORM:
            case FORMAT_BC5_SNORM:
            case FORMAT_BC5_UNORM:
            case FORMAT_BC6H_UF16:
            case FORMAT_BC6H_SF16:
            case FORMAT_BC7_UNORM:
            case FORMAT_BC7_UNORM_SRGB:
                return 16;

            case FORMAT_R32G32B32_FLOAT:
            case FORMAT_R32G32B32_UINT:
            case FORMAT_R32G32B32_SINT:
                return 12;

            case FORMAT_R16G16B16A16_FLOAT:
            case FORMAT_R16G16B16A16_UNORM:
            case FORMAT_R16G16B16A16_UINT:
            case FORMAT_R16G16B16A16_SNORM:
            case FORMAT_R16G16B16A16_SINT:
                return 8;

            case FORMAT_R32G32_FLOAT:
            case FORMAT_R32G32_UINT:
            case FORMAT_R32G32_SINT:
            case FORMAT_R32G8X24_TYPELESS:
            case FORMAT_D32_FLOAT_S8X24_UINT:
                return 8;

            case FORMAT_R10G10B10A2_UNORM:
            case FORMAT_R10G10B10A2_UINT:
            case FORMAT_R11G11B10_FLOAT:
            case FORMAT_R8G8B8A8_UNORM:
            case FORMAT_R8G8B8A8_UNORM_SRGB:
            case FORMAT_R8G8B8A8_UINT:
            case FORMAT_R8G8B8A8_SNORM:
            case FORMAT_R8G8B8A8_SINT:
            case FORMAT_B8G8R8A8_UNORM:
            case FORMAT_B8G8R8A8_UNORM_SRGB:
            case FORMAT_R16G16_FLOAT:
            case FORMAT_R16G16_UNORM:
            case FORMAT_R16G16_UINT:
            case FORMAT_R16G16_SNORM:
            case FORMAT_R16G16_SINT:
            case FORMAT_R32_TYPELESS:
            case FORMAT_D32_FLOAT:
            case FORMAT_R32_FLOAT:
            case FORMAT_R32_UINT:
            case FORMAT_R32_SINT:
            case FORMAT_R24G8_TYPELESS:
            case FORMAT_D24_UNORM_S8_UINT:
                return 4;

            case FORMAT_R8G8_UNORM:
            case FORMAT_R8G8_UINT:
            case FORMAT_R8G8_SNORM:
            case FORMAT_R8G8_SINT:
            case FORMAT_R16_TYPELESS:
            case FORMAT_R16_FLOAT:
            case FORMAT_D16_UNORM:
            case FORMAT_R16_UNORM:
            case FORMAT_R16_UINT:
            case FORMAT_R16_SNORM:
            case FORMAT_R16_SINT:
                return 2;

            case FORMAT_R8_UNORM:
            case FORMAT_R8_UINT:
            case FORMAT_R8_SNORM:
            case FORMAT_R8_SINT:
                return 1;


            default:
                assert(0); // didn't catch format!
                break;
        }

        return 16;
    }

    bool IsFormatUnorm(FORMAT value) 
    {
        switch (value)
        {
            case FORMAT_R16G16B16A16_UNORM:
            case FORMAT_R10G10B10A2_UNORM:
            case FORMAT_R8G8B8A8_UNORM:
            case FORMAT_R8G8B8A8_UNORM_SRGB:
            case FORMAT_B8G8R8A8_UNORM:
            case FORMAT_B8G8R8A8_UNORM_SRGB:
            case FORMAT_R16G16_UNORM:
            case FORMAT_D24_UNORM_S8_UINT:
            case FORMAT_R8G8_UNORM:
            case FORMAT_D16_UNORM:
            case FORMAT_R16_UNORM:
            case FORMAT_R8_UNORM:
                return true;
        }

        return false;
    }

    bool IsFormatBlockCompressed(FORMAT value) 
    {
        switch (value)
        {
            case FORMAT_BC1_UNORM:
            case FORMAT_BC1_UNORM_SRGB:
            case FORMAT_BC2_UNORM:
            case FORMAT_BC2_UNORM_SRGB:
            case FORMAT_BC3_UNORM:
            case FORMAT_BC3_UNORM_SRGB:
            case FORMAT_BC4_UNORM:
            case FORMAT_BC4_SNORM:
            case FORMAT_BC5_UNORM:
            case FORMAT_BC5_SNORM:
            case FORMAT_BC6H_UF16:
            case FORMAT_BC6H_SF16:
            case FORMAT_BC7_UNORM:
            case FORMAT_BC7_UNORM_SRGB:
                return true;
        }

        return false;
    }

    bool IsFormatStencilSupport(FORMAT value) 
    {
        switch (value)
        {
            case FORMAT_R32G8X24_TYPELESS:
            case FORMAT_D32_FLOAT_S8X24_UINT:
            case FORMAT_R24G8_TYPELESS:
            case FORMAT_D24_UNORM_S8_UINT:
                return true;
        }

        return false;
    }

    /* RHIDevice */
    RHIDevice* GDevice = nullptr;

    bool RHIInitialize(ValidationMode validationMode, BackendType backendType)
    {
        if (GDevice != nullptr)
            return true;

        if (backendType == BackendType::Count)
        {
#if defined(ALIMER_RHI_D3D12)
            if (RHIDeviceD3D12::IsAvailable())
                backendType = BackendType::Direct3D12;
#endif

            if (backendType == BackendType::Count)
            {
#if defined(ALIMER_RHI_VULKAN)
                if (RHIDeviceVulkan::IsAvailable())
                    backendType = BackendType::Vulkan;
#endif
            }
        }

        switch (backendType)
        {
#if defined(ALIMER_RHI_D3D12)
            case BackendType::Direct3D12:
                GDevice = new RHIDeviceD3D12(validationMode);
                break;
#endif

#if defined(ALIMER_RHI_VULKAN)
            case BackendType::Vulkan:
                GDevice = new RHIDeviceVulkan(validationMode);
                break;
#endif

            default:
                LOGE("RHI: Cannot detect supported backend");
                return false;
        }

        return GDevice->Initialize();
    }

    void RHIShutdown()
    {
        if (GDevice != nullptr)
        {
            GDevice->Shutdown();
            delete GDevice;
            GDevice = nullptr;
        }
    }

    void RHIWaitForGPU()
    {
        GDevice->WaitForGPU();
    }
}
