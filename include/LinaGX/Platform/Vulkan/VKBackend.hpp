/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2023-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#pragma once

#include "LinaGX/Core/Backend.hpp"
#include <atomic>

#ifdef LINAGX_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.h"

struct VkDebugUtilsMessengerEXT_T;
struct VmaAllocator_T;
struct VmaAllocation_T;

namespace LinaGX
{
    enum class PreferredGPUType;

#define SEMAPHORE_RING_BUF_SIZE 30

    struct VKBShader
    {
        bool                                                 usingCustomLayout = false;
        bool                                                 isValid           = false;
        bool                                                 isCompute         = false;
        VkPipeline                                           ptrPipeline       = nullptr;
        VkPipelineLayout                                     ptrLayout         = nullptr;
        LINAGX_VEC<LINAGX_PAIR<ShaderStage, VkShaderModule>> modules;
        LINAGX_VEC<VkDescriptorSetLayout>                    layouts;
    };

    struct VKBTexture2D
    {
        bool                    isValid     = false;
        uint16                  flags       = 0;
        uint32                  arrayLength = 0;
        uint32                  mipLevels   = 0;
        VkImageLayout           imgLayout   = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        LINAGX_VEC<VkImageView> imgViews    = {};
        VkImage                 img         = nullptr;
        VmaAllocation_T*        allocation  = nullptr;
        VkExtent3D              extent      = {};
        VkImageAspectFlags      aspectFlags = 0;
    };

    struct VKBSampler
    {
        bool      isValid = false;
        VkSampler ptr     = nullptr;
    };

    struct VKBSwapchain
    {
        bool                      isValid     = false;
        bool                      isActive    = true;
        uint32                    width       = 0;
        uint32                    height      = 0;
        VkFormat                  format      = VkFormat::VK_FORMAT_B8G8R8A8_SNORM;
        VkSwapchainKHR            ptr         = nullptr;
        VkSurfaceKHR              surface     = nullptr;
        VkPresentModeKHR          presentMode = VK_PRESENT_MODE_FIFO_KHR;
        LINAGX_VEC<VkImageLayout> imgLayouts  = {VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED};
        LINAGX_VEC<VkImage>       imgs;
        LINAGX_VEC<VkImageView>   views;
        uint32                    _imageIndex              = 0;
        bool                      gotImage                 = false;
        uint8                     _submittedQueue          = 0;
        uint32                    _submittedSemaphoreIndex = 0;
        LINAGX_VEC<VkSemaphore>   imageAcquiredSemaphores;
    };

    struct VKBPerFrameData
    {
        uint32 submissionCount = 0;
    };

    struct VKBRenderPassImage
    {
        VkImage ptr         = nullptr;
        bool    isSwapchain = false;
    };

    struct VKBCommandStream
    {
        bool                                    isValid     = false;
        CommandType                             type        = CommandType::Graphics;
        uint32                                  boundShader = 0;
        VkCommandBuffer                         buffer      = nullptr;
        VkCommandPool                           pool        = nullptr;
        LINAGX_VEC<LINAGX_PAIR<uint32, uint64>> intermediateResources;
        LINAGX_VEC<uint8>                       swapchainWrites;
        CommandStream*                          streamImpl = nullptr;
    };

    struct VKBResource
    {
        bool             isValid    = false;
        bool             isMapped   = false;
        uint64           size       = 0;
        ResourceHeap     heapType   = ResourceHeap::StagingHeap;
        VkBuffer         buffer     = nullptr;
        VmaAllocation_T* allocation = nullptr;
    };

    struct VKBUserSemaphore
    {
        bool        isValid = false;
        VkSemaphore ptr     = nullptr;
    };

    struct VKBDescriptorSet
    {
        bool                          isValid  = false;
        VkDescriptorSet*              sets     = nullptr;
        uint32                        setCount = 1;
        VkDescriptorSetLayout         layout   = nullptr;
        LINAGX_VEC<DescriptorBinding> bindings;
    };

    struct VKBQueuePerFrameData
    {
        VkSemaphore             startFrameWaitSemaphore        = nullptr;
        uint64                  storedStartFrameSemaphoreValue = 0;
        LINAGX_VEC<VkSemaphore> submitSemaphoreBuffer;
        uint32                  submitSemaphoreIndex = 0;
    };

    struct VKBQueue
    {
        bool                             isValid             = false;
        VkQueue                          queue               = nullptr;
        CommandType                      type                = CommandType::Graphics;
        uint64                           frameSemaphoreValue = 0;
        LINAGX_VEC<VKBQueuePerFrameData> pfd;
        LINAGX_VEC<bool>                 wasSubmitted = {false};
    };

    struct VKBQueueData
    {
        uint32              createRequestCount = 0;
        uint32              totalQueueCount    = 0;
        LINAGX_VEC<VkQueue> queues;
        uint32              familyIndex     = 0;
        CommandType         actualQueueType = CommandType::Graphics;
    };

    struct VKBPipelineLayout
    {
        bool                              isValid = false;
        VkPipelineLayout                  ptr     = nullptr;
        LINAGX_VEC<VkDescriptorSetLayout> setLayouts;
    };

    class VKBackend : public Backend
    {
    private:
        typedef void (VKBackend::*CommandFunction)(uint8*, VKBCommandStream& stream);

    public:
        VKBackend()
            : Backend(){};
        virtual ~VKBackend(){};

        virtual uint16 CreateUserSemaphore() override;
        virtual void   DestroyUserSemaphore(uint16 handle) override;
        virtual void   WaitForUserSemaphore(uint16 handle, uint64 value) override;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual void   RecreateSwapchain(const SwapchainRecreateDesc& desc) override;
        virtual void   SetSwapchainActive(uint8 swp, bool isActive) override;
        virtual uint16 CreateShader(const ShaderDesc& shaderDesc) override;
        virtual void   DestroyShader(uint16 handle) override;
        virtual uint32 CreateTexture(const TextureDesc& desc) override;
        virtual void   DestroyTexture(uint32 handle) override;
        virtual uint32 CreateSampler(const SamplerDesc& desc) override;
        virtual void   DestroySampler(uint32 handle) override;
        virtual uint32 CreateResource(const ResourceDesc& desc) override;
        virtual void   MapResource(uint32 handle, uint8*& ptr) override;
        virtual void   UnmapResource(uint32 handle) override;
        virtual void   DestroyResource(uint32 handle) override;
        virtual uint16 CreateDescriptorSet(const DescriptorSetDesc& desc) override;
        virtual void   DestroyDescriptorSet(uint16 handle) override;
        virtual void   DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc) override;
        virtual void   DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc) override;
        virtual uint16 CreatePipelineLayout(const PipelineLayoutDesc& desc) override;
        virtual void   DestroyPipelineLayout(uint16 layout) override;
        virtual uint32 CreateCommandStream(const CommandStreamDesc& desc) override;
        virtual void   DestroyCommandStream(uint32 handle) override;
        virtual void   SetCommandStreamImpl(uint32 handle, CommandStream* stream) override;
        virtual void   CloseCommandStreams(CommandStream** streams, uint32 streamCount) override;
        virtual void   SubmitCommandStreams(const SubmitDesc& desc) override;
        virtual uint8  CreateQueue(const QueueDesc& desc) override;
        virtual void   DestroyQueue(uint8 queue) override;
        virtual uint8  GetPrimaryQueue(CommandType type) override;

        static uint32 QueryFeatureSupport(PreferredGPUType gpuType);

    private:
        uint16                CreateFence();
        void                  DestroyFence(uint16 handle);
        VkDescriptorSetLayout CreateDescriptorSetLayout(const DescriptorSetDesc& desc);

    public:
        virtual bool Initialize() override;
        virtual void Shutdown();
        virtual void Join();
        virtual void StartFrame(uint32 frameIndex) override;
        virtual void Present(const PresentDesc& present) override;
        virtual void EndFrame() override;

    private:
        void CMD_BeginRenderPass(uint8* data, VKBCommandStream& stream);
        void CMD_EndRenderPass(uint8* data, VKBCommandStream& stream);
        void CMD_SetViewport(uint8* data, VKBCommandStream& stream);
        void CMD_SetScissors(uint8* data, VKBCommandStream& stream);
        void CMD_BindPipeline(uint8* data, VKBCommandStream& stream);
        void CMD_DrawInstanced(uint8* data, VKBCommandStream& stream);
        void CMD_DrawIndexedInstanced(uint8* data, VKBCommandStream& stream);
        void CMD_DrawIndexedIndirect(uint8* data, VKBCommandStream& stream);
        void CMD_DrawIndirect(uint8* data, VKBCommandStream& stream);
        void CMD_BindVertexBuffers(uint8* data, VKBCommandStream& stream);
        void CMD_BindIndexBuffers(uint8* data, VKBCommandStream& stream);
        void CMD_CopyResource(uint8* data, VKBCommandStream& stream);
        void CMD_CopyBufferToTexture2D(uint8* data, VKBCommandStream& stream);
        void CMD_CopyTexture2DToBuffer(uint8* data, VKBCommandStream& stream);
        void CMD_CopyTexture(uint8* data, VKBCommandStream& stream);
        void CMD_BindDescriptorSets(uint8* data, VKBCommandStream& stream);
        void CMD_BindConstants(uint8* data, VKBCommandStream& stream);
        void CMD_Dispatch(uint8* data, VKBCommandStream& stream);
        void CMD_ExecuteSecondaryStream(uint8* data, VKBCommandStream& stream);
        void CMD_Barrier(uint8* data, VKBCommandStream& stream);
        void CMD_Debug(uint8* data, VKBCommandStream& stream);
        void CMD_DebugBeginLabel(uint8* data, VKBCommandStream& stream);
        void CMD_DebugEndLabel(uint8* data, VKBCommandStream& stream);

    private:
    private:
        VkInstance               m_vkInstance     = nullptr;
        VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
        VkAllocationCallbacks*   m_allocator      = nullptr;
        VkDevice                 m_device         = nullptr;
        VkPhysicalDevice         m_gpu            = nullptr;
        VmaAllocator_T*          m_vmaAllocator   = nullptr;

        uint64 m_minUniformBufferOffsetAlignment = 0;
        uint64 m_minStorageBufferOffsetAlignment = 0;
        bool   m_supportsMultiDrawIndirect       = false;
        bool   m_supportsAnisotropy              = false;
        bool   m_supportsDedicatedTransferQueue  = false;
        bool   m_supportsDedicatedComputeQueue   = false;
        bool   m_supportsSeparateTransferQueue   = false;
        bool   m_supportsSeparateComputeQueue    = false;

        uint32 m_currentFrameIndex = 0;
        uint32 m_currentImageIndex = 0;

        IDList<uint8, VKBSwapchain>       m_swapchains      = {5};
        IDList<uint16, VKBShader>         m_shaders         = {10};
        IDList<uint32, VKBTexture2D>      m_textures        = {100};
        IDList<uint32, VKBCommandStream>  m_cmdStreams      = {100};
        IDList<uint16, VKBUserSemaphore>  m_userSemaphores  = {20};
        IDList<uint16, VkFence>           m_fences          = {20};
        IDList<uint32, VKBResource>       m_resources       = {100};
        IDList<uint32, VKBSampler>        m_samplers        = {100};
        IDList<uint16, VKBDescriptorSet>  m_descriptorSets  = {20};
        IDList<uint8, VKBQueue>           m_queues          = {5};
        IDList<uint16, VKBPipelineLayout> m_pipelineLayouts = {10};

        LINAGX_VEC<VKBPerFrameData>                             m_perFrameData = {};
        LINAGX_VEC<LINAGX_PAIR<LINAGX_TYPEID, CommandFunction>> m_cmdFunctions = {};
        LINAGX_VEC<LINAGX_PAIR<CommandType, uint8>>             m_primaryQueues;
        LINAGX_VEC<LINAGX_PAIR<VkQueue, std::atomic_flag*>>     m_flagsPerQueue;

        VkDescriptorPool           m_descriptorPool = nullptr;
        VkPhysicalDeviceProperties m_gpuProperties;

        LINAGX_VEC<LINAGX_PAIR<CommandType, VKBQueueData>> m_queueData;
    };
} // namespace LinaGX
