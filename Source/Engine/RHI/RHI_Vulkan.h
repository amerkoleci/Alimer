// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.
// Based on Wicked Engine RHI: https://github.com/turanszkij/WickedEngine/blob/master/LICENSE.txt

#pragma once

#include "AlimerConfig.h"

#if defined(ALIMER_RHI_VULKAN)
#include "RHI/RHI.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "PlatformInclude.h"
#include "volk.h"
#include "vk_mem_alloc.h"

#include <deque>

namespace Alimer::RHI
{
    constexpr const char* ToString(VkResult result)
    {
        switch (result)
        {
#define STR(r)   \
	case VK_##r: \
		return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
#undef STR
            default:
                return "UNKNOWN_ERROR";
        }
    }

    /// Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x) \
	do \
	{ \
		VkResult err = x; \
		if (err) \
		{ \
			LOGE("Detected Vulkan error: {}", ToString(err)); \
		} \
	} while (0)

#define VK_LOG_ERROR(result, message) LOGE("Vulkan: {}, error: {}", message, ToString(result));

    class RHIDeviceVulkan final : public RHIDevice
    {
    private:
        bool debugUtils = false;
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        std::vector<VkQueueFamilyProperties> queueFamilies;
        int graphicsFamily = -1;
        int computeFamily = -1;
        int copyFamily = -1;
        std::vector<uint32_t> families;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue computeQueue = VK_NULL_HANDLE;
        VkQueue copyQueue = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties2 properties2 = {};
        VkPhysicalDeviceVulkan11Properties properties_1_1 = {};
        VkPhysicalDeviceVulkan12Properties properties_1_2 = {};
        VkPhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties = {};
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR raytracing_properties = {};
        VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties = {};
        VkPhysicalDeviceMeshShaderPropertiesNV mesh_shader_properties = {};

        VkPhysicalDeviceFeatures2 features2 = {};
        VkPhysicalDeviceVulkan11Features features_1_1 = {};
        VkPhysicalDeviceVulkan12Features features_1_2 = {};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features = {};
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR raytracing_features = {};
        VkPhysicalDeviceRayQueryFeaturesKHR raytracing_query_features = {};
        VkPhysicalDeviceFragmentShadingRateFeaturesKHR fragment_shading_rate_features = {};
        VkPhysicalDeviceMeshShaderFeaturesNV mesh_shader_features = {};

        std::vector<VkDynamicState> pso_dynamicStates;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};

        VkBuffer		nullBuffer = VK_NULL_HANDLE;
        VmaAllocation	nullBufferAllocation = VK_NULL_HANDLE;
        VkBufferView	nullBufferView = VK_NULL_HANDLE;
        VkSampler		nullSampler = VK_NULL_HANDLE;
        VmaAllocation	nullImageAllocation1D = VK_NULL_HANDLE;
        VmaAllocation	nullImageAllocation2D = VK_NULL_HANDLE;
        VmaAllocation	nullImageAllocation3D = VK_NULL_HANDLE;
        VkImage			nullImage1D = VK_NULL_HANDLE;
        VkImage			nullImage2D = VK_NULL_HANDLE;
        VkImage			nullImage3D = VK_NULL_HANDLE;
        VkImageView		nullImageView1D = VK_NULL_HANDLE;
        VkImageView		nullImageView1DArray = VK_NULL_HANDLE;
        VkImageView		nullImageView2D = VK_NULL_HANDLE;
        VkImageView		nullImageView2DArray = VK_NULL_HANDLE;
        VkImageView		nullImageViewCube = VK_NULL_HANDLE;
        VkImageView		nullImageViewCubeArray = VK_NULL_HANDLE;
        VkImageView		nullImageView3D = VK_NULL_HANDLE;

        struct CommandQueue
        {
            VkQueue queue = VK_NULL_HANDLE;
            VkSemaphore semaphore = VK_NULL_HANDLE;
            std::vector<VkSwapchainKHR> submit_swapchains;
            std::vector<uint32_t> submit_swapChainImageIndices;
            std::vector<VkPipelineStageFlags> submit_waitStages;
            std::vector<VkSemaphore> submit_waitSemaphores;
            std::vector<uint64_t> submit_waitValues;
            std::vector<VkSemaphore> submit_signalSemaphores;
            std::vector<uint64_t> submit_signalValues;
            std::vector<VkCommandBuffer> submit_cmds;

            void submit(VkFence fence)
            {
                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount = (uint32_t)submit_cmds.size();
                submitInfo.pCommandBuffers = submit_cmds.data();

                submitInfo.waitSemaphoreCount = (uint32_t)submit_waitSemaphores.size();
                submitInfo.pWaitSemaphores = submit_waitSemaphores.data();
                submitInfo.pWaitDstStageMask = submit_waitStages.data();

                submitInfo.signalSemaphoreCount = (uint32_t)submit_signalSemaphores.size();
                submitInfo.pSignalSemaphores = submit_signalSemaphores.data();

                VkTimelineSemaphoreSubmitInfo timelineInfo = {};
                timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
                timelineInfo.pNext = nullptr;
                timelineInfo.waitSemaphoreValueCount = (uint32_t)submit_waitValues.size();
                timelineInfo.pWaitSemaphoreValues = submit_waitValues.data();
                timelineInfo.signalSemaphoreValueCount = (uint32_t)submit_signalValues.size();
                timelineInfo.pSignalSemaphoreValues = submit_signalValues.data();

                submitInfo.pNext = &timelineInfo;

                VkResult res = vkQueueSubmit(queue, 1, &submitInfo, fence);
                ALIMER_ASSERT(res == VK_SUCCESS);

                if (!submit_swapchains.empty())
                {
                    VkPresentInfoKHR presentInfo = {};
                    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                    presentInfo.waitSemaphoreCount = (uint32_t)submit_signalSemaphores.size();
                    presentInfo.pWaitSemaphores = submit_signalSemaphores.data();
                    presentInfo.swapchainCount = (uint32_t)submit_swapchains.size();
                    presentInfo.pSwapchains = submit_swapchains.data();
                    presentInfo.pImageIndices = submit_swapChainImageIndices.data();
                    res = vkQueuePresentKHR(queue, &presentInfo);
                    ALIMER_ASSERT(res == VK_SUCCESS);
                }

                submit_swapchains.clear();
                submit_swapChainImageIndices.clear();
                submit_waitStages.clear();
                submit_waitSemaphores.clear();
                submit_waitValues.clear();
                submit_signalSemaphores.clear();
                submit_signalValues.clear();
                submit_cmds.clear();
            }

        } queues[(uint32_t)RHIQueueType::Count];

        struct CopyAllocator
        {
            RHIDeviceVulkan* device = nullptr;
            VkSemaphore semaphore = VK_NULL_HANDLE;
            uint64_t fenceValue = 0;
            std::mutex locker;

            struct CopyCMD
            {
                VkCommandPool commandPool = VK_NULL_HANDLE;
                VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
                uint64_t target = 0;
                GPUBuffer uploadbuffer;
                void* data = nullptr;
                VkBuffer upload_resource = VK_NULL_HANDLE;
            };
            std::vector<CopyCMD> freelist; // available
            std::vector<CopyCMD> worklist; // in progress
            std::vector<VkCommandBuffer> submit_cmds; // for next submit
            uint64_t submit_wait = 0; // last submit wait value

            void init(RHIDeviceVulkan* device);
            void destroy();
            CopyCMD allocate(uint32_t staging_size);
            void submit(CopyCMD cmd);
            uint64_t flush();
        };
        mutable CopyAllocator copyAllocator;

        mutable std::mutex transitionLocker;
        mutable std::vector<VkImageMemoryBarrier> transitions;

        struct FrameResources
        {
            VkFence fence[(uint32_t)RHIQueueType::Count] = {};
            VkCommandPool commandPools[COMMANDLIST_COUNT][(uint32_t)RHIQueueType::Count] = {};
            VkCommandBuffer commandBuffers[COMMANDLIST_COUNT][(uint32_t)RHIQueueType::Count] = {};

            VkCommandPool transitionCommandPool = VK_NULL_HANDLE;
            VkCommandBuffer transitionCommandBuffer = VK_NULL_HANDLE;

            struct DescriptorBinder
            {
                RHIDeviceVulkan* device;
                VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
                uint32_t poolSize = 256;

                std::vector<VkWriteDescriptorSet> descriptorWrites;
                std::vector<VkDescriptorBufferInfo> bufferInfos;
                std::vector<VkDescriptorImageInfo> imageInfos;
                std::vector<VkBufferView> texelBufferViews;
                std::vector<VkWriteDescriptorSetAccelerationStructureKHR> accelerationStructureViews;
                bool dirty = false;

                const GPUBuffer* CBV[GPU_RESOURCE_HEAP_CBV_COUNT];
                const GPUResource* SRV[GPU_RESOURCE_HEAP_SRV_COUNT];
                int SRV_index[GPU_RESOURCE_HEAP_SRV_COUNT];
                const GPUResource* UAV[GPU_RESOURCE_HEAP_UAV_COUNT];
                int UAV_index[GPU_RESOURCE_HEAP_UAV_COUNT];
                const Sampler* SAM[GPU_SAMPLER_HEAP_COUNT];

                void init(RHIDeviceVulkan* device);
                void destroy();
                void reset();
                void flush(bool graphics, CommandList cmd);
            };
            DescriptorBinder descriptors[COMMANDLIST_COUNT];


            struct ResourceFrameAllocator
            {
                RHIDeviceVulkan* device = nullptr;
                GPUBuffer				buffer;
                uint8_t* dataBegin = nullptr;
                uint8_t* dataCur = nullptr;
                uint8_t* dataEnd = nullptr;

                void init(RHIDeviceVulkan* device, size_t size);

                uint8_t* allocate(size_t dataSize, size_t alignment);
                void clear();
                uint64_t calculateOffset(uint8_t* address);
            };
            ResourceFrameAllocator resourceBuffer[COMMANDLIST_COUNT];
        };
        FrameResources frames[kMaxFramesInFlight];
        const FrameResources& GetFrameResources() const { return frames[GetFrameIndex()]; }
        FrameResources& GetFrameResources() { return frames[GetFrameIndex()]; }

        struct CommandListMetadata
        {
            RHIQueueType queue = {};
            std::vector<CommandList> waits;
        } cmd_meta[COMMANDLIST_COUNT];

        inline VkCommandBuffer GetCommandList(CommandList cmd)
        {
            return GetFrameResources().commandBuffers[cmd][(uint32_t)cmd_meta[cmd].queue];
        }

        std::vector<VkMemoryBarrier> frame_memoryBarriers[COMMANDLIST_COUNT];
        std::vector<VkImageMemoryBarrier> frame_imageBarriers[COMMANDLIST_COUNT];
        std::vector<VkBufferMemoryBarrier> frame_bufferBarriers[COMMANDLIST_COUNT];

        struct PSOLayout
        {
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            std::vector<VkDescriptorSet> bindlessSets;
            uint32_t bindlessFirstSet = 0;
        };
        mutable std::unordered_map<size_t, PSOLayout> pso_layout_cache;
        mutable std::mutex pso_layout_cache_mutex;

        std::unordered_map<size_t, VkPipeline> pipelines_global;
        std::vector<std::pair<size_t, VkPipeline>> pipelines_worker[COMMANDLIST_COUNT];
        size_t prev_pipeline_hash[COMMANDLIST_COUNT] = {};
        const PipelineState* active_pso[COMMANDLIST_COUNT] = {};
        const Shader* active_cs[COMMANDLIST_COUNT] = {};
        const RaytracingPipelineState* active_rt[COMMANDLIST_COUNT] = {};
        const RenderPass* active_renderpass[COMMANDLIST_COUNT] = {};
        SHADING_RATE prev_shadingrate[COMMANDLIST_COUNT] = {};
        std::vector<const SwapChain*> prev_swapchains[COMMANDLIST_COUNT];

        uint32_t vb_strides[COMMANDLIST_COUNT][8] = {};
        size_t vb_hash[COMMANDLIST_COUNT] = {};

        struct DeferredPushConstantData
        {
            uint8_t data[128];
            uint32_t size;
        };
        DeferredPushConstantData pushconstants[COMMANDLIST_COUNT] = {};

        bool dirty_pso[COMMANDLIST_COUNT] = {};
        void pso_validate(CommandList cmd);

        void barrier_flush(CommandList cmd);
        void predraw(CommandList cmd);
        void predispatch(CommandList cmd);

        std::atomic<CommandList> cmd_count{ 0 };

        std::vector<StaticSampler> common_samplers;

    public:
        [[nodiscard]] static bool IsAvailable();

        RHIDeviceVulkan(ValidationMode validationMode);

        bool Initialize() override;
        void Shutdown() override;

        bool CreateSwapChain(const SwapChainDescriptor* desc, void* window, SwapChain* swapChain) const override;
        bool CreateBuffer(const GPUBufferDesc* pDesc, const void* initialData, GPUBuffer* pBuffer) const override;
        bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, RHITexture* pTexture) const override;
        bool CreateShader(ShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader) const override;
        bool CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) const override;
        bool CreateQueryHeap(const GPUQueryHeapDesc* pDesc, GPUQueryHeap* pQueryHeap) const override;
        bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) const override;
        bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) const override;
        bool CreateRaytracingAccelerationStructure(const RaytracingAccelerationStructureDesc* pDesc, RaytracingAccelerationStructure* bvh) const override;
        bool CreateRaytracingPipelineState(const RaytracingPipelineStateDesc* pDesc, RaytracingPipelineState* rtpso) const override;

        int CreateSubresource(RHITexture* texture, SUBRESOURCE_TYPE type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) const override;
        int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, uint64_t offset, uint64_t size = ~0) const override;

        int GetDescriptorIndex(const GPUResource* resource, SUBRESOURCE_TYPE type, int subresource = -1) const override;
        int GetDescriptorIndex(const Sampler* sampler) const override;

        void WriteShadingRateValue(SHADING_RATE rate, void* dest) const override;
        void WriteTopLevelAccelerationStructureInstance(const RaytracingAccelerationStructureDesc::TopLevel::Instance* instance, void* dest) const override;
        void WriteShaderIdentifier(const RaytracingPipelineState* rtpso, uint32_t group_index, void* dest) const override;

        void Map(const GPUResource* resource, Mapping* mapping) const override;
        void Unmap(const GPUResource* resource) const override;
        void QueryRead(const GPUQueryHeap* heap, uint32_t index, uint32_t count, uint64_t* results) const override;

        void SetCommonSampler(const StaticSampler* sam) override;

        void SetName(GPUResource* pResource, const StringView& name) override;

        CommandList BeginCommandList(RHIQueueType queueType = RHIQueueType::Graphics) override;
        void SubmitCommandLists() override;

        void WaitForGPU() const override;
        void ClearPipelineStateCache() override;

        ShaderFormat GetShaderFormat() const override { return ShaderFormat::SPIRV; }

        RHITexture GetBackBuffer(const SwapChain* swapchain) const override;

        ///////////////Thread-sensitive////////////////////////

        void WaitCommandList(CommandList cmd, CommandList wait_for) override;
        void BeginRenderPass(CommandList commandList, const SwapChain* swapchain, const float clearColor[4]) override;
        void BeginRenderPass(CommandList commandList, const RenderPass* renderpass) override;
        void EndRenderPass(CommandList commandList) override;
        void BindScissorRects(uint32_t numRects, const Rect* rects, CommandList cmd) override;
        void BindViewport(CommandList commandList, const RHIViewport& viewport) override;
        void BindViewports(CommandList commandList, uint32_t viewportCount, const RHIViewport* pViewports) override;
        void BindResource(ShaderStage stage, const GPUResource* resource, uint32_t slot, CommandList cmd, int subresource = -1) override;
        void BindResources(ShaderStage stage, const GPUResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd) override;
        void BindUAV(ShaderStage stage, const GPUResource* resource, uint32_t slot, CommandList cmd, int subresource = -1) override;
        void BindUAVs(ShaderStage stage, const GPUResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd) override;
        void BindSampler(ShaderStage stage, const Sampler* sampler, uint32_t slot, CommandList cmd) override;
        void BindConstantBuffer(ShaderStage stage, const GPUBuffer* buffer, uint32_t slot, CommandList cmd) override;
        void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets, CommandList cmd) override;
        void BindIndexBuffer(const GPUBuffer* indexBuffer, const INDEXBUFFER_FORMAT format, uint32_t offset, CommandList cmd) override;
        void BindStencilRef(uint32_t value, CommandList cmd) override;
        void BindBlendFactor(float r, float g, float b, float a, CommandList cmd) override;
        void BindShadingRate(SHADING_RATE rate, CommandList cmd) override;
        void BindPipelineState(const PipelineState* pso, CommandList cmd) override;
        void BindComputeShader(const Shader* cs, CommandList cmd) override;
        void Draw(uint32_t vertexCount, uint32_t startVertexLocation, CommandList cmd) override;
        void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, CommandList cmd) override;
        void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation, CommandList cmd) override;
        void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation, CommandList cmd) override;
        void DrawInstancedIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) override;
        void DrawIndexedInstancedIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) override;
        void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ, CommandList cmd) override;
        void DispatchIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) override;
        void DispatchMesh(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ, CommandList cmd) override;
        void DispatchMeshIndirect(const GPUBuffer* args, uint32_t args_offset, CommandList cmd) override;
        void CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd) override;
        void UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize = -1) override;
        void QueryBegin(const GPUQueryHeap* heap, uint32_t index, CommandList cmd) override;
        void QueryEnd(const GPUQueryHeap* heap, uint32_t index, CommandList cmd) override;
        void Barrier(const GPUBarrier* barriers, uint32_t numBarriers, CommandList cmd) override;
        void BuildRaytracingAccelerationStructure(const RaytracingAccelerationStructure* dst, CommandList cmd, const RaytracingAccelerationStructure* src = nullptr) override;
        void BindRaytracingPipelineState(const RaytracingPipelineState* rtpso, CommandList cmd) override;
        void DispatchRays(const DispatchRaysDesc* desc, CommandList cmd) override;
        void PushConstants(const void* data, uint32_t size, CommandList cmd) override;

        GPUAllocation AllocateGPU(size_t dataSize, CommandList cmd) override;

        void EventBegin(const StringView& name, CommandList cmd) override;
        void EventEnd(CommandList cmd) override;
        void SetMarker(const StringView& name, CommandList cmd) override;

        struct AllocationHandler
        {
            VmaAllocator allocator = VK_NULL_HANDLE;
            VkDevice device = VK_NULL_HANDLE;
            VkInstance instance;
            uint64_t framecount = 0;
            std::mutex destroylocker;

            struct BindlessDescriptorHeap
            {
                VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
                VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
                VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
                std::vector<int> freelist;
                std::mutex locker;

                void init(VkDevice device, VkDescriptorType type, uint32_t descriptorCount)
                {
                    descriptorCount = std::min(descriptorCount, 100000u);

                    VkDescriptorPoolSize poolSize = {};
                    poolSize.type = type;
                    poolSize.descriptorCount = descriptorCount;

                    VkDescriptorPoolCreateInfo poolInfo = {};
                    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    poolInfo.poolSizeCount = 1;
                    poolInfo.pPoolSizes = &poolSize;
                    poolInfo.maxSets = 1;
                    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

                    VkResult res = vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
                    ALIMER_ASSERT(res == VK_SUCCESS);

                    VkDescriptorSetLayoutBinding binding = {};
                    binding.descriptorType = type;
                    binding.binding = 0;
                    binding.descriptorCount = descriptorCount;
                    binding.stageFlags = VK_SHADER_STAGE_ALL;
                    binding.pImmutableSamplers = nullptr;

                    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
                    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    layoutInfo.bindingCount = 1;
                    layoutInfo.pBindings = &binding;
                    layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

                    VkDescriptorBindingFlags bindingFlags =
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
                        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                        VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
                    //| VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
                    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo = {};
                    bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
                    bindingFlagsInfo.bindingCount = 1;
                    bindingFlagsInfo.pBindingFlags = &bindingFlags;
                    layoutInfo.pNext = &bindingFlagsInfo;

                    res = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
                    ALIMER_ASSERT(res == VK_SUCCESS);

                    VkDescriptorSetAllocateInfo allocInfo = {};
                    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                    allocInfo.descriptorPool = descriptorPool;
                    allocInfo.descriptorSetCount = 1;
                    allocInfo.pSetLayouts = &descriptorSetLayout;
                    res = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
                    ALIMER_ASSERT(res == VK_SUCCESS);

                    for (int i = 0; i < (int)descriptorCount; ++i)
                    {
                        freelist.push_back((int)descriptorCount - i - 1);
                    }
                }
                void destroy(VkDevice device)
                {
                    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
                    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
                }

                int allocate()
                {
                    locker.lock();
                    if (!freelist.empty())
                    {
                        int index = freelist.back();
                        freelist.pop_back();
                        locker.unlock();
                        return index;
                    }
                    locker.unlock();
                    return -1;
                }
                void free(int index)
                {
                    locker.lock();
                    freelist.push_back(index);
                    locker.unlock();
                }
            };
            BindlessDescriptorHeap bindlessUniformBuffers;
            BindlessDescriptorHeap bindlessSampledImages;
            BindlessDescriptorHeap bindlessUniformTexelBuffers;
            BindlessDescriptorHeap bindlessStorageBuffers;
            BindlessDescriptorHeap bindlessStorageImages;
            BindlessDescriptorHeap bindlessStorageTexelBuffers;
            BindlessDescriptorHeap bindlessSamplers;
            BindlessDescriptorHeap bindlessAccelerationStructures;

            std::deque<std::pair<std::pair<VkImage, VmaAllocation>, uint64_t>> destroyer_images;
            std::deque<std::pair<VkImageView, uint64_t>> destroyer_imageviews;
            std::deque<std::pair<std::pair<VkBuffer, VmaAllocation>, uint64_t>> destroyer_buffers;
            std::deque<std::pair<VkBufferView, uint64_t>> destroyer_bufferviews;
            std::deque<std::pair<VkAccelerationStructureKHR, uint64_t>> destroyer_bvhs;
            std::deque<std::pair<VkSampler, uint64_t>> destroyer_samplers;
            std::deque<std::pair<VkDescriptorPool, uint64_t>> destroyer_descriptorPools;
            std::deque<std::pair<VkDescriptorSetLayout, uint64_t>> destroyer_descriptorSetLayouts;
            std::deque<std::pair<VkDescriptorUpdateTemplate, uint64_t>> destroyer_descriptorUpdateTemplates;
            std::deque<std::pair<VkShaderModule, uint64_t>> destroyer_shadermodules;
            std::deque<std::pair<VkPipelineLayout, uint64_t>> destroyer_pipelineLayouts;
            std::deque<std::pair<VkPipeline, uint64_t>> destroyer_pipelines;
            std::deque<std::pair<VkRenderPass, uint64_t>> destroyer_renderpasses;
            std::deque<std::pair<VkFramebuffer, uint64_t>> destroyer_framebuffers;
            std::deque<std::pair<VkQueryPool, uint64_t>> destroyer_querypools;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessUniformBuffers;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessSampledImages;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessUniformTexelBuffers;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessStorageBuffers;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessStorageImages;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessStorageTexelBuffers;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessSamplers;
            std::deque<std::pair<int, uint64_t>> destroyer_bindlessAccelerationStructures;

            ~AllocationHandler()
            {
                bindlessUniformBuffers.destroy(device);
                bindlessSampledImages.destroy(device);
                bindlessUniformTexelBuffers.destroy(device);
                bindlessStorageBuffers.destroy(device);
                bindlessStorageImages.destroy(device);
                bindlessStorageTexelBuffers.destroy(device);
                bindlessSamplers.destroy(device);
                bindlessAccelerationStructures.destroy(device);
                Update(~0, 0); // destroy all remaining
                vmaDestroyAllocator(allocator);
                vkDestroyDevice(device, nullptr);
                vkDestroyInstance(instance, nullptr);
            }

            // Deferred destroy of resources that the GPU is already finished with:
            void Update(uint64_t FRAMECOUNT, uint32_t BUFFERCOUNT)
            {
                destroylocker.lock();
                framecount = FRAMECOUNT;
                while (!destroyer_images.empty())
                {
                    if (destroyer_images.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_images.front();
                        destroyer_images.pop_front();
                        vmaDestroyImage(allocator, item.first.first, item.first.second);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_imageviews.empty())
                {
                    if (destroyer_imageviews.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_imageviews.front();
                        destroyer_imageviews.pop_front();
                        vkDestroyImageView(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_buffers.empty())
                {
                    if (destroyer_buffers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_buffers.front();
                        destroyer_buffers.pop_front();
                        vmaDestroyBuffer(allocator, item.first.first, item.first.second);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bufferviews.empty())
                {
                    if (destroyer_bufferviews.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_bufferviews.front();
                        destroyer_bufferviews.pop_front();
                        vkDestroyBufferView(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bvhs.empty())
                {
                    if (destroyer_bvhs.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_bvhs.front();
                        destroyer_bvhs.pop_front();
                        vkDestroyAccelerationStructureKHR(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_samplers.empty())
                {
                    if (destroyer_samplers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_samplers.front();
                        destroyer_samplers.pop_front();
                        vkDestroySampler(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_descriptorPools.empty())
                {
                    if (destroyer_descriptorPools.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_descriptorPools.front();
                        destroyer_descriptorPools.pop_front();
                        vkDestroyDescriptorPool(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_descriptorSetLayouts.empty())
                {
                    if (destroyer_descriptorSetLayouts.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_descriptorSetLayouts.front();
                        destroyer_descriptorSetLayouts.pop_front();
                        vkDestroyDescriptorSetLayout(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_descriptorUpdateTemplates.empty())
                {
                    if (destroyer_descriptorUpdateTemplates.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_descriptorUpdateTemplates.front();
                        destroyer_descriptorUpdateTemplates.pop_front();
                        vkDestroyDescriptorUpdateTemplate(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_shadermodules.empty())
                {
                    if (destroyer_shadermodules.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_shadermodules.front();
                        destroyer_shadermodules.pop_front();
                        vkDestroyShaderModule(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_pipelineLayouts.empty())
                {
                    if (destroyer_pipelineLayouts.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_pipelineLayouts.front();
                        destroyer_pipelineLayouts.pop_front();
                        vkDestroyPipelineLayout(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_pipelines.empty())
                {
                    if (destroyer_pipelines.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_pipelines.front();
                        destroyer_pipelines.pop_front();
                        vkDestroyPipeline(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_renderpasses.empty())
                {
                    if (destroyer_renderpasses.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_renderpasses.front();
                        destroyer_renderpasses.pop_front();
                        vkDestroyRenderPass(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_framebuffers.empty())
                {
                    if (destroyer_framebuffers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_framebuffers.front();
                        destroyer_framebuffers.pop_front();
                        vkDestroyFramebuffer(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_querypools.empty())
                {
                    if (destroyer_querypools.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        auto item = destroyer_querypools.front();
                        destroyer_querypools.pop_front();
                        vkDestroyQueryPool(device, item.first, nullptr);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessUniformBuffers.empty())
                {
                    if (destroyer_bindlessUniformBuffers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessUniformBuffers.front().first;
                        destroyer_bindlessUniformBuffers.pop_front();
                        bindlessUniformBuffers.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessSampledImages.empty())
                {
                    if (destroyer_bindlessSampledImages.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessSampledImages.front().first;
                        destroyer_bindlessSampledImages.pop_front();
                        bindlessSampledImages.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessUniformTexelBuffers.empty())
                {
                    if (destroyer_bindlessUniformTexelBuffers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessUniformTexelBuffers.front().first;
                        destroyer_bindlessUniformTexelBuffers.pop_front();
                        bindlessUniformTexelBuffers.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessStorageBuffers.empty())
                {
                    if (destroyer_bindlessStorageBuffers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessStorageBuffers.front().first;
                        destroyer_bindlessStorageBuffers.pop_front();
                        bindlessStorageBuffers.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessStorageImages.empty())
                {
                    if (destroyer_bindlessStorageImages.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessStorageImages.front().first;
                        destroyer_bindlessStorageImages.pop_front();
                        bindlessStorageImages.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessStorageTexelBuffers.empty())
                {
                    if (destroyer_bindlessStorageTexelBuffers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessStorageTexelBuffers.front().first;
                        destroyer_bindlessStorageTexelBuffers.pop_front();
                        bindlessStorageTexelBuffers.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessSamplers.empty())
                {
                    if (destroyer_bindlessSamplers.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessSamplers.front().first;
                        destroyer_bindlessSamplers.pop_front();
                        bindlessSamplers.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                while (!destroyer_bindlessAccelerationStructures.empty())
                {
                    if (destroyer_bindlessAccelerationStructures.front().second + BUFFERCOUNT < FRAMECOUNT)
                    {
                        int index = destroyer_bindlessAccelerationStructures.front().first;
                        destroyer_bindlessAccelerationStructures.pop_front();
                        bindlessAccelerationStructures.free(index);
                    }
                    else
                    {
                        break;
                    }
                }
                destroylocker.unlock();
            }
        };
        std::shared_ptr<AllocationHandler> allocationhandler;
    };
}

#endif /* defined(ALIMER_RHI_VULKAN) */
