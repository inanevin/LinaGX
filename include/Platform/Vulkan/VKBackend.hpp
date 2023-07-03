/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef LINAGX_VKBackend_HPP
#define LINAGX_VKBackend_HPP

#include "Core/Backend.hpp"
#ifdef LINAGX_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.h"

struct VkDebugUtilsMessengerEXT_T;
struct VmaAllocator_T;
struct VmaAllocation_T;

namespace LinaGX
{
#define SEMAPHORE_RING_BUF_SIZE 30

    struct VKBShader
    {
        bool                                    isValid     = false;
        VkPipeline                              ptrPipeline = nullptr;
        VkPipelineLayout                        ptrLayout   = nullptr;
        LINAGX_MAP<ShaderStage, VkShaderModule> modules;
        LINAGX_VEC<VkDescriptorSetLayout>       layouts;
    };

    struct VKBTexture2D
    {
        Texture2DUsage     usage              = Texture2DUsage::ColorTexture;
        DepthStencilAspect depthStencilAspect = DepthStencilAspect::DepthOnly;
        VkImage            img                = nullptr;
        VkImageView        imgView            = nullptr;
        VmaAllocation_T*   allocation         = nullptr;
        bool               isValid            = false;
    };

    struct VKBSampler
    {
        bool      isValid = false;
        VkSampler ptr     = nullptr;
    };

    struct VKBSwapchain
    {
        bool                    isValid     = false;
        uint32                  width       = 0;
        uint32                  height      = 0;
        VkFormat                format      = VkFormat::VK_FORMAT_B8G8R8A8_SNORM;
        VkSwapchainKHR          ptr         = nullptr;
        VkSurfaceKHR            surface     = nullptr;
        VkPresentModeKHR        presentMode = VK_PRESENT_MODE_FIFO_KHR;
        LINAGX_VEC<VkImage>     imgs;
        LINAGX_VEC<VkImageView> views;
        LINAGX_VEC<uint32>      depthTextures;
        uint32                  _imageIndex = 0;
    };

    struct VKBPerFrameData
    {
        LINAGX_VEC<VkFence>     submitFences;
        LINAGX_VEC<VkSemaphore> submitSemaphores;
        LINAGX_VEC<VkSemaphore> imageAcquiredSemaphores;
        uint32                  submissionCount = 0;
    };

    struct VKBCommandStream
    {
        bool                        isValid = false;
        QueueType                   type    = QueueType::Graphics;
        LINAGX_VEC<VkCommandBuffer> buffers;
        LINAGX_VEC<VkCommandPool>   pools;
        uint32                      boundShader = 0;
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

    struct VKBQueueData
    {
        VkQueue queue = nullptr;
    };

    struct VKBUserSemaphore
    {
        bool        isValid = false;
        VkSemaphore ptr     = nullptr;
    };

    struct VKBDescriptorSet
    {
        bool                  isValid = false;
        VkDescriptorSet       ptr     = nullptr;
        VkDescriptorSetLayout layout  = nullptr;
    };

    class VKBackend : public Backend
    {
    private:
        typedef void (VKBackend::*CommandFunction)(uint8*, VKBCommandStream& stream);

    public:
        VKBackend(Renderer* renderer)
            : Backend(renderer){};
        virtual ~VKBackend(){};

        virtual uint16 CreateUserSemaphore() override;
        virtual void   DestroyUserSemaphore(uint16 handle) override;
        virtual void   WaitForUserSemaphore(uint16 handle, uint64 value) override;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual void   RecreateSwapchain(const SwapchainRecreateDesc& desc) override;
        virtual bool   CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob);
        virtual uint16 CreateShader(const ShaderDesc& shaderDesc) override;
        virtual void   DestroyShader(uint16 handle) override;
        virtual uint32 CreateTexture2D(const Texture2DDesc& desc) override;
        virtual void   DestroyTexture2D(uint32 handle) override;
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
        virtual uint32 CreateCommandStream(QueueType cmdType) override;
        virtual void   DestroyCommandStream(uint32 handle) override;
        virtual void   CloseCommandStreams(CommandStream** streams, uint32 streamCount) override;
        virtual void   SubmitCommandStreams(const SubmitDesc& desc) override;

    private:
        uint16 CreateFence();
        void   DestroyFence(uint16 handle);

    public:
        virtual bool Initialize(const InitInfo& initInfo) override;
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
        void CMD_BindVertexBuffers(uint8* data, VKBCommandStream& stream);
        void CMD_BindIndexBuffers(uint8* data, VKBCommandStream& stream);
        void CMD_CopyResource(uint8* data, VKBCommandStream& stream);
        void CMD_CopyBufferToTexture2D(uint8* data, VKBCommandStream& stream);
        void CMD_BindDescriptorSets(uint8* data, VKBCommandStream& stream);

    private:
        void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    private:
        VkInstance               m_vkInstance     = nullptr;
        VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
        VkAllocationCallbacks*   m_allocator      = nullptr;
        VkDevice                 m_device         = nullptr;
        VkPhysicalDevice         m_gpu            = nullptr;
        VmaAllocator_T*          m_vmaAllocator   = nullptr;

        std::pair<uint32, uint32> m_graphicsQueueIndices;
        std::pair<uint32, uint32> m_transferQueueIndices;
        std::pair<uint32, uint32> m_computeQueueIndices;
        uint64                    m_minUniformBufferOffsetAlignment = 0;
        bool                      m_supportsAsyncTransferQueue      = false;
        bool                      m_supportsAsyncComputeQueue       = false;

        uint32 m_currentFrameIndex = 0;
        uint32 m_currentImageIndex = 0;

        IDList<uint8, VKBSwapchain>      m_swapchains     = {5};
        IDList<uint16, VKBShader>        m_shaders        = {10};
        IDList<uint32, VKBTexture2D>     m_texture2Ds     = {100};
        IDList<uint32, VKBCommandStream> m_cmdStreams     = {100};
        IDList<uint16, VKBUserSemaphore> m_userSemaphores = {20};
        IDList<uint16, VkFence>          m_fences         = {20};
        IDList<uint32, VKBResource>      m_resources      = {100};
        IDList<uint32, VKBSampler>       m_samplers       = {100};
        IDList<uint16, VKBDescriptorSet> m_descriptorSets = {20};

        InitInfo                            m_initInfo     = {};
        LINAGX_VEC<VKBPerFrameData>         m_perFrameData = {};
        LINAGX_MAP<TypeID, CommandFunction> m_cmdFunctions = {};
        LINAGX_MAP<QueueType, VKBQueueData> m_queueData    = {};

        VkDescriptorPool m_descriptorPool          = nullptr;
        uint32           m_imageAcqSemaphoresCount = 0;
    };
} // namespace LinaGX

#endif
