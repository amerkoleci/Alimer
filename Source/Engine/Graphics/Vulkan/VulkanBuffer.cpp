// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphics.h"

namespace Alimer
{
    namespace
    {
        inline VkPipelineStageFlags VulkanPipelineStage(VulkanBufferState state, bool src)
        {
            if (state == VulkanBufferState::Undefined)
            {
                ALIMER_ASSERT(src);
                return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            }

            VkPipelineStageFlags flags = 0u;
            if (Any(state, VulkanBufferState::Vertex | VulkanBufferState::Index)) {
                flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            }

            if (Any(state, VulkanBufferState::Uniform | VulkanBufferState::ShaderRead | VulkanBufferState::ShaderWrite))
            {
                flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            }

            if (Any(state, VulkanBufferState::IndirectArgument)) {
                flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            }

            if (Any(state, VulkanBufferState::CopySource | VulkanBufferState::CopyDest)) {
                flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            }

            if (Any(state, VulkanBufferState::AccelerationStructureRead | VulkanBufferState::AccelerationStructureWrite))
            {
                flags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
            }

            return flags;
        }

        inline VkAccessFlags VulkanAccessFlags(VulkanBufferState state) {

            VkAccessFlags flags = 0; // VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT?

            if (Any(state, VulkanBufferState::Vertex)) {
                flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
            }
            if (Any(state, VulkanBufferState::Index)) {
                flags |= VK_ACCESS_INDEX_READ_BIT;
            }
            if (Any(state, VulkanBufferState::Uniform)) {
                flags |= VK_ACCESS_UNIFORM_READ_BIT;
            }
            if (Any(state, VulkanBufferState::ShaderRead)) {
                flags |= VK_ACCESS_SHADER_READ_BIT;
            }
            if (Any(state, VulkanBufferState::ShaderWrite)) {
                flags |= VK_ACCESS_SHADER_WRITE_BIT;
            }
            if (Any(state, VulkanBufferState::IndirectArgument)) {
                flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
            }
            if (Any(state, VulkanBufferState::CopySource)) {
                flags |= VK_ACCESS_TRANSFER_READ_BIT;
            }
            if (Any(state, VulkanBufferState::CopyDest)) {
                flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            if (Any(state, VulkanBufferState::AccelerationStructureRead)) {
                flags |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
            }
            if (Any(state, VulkanBufferState::AccelerationStructureWrite)) {
                flags |= VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
            }

            return flags;
        }
    }

    VulkanBuffer::VulkanBuffer(VulkanGraphics& device_, const BufferDescription& desc, const void* initialData)
        : Buffer(desc)
        , device(device_)
    {
        VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        createInfo.size = size;
        createInfo.usage = 0u;

        if (Any(usage, BufferUsage::InputAssembly))
        {
            createInfo.usage |=
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

            if (device.BufferDeviceAddressSupported())
            {
                createInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
            }
        }

        if (Any(usage, BufferUsage::Constant))
        {
            // Align the buffer size to multiples of the dynamic uniform buffer minimum size
            uint64_t minAlignment = gGraphics().GetCaps().limits.minUniformBufferOffsetAlignment;
            createInfo.size = AlignTo(createInfo.size, minAlignment);

            createInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        if (Any(usage, BufferUsage::ShaderRead))
        {
            createInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
        }

        if (Any(usage, BufferUsage::ShaderWrite))
        {
            createInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
        }

        if (Any(usage, BufferUsage::Indirect))
        {
            createInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        if (Any(usage, BufferUsage::RayTracingAccelerationStructure))
        {
            createInfo.usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
        }

        //if (Any(usage, BufferUsage::RayTracingShaderTable))
        //{
        //    createInfo.usage |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        //}

        VmaAllocationCreateInfo memoryInfo{};
        memoryInfo.flags = 0;
        memoryInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;

        switch (memoryUsage)
        {
            case MemoryUsage::CpuOnly:
                createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                memoryInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
                memoryInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                memoryInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                memoryInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                //state = VulkanBufferState::CopySource;
                break;

            case MemoryUsage::CpuToGpu:
                createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                memoryInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
                memoryInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                memoryInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                memoryInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                break;

            case MemoryUsage::GpuToCpu:
                memoryInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
                memoryInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                memoryInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                break;

            default:
                createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                memoryInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
                memoryInfo.requiredFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                break;
        }

        uint32_t sharingIndices[3];
        if (device.GetGraphicsQueueFamily() != device.GetComputeQueueFamily()
            || device.GetGraphicsQueueFamily() != device.GetCopyQueueFamily())
        {
            // For buffers, always just use CONCURRENT access modes,
            // so we don't have to deal with acquire/release barriers in async compute.
            createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;

            sharingIndices[createInfo.queueFamilyIndexCount++] = device.GetGraphicsQueueFamily();

            if (device.GetGraphicsQueueFamily() != device.GetComputeQueueFamily())
            {
                sharingIndices[createInfo.queueFamilyIndexCount++] = device.GetComputeQueueFamily();
            }

            if (device.GetGraphicsQueueFamily() != device.GetCopyQueueFamily()
                && device.GetComputeQueueFamily() != device.GetCopyQueueFamily())
            {
                sharingIndices[createInfo.queueFamilyIndexCount++] = device.GetCopyQueueFamily();
            }

            createInfo.pQueueFamilyIndices = sharingIndices;
        }

        VmaAllocationInfo allocationInfo{};
        VkResult result = vmaCreateBuffer(device.GetAllocator(),
            &createInfo, &memoryInfo,
            &handle, &allocation, &allocationInfo);

        if (result != VK_SUCCESS)
        {
            VK_LOG_ERROR(result, "Failed to create buffer.");
            return;
        }

        if (desc.label != nullptr)
        {
            device.SetObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)handle, desc.label);
        }

        allocatedSize = allocationInfo.size;

        if (createInfo.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
        {
            VkBufferDeviceAddressInfo info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
            info.buffer = handle;
            address = vkGetBufferDeviceAddress(device.GetHandle(), &info);
        }

        persistent = (memoryInfo.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;

        if (persistent)
        {
            mappedData = static_cast<uint8_t*>(allocationInfo.pMappedData);
        }

        if (initialData != nullptr)
        {
            UploadData(initialData, 0, size);
        }

        OnCreated();
    }

    VulkanBuffer::~VulkanBuffer()
    {
        Destroy();
    }

    void VulkanBuffer::Destroy()
    {
        if (handle != VK_NULL_HANDLE
            && allocation != VK_NULL_HANDLE)
        {
            Unmap();
            device.DeferDestroy(handle, allocation);
        }

        handle = VK_NULL_HANDLE;
        allocation = VK_NULL_HANDLE;
        OnDestroyed();
    }

    uint8_t* VulkanBuffer::Map()
    {
        if (!mapped && !mappedData)
        {
            VK_CHECK(vmaMapMemory(device.GetAllocator(), allocation, reinterpret_cast<void**>(&mappedData)));
            mapped = true;
        }

        return mappedData;
    }

    void VulkanBuffer::Unmap()
    {
        if (mapped)
        {
            vmaUnmapMemory(device.GetAllocator(), allocation);
            mappedData = nullptr;
            mapped = false;
        }
    }

    void VulkanBuffer::UploadData(const void* data, uint64_t offset, uint64_t size)
    {
        if (memoryUsage == MemoryUsage::GpuOnly)
        {
            VulkanUploadContext context = device.UploadBegin(size);
            memcpy(context.data, data, size);

            VkBufferMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.buffer = handle;
            barrier.size = VK_WHOLE_SIZE;

            vkCmdPipelineBarrier(
                context.commandBuffer,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0, nullptr,
                1, &barrier,
                0, nullptr
            );

            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = offset;
            copyRegion.size = size;

            vkCmdCopyBuffer(
                context.commandBuffer,
                ToVulkan(context.uploadBuffer.Get())->handle,
                handle,
                1,
                &copyRegion
            );

            VkAccessFlags tmp = barrier.srcAccessMask;
            barrier.srcAccessMask = barrier.dstAccessMask;
            barrier.dstAccessMask = 0;

            if (Any(usage, BufferUsage::InputAssembly))
            {
                barrier.dstAccessMask |= VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
            }
                
            if (Any(usage, BufferUsage::Constant))
            {
                barrier.dstAccessMask |= VK_ACCESS_UNIFORM_READ_BIT;
            }
            if (Any(usage, BufferUsage::ShaderRead))
            {
                barrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
            }
            if (Any(usage, BufferUsage::ShaderRead))
            {
                barrier.dstAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
            }

            //if (Any(usage, BufferUsage::Predication))
            //{
            //    barrier.dstAccessMask |= VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT;
            //}

            if (Any(usage, BufferUsage::Indirect))
            {
                barrier.dstAccessMask |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
            }

            if (Any(usage, BufferUsage::RayTracingAccelerationStructure))
            {
                barrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
            }

            vkCmdPipelineBarrier(
                context.commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                0,
                0, nullptr,
                1, &barrier,
                0, nullptr
            );

            device.UploadEnd(context);

#if TODO
            VulkanBufferState newState = VulkanBufferState::Undefined;
            if (any(usage & BufferUsage::Vertex)) {
                newState |= VulkanBufferState::Vertex;
            }
            if (any(usage & BufferUsage::Index)) {
                newState |= VulkanBufferState::Index;
            }
            if (any(usage & BufferUsage::Uniform)) {
                newState |= VulkanBufferState::Uniform;
            }
            if (any(usage & (BufferUsage::ShaderResource))) {
                newState |= VulkanBufferState::ShaderRead;
            }
            if (any(usage & (BufferUsage::UnorderedAccess))) {
                newState |= VulkanBufferState::ShaderWrite;
            }
            if (any(usage & BufferUsage::Indirect)) {
                newState |= VulkanBufferState::IndirectArgument;
            }

            Barrier(commandBuffer->GetHandle(), newState, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
#endif // TODO

        }
        else
        {
            memcpy(mappedData + offset, data, size);
            vmaFlushAllocation(device.GetAllocator(), allocation, offset, size);
    }
}

#if TODO
    void VulkanBuffer::Barrier(VkCommandBuffer commandBuffer,
        VulkanBufferState newState,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask) const
    {
        if (state == newState)
            return;

        if (!srcStageMask)
        {
            srcStageMask = VulkanPipelineStage(state, true);
        }

        if (!dstStageMask)
        {
            dstStageMask = VulkanPipelineStage(newState, false);
        }

        if (srcStageMask != dstStageMask)
        {
            VkBufferMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.srcAccessMask = VulkanAccessFlags(state);
            barrier.dstAccessMask = VulkanAccessFlags(newState);
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.buffer = handle;
            barrier.offset = 0;
            barrier.size = VK_WHOLE_SIZE;

            vkCmdPipelineBarrier(commandBuffer,
                srcStageMask, dstStageMask,
                0,
                0, nullptr,
                1, &barrier,
                0, nullptr);

            state = newState;
        }
    }
#endif // TODO

}
