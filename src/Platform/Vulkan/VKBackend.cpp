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
#include "Core/Commands.hpp"
#include "Core/Renderer.hpp"
#include "Core/CommandStream.hpp"

#define VMA_IMPLEMENTATION
#include "Platform/Vulkan/SDK/vk_mem_alloc.h"

namespace LinaGX
{
#define LGX_VK_MAJOR 1
#define LGX_VK_MINOR 2

    PFN_vkCmdBeginRenderingKHR g_vkCmdBeginRenderingKHR = nullptr;
    PFN_vkCmdEndRenderingKHR   g_vkCmdEndRenderingKHR   = nullptr;
#define pfn_vkCmdBeginRenderingKHR g_vkCmdBeginRenderingKHR
#define pfn_vkCmdEndRenderingKHR   g_vkCmdEndRenderingKHR

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

    VkImageUsageFlags GetVKImageUsage(Texture2DUsage usage)
    {
        switch (usage)
        {
        case Texture2DUsage::ColorTexture:
        case Texture2DUsage::ColorTextureDynamic:
            return VK_IMAGE_USAGE_SAMPLED_BIT;
        case Texture2DUsage::ColorTextureRenderTarget:
            return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        case Texture2DUsage::DepthStencilTexture:
            return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        default:
            return VK_IMAGE_USAGE_SAMPLED_BIT;
        }
    }
    VkImageLayout GetVKImageLayout(Texture2DUsage usage)
    {
        switch (usage)
        {
        case Texture2DUsage::ColorTexture:
        case Texture2DUsage::ColorTextureDynamic:
            return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
        case Texture2DUsage::ColorTextureRenderTarget:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case Texture2DUsage::DepthStencilTexture:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    VkAttachmentLoadOp GetLoadOp(LoadOp p)
    {
        switch (p)
        {
        case LoadOp::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOp::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOp::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        case LoadOp::None:
            return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
        default:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    VkAttachmentStoreOp GetStoreOp(StoreOp op)
    {
        switch (op)
        {
        case StoreOp::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case StoreOp::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        case StoreOp::None:
            return VK_ATTACHMENT_STORE_OP_NONE;
        default:
            return VK_ATTACHMENT_STORE_OP_STORE;
        }
    }

    VkFilter GetFilter(Filter f)
    {
        switch (f)
        {
        case Filter::Linear:
            return VK_FILTER_LINEAR;
        case Filter::Nearest:
            return VK_FILTER_NEAREST;
        default:
            return VK_FILTER_LINEAR;
        }
    }

    VkSamplerAddressMode GetSamplerAddressMode(SamplerAddressMode m)
    {
        switch (m)
        {
        case SamplerAddressMode::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerAddressMode::MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
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
        m_initInfo = initInfo;

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
                                                 .add_required_extensions(deviceExtensions)
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
                formats.push_back(format);
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

        // Per frame data
        {
            for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
            {
                VKBPerFrameData pfd = {};
                pfd.graphicsFence   = CreateFence();
                pfd.transferFence   = CreateFence();
                m_perFrameData.push_back(pfd);
            }
        }

        // Command functions
        {
            m_cmdFunctions[GetTypeID<CMDBeginRenderPass>()]      = &VKBackend::CMD_BeginRenderPass;
            m_cmdFunctions[GetTypeID<CMDEndRenderPass>()]        = &VKBackend::CMD_EndRenderPass;
            m_cmdFunctions[GetTypeID<CMDSetViewport>()]          = &VKBackend::CMD_SetViewport;
            m_cmdFunctions[GetTypeID<CMDSetScissors>()]          = &VKBackend::CMD_SetScissors;
            m_cmdFunctions[GetTypeID<CMDBindPipeline>()]         = &VKBackend::CMD_BindPipeline;
            m_cmdFunctions[GetTypeID<CMDDrawInstanced>()]        = &VKBackend::CMD_DrawInstanced;
            m_cmdFunctions[GetTypeID<CMDDrawIndexedInstanced>()] = &VKBackend::CMD_DrawIndexedInstanced;
        }
        return true;
    }

    void VKBackend::Shutdown()
    {
        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            auto& pfd = m_perFrameData[i];
            DestroyFence(pfd.graphicsFence);
            DestroyFence(pfd.transferFence);
        }

        for (auto& swp : m_swapchains)
        {
            LOGA(!swp.isValid, "VKBackend -> Some swapchains were not destroyed!");
        }

        for (auto& pp : m_shaders)
        {
            LOGA(!pp.isValid, "VKBackend -> Some shader pipelines were not destroyed!");
        }

        for (auto& txt : m_texture2Ds)
        {
            LOGA(!txt.isValid, "DX12Backend -> Some textures were not destroyed!");
        }

        for (auto& str : m_cmdStreams)
        {
            LOGA(!str.isValid, "DX12Backend -> Some command streams were not destroyed!");
        }

        vmaDestroyAllocator(m_vmaAllocator);
        vkDestroyDevice(m_device, m_allocator);
        vkb::destroy_debug_utils_messenger(m_vkInstance, m_debugMessenger);
        vkDestroyInstance(m_vkInstance, m_allocator);
    }

    void VKBackend::Join()
    {
        LINAGX_VEC<VkFence> fences;

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
            fences.push_back(m_fences.GetItemR(m_perFrameData[i].graphicsFence));

        vkDeviceWaitIdle(m_device);
        vkWaitForFences(m_device, static_cast<uint32>(fences.size()), fences.data(), true, 50000000);
    }

    void VKBackend::StartFrame(uint32 frameIndex)
    {
        m_currentFrameIndex         = frameIndex;
        const auto& frame           = m_perFrameData[frameIndex];
        auto        vkGraphicsFence = m_fences.GetItemR(frame.graphicsFence);

        // Wait for graphics present operations.
        uint64   timeout = static_cast<uint64>(10000000000);
        VkResult result  = vkWaitForFences(m_device, 1, &vkGraphicsFence, true, timeout);

        // Acquire images for each swapchain
        for (auto& swp : m_swapchains)
        {
            if (!swp.isValid)
                continue;

            auto semaphore = m_semaphores.GetItemR(swp.submitSemaphores[frameIndex]);
            result         = vkAcquireNextImageKHR(m_device, swp.ptr, timeout, semaphore, nullptr, &swp._imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                LOGA(false, "!!");
                // TODO: CHECK FOR SWAPCHAIN RECREATION
            }
        }

        // reset
        result = vkResetFences(m_device, 1, &vkGraphicsFence);
        LOGA(result == VK_SUCCESS, "VKBackend -> Failed resetting graphics fence");
    }

    void VKBackend::FlushCommandStreams()
    {
        LINAGX_VEC<VkCommandBuffer> cmds;

        for (auto stream : m_renderer->m_commandStreams)
        {
            auto& sr = m_cmdStreams.GetItemR(stream->m_gpuHandle);

            VkResult res = vkResetCommandPool(m_device, sr.pool, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            LOGA(res == VK_SUCCESS, "VKBackend -> Failed resetting command pool!");

            VkCommandBufferBeginInfo beginInfo = VkCommandBufferBeginInfo{};
            beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.pNext                    = nullptr;
            beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            beginInfo.pInheritanceInfo         = nullptr;
            res                                = vkBeginCommandBuffer(sr.buffer, &beginInfo);

            for (uint32 i = 0; i < stream->m_commandCount; i++)
            {
                uint64* cmd = stream->m_commands[i];
                TypeID  tid = stream->m_types[i];
                (this->*m_cmdFunctions[tid])(cmd, sr);
            }

            VkResult result = vkEndCommandBuffer(sr.buffer);
            LOGA(result == VK_SUCCESS, "VKBackend-> Failed ending command buffer!");

            cmds.push_back(sr.buffer);
        }

        auto&                            frame     = m_perFrameData[m_currentFrameIndex];
        VkPipelineStageFlags             waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        LINAGX_VEC<VkSemaphore>          waitSemaphores;
        LINAGX_VEC<VkSemaphore>          signalSemaphores;
        LINAGX_VEC<VkPipelineStageFlags> waitStages;

        // Make sure we wait for all image acquire requests before submitting the cmd buffers.
        for (auto& swp : m_swapchains)
        {
            if (!swp.isValid)
                continue;

            auto submitSemaphore  = m_semaphores.GetItemR(swp.submitSemaphores[m_currentFrameIndex]);
            auto presentSemaphore = m_semaphores.GetItemR(swp.presentSemaphores[m_currentFrameIndex]);
            waitStages.push_back(waitStage);
            waitSemaphores.push_back(submitSemaphore);
            signalSemaphores.push_back(presentSemaphore);
            swp._presentSemaphoresActive = true;
        }

        VkSubmitInfo info = VkSubmitInfo{};

        info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext                = nullptr;
        info.waitSemaphoreCount   = static_cast<uint32>(waitSemaphores.size());
        info.pWaitSemaphores      = waitSemaphores.empty() ? nullptr : &waitSemaphores[0];
        info.pWaitDstStageMask    = waitStages.empty() ? nullptr : &waitStages[0];
        info.commandBufferCount   = static_cast<uint32>(cmds.size());
        info.pCommandBuffers      = cmds.empty() ? nullptr : &cmds[0];
        info.signalSemaphoreCount = static_cast<uint32>(signalSemaphores.size());
        info.pSignalSemaphores    = signalSemaphores.empty() ? nullptr : &signalSemaphores[0];
        VkResult result           = vkQueueSubmit(m_graphicsQueue, 1, &info, m_fences.GetItemR(frame.graphicsFence));
        LOGA(result == VK_SUCCESS, "VKBackend -> Failed submitting to queue!");
    }

    void VKBackend::Present(const PresentDesc& present)
    {
        auto& swp = m_swapchains.GetItemR(present.swapchain);

        if (!swp._presentSemaphoresActive)
            return;

        LINAGX_VEC<VkSemaphore> waitSemaphores;

        swp._presentSemaphoresActive = false;

        waitSemaphores.push_back(m_semaphores.GetItemR(swp.presentSemaphores[m_currentFrameIndex]));

        VkPresentInfoKHR info   = VkPresentInfoKHR{};
        info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext              = nullptr;
        info.waitSemaphoreCount = static_cast<uint32>(waitSemaphores.size());
        info.pWaitSemaphores    = waitSemaphores.empty() ? nullptr : &waitSemaphores[0];
        info.swapchainCount     = 1;
        info.pSwapchains        = &swp.ptr;
        info.pImageIndices      = &swp._imageIndex;

        VkResult result = vkQueuePresentKHR(m_graphicsQueue, &info);
        // LOGA(result == VK_SUCCESS, "VKBackend -> Failed presenting image from queue!");

        // TODO: check for swapchain recreation.
    }

    void VKBackend::EndFrame()
    {
    }

    uint32 VKBackend::CreateCommandStream(CommandType cmdType)
    {
        VKBCommandStream item = {};
        item.isValid          = true;

        uint32 familyIndex = 0;
        if (cmdType == CommandType::Graphics)
            familyIndex = m_graphicsQueueIndices.first;
        else if (cmdType == CommandType::Compute)
            familyIndex = m_computeQueueIndices.first;
        else if (cmdType == CommandType::Transfer)
            familyIndex = m_transferQueueIndices.first;

        VkCommandPoolCreateInfo commandPoolInfo = VkCommandPoolCreateInfo{};
        commandPoolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.pNext                   = nullptr;
        commandPoolInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex        = familyIndex;

        VkResult result = vkCreateCommandPool(m_device, &commandPoolInfo, m_allocator, &item.pool);
        LOGA(result == VK_SUCCESS, "VKBackend -> Could not create command pool!");

        VkCommandBufferAllocateInfo cmdAllocInfo = VkCommandBufferAllocateInfo{};
        cmdAllocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext                       = nullptr;
        cmdAllocInfo.commandPool                 = item.pool;
        cmdAllocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandBufferCount          = 1;

        result = vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &item.buffer);
        LOGA(result == VK_SUCCESS, "VKBackend -> Could not allocate command buffers!");

        return m_cmdStreams.AddItem(item);
    }

    void VKBackend::DestroyCommandStream(uint32 handle)
    {
        auto& stream = m_cmdStreams.GetItemR(handle);
        if (!stream.isValid)
        {
            LOGE("VKBackend -> Command Stream to be destroyed is not valid!");
            return;
        }

        stream.isValid = false;

        vkDestroyCommandPool(m_device, stream.pool, m_allocator);
        m_cmdStreams.RemoveItem(handle);
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
        VkFormat vkformat = GetVKFormat(Config.rtSwapchainFormat);

        vkb::SwapchainBuilder swapchainBuilder{m_gpu, m_device, surface};
        swapchainBuilder = swapchainBuilder
                               //.use_default_format_selection()
                               .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
                               .set_desired_extent(desc.width, desc.height)
                               .set_desired_format({vkformat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                               .set_desired_min_image_count(m_initInfo.backbufferCount);

        vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
        swp.ptr                     = vkbSwapchain.swapchain;
        swp.format                  = vkbSwapchain.image_format;
        swp.imgs                    = vkbSwapchain.get_images().value();
        swp.views                   = vkbSwapchain.get_image_views().value();
        swp.surface                 = surface;
        swp.isValid                 = true;

        for (const auto& img : swp.imgs)
        {
            Texture2DDesc depthDesc = {};
            depthDesc.format        = Config.rtDepthFormat;
            depthDesc.width         = desc.width;
            depthDesc.height        = desc.height;
            depthDesc.mipLevels     = 1;
            depthDesc.usage         = Texture2DUsage::DepthStencilTexture;
            swp.depthTextures.push_back(CreateTexture2D(depthDesc));
        }

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            swp.submitSemaphores.push_back(CreateSyncSemaphore());
            swp.presentSemaphores.push_back(CreateSyncSemaphore());
        }

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

        for (auto t : swp.depthTextures)
            DestroyTexture2D(t);

        vkDestroySwapchainKHR(m_device, swp.ptr, m_allocator);

        for (auto view : swp.views)
            vkDestroyImageView(m_device, view, m_allocator);

        vkDestroySurfaceKHR(m_vkInstance, swp.surface, m_allocator);

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            DestroySyncSemaphore(swp.presentSemaphores[i]);
            DestroySyncSemaphore(swp.submitSemaphores[i]);
        }

        swp.isValid = false;
        m_swapchains.RemoveItem(handle);
        LOGT("VKBackend -> Destroyed swapchain.");
    }

    bool VKBackend::CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob)
    {
        LOGA(false, "!!");
        return false;
    }

    uint16 VKBackend::CreateShader(const LINAGX_MAP<ShaderStage, DataBlob>& stages, const ShaderDesc& shaderDesc)
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
            info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext                           = nullptr;
            info.stage                           = static_cast<VkShaderStageFlagBits>(GetVKShaderStage(stage));
            info.module                          = ptr;
            info.pName                           = "main";

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
        raster.polygonMode                                       = GetVKPolygonMode(shaderDesc.polygonMode);
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

    uint32 VKBackend::CreateTexture2D(const Texture2DDesc& txtDesc)
    {
        VKBTexture2D item = {};
        item.usage        = txtDesc.usage;
        item.isValid      = true;

        VkImageCreateInfo imgCreateInfo = VkImageCreateInfo{};
        imgCreateInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imgCreateInfo.pNext             = nullptr;
        imgCreateInfo.imageType         = VK_IMAGE_TYPE_2D;
        imgCreateInfo.format            = GetVKFormat(txtDesc.format);
        imgCreateInfo.extent            = VkExtent3D{txtDesc.width, txtDesc.height, 1};
        imgCreateInfo.mipLevels         = txtDesc.mipLevels;
        imgCreateInfo.arrayLayers       = 1;
        imgCreateInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
        imgCreateInfo.tiling            = txtDesc.usage == Texture2DUsage::ColorTextureDynamic ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
        imgCreateInfo.usage             = GetVKImageUsage(txtDesc.usage);
        imgCreateInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        imgCreateInfo.initialLayout     = GetVKImageLayout(txtDesc.usage);

        VmaAllocationCreateInfo allocinfo = VmaAllocationCreateInfo{};
        allocinfo.usage                   = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocinfo.requiredFlags           = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        VkResult res = vmaCreateImage(m_vmaAllocator, &imgCreateInfo, &allocinfo, &item.img, &item.allocation, nullptr);
        LOGA(res == VK_SUCCESS, "VKBackend -> Could not create image!");

        VkImageSubresourceRange subResRange = VkImageSubresourceRange{};
        subResRange.aspectMask              = txtDesc.usage == Texture2DUsage::DepthStencilTexture ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        subResRange.baseMipLevel            = 0;
        subResRange.levelCount              = 1;
        subResRange.baseArrayLayer          = 0;
        subResRange.layerCount              = 1;

        VkImageViewCreateInfo viewInfo = VkImageViewCreateInfo{};
        viewInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext                 = nullptr;
        viewInfo.image                 = item.img;
        viewInfo.viewType              = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                = GetVKFormat(txtDesc.format);
        viewInfo.subresourceRange      = subResRange;

        res = vkCreateImageView(m_device, &viewInfo, m_allocator, &item.imgView);
        LOGA(res == VK_SUCCESS, "VKBackend -> Could not create image view!");

        return m_texture2Ds.AddItem(item);
    }

    void VKBackend::DestroyTexture2D(uint32 handle)
    {
        auto& txt = m_texture2Ds.GetItemR(handle);
        if (!txt.isValid)
        {
            LOGE("VKBackend -> Texture to be destroyed is not valid!");
            return;
        }

        vkDestroyImageView(m_device, txt.imgView, m_allocator);
        vmaDestroyImage(m_vmaAllocator, txt.img, txt.allocation);

        txt.isValid = false;
        m_texture2Ds.RemoveItem(handle);
    }

    uint16 VKBackend::CreateSyncSemaphore()
    {
        VkSemaphore           semaphore = nullptr;
        VkSemaphoreCreateInfo info      = VkSemaphoreCreateInfo{};
        info.sType                      = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        info.pNext                      = nullptr,
        info.flags                      = 0;

        VkResult result = vkCreateSemaphore(m_device, &info, m_allocator, &semaphore);
        LOGA(result == VK_SUCCESS, "VKBackend -> Could not create emaphore!");
        return m_semaphores.AddItem(semaphore);
    }

    void VKBackend::DestroySyncSemaphore(uint16 handle)
    {
        auto semaphore = m_semaphores.GetItemR(handle);
        if (semaphore == nullptr)
        {
            LOGE("VKBackend -> Semaphore to be destroyed is not valid!");
            return;
        }

        vkDestroySemaphore(m_device, semaphore, m_allocator);

        semaphore = nullptr;
        m_semaphores.RemoveItem(handle);
    }

    uint16 VKBackend::CreateFence()
    {
        VkFence           fence = nullptr;
        VkFenceCreateInfo info  = VkFenceCreateInfo{};
        info.sType              = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext              = nullptr;
        info.flags              = VK_FENCE_CREATE_SIGNALED_BIT;

        VkResult result = vkCreateFence(m_device, &info, m_allocator, &fence);
        LOGA(result == VK_SUCCESS, "VKBackend -> Could not create fence!");
        return m_fences.AddItem(fence);
    }

    void VKBackend::DestroyFence(uint16 handle)
    {
        auto fence = m_fences.GetItemR(handle);
        if (fence == nullptr)
        {
            LOGE("VKBackend -> Fence to be destroyed is not valid!");
            return;
        }

        vkDestroyFence(m_device, fence, m_allocator);

        fence = nullptr;
        m_fences.RemoveItem(handle);
    }

    void VKBackend::CMD_BeginRenderPass(void* data, const VKBCommandStream& stream)
    {
        CMDBeginRenderPass* begin = static_cast<CMDBeginRenderPass*>(data);

        VkImageView colorImageView = nullptr;
        VkImage     colorImage     = nullptr;

        if (begin->isSwapchain)
        {
            const auto& swp = m_swapchains.GetItemR(begin->swapchain);
            colorImageView  = swp.views[swp._imageIndex];
            colorImage      = swp.imgs[swp._imageIndex];
        }
        else
        {
            const auto& txt = m_texture2Ds.GetItemR(begin->texture);
            colorImageView  = txt.imgView;
            colorImage      = txt.img;
        }

        VkClearValue cvColor     = {};
        cvColor.color.float32[0] = begin->clearColor[0];
        cvColor.color.float32[1] = begin->clearColor[1];
        cvColor.color.float32[2] = begin->clearColor[2];
        cvColor.color.float32[3] = begin->clearColor[3];

        VkRenderingAttachmentInfo colorAttachment = VkRenderingAttachmentInfo{};
        colorAttachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachment.pNext                     = nullptr;
        colorAttachment.imageView                 = colorImageView;
        colorAttachment.imageLayout               = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
        colorAttachment.resolveMode               = VK_RESOLVE_MODE_NONE;
        colorAttachment.resolveImageView          = nullptr;
        colorAttachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue                = cvColor;

        CMDSetViewport interVP = {};
        CMDSetScissors interSC = {};
        interVP.x              = begin->viewport.x;
        interVP.y              = begin->viewport.x;
        interVP.width          = begin->viewport.width;
        interVP.height         = begin->viewport.height;
        interVP.minDepth       = begin->viewport.minDepth;
        interVP.maxDepth       = begin->viewport.maxDepth;
        interSC.x              = begin->scissors.x;
        interSC.y              = begin->scissors.y;
        interSC.width          = begin->scissors.width;
        interSC.height         = begin->scissors.height;

        VkRect2D area      = {};
        area.extent.width  = interVP.width;
        area.extent.height = interVP.height;
        area.offset.x      = interVP.x;
        area.offset.y      = interVP.y;

     
        VkRenderingInfo renderingInfo      = VkRenderingInfo{};
        renderingInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderingInfo.pNext                = nullptr;
        renderingInfo.flags                = 0;
        renderingInfo.renderArea           = area;
        renderingInfo.layerCount           = 1;
        renderingInfo.viewMask             = 0;
        renderingInfo.pDepthAttachment     = nullptr;
        renderingInfo.pStencilAttachment   = nullptr;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments    = &colorAttachment;

        ImageTransition(ImageTransitionType::Present2RT, stream.buffer, colorImage, true);
        pfn_vkCmdBeginRenderingKHR(stream.buffer, &renderingInfo);

        CMD_SetViewport(&interVP, stream);
        CMD_SetScissors(&interSC, stream);
    }

    void VKBackend::CMD_EndRenderPass(void* data, const VKBCommandStream& stream)
    {
        CMDEndRenderPass* end = static_cast<CMDEndRenderPass*>(data);
        pfn_vkCmdEndRenderingKHR(stream.buffer);

        if (end->isSwapchain)
        {
            const auto& swp = m_swapchains.GetItemR(end->swapchain);
            ImageTransition(ImageTransitionType::RT2Present, stream.buffer, swp.imgs[swp._imageIndex], true);
        }
    }

    void VKBackend::CMD_SetViewport(void* data, const VKBCommandStream& stream)
    {
        CMDSetViewport* cmd = static_cast<CMDSetViewport*>(data);
        VkViewport      vp  = VkViewport{};
        vp.x                = static_cast<float>(cmd->x);
        vp.y                = Config.vulkanFlipViewport ? static_cast<float>(cmd->height) : static_cast<float>(cmd->y);
        vp.width            = static_cast<float>(cmd->width);
        vp.height           = Config.vulkanFlipViewport ? -static_cast<float>(cmd->height) : static_cast<float>(cmd->height);
        vp.minDepth         = cmd->minDepth;
        vp.maxDepth         = cmd->maxDepth;

        vkCmdSetViewport(stream.buffer, 0, 1, &vp);
    }

    void VKBackend::CMD_SetScissors(void* data, const VKBCommandStream& stream)
    {
        CMDSetViewport* cmd  = static_cast<CMDSetViewport*>(data);
        VkRect2D        rect = VkRect2D{};
        rect.offset.x        = static_cast<int32>(cmd->x);
        rect.offset.y        = static_cast<int32>(cmd->y);
        rect.extent.width    = cmd->width;
        rect.extent.height   = cmd->height;
        vkCmdSetScissor(stream.buffer, 0, 1, &rect);
    }

    void VKBackend::CMD_BindPipeline(void* data, const VKBCommandStream& stream)
    {
        CMDBindPipeline* cmd    = static_cast<CMDBindPipeline*>(data);
        const auto&      shader = m_shaders.GetItemR(cmd->shader);
        vkCmdBindPipeline(stream.buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.ptrPipeline);
    }

    void VKBackend::CMD_DrawInstanced(void* data, const VKBCommandStream& stream)
    {
        CMDDrawInstanced* cmd = static_cast<CMDDrawInstanced*>(data);
        vkCmdDraw(stream.buffer, cmd->vertexCountPerInstance, cmd->instanceCount, cmd->startVertexLocation, cmd->startInstanceLocation);
    }

    void VKBackend::CMD_DrawIndexedInstanced(void* data, const VKBCommandStream& stream)
    {
        CMDDrawIndexedInstanced* cmd = static_cast<CMDDrawIndexedInstanced*>(data);
        vkCmdDrawIndexed(stream.buffer, cmd->indexCountPerInstance, cmd->instanceCount, cmd->startIndexLocation, cmd->baseVertexLocation, cmd->startInstanceLocation);
    }

    void VKBackend::ImageTransition(ImageTransitionType type, VkCommandBuffer buffer, VkImage img, bool isColor)
    {
        VkImageSubresourceRange subresRange = VkImageSubresourceRange{};
        subresRange.aspectMask              = isColor ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;
        subresRange.baseMipLevel            = 0;
        subresRange.levelCount              = 1;
        subresRange.baseArrayLayer          = 0;
        subresRange.layerCount              = 1;

        VkImageMemoryBarrier imgBarrier = VkImageMemoryBarrier{};
        imgBarrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.pNext                = nullptr;
        imgBarrier.srcQueueFamilyIndex  = m_graphicsQueueIndices.first;
        imgBarrier.dstQueueFamilyIndex  = m_graphicsQueueIndices.first;
        imgBarrier.image                = img;
        imgBarrier.subresourceRange     = subresRange;

        VkPipelineStageFlags srcStageMask;
        VkPipelineStageFlags dstStageMask;

        if (type == ImageTransitionType::Present2RT)
        {
            imgBarrier.srcAccessMask = VK_ACCESS_NONE;
            imgBarrier.dstAccessMask = isColor ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT : VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            imgBarrier.oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
            imgBarrier.newLayout     = isColor ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            srcStageMask = isColor ? VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT : VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dstStageMask = isColor ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT : VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }
        else if (type == ImageTransitionType::RT2Present)
        {
            imgBarrier.srcAccessMask = isColor ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT : VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            imgBarrier.dstAccessMask = VK_ACCESS_NONE;
            imgBarrier.oldLayout     = isColor ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            imgBarrier.newLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        else
        {
            LOGA(false, "!!");
        }

        vkCmdPipelineBarrier(buffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imgBarrier);
    }

} // namespace LinaGX
