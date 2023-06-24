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

namespace LinaGX
{
    struct VKBSwapchain
    {
        bool                    isValid = false;
        VkFormat                format  = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
        VkSwapchainKHR          ptr     = nullptr;
        VkSurfaceKHR            surface = nullptr;
        LINAGX_VEC<VkImage>     imgs;
        LINAGX_VEC<VkImageView> views;
    };

    struct VKBShader
    {
        bool                                    isValid     = false;
        VkPipeline                              ptrPipeline = nullptr;
        VkPipelineLayout                        ptrLayout   = nullptr;
        LINAGX_MAP<ShaderStage, VkShaderModule> modules;

        LINAGX_VEC<VkDescriptorSetLayout> layouts;
    };

    class VKBackend : public Backend
    {
    public:
        VKBackend(){};
        virtual ~VKBackend(){};

        virtual bool   Initialize(const InitInfo& initInfo) override;
        virtual void   Shutdown();
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual uint16 GenerateShader(const LINAGX_MAP<ShaderStage, CompiledShaderBlob>& stages, const ShaderDesc& shaderDesc) override;
        virtual void   DestroyShader(uint16 handle) override;

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

        LINAGX_VEC<VkQueueFamilyProperties> m_queueFamilies;
        IDList<uint8, VKBSwapchain>         m_swapchains = {10};
        IDList<uint16, VKBShader>           m_shaders    = {10};
    };
} // namespace LinaGX

#endif
