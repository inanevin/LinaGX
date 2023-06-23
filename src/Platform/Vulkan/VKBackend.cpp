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

#include "Platform/Vulkan/VKBackend.hpp"
#include "Platform/Vulkan/SDK/VkBootstrap.h"

#define VMA_IMPLEMENTATION
#include "Platform/Vulkan/SDK/vk_mem_alloc.h"

namespace LinaGX
{
#define LGX_VK_MAJOR 1
#define LGX_VK_MINOR 2

    PFN_vkCmdBeginRenderingKHR g_vkCmdBeginRenderingKHR = nullptr;
    PFN_vkCmdEndRenderingKHR   g_vkCmdEndRenderingKHR   = nullptr;

    static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOGE("VKBackend -> Validation Layer: %s", pCallbackData->pMessage);
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOGT("VKBackend -> Validation Layer: %s", pCallbackData->pMessage);
            break;
        default:
            LOGT("VKBackend -> Validation Layer: %s", pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }

    bool VKBackend::Initialize(const InitInfo& initInfo)
    {
        // Total extensions
        LINAGX_VEC<const char*> requiredExtensions;
        requiredExtensions.push_back("VK_KHR_surface");
        requiredExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

#ifndef NDEBUG
        // Debug messenger
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        // Instance builder
        vkb::InstanceBuilder builder;
        builder = builder.set_app_name(initInfo.appName).request_validation_layers(true).require_api_version(LGX_VK_MAJOR, LGX_VK_MINOR, 0);

        // Extensions
        for (auto ext : requiredExtensions)
            builder.enable_extension(ext);

        // Messenger
        builder.set_debug_callback(VkDebugCallback);

#ifndef NDEBUG
        // VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        VkDebugUtilsMessageSeverityFlagsEXT severity = 0;
#else
        VkDebugUtilsMessageSeverityFlagsEXT severity = 0;
#endif
        builder.set_debug_messenger_severity(severity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
        builder.set_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

        auto res = builder.build();
        if (!res)
        {
            LOGE("VKBackend ->Vulkan builder failed!");
            return false;
        }

        vkb::Instance inst = res.value();
        m_vkInstance       = inst.instance;
        m_debugMessenger   = inst.debug_messenger;

        vkb::PreferredDeviceType targetDeviceType = vkb::PreferredDeviceType::discrete;

        if (initInfo.gpu == PreferredGPUType::CPU)
            targetDeviceType = vkb::PreferredDeviceType::cpu;
        else if (initInfo.gpu == PreferredGPUType::Integrated)
            targetDeviceType = vkb::PreferredDeviceType::integrated;

        // Physical device
        VkPhysicalDeviceFeatures features{};
        features.multiDrawIndirect         = true;
        features.drawIndirectFirstInstance = true;
        features.samplerAnisotropy         = true;
        features.fillModeNonSolid          = true;

        std::vector<const char*> deviceExtensions;
        deviceExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_MULTIVIEW_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);

        vkb::PhysicalDeviceSelector selector{inst};
        vkb::PhysicalDevice         physicalDevice = selector.set_minimum_version(LGX_VK_MAJOR, LGX_VK_MINOR)
                                                 .add_required_extensions(deviceExtensions)
                                                 .prefer_gpu_device_type(targetDeviceType)
                                                 .allow_any_gpu_device_type(false)
                                                 .set_required_features(features)
                                                 .select(vkb::DeviceSelectionMode::partially_and_fully_suitable)
                                                 .value();
        // create the final Vulkan device
        vkb::DeviceBuilder                           deviceBuilder{physicalDevice};
        VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParamsFeature;

        shaderDrawParamsFeature.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        shaderDrawParamsFeature.pNext                = nullptr;
        shaderDrawParamsFeature.shaderDrawParameters = VK_TRUE;

        // For using UPDATE_AFTER_BIND_BIT on material bindings
        VkPhysicalDeviceDescriptorIndexingFeatures descFeatures;
        descFeatures.sType                                         = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descFeatures.pNext                                         = nullptr;
        descFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
        descFeatures.descriptorBindingSampledImageUpdateAfterBind  = VK_TRUE;

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature;
        dynamic_rendering_feature.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamic_rendering_feature.dynamicRendering = VK_TRUE;

        deviceBuilder.add_pNext(&shaderDrawParamsFeature);
        deviceBuilder.add_pNext(&descFeatures);
        deviceBuilder.add_pNext(&dynamic_rendering_feature);

        bool hasDedicatedTransferQueue = physicalDevice.has_dedicated_transfer_queue();
        bool hasDedicatedComputeQueue  = physicalDevice.has_dedicated_compute_queue();

        auto queue_families = physicalDevice.get_queue_families();

        uint32 transferQueueFamily = 0;
        uint32 transferQueueIndex  = 0;
        uint32 graphicsQueueFamily = 0;
        uint32 graphicsQueueIndex  = 0;
        uint32 computeQueueFamily  = 0;
        uint32 computeQueueIndex   = 0;

        std::vector<vkb::CustomQueueDescription> queue_descriptions;
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families.size()); i++)
        {
            uint32 count = 1;

            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueFamily = i;

                if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                {
                    transferQueueFamily = i;

                    if (queue_families[i].queueCount > count + 1)
                        count++;

                    transferQueueIndex = count - 1;
                }

                if (!hasDedicatedComputeQueue && queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                {
                    computeQueueFamily = i;

                    if (queue_families[i].queueCount > count + 1)
                        count++;
                    computeQueueIndex = count - 1;
                }
            }

            queue_descriptions.push_back(vkb::CustomQueueDescription(i, count, std::vector<float>(count, 1.0f)));
        }

        deviceBuilder.custom_queue_setup(queue_descriptions);

        // deviceBuilder.custom_queue_setup(desc);
        vkb::Device vkbDevice    = deviceBuilder.build().value();
        m_device                 = vkbDevice.device;
        m_gpu                    = physicalDevice.physical_device;
        g_vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(m_device, "vkCmdBeginRenderingKHR"));
        g_vkCmdEndRenderingKHR   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(m_device, "vkCmdEndRenderingKHR"));

        // if (hasDedicatedTransferQueue)
        // {
        //     auto res            = vkbDevice.get_dedicated_queue_index(vkb::QueueType::transfer);
        //     transferQueueFamily = res.value();
        //     transferQueueIndex  = 0;
        // }

        if (hasDedicatedComputeQueue)
        {
            auto res           = vkbDevice.get_dedicated_queue_index(vkb::QueueType::compute);
            computeQueueFamily = res.value();
            computeQueueIndex  = 0;
        }

        m_graphicsQueueIndices = std::make_pair(graphicsQueueFamily, graphicsQueueIndex);
        m_transferQueueIndices = std::make_pair(transferQueueFamily, transferQueueIndex);
        m_computeQueueIndices  = std::make_pair(computeQueueFamily, computeQueueIndex);

        // VkSurfaceCapabilitiesKHR surfaceCapabilities;
        // vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_gpu, mainSurface, &surfaceCapabilities);

        VkPhysicalDeviceProperties gpuProps;
        vkGetPhysicalDeviceProperties(m_gpu, &gpuProps);

        VkPhysicalDeviceMemoryProperties gpuMemProps;
        vkGetPhysicalDeviceMemoryProperties(m_gpu, &gpuMemProps);

        VkImageFormatProperties p;

        LINAGX_VEC<VkFormat> formats;
        for (int i = 1; i < 130; i++)
        {
            const VkFormat format    = static_cast<VkFormat>(i);
            VkResult       supported = vkGetPhysicalDeviceImageFormatProperties(m_gpu, format, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_SAMPLE_COUNT_1_BIT, &p);
            if (supported == VK_SUCCESS)
            {
                formats.push_back(format);
            }
        }

        LINAGX_VEC<VkFormat> requiredFormats;
        requiredFormats.push_back(VkFormat::VK_FORMAT_R8_UNORM);
        requiredFormats.push_back(VkFormat::VK_FORMAT_R8G8_UNORM);
        requiredFormats.push_back(VkFormat::VK_FORMAT_R8G8B8A8_SRGB);

        for (auto& f : requiredFormats)
        {
            auto it = std::find_if(formats.begin(), formats.end(), [&](VkFormat format) { return f == format; });

            if (it == formats.end())
                LOGE("VKBackend -> Required format is not supported by the GPU device! %d", static_cast<int>(f));
        }

        m_minUniformBufferOffsetAlignment = gpuProps.limits.minUniformBufferOffsetAlignment;

        LOGT("VKBackend -> Selected GPU: %s - %f mb", gpuProps.deviceName, gpuMemProps.memoryHeaps->size / 1000000.0);

        // Query queue family indices.
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueFamilyCount, nullptr);

        m_queueFamilies = LINAGX_VEC<VkQueueFamilyProperties>(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueFamilyCount, m_queueFamilies.data());

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice         = m_gpu;
        allocatorInfo.device                 = m_device;
        allocatorInfo.instance               = m_vkInstance;
        vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);

        vkGetDeviceQueue(m_device, m_graphicsQueueIndices.first, m_graphicsQueueIndices.second, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, m_transferQueueIndices.first, m_transferQueueIndices.second, &m_transferQueue);
        vkGetDeviceQueue(m_device, m_computeQueueIndices.first, m_computeQueueIndices.second, &m_computeQueue);

        if (m_transferQueueIndices.first != m_graphicsQueueIndices.first)
            m_supportsAsyncTransferQueue = true;
        else if (m_transferQueueIndices.second != m_graphicsQueueIndices.second)
            m_supportsAsyncTransferQueue = true;
        else
            m_supportsAsyncTransferQueue = false;

        if (m_computeQueueIndices.first != m_graphicsQueueIndices.first)
            m_supportsAsyncComputeQueue = true;
        else if (m_computeQueueIndices.second != m_graphicsQueueIndices.second)
            m_supportsAsyncComputeQueue = true;
        else
            m_supportsAsyncComputeQueue = false;

        // m_currentPresentMode = VsyncToPresentMode(initInfo.windowProperties.vsync);

        // VkSurfaceKHR mainSurface;

        // #ifdef LINAGX_PLATFORM_WINDOWS
        //         VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
        //         surfaceCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        //         surfaceCreateInfo.pNext     = nullptr;
        //         surfaceCreateInfo.hinstance = static_cast<HINSTANCE>(mw.GetRegisteryHandle());
        //         surfaceCreateInfo.hwnd      = static_cast<HWND>(mw.GetHandle());
        //
        //         vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &mainSurface);
        // #elif LINAGX_PLATFORM_APPLE
        //
        // #endif
        //
        //         Swapchain* mainSwapchain = new Swapchain{
        //             .size          = Vector2i(static_cast<uint32>(initInfo.windowProperties.width), static_cast<uint32>(initInfo.windowProperties.height)),
        //             .format        = Format::B8G8R8A8_UNORM,
        //             .colorSpace    = ColorSpace::SRGB_NONLINEAR,
        //             .presentMode   = m_currentPresentMode,
        //             ._windowHandle = mw.GetHandle(),
        //         };
        //
        //         mainSwapchain->surface = mainSurface;
        //         mainSwapchain->Create(LINA_MAIN_SWAPCHAIN_ID);
        //         m_swapchains[LINA_MAIN_SWAPCHAIN_ID] = mainSwapchain;
        //         LINA_TRACE("[Swapchain] -> Swapchain created: {0} x {1}", mainSwapchain->size.x, mainSwapchain->size.y);

        return true;
    }
    void VKBackend::Shutdown()
    {
    }
} // namespace LinaGX
