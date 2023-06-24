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

    VkFormat GetVKFormat(Format f)
    {
        switch (f)
        {
        case Format::UNDEFINED:
            return VK_FORMAT_UNDEFINED;
        case Format::B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case Format::B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case Format::R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;
        case Format::D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case Format::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case Format::R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;
        case Format::R16_SINT:
            return VK_FORMAT_R16_SINT;
        case Format::R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case Format::R32_SINT:
            return VK_FORMAT_R32_SINT;
        case Format::R8_UNORM:
            return VK_FORMAT_R8_UNORM;
        case Format::R8_UINT:
            return VK_FORMAT_R8_UINT;
        case Format::R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;
        default:
            return VK_FORMAT_B8G8R8A8_SRGB;
        }
    }

    VkPolygonMode GetVKPolygonMode(PolygonMode m)
    {
        switch (m)
        {
        case PolygonMode::Fill:
            return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line:
            return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point:
            return VK_POLYGON_MODE_POINT;
        default:
            return VK_POLYGON_MODE_FILL;
        }
    }

    uint32 GetVKCullMode(CullMode m)
    {
        switch (m)
        {
        case CullMode::None:
            return VK_CULL_MODE_NONE;
        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
        case CullMode::FrontAndBack:
            return VK_CULL_MODE_FRONT_AND_BACK;
        default:
            return VK_CULL_MODE_NONE;
        }
    }

    VkFrontFace GetVKFrontFace(FrontFace face)
    {
        switch (face)
        {
        case FrontFace::CW:
            return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CCW:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
    }

    VkCompareOp GetVKCompareOp(CompareOp op)
    {
        switch (op)
        {
        case CompareOp::Never:
            return VK_COMPARE_OP_NEVER;
        case CompareOp::Less:
            return VK_COMPARE_OP_LESS;
        case CompareOp::Equal:
            return VK_COMPARE_OP_EQUAL;
        case CompareOp::LEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::Greater:
            return VK_COMPARE_OP_GREATER;
        case CompareOp::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::Always:
            return VK_COMPARE_OP_ALWAYS;
        default:
            return VK_COMPARE_OP_ALWAYS;
        }
    }

    VkPrimitiveTopology GetVKTopology(Topology t)
    {
        switch (t)
        {
        case Topology::PointList:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case Topology::LineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case Topology::LineStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case Topology::TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case Topology::TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case Topology::TriangleFan:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case Topology::TriangleListAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
        case Topology::TriangleStripAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
        default:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    VkLogicOp GetVKLogicOp(LogicOp op)
    {
        switch (op)
        {
        case LogicOp::Clear:
            return VK_LOGIC_OP_CLEAR;
        case LogicOp::And:
            return VK_LOGIC_OP_AND;
        case LogicOp::AndReverse:
            return VK_LOGIC_OP_AND_REVERSE;
        case LogicOp::Copy:
            return VK_LOGIC_OP_COPY;
        case LogicOp::AndInverted:
            return VK_LOGIC_OP_AND_INVERTED;
        case LogicOp::NoOp:
            return VK_LOGIC_OP_NO_OP;
        case LogicOp::XOR:
            return VK_LOGIC_OP_XOR;
        case LogicOp::OR:
            return VK_LOGIC_OP_OR;
        case LogicOp::NOR:
            return VK_LOGIC_OP_NOR;
        case LogicOp::Equivalent:
            return VK_LOGIC_OP_EQUIVALENT;
        default:
            return VK_LOGIC_OP_CLEAR;
        }
    }

    VkBlendFactor GetVKBlendFactor(BlendFactor factor)
    {
        VK_BLEND_FACTOR_CONSTANT_ALPHA;
        switch (factor)
        {

        case BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        default:
            return VK_BLEND_FACTOR_ZERO;
        }
    }

    VkBlendOp GetVKBlendOp(BlendOp op)
    {
        switch (op)
        {
        case BlendOp::Add:
            return VK_BLEND_OP_ADD;
        case BlendOp::Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOp::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::Min:
            return VK_BLEND_OP_MIN;
        case BlendOp::Max:
            return VK_BLEND_OP_MAX;
        default:
            return VK_BLEND_OP_ADD;
        }
    }

    uint32 GetVKColorComponentFlags(ColorComponentFlags flags)
    {
        switch (flags)
        {
        case ColorComponentFlags::R:
            return VK_COLOR_COMPONENT_R_BIT;
        case ColorComponentFlags::G:
            return VK_COLOR_COMPONENT_G_BIT;
        case ColorComponentFlags::B:
            return VK_COLOR_COMPONENT_B_BIT;
        case ColorComponentFlags::A:
            return VK_COLOR_COMPONENT_A_BIT;
        case ColorComponentFlags::RG:
            return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT;
        case ColorComponentFlags::RGB:
            return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
        case ColorComponentFlags::RGBA:
            return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        default:
            return VK_COLOR_COMPONENT_R_BIT;
        }
    }

    uint32 GetVKShaderStage(ShaderStage s)
    {
        switch (s)
        {
        case ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::Tesellation:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::Pixel:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_VERTEX_BIT;
        }
    }

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
        VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
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
                                                 .defer_surface_initialization()
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
        VkPhysicalDeviceDescriptorIndexingFeatures descFeatures    = VkPhysicalDeviceDescriptorIndexingFeatures{};
        descFeatures.sType                                         = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descFeatures.pNext                                         = nullptr;
        descFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
        descFeatures.descriptorBindingSampledImageUpdateAfterBind  = VK_TRUE;

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature = VkPhysicalDeviceDynamicRenderingFeaturesKHR{};
        dynamic_rendering_feature.sType                                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamic_rendering_feature.dynamicRendering                            = VK_TRUE;

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

        vkb::Device vkbDevice    = deviceBuilder.build().value();
        m_device                 = vkbDevice.device;
        m_gpu                    = physicalDevice.physical_device;
        g_vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(m_device, "vkCmdBeginRenderingKHR"));
        g_vkCmdEndRenderingKHR   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(m_device, "vkCmdEndRenderingKHR"));

        if (hasDedicatedComputeQueue)
        {
            auto res           = vkbDevice.get_dedicated_queue_index(vkb::QueueType::compute);
            computeQueueFamily = res.value();
            computeQueueIndex  = 0;
        }

        m_graphicsQueueIndices = std::make_pair(graphicsQueueFamily, graphicsQueueIndex);
        m_transferQueueIndices = std::make_pair(transferQueueFamily, transferQueueIndex);
        m_computeQueueIndices  = std::make_pair(computeQueueFamily, computeQueueIndex);

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

        return true;
    }

    void VKBackend::Shutdown()
    {
        for (auto& swp : m_swapchains)
        {
            LOGA(!swp.isValid, "VKBackend -> Some swapchains were not destroyed!");
        }

        for (auto& pp : m_shaders)
        {
            LOGA(!pp.isValid, "VKBackend -> Some shader pipelines were not destroyed!");
        }

        vmaDestroyAllocator(m_vmaAllocator);
        vkDestroyDevice(m_device, m_allocator);
        vkb::destroy_debug_utils_messenger(m_vkInstance, m_debugMessenger);
        vkDestroyInstance(m_vkInstance, m_allocator);
    }

    uint8 VKBackend::CreateSwapchain(const SwapchainDesc& desc)
    {
        VKBSwapchain swp;

        VkSurfaceKHR surface;

#ifdef LINAGX_PLATFORM_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = VkWin32SurfaceCreateInfoKHR{};
        surfaceCreateInfo.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext                       = nullptr;
        surfaceCreateInfo.hinstance                   = static_cast<HINSTANCE>(desc.osHandle);
        surfaceCreateInfo.hwnd                        = static_cast<HWND>(desc.window);
        vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &surface);
#else
        LOGA(false, "VKBackend -> Vulkan backend is only supported for Windows at the moment!");
#endif

        vkb::SwapchainBuilder swapchainBuilder{m_gpu, m_device, surface};
        swapchainBuilder = swapchainBuilder
                               //.use_default_format_selection()
                               .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                               .set_desired_extent(desc.width, desc.height);

        VkFormat vkformat = GetVKFormat(desc.format);
        swapchainBuilder  = swapchainBuilder.set_desired_format({vkformat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});

        vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
        swp.ptr                     = vkbSwapchain.swapchain;
        swp.format                  = vkbSwapchain.image_format;
        swp.imgs                    = vkbSwapchain.get_images().value();
        swp.views                   = vkbSwapchain.get_image_views().value();
        swp.surface                 = surface;
        swp.isValid                 = true;

        LOGT("VKBackend -> Successfuly created swapchain with size %d x %d", desc.width, desc.height);

        return m_swapchains.AddItem(swp);
    }

    void VKBackend::DestroySwapchain(uint8 handle)
    {
        auto& swp = m_swapchains.GetItemR(handle);
        if (!swp.isValid)
        {
            LOGE("VKBackend -> Swapchain to be destroyed is not valid!");
            return;
        }

        vkDestroySwapchainKHR(m_device, swp.ptr, m_allocator);

        for (auto view : swp.views)
            vkDestroyImageView(m_device, view, m_allocator);

        vkDestroySurfaceKHR(m_vkInstance, swp.surface, m_allocator);

        m_swapchains.RemoveItem(handle);

        LOGT("VKBackend -> Destroyed swapchain.");
    }

    uint16 VKBackend::GenerateShader(const LINAGX_MAP<ShaderStage, CompiledShaderBlob>& stages, const ShaderDesc& shaderDesc)
    {
        VKBShader shader = {};

        // Vertex Input Layout
        LINAGX_VEC<VkVertexInputBindingDescription>   bindingDescs;
        LINAGX_VEC<VkVertexInputAttributeDescription> attDescs;
        if (!shaderDesc.layout.vertexInputs.empty())
        {
            size_t stride = 0;
            for (auto& i : shaderDesc.layout.vertexInputs)
            {
                VkVertexInputAttributeDescription att = VkVertexInputAttributeDescription{};
                att.location                          = i.location;
                att.binding                           = 0;
                att.format                            = GetVKFormat(i.format);
                att.offset                            = static_cast<uint32>(i.offset);
                attDescs.push_back(att);
                stride += i.size;
            }

            VkVertexInputBindingDescription binding = VkVertexInputBindingDescription{};
            binding.binding                         = 0;
            binding.stride                          = static_cast<uint32>(stride);
            binding.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescs.push_back(binding);
        }
        VkPipelineVertexInputStateCreateInfo vertexInput = VkPipelineVertexInputStateCreateInfo{};
        vertexInput.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.pNext                                = nullptr;
        vertexInput.vertexBindingDescriptionCount        = static_cast<uint32>(bindingDescs.size());
        vertexInput.pVertexBindingDescriptions           = bindingDescs.data();
        vertexInput.vertexAttributeDescriptionCount      = static_cast<uint32>(attDescs.size());
        vertexInput.pVertexAttributeDescriptions         = attDescs.data();

        // Modules & stages
        LINAGX_VEC<VkPipelineShaderStageCreateInfo> shaderStages;
        for (auto& [stage, blob] : stages)
        {
            auto& ptr = shader.modules[stage];

            VkShaderModuleCreateInfo shaderModule = VkShaderModuleCreateInfo{};
            shaderModule.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModule.pNext                    = nullptr;
            shaderModule.codeSize                 = blob.size;
            shaderModule.pCode                    = reinterpret_cast<uint32*>(blob.ptr);

            VkResult res = vkCreateShaderModule(m_device, &shaderModule, nullptr, &ptr);

            VkPipelineShaderStageCreateInfo info = VkPipelineShaderStageCreateInfo{};

            info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext  = nullptr;
            info.stage  = static_cast<VkShaderStageFlagBits>(GetVKShaderStage(stage));
            info.module = ptr;
            info.pName  = "main";

            if (res != VK_SUCCESS)
            {
                LOGA(false, "VKBackend -> Could not create shader module!");
                return 0;
            }

            shaderStages.push_back(info);
        }

        // Misc state
        VkPipelineInputAssemblyStateCreateInfo inputAssembly     = VkPipelineInputAssemblyStateCreateInfo{};
        inputAssembly.sType                                      = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.pNext                                      = nullptr;
        inputAssembly.topology                                   = GetVKTopology(shaderDesc.topology);
        inputAssembly.primitiveRestartEnable                     = VK_FALSE;
        VkPipelineViewportStateCreateInfo viewportState          = VkPipelineViewportStateCreateInfo{};
        viewportState.sType                                      = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext                                      = nullptr;
        viewportState.viewportCount                              = 1;
        viewportState.pViewports                                 = nullptr;
        viewportState.scissorCount                               = 1;
        viewportState.pScissors                                  = nullptr;
        VkPipelineRasterizationStateCreateInfo raster            = VkPipelineRasterizationStateCreateInfo{};
        raster.sType                                             = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        raster.pNext                                             = nullptr;
        raster.depthClampEnable                                  = VK_FALSE;
        raster.rasterizerDiscardEnable                           = VK_FALSE;
        raster.polygonMode                                       = GetVKPolygonMode(shaderDesc.polgyonMode);
        raster.cullMode                                          = GetVKCullMode(shaderDesc.cullMode);
        raster.frontFace                                         = GetVKFrontFace(shaderDesc.frontFace);
        raster.depthBiasEnable                                   = VK_FALSE;
        raster.depthBiasConstantFactor                           = 0.0f;
        raster.depthBiasClamp                                    = 0.0f;
        raster.depthBiasSlopeFactor                              = 0.0f;
        raster.lineWidth                                         = 1.0f;
        VkPipelineMultisampleStateCreateInfo msaa                = VkPipelineMultisampleStateCreateInfo{};
        msaa.sType                                               = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        msaa.pNext                                               = nullptr;
        msaa.rasterizationSamples                                = VK_SAMPLE_COUNT_1_BIT;
        msaa.sampleShadingEnable                                 = VK_FALSE;
        msaa.minSampleShading                                    = 1.0f;
        msaa.pSampleMask                                         = nullptr;
        msaa.alphaToCoverageEnable                               = VK_FALSE;
        msaa.alphaToOneEnable                                    = VK_FALSE;
        VkPipelineDepthStencilStateCreateInfo depthStencil       = VkPipelineDepthStencilStateCreateInfo{};
        depthStencil.sType                                       = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.pNext                                       = nullptr;
        depthStencil.depthTestEnable                             = shaderDesc.depthTest ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable                            = shaderDesc.depthWrite ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp                              = shaderDesc.depthTest ? GetVKCompareOp(shaderDesc.depthCompare) : VK_COMPARE_OP_ALWAYS;
        depthStencil.depthBoundsTestEnable                       = VK_FALSE;
        depthStencil.stencilTestEnable                           = VK_FALSE;
        depthStencil.minDepthBounds                              = 0.0f;
        depthStencil.maxDepthBounds                              = 1.0f;
        VkPipelineColorBlendAttachmentState colorBlendAttachment = VkPipelineColorBlendAttachmentState{};
        colorBlendAttachment.blendEnable                         = shaderDesc.blendAttachment.blendEnabled ? VK_TRUE : VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor                 = GetVKBlendFactor(shaderDesc.blendAttachment.srcColorBlendFactor);
        colorBlendAttachment.dstColorBlendFactor                 = GetVKBlendFactor(shaderDesc.blendAttachment.dstColorBlendFactor);
        colorBlendAttachment.colorBlendOp                        = GetVKBlendOp(shaderDesc.blendAttachment.colorBlendOp);
        colorBlendAttachment.srcAlphaBlendFactor                 = GetVKBlendFactor(shaderDesc.blendAttachment.srcAlphaBlendFactor);
        colorBlendAttachment.dstAlphaBlendFactor                 = GetVKBlendFactor(shaderDesc.blendAttachment.dstAlphaBlendFactor);
        colorBlendAttachment.alphaBlendOp                        = GetVKBlendOp(shaderDesc.blendAttachment.alphaBlendOp);
        colorBlendAttachment.colorWriteMask                      = GetVKColorComponentFlags(shaderDesc.blendAttachment.componentFlags);
        VkPipelineColorBlendStateCreateInfo colorBlending        = VkPipelineColorBlendStateCreateInfo{};
        colorBlending.sType                                      = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext                                      = nullptr;
        colorBlending.logicOpEnable                              = shaderDesc.blendLogicOpEnabled ? VK_TRUE : VK_FALSE;
        colorBlending.logicOp                                    = GetVKLogicOp(shaderDesc.blendLogicOp);
        colorBlending.attachmentCount                            = 1;
        colorBlending.pAttachments                               = &colorBlendAttachment;

        // Dynamic state
        LINAGX_VEC<VkDynamicState> dynamicStates;
        dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
        // dynamicStates.push_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
        VkPipelineDynamicStateCreateInfo dynamicStateCreate = VkPipelineDynamicStateCreateInfo{};
        dynamicStateCreate.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreate.pNext                            = nullptr;
        dynamicStateCreate.flags                            = 0;
        dynamicStateCreate.dynamicStateCount                = static_cast<uint32>(dynamicStates.size());
        dynamicStateCreate.pDynamicStates                   = dynamicStates.data();

        // Descriptor sets & layouts
        LINAGX_MAP<uint32, LINAGX_VEC<VkDescriptorSetLayoutBinding>> bindingMap;
        for (const auto& ubo : shaderDesc.layout.ubos)
        {
            VkDescriptorSetLayoutBinding binding = VkDescriptorSetLayoutBinding{};
            binding.binding                      = ubo.binding;
            binding.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.descriptorCount              = 1;

            for (ShaderStage stg : ubo.stages)
                binding.stageFlags |= GetVKShaderStage(stg);

            bindingMap[ubo.set].push_back(binding);
        }

        for (const auto& ssbo : shaderDesc.layout.ssbos)
        {
            VkDescriptorSetLayoutBinding binding = VkDescriptorSetLayoutBinding{};
            binding.binding                      = ssbo.binding;
            binding.descriptorType               = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            binding.descriptorCount              = 1;

            for (ShaderStage stg : ssbo.stages)
                binding.stageFlags |= GetVKShaderStage(stg);

            bindingMap[ssbo.set].push_back(binding);
        }

        for (const auto& t2d : shaderDesc.layout.combinedImageSamplers)
        {
            VkDescriptorSetLayoutBinding binding = VkDescriptorSetLayoutBinding{};
            binding.binding                      = t2d.binding;
            binding.descriptorType               = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            binding.descriptorCount              = 1;

            for (ShaderStage stg : t2d.stages)
                binding.stageFlags |= GetVKShaderStage(stg);

            bindingMap[t2d.set].push_back(binding);
        }

        // TODO: Descriptor count reflection?
        for (const auto& sampler : shaderDesc.layout.samplers)
        {
            VkDescriptorSetLayoutBinding binding = VkDescriptorSetLayoutBinding{};
            binding.binding                      = sampler.binding;
            binding.descriptorType               = VK_DESCRIPTOR_TYPE_SAMPLER;
            binding.descriptorCount              = 1;

            for (ShaderStage stg : sampler.stages)
                binding.stageFlags |= GetVKShaderStage(stg);

            bindingMap[sampler.set].push_back(binding);
        }

        // TODO: descriptor set flag reflection?
        for (const auto& [set, bindings] : bindingMap)
        {
            VkDescriptorSetLayoutCreateInfo setInfo = VkDescriptorSetLayoutCreateInfo{};
            setInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setInfo.flags                           = 0;
            setInfo.bindingCount                    = static_cast<uint32>(bindings.size());
            setInfo.pBindings                       = bindings.data();

            VkDescriptorSetLayout layout = nullptr;
            VkResult              res    = vkCreateDescriptorSetLayout(m_device, &setInfo, m_allocator, &layout);
            LOGA(res == VK_SUCCESS, "VKBackend -> Could not create descriptor set layout!");
            shader.layouts.push_back(layout);
        }

        // Push constants
        LINAGX_VEC<VkPushConstantRange> constants;

        if (shaderDesc.layout.constantBlock.size != 0)
        {
            const auto&         pc    = shaderDesc.layout.constantBlock;
            VkPushConstantRange range = VkPushConstantRange{};
            range.size                = static_cast<uint32>(pc.size);
            range.offset              = 0;

            for (ShaderStage stg : pc.stages)
                range.stageFlags |= GetVKShaderStage(stg);

            constants.push_back(range);
        }

        // pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = VkPipelineLayoutCreateInfo{};
        pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pNext                      = nullptr;
        pipelineLayoutInfo.flags                      = 0;
        pipelineLayoutInfo.setLayoutCount             = static_cast<uint32>(shader.layouts.size());
        pipelineLayoutInfo.pSetLayouts                = shader.layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount     = static_cast<uint32>(constants.size());
        pipelineLayoutInfo.pPushConstantRanges        = constants.data();
        VkResult res                                  = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, m_allocator, &shader.ptrLayout);
        LOGA(res == VK_SUCCESS, "VKBackend -> Could not create shader pipeline layout!");

        // Actual pipeline.
        VkGraphicsPipelineCreateInfo pipelineInfo = VkGraphicsPipelineCreateInfo{};
        pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext                        = nullptr;
        pipelineInfo.stageCount                   = static_cast<uint32>(shaderStages.size());
        pipelineInfo.pStages                      = shaderStages.data();
        pipelineInfo.pVertexInputState            = &vertexInput;
        pipelineInfo.pInputAssemblyState          = &inputAssembly;
        pipelineInfo.pViewportState               = &viewportState;
        pipelineInfo.pRasterizationState          = &raster;
        pipelineInfo.pMultisampleState            = &msaa;
        pipelineInfo.pDepthStencilState           = &depthStencil;
        pipelineInfo.pColorBlendState             = &colorBlending;
        pipelineInfo.pDynamicState                = &dynamicStateCreate;
        pipelineInfo.layout                       = shader.ptrLayout;
        pipelineInfo.renderPass                   = VK_NULL_HANDLE;
        pipelineInfo.subpass                      = 0;
        pipelineInfo.basePipelineHandle           = VK_NULL_HANDLE;
        res                                       = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, m_allocator, &shader.ptrPipeline);
        LOGA(res == VK_SUCCESS, "VKBackend -> Could not create shader pipeline!");

        // Done with the module
        for (auto [stg, mod] : shader.modules)
            vkDestroyShaderModule(m_device, mod, m_allocator);
        shader.isValid = true;

        return m_shaders.AddItem(shader);
    }

    void VKBackend::DestroyShader(uint16 handle)
    {
        auto& shader = m_shaders.GetItemR(handle);
        if (!shader.isValid)
        {
            LOGE("VKBackend -> Shader to be destroyed is not valid!");
            return;
        }

        for (auto layout : shader.layouts)
            vkDestroyDescriptorSetLayout(m_device, layout, m_allocator);

        vkDestroyPipelineLayout(m_device, shader.ptrLayout, m_allocator);
        vkDestroyPipeline(m_device, shader.ptrPipeline, m_allocator);
        m_shaders.RemoveItem(handle);
    }
} // namespace LinaGX
