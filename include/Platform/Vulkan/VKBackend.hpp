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
        Texture2DUsage   usage      = Texture2DUsage::ColorTexture;
        VkImage          img        = nullptr;
        VkImageView      imgView    = nullptr;
        VmaAllocation_T* allocation = nullptr;
        bool             isValid    = false;
    };

    struct VKBSwapchain
    {
        bool                    isValid = false;
        VkFormat                format  = VkFormat::VK_FORMAT_B8G8R8A8_SNORM;
        VkSwapchainKHR          ptr     = nullptr;
        VkSurfaceKHR            surface = nullptr;
        LINAGX_VEC<VkImage>     imgs;
        LINAGX_VEC<VkImageView> views;
        LINAGX_VEC<uint32>      depthTextures;
        LINAGX_VEC<uint16>      submitSemaphores;
        LINAGX_VEC<uint16>      presentSemaphores;
        uint32                  _imageIndex              = 0;
        bool                    _presentSemaphoresActive = false;
    };

    struct VKBPerFrameData
    {
        uint16 graphicsFence = 0;
        uint16 transferFence = 0;
    };

    struct VKBCommandStream
    {
        bool            isValid = false;
        VkCommandPool   pool    = nullptr;
        VkCommandBuffer buffer  = nullptr;
        CommandType     type    = CommandType::Graphics;
    };

    class VKBackend : public Backend
    {
    private:
        typedef void (VKBackend::*CommandFunction)(void*, const VKBCommandStream& stream);

    public:
        VKBackend(Renderer* renderer)
            : Backend(renderer){};
        virtual ~VKBackend(){};

        virtual bool   Initialize(const InitInfo& initInfo) override;
        virtual void   Shutdown();
        virtual void   Join();
        virtual void   StartFrame(uint32 frameIndex) override;
        virtual void   EndFrame() override;
        virtual void   Present(const PresentDesc& present) override;
        virtual void   FlushCommandStreams() override;
        virtual uint32 CreateCommandStream(CommandType cmdType) override;
        virtual void   DestroyCommandStream(uint32 handle) override;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual bool   CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob);
        virtual uint16 CreateShader(const LINAGX_MAP<ShaderStage, DataBlob>& stages, const ShaderDesc& shaderDesc) override;
        virtual void   DestroyShader(uint16 handle) override;
        virtual uint32 CreateTexture2D(const Texture2DDesc& desc) override;
        virtual void   DestroyTexture2D(uint32 handle) override;

    private:
        uint16 CreateSyncSemaphore();
        void   DestroySyncSemaphore(uint16 handle);
        uint16 CreateFence();
        void   DestroyFence(uint16 handle);

    private:
        void CMD_BeginRenderPass(void* data, const VKBCommandStream& stream);
        void CMD_EndRenderPass(void* data, const VKBCommandStream& stream);
        void CMD_SetViewport(void* data, const VKBCommandStream& stream);
        void CMD_SetScissors(void* data, const VKBCommandStream& stream);
        void CMD_BindPipeline(void* data, const VKBCommandStream& stream);
        void CMD_DrawInstanced(void* data, const VKBCommandStream& stream);
        void CMD_DrawIndexedInstanced(void* data, const VKBCommandStream& stream);

    private:
        void ImageTransition(ImageTransitionType type, VkCommandBuffer buffer, VkImage img, bool isColor);

    private:
        VkInstance               m_vkInstance     = nullptr;
        VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
        VkAllocationCallbacks*   m_allocator      = nullptr;
        VkDevice                 m_device         = nullptr;
        VkPhysicalDevice         m_gpu            = nullptr;
        VmaAllocator_T*          m_vmaAllocator   = nullptr;
        VkQueue                  m_graphicsQueue  = nullptr;
        VkQueue                  m_transferQueue  = nullptr;
        VkQueue                  m_computeQueue   = nullptr;

        std::pair<uint32, uint32> m_graphicsQueueIndices;
        std::pair<uint32, uint32> m_transferQueueIndices;
        std::pair<uint32, uint32> m_computeQueueIndices;
        uint64                    m_minUniformBufferOffsetAlignment = 0;
        bool                      m_supportsAsyncTransferQueue      = false;
        bool                      m_supportsAsyncComputeQueue       = false;

        uint32 m_currentFrameIndex = 0;
        uint32 m_currentImageIndex = 0;

        LINAGX_VEC<VkQueueFamilyProperties> m_queueFamilies;
        IDList<uint8, VKBSwapchain>         m_swapchains = {5};
        IDList<uint16, VKBShader>           m_shaders    = {10};
        IDList<uint32, VKBTexture2D>        m_texture2Ds = {100};
        IDList<uint32, VKBCommandStream>    m_cmdStreams = {100};
        IDList<uint16, VkSemaphore>         m_semaphores = {20};
        IDList<uint16, VkFence>             m_fences     = {20};

        InitInfo                            m_initInfo     = {};
        LINAGX_VEC<VKBPerFrameData>         m_perFrameData = {};
        LINAGX_MAP<TypeID, CommandFunction> m_cmdFunctions;
    };
} // namespace LinaGX

#endif
