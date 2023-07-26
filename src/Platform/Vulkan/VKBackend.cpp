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

#include "LinaGX/Platform/Vulkan/VKBackend.hpp"
#include "LinaGX/Platform/Vulkan/SDK/VkBootstrap.h"
#include "LinaGX/Core/Commands.hpp"
#include "LinaGX/Core/Renderer.hpp"
#include "LinaGX/Core/CommandStream.hpp"

#define VMA_IMPLEMENTATION
#include "LinaGX/Platform/Vulkan/SDK/vk_mem_alloc.h"

namespace LinaGX
{

    PFN_vkSetDebugUtilsObjectNameEXT g_vkSetDebugUtilsObjectNameEXT;
#define pfn_vkSetDebugUtilsObjectNameEXT g_vkSetDebugUtilsObjectNameEXT

#ifndef NDEBUG
#define VK_NAME_OBJECT(namedObject, objType, name, structName)                                     \
    VkDebugUtilsObjectNameInfoEXT structName = {};                                                 \
    structName.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT; \
    structName.objectType                    = objType;                                            \
    structName.objectHandle                  = (uint64_t)namedObject;                              \
    structName.pObjectName                   = name;                                               \
    pfn_vkSetDebugUtilsObjectNameEXT(m_device, &structName);

#else
#define VK_NAME_OBJECT(namedObject, name, structName)
#endif

#define LGX_VK_MAJOR 1
#define LGX_VK_MINOR 3

    LINAGX_STRING LinaGX_VkErr(VkResult errorCode)
    {
        switch (errorCode)
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

#define VK_CHECK_RESULT(item, err)                                                                              \
    {                                                                                                           \
        LOGA(item == VK_SUCCESS, "Backend -> Fatal, operation failed! Result: %s", LinaGX_VkErr(item).c_str()); \
        if (item != VK_SUCCESS)                                                                                 \
            throw std::runtime_error(err);                                                                      \
    }

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
        case Format::R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
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
        case Format::R32_UINT:
            return VK_FORMAT_R32_UINT;
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

    VkIndexType GetVKIndexType(IndexType type)
    {
        switch (type)
        {
        case IndexType::Uint16:
            return VK_INDEX_TYPE_UINT16;
        case IndexType::Uint32:
            return VK_INDEX_TYPE_UINT32;
        default:
            return VK_INDEX_TYPE_UINT32;
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
        case ShaderStage::Fragment:
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

    VkImageAspectFlags GetVKAspectFlags(DepthStencilAspect depthStencilAspect)
    {
        switch (depthStencilAspect)
        {
        case LinaGX::DepthStencilAspect::DepthOnly:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case LinaGX::DepthStencilAspect::StencilOnly:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        case LinaGX::DepthStencilAspect::DepthStencil:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
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

    VkFilter GetVKFilter(Filter f)
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

    VkSamplerAddressMode GetVKSamplerAddressMode(SamplerAddressMode m)
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

    VkSamplerMipmapMode GetVKMipmapMode(MipmapMode mode)
    {
        switch (mode)
        {
        case MipmapMode::Nearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case MipmapMode::Linear:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }

    VkPresentModeKHR GetVKPresentMode(VsyncMode vsync)
    {
        switch (vsync)
        {
        case VsyncMode::None:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case VsyncMode::EveryVBlank:
            return VK_PRESENT_MODE_FIFO_KHR;
        case VsyncMode::EverySecondVBlank:
            return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        default:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    VkDescriptorType GetVKDescriptorType(DescriptorType type)
    {
        switch (type)
        {
        case LinaGX::DescriptorType::CombinedImageSampler:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case LinaGX::DescriptorType::SeparateSampler:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case LinaGX::DescriptorType::SSBO:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case LinaGX::DescriptorType::UBO:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case LinaGX::DescriptorType::ConstantBlock:
            LOGA(false, "Can not use!");
        default:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }

    VkBorderColor GetVKBorderColor(BorderColor col)
    {
        switch (col)
        {
        case BorderColor::BlackTransparent:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case BorderColor::BlackOpaque:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case BorderColor::WhiteOpaque:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        default:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOGE("Backend -> Validation Layer: %s", pCallbackData->pMessage);
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOGT("Backend -> Validation Layer: %s", pCallbackData->pMessage);
            break;
        default:
            LOGT("Backend -> Validation Layer: %s", pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }

    uint16 VKBackend::CreateUserSemaphore()
    {
        VKBUserSemaphore item = {};
        item.isValid          = true;

        VkSemaphoreTypeCreateInfo timelineCreateInfo;
        timelineCreateInfo.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        timelineCreateInfo.pNext         = NULL;
        timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        timelineCreateInfo.initialValue  = 0;

        VkSemaphoreCreateInfo info = VkSemaphoreCreateInfo{};
        info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        info.pNext                 = &timelineCreateInfo;
        info.flags                 = 0;

        VkResult result = vkCreateSemaphore(m_device, &info, m_allocator, &item.ptr);
        VK_CHECK_RESULT(result, "Failed creating semaphore.");
        return m_userSemaphores.AddItem(item);
    }

    void VKBackend::DestroyUserSemaphore(uint16 handle)
    {
        auto& semaphore = m_userSemaphores.GetItemR(handle);
        if (!semaphore.isValid)
        {
            LOGE("Backend -> Semaphore to be destroyed is not valid!");
            return;
        }

        vkDestroySemaphore(m_device, semaphore.ptr, m_allocator);
        m_userSemaphores.RemoveItem(handle);
    }

    void VKBackend::WaitForUserSemaphore(uint16 handle, uint64 value)
    {
        const uint64        timeout  = static_cast<uint64>(5000000000);
        VkSemaphore         s        = m_userSemaphores.GetItemR(handle).ptr;
        VkSemaphoreWaitInfo waitInfo = VkSemaphoreWaitInfo{};
        waitInfo.sType               = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
        waitInfo.pNext               = nullptr;
        waitInfo.flags               = 0;
        waitInfo.semaphoreCount      = 1;
        waitInfo.pSemaphores         = &s;
        waitInfo.pValues             = &value;
        vkWaitSemaphores(m_device, &waitInfo, timeout);
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
        LOGA(false, "Backend -> Vulkan backend is only supported for Windows at the moment!");
#endif

        VkPresentModeKHR presentMode = GetVKPresentMode(desc.vsyncMode);

        VkFormat              swpFormat = GetVKFormat(desc.format);
        vkb::SwapchainBuilder swapchainBuilder{m_gpu, m_device, surface};
        swapchainBuilder = swapchainBuilder
                               //.use_default_format_selection()
                               .set_desired_present_mode(presentMode)
                               .set_desired_extent(desc.width, desc.height)
                               .set_desired_format({swpFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                               .set_desired_min_image_count(m_initInfo.backbufferCount);

        vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
        swp.ptr                     = vkbSwapchain.swapchain;
        swp.format                  = vkbSwapchain.image_format;
        swp.depthFormat             = desc.depthFormat;
        swp.imgs                    = vkbSwapchain.get_images().value();
        swp.views                   = vkbSwapchain.get_image_views().value();
        swp.surface                 = surface;
        swp.presentMode             = presentMode;
        swp.width                   = vkbSwapchain.extent.width;
        swp.height                  = vkbSwapchain.extent.height;
        swp.isValid                 = true;

        if (swp.format != swpFormat)
        {
            LOGE("Backend -> Desired color format for swapchain was not supported, selected a supported format!");
        }

        for (const auto& img : swp.imgs)
        {
            Texture2DDesc depthDesc      = {};
            depthDesc.format             = desc.depthFormat;
            depthDesc.width              = swp.width;
            depthDesc.height             = swp.height;
            depthDesc.mipLevels          = 1;
            depthDesc.usage              = Texture2DUsage::DepthStencilTexture;
            depthDesc.depthStencilAspect = DepthStencilAspect::DepthOnly;
            swp.depthTextures.push_back(CreateTexture2D(depthDesc));
        }

        LOGT("Backend -> Successfuly created swapchain with size %d x %d", desc.width, desc.height);

        return m_swapchains.AddItem(swp);
    }

    void VKBackend::DestroySwapchain(uint8 handle)
    {
        auto& swp = m_swapchains.GetItemR(handle);
        if (!swp.isValid)
        {
            LOGE("Backend -> Swapchain to be destroyed is not valid!");
            return;
        }

        for (auto t : swp.depthTextures)
            DestroyTexture2D(t);

        vkDestroySwapchainKHR(m_device, swp.ptr, m_allocator);

        for (auto view : swp.views)
            vkDestroyImageView(m_device, view, m_allocator);

        vkDestroySurfaceKHR(m_vkInstance, swp.surface, m_allocator);

        swp.isValid = false;
        m_swapchains.RemoveItem(handle);
        LOGT("Backend -> Destroyed swapchain.");
    }

    void VKBackend::RecreateSwapchain(const SwapchainRecreateDesc& desc)
    {
        auto& swp       = m_swapchains.GetItemR(desc.swapchain);
        swp.width       = desc.width;
        swp.height      = desc.height;
        swp.presentMode = GetVKPresentMode(desc.vsyncMode);

        if (desc.width == 0 || desc.height == 0)
            return;

        Join();

        if (!swp.isValid)
        {
            LOGE("Backend -> Swapchain to be re-created is not valid!");
            return;
        }

        auto                  oldSwapchain = swp.ptr;
        vkb::SwapchainBuilder swapchainBuilder{m_gpu, m_device, swp.surface};
        vkb::Swapchain        vkbSwapchain = swapchainBuilder
                                          .set_old_swapchain(swp.ptr)
                                          .set_desired_present_mode(swp.presentMode)
                                          .set_desired_extent(desc.width, desc.height)
                                          .set_desired_format({swp.format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                                          .set_required_min_image_count(m_initInfo.backbufferCount)
                                          .build()
                                          .value();
        swp.ptr    = vkbSwapchain.swapchain;
        swp.format = vkbSwapchain.image_format;
        swp.width  = vkbSwapchain.extent.width;
        swp.height = vkbSwapchain.extent.height;

        vkDestroySwapchainKHR(m_device, oldSwapchain, m_allocator);

        for (auto view : swp.views)
            vkDestroyImageView(m_device, view, m_allocator);

        for (auto depth : swp.depthTextures)
            DestroyTexture2D(depth);

        std::vector<VkImage>     imgs  = vkbSwapchain.get_images().value();
        std::vector<VkImageView> views = vkbSwapchain.get_image_views().value();
        swp.imgs.clear();
        swp.views.clear();
        swp.depthTextures.clear();

        for (VkImage img : imgs)
        {
            swp.imgs.push_back(img);

            Texture2DDesc depthDesc      = {};
            depthDesc.format             = swp.depthFormat;
            depthDesc.width              = swp.width;
            depthDesc.height             = swp.height;
            depthDesc.mipLevels          = 1;
            depthDesc.usage              = Texture2DUsage::DepthStencilTexture;
            depthDesc.depthStencilAspect = DepthStencilAspect::DepthOnly;
            swp.depthTextures.push_back(CreateTexture2D(depthDesc));
        }

        for (VkImageView view : views)
            swp.views.push_back(view);
    }

    bool VKBackend::CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob)
    {
        LOGA(false, "!!");
        return false;
    }

    uint16 VKBackend::CreateShader(const ShaderDesc& shaderDesc)
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

        for (auto& [stage, blob] : shaderDesc.stages)
        {
            auto&                    ptr          = shader.modules[stage];
            VkShaderModuleCreateInfo shaderModule = VkShaderModuleCreateInfo{};
            shaderModule.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModule.pNext                    = nullptr;
            shaderModule.codeSize                 = blob.size;
            shaderModule.pCode                    = reinterpret_cast<uint32*>(blob.ptr);

            VkResult res = vkCreateShaderModule(m_device, &shaderModule, nullptr, &ptr);
            VK_CHECK_RESULT(res, "Failed creating shader module");

            VkPipelineShaderStageCreateInfo info = VkPipelineShaderStageCreateInfo{};
            info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext                           = nullptr;
            info.stage                           = static_cast<VkShaderStageFlagBits>(GetVKShaderStage(stage));
            info.module                          = ptr;
            info.pName                           = "main";
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
        depthStencil.depthCompareOp                              = GetVKCompareOp(shaderDesc.depthCompare);
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
            binding.descriptorCount              = ubo.elementSize;

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
            binding.descriptorCount              = t2d.elementSize == 0 ? m_gpuProperties.limits.maxPerStageDescriptorSamplers : t2d.elementSize;

            for (ShaderStage stg : t2d.stages)
                binding.stageFlags |= GetVKShaderStage(stg);

            bindingMap[t2d.set].push_back(binding);
        }

        for (const auto& t2d : shaderDesc.layout.separateImages)
        {
            VkDescriptorSetLayoutBinding binding = VkDescriptorSetLayoutBinding{};
            binding.binding                      = t2d.binding;
            binding.descriptorType               = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            binding.descriptorCount              = t2d.elementSize == 0 ? m_gpuProperties.limits.maxDescriptorSetSampledImages : t2d.elementSize;

            for (ShaderStage stg : t2d.stages)
                binding.stageFlags |= GetVKShaderStage(stg);

            bindingMap[t2d.set].push_back(binding);
        }

        for (const auto& sampler : shaderDesc.layout.samplers)
        {
            VkDescriptorSetLayoutBinding binding = VkDescriptorSetLayoutBinding{};
            binding.binding                      = sampler.binding;
            binding.descriptorType               = VK_DESCRIPTOR_TYPE_SAMPLER;
            binding.descriptorCount              = sampler.elementSize == 0 ? m_gpuProperties.limits.maxDescriptorSetSamplers : sampler.elementSize;

            for (ShaderStage stg : sampler.stages)
                binding.stageFlags |= GetVKShaderStage(stg);

            bindingMap[sampler.set].push_back(binding);
        }

        shader.layouts.resize(bindingMap.size());

        // TODO: descriptor set flag reflection?
        for (const auto& [set, bindings] : bindingMap)
        {
            VkDescriptorSetLayoutCreateInfo setInfo = VkDescriptorSetLayoutCreateInfo{};
            setInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setInfo.flags                           = 0;
            setInfo.bindingCount                    = static_cast<uint32>(bindings.size());
            setInfo.pBindings                       = bindings.data();

            LINAGX_VEC<VkDescriptorBindingFlags> bindlessFlags;

            bool containsBindless = false;
            for (const auto& b : bindings)
            {
                if (b.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE && b.descriptorCount == m_gpuProperties.limits.maxPerStageDescriptorSampledImages)
                {
                    containsBindless = true;
                    bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
                }
                else if (b.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER && b.descriptorCount == m_gpuProperties.limits.maxPerStageDescriptorSamplers)
                {
                    containsBindless = true;
                    bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
                }
                else if (b.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && b.descriptorCount == m_gpuProperties.limits.maxPerStageDescriptorSamplers)
                {
                    containsBindless = true;
                    bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
                }
                else
                {
                    bindlessFlags.push_back(0);
                }
            }

            VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extInfo;
            extInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
            extInfo.pBindingFlags = bindlessFlags.data();
            extInfo.bindingCount  = setInfo.bindingCount;
            extInfo.pNext         = nullptr;

            if (containsBindless)
            {
                setInfo.pNext = &extInfo;
                LOGA(m_initInfo.gpuFeatures.enableBindless, "Enable bindless feature in InitInfo to use bindless descriptors!");
            }

            VkDescriptorSetLayout layout = nullptr;
            VkResult              res    = vkCreateDescriptorSetLayout(m_device, &setInfo, m_allocator, &layout);
            VK_CHECK_RESULT(res, "Failed creating descriptor set layout.");
            shader.layouts[set] = layout;
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
        VK_CHECK_RESULT(res, "Failed creating pipeline layout!");

        VkFormat                         attFormat                   = GetVKFormat(shaderDesc.colorAttachmentFormat);
        VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {};
        pipelineRenderingCreateInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipelineRenderingCreateInfo.colorAttachmentCount             = 1;
        pipelineRenderingCreateInfo.pColorAttachmentFormats          = &attFormat;
        pipelineRenderingCreateInfo.depthAttachmentFormat            = GetVKFormat(shaderDesc.depthAttachmentFormat);

        // Actual pipeline.
        // Actual pipeline.
        VkGraphicsPipelineCreateInfo pipelineInfo = VkGraphicsPipelineCreateInfo{};
        pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext                        = &pipelineRenderingCreateInfo;
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
        LOGA(res == VK_SUCCESS, "Backend -> Could not create shader pipeline!");

        VK_NAME_OBJECT(shader.ptrPipeline, VK_OBJECT_TYPE_PIPELINE, shaderDesc.debugName, info);
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
            LOGE("Backend -> Shader to be destroyed is not valid!");
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
        VKBTexture2D item       = {};
        item.usage              = txtDesc.usage;
        item.isValid            = true;
        item.depthStencilAspect = txtDesc.depthStencilAspect;

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
        imgCreateInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        imgCreateInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;

        if (txtDesc.usage == Texture2DUsage::DepthStencilTexture)
        {
            imgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        else if (txtDesc.usage == Texture2DUsage::ColorTextureRenderTarget)
        {
            imgCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        else
        {
            imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        VmaAllocationCreateInfo allocinfo = VmaAllocationCreateInfo{};
        allocinfo.usage                   = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocinfo.requiredFlags           = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        VkResult res = vmaCreateImage(m_vmaAllocator, &imgCreateInfo, &allocinfo, &item.img, &item.allocation, nullptr);
        VK_CHECK_RESULT(res, "Failed creating image.");

        VK_NAME_OBJECT(item.img, VK_OBJECT_TYPE_IMAGE, txtDesc.debugName, info);

        VkImageSubresourceRange subResRange = VkImageSubresourceRange{};
        subResRange.aspectMask              = txtDesc.usage == Texture2DUsage::DepthStencilTexture ? (GetVKAspectFlags(txtDesc.depthStencilAspect)) : VK_IMAGE_ASPECT_COLOR_BIT;
        subResRange.baseMipLevel            = 0;
        subResRange.levelCount              = txtDesc.mipLevels;
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
        VK_CHECK_RESULT(res, "Failed creating image view.");

        return m_texture2Ds.AddItem(item);
    }

    void VKBackend::DestroyTexture2D(uint32 handle)
    {
        auto& txt = m_texture2Ds.GetItemR(handle);
        if (!txt.isValid)
        {
            LOGE("Backend -> Texture to be destroyed is not valid!");
            return;
        }

        vkDestroyImageView(m_device, txt.imgView, m_allocator);
        vmaDestroyImage(m_vmaAllocator, txt.img, txt.allocation);

        txt.isValid = false;
        m_texture2Ds.RemoveItem(handle);
    }

    uint32 VKBackend::CreateResource(const ResourceDesc& desc)
    {
        VKBResource item = {};
        item.size        = desc.size;
        item.isValid     = true;
        item.heapType    = desc.heapType;

        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size               = desc.size;

        if (desc.typeHintFlags & TH_VertexBuffer)
            bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (desc.typeHintFlags & TH_ConstantBuffer)
            bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (desc.typeHintFlags & TH_IndexBuffer)
            bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (desc.typeHintFlags & TH_IndexBuffer)
            bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        if (desc.typeHintFlags & TH_StorageBuffer)
            bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

        if (bufferInfo.usage == 0)
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo allocInfo = {};

        if (desc.heapType == ResourceHeap::CPUVisibleGPUMemory)
        {
            LOGA(GPUInfo.totalCPUVisibleGPUMemorySize != 0, "Backend -> Trying to create a host-visible gpu resource meanwhile current gpu doesn't support this!");
            allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags         = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        else if (desc.heapType == ResourceHeap::GPUOnly)
        {
            allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            allocInfo.usage         = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        else if (desc.heapType == ResourceHeap::StagingHeap)
        {
            allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            allocInfo.usage         = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            allocInfo.flags         = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }

        vmaCreateBuffer(m_vmaAllocator, &bufferInfo, &allocInfo, &item.buffer, &item.allocation, nullptr);
        VK_NAME_OBJECT(item.buffer, VK_OBJECT_TYPE_BUFFER, desc.debugName, info);

        return m_resources.AddItem(item);
    }

    uint32 VKBackend::CreateSampler(const SamplerDesc& desc)
    {
        VKBSampler item = {};
        item.isValid    = true;

        VkSamplerCreateInfo info = VkSamplerCreateInfo{};
        info.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.pNext               = nullptr;
        info.magFilter           = GetVKFilter(desc.magFilter);
        info.minFilter           = GetVKFilter(desc.minFilter);
        info.mipmapMode          = GetVKMipmapMode(desc.mipmapMode);
        info.addressModeU        = GetVKSamplerAddressMode(desc.mode);
        info.addressModeV        = GetVKSamplerAddressMode(desc.mode);
        info.addressModeW        = GetVKSamplerAddressMode(desc.mode);
        info.mipLodBias          = desc.mipLodBias;
        info.anisotropyEnable    = desc.anisotropy != 0;
        info.maxAnisotropy       = static_cast<float>(desc.anisotropy);
        info.minLod              = desc.minLod;
        info.maxLod              = desc.maxLod;
        info.borderColor         = GetVKBorderColor(desc.borderColor);

        VkResult res = vkCreateSampler(m_device, &info, m_allocator, &item.ptr);
        VK_CHECK_RESULT(res, "Backend -> Could not create sampler!");

        return m_samplers.AddItem(item);
    }

    void VKBackend::DestroySampler(uint32 handle)
    {
        auto& item = m_samplers.GetItemR(handle);
        if (!item.isValid)
        {
            LOGE("Backend -> Sampler to be destroyed is not valid!");
            return;
        }

        vkDestroySampler(m_device, item.ptr, m_allocator);
        m_samplers.RemoveItem(handle);
    }

    void VKBackend::MapResource(uint32 handle, uint8*& ptr)
    {
        auto& item    = m_resources.GetItemR(handle);
        item.isMapped = true;
        vmaMapMemory(m_vmaAllocator, item.allocation, reinterpret_cast<void**>(&ptr));
    }

    void VKBackend::UnmapResource(uint32 handle)
    {
        auto& item = m_resources.GetItemR(handle);
        if (!item.isMapped)
            return;

        vmaUnmapMemory(m_vmaAllocator, item.allocation);
        item.isMapped = false;
    }

    void VKBackend::DestroyResource(uint32 handle)
    {
        auto& item = m_resources.GetItemR(handle);
        if (!item.isValid)
        {
            LOGE("Backend -> Shader to be destroyed is not valid!");
            return;
        }

        if (item.isMapped)
        {
            UnmapResource(handle);
        }

        vmaDestroyBuffer(m_vmaAllocator, item.buffer, item.allocation);

        m_resources.RemoveItem(handle);
    }

    uint16 VKBackend::CreateDescriptorSet(const DescriptorSetDesc& desc)
    {
        VKBDescriptorSet item = {};
        item.isValid          = true;

        LINAGX_VEC<VkDescriptorSetLayoutBinding> bindings;
        bool                                     containsBindlessTextures = false;
        LINAGX_VEC<VkDescriptorBindingFlags>     bindlessFlags;

        for (uint32 i = 0; i < desc.bindingsCount; i++)
        {
            DescriptorBinding&           binding   = desc.bindings[i];
            VkDescriptorSetLayoutBinding vkBinding = {};
            vkBinding.binding                      = binding.binding;
            vkBinding.descriptorType               = GetVKDescriptorType(binding.type);
            vkBinding.descriptorCount              = binding.descriptorCount;

            if (binding.type == DescriptorType::SeparateImage && binding.bindless)
            {
                containsBindlessTextures = true;
                bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
                vkBinding.descriptorCount = m_gpuProperties.limits.maxDescriptorSetSampledImages;

            }
            else if (binding.type == DescriptorType::SeparateSampler && binding.bindless)
            {
                containsBindlessTextures = true;
                bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
                vkBinding.descriptorCount = m_gpuProperties.limits.maxPerStageDescriptorSamplers;
            }
            else if (binding.type == DescriptorType::CombinedImageSampler && binding.bindless)
            {
                containsBindlessTextures = true;
                bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
                vkBinding.descriptorCount = m_gpuProperties.limits.maxPerStageDescriptorSamplers;
            }
            else
                bindlessFlags.push_back(0);

            for (auto stg : binding.stages)
                vkBinding.stageFlags |= GetVKShaderStage(stg);

            bindings.push_back(vkBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.flags                           = 0;
        layoutInfo.bindingCount                    = static_cast<uint32>(bindings.size());
        layoutInfo.pBindings                       = bindings.data();

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extInfo;
        extInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
        extInfo.pBindingFlags = bindlessFlags.data();
        extInfo.bindingCount  = layoutInfo.bindingCount;
        extInfo.pNext         = nullptr;

        if (containsBindlessTextures)
        {
            layoutInfo.pNext = &extInfo;
            LOGA(m_initInfo.gpuFeatures.enableBindless, "Enable bindless feature in InitInfo to use bindless descriptors!");
        }

        VkResult res = vkCreateDescriptorSetLayout(m_device, &layoutInfo, m_allocator, &item.layout);
        VK_CHECK_RESULT(res, "Backend -> Could not create layout!");

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext                       = nullptr;
        allocInfo.descriptorPool              = m_descriptorPool;
        allocInfo.descriptorSetCount          = 1;
        allocInfo.pSetLayouts                 = &item.layout;

        res = vkAllocateDescriptorSets(m_device, &allocInfo, &item.ptr);
        VK_CHECK_RESULT(res, "Backend -> Could not allocate descriptor set, make sure you have set enough descriptor limits in LinaGX::InitInfo::GPULimits structure!");
        return m_descriptorSets.AddItem(item);
    }

    void VKBackend::DestroyDescriptorSet(uint16 handle)
    {
        auto& item = m_descriptorSets.GetItemR(handle);
        if (!item.isValid)
        {
            LOGE("Backend -> Descriptor set to be destroyed is not valid!");
            return;
        }

        vkDestroyDescriptorSetLayout(m_device, item.layout, m_allocator);

        m_descriptorSets.RemoveItem(handle);
    }

    void VKBackend::DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc)
    {
        LINAGX_VEC<VkDescriptorBufferInfo> bufferInfos;

        for (uint32 i = 0; i < desc.descriptorCount; i++)
        {
            const auto&            res   = m_resources.GetItemR(desc.resources[i]);
            VkDescriptorBufferInfo binfo = {};
            binfo.buffer                 = res.buffer;
            binfo.offset                 = 0;
            binfo.range                  = res.size;
            bufferInfos.push_back(binfo);
        }

        VkWriteDescriptorSet write = {};
        write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext                = nullptr;
        write.dstSet               = m_descriptorSets.GetItemR(desc.setHandle).ptr;
        write.dstBinding           = desc.binding;
        write.descriptorCount      = desc.descriptorCount;
        write.descriptorType       = GetVKDescriptorType(desc.descriptorType);
        write.pBufferInfo          = bufferInfos.data();
        vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
    }

    void VKBackend::DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc)
    {

        LINAGX_VEC<VkDescriptorImageInfo> imgInfos;

        for (uint32 i = 0; i < desc.descriptorCount; i++)
        {
            VkDescriptorImageInfo imgInfo = VkDescriptorImageInfo{};
            imgInfo.sampler               = m_samplers.GetItemR(desc.samplers[i]).ptr;
            imgInfo.imageView             = m_texture2Ds.GetItemR(desc.textures[i]).imgView;
            imgInfo.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imgInfos.push_back(imgInfo);
        }

        VkWriteDescriptorSet write = {};
        write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext                = nullptr;
        write.dstSet               = m_descriptorSets.GetItemR(desc.setHandle).ptr;
        write.dstBinding           = desc.binding;
        write.descriptorCount      = desc.descriptorCount;
        write.descriptorType       = GetVKDescriptorType(desc.descriptorType);
        write.pImageInfo           = imgInfos.data();
        vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
    }

    uint32 VKBackend::CreateCommandStream(QueueType cmdType)
    {
        VKBCommandStream item = {};
        item.isValid          = true;

        uint32 familyIndex = 0;
        if (cmdType == QueueType::Graphics)
            familyIndex = m_graphicsQueueIndices.first;
        else if (cmdType == QueueType::Compute)
            familyIndex = m_computeQueueIndices.first;
        else if (cmdType == QueueType::Transfer)
            familyIndex = m_transferQueueIndices.first;

        VkCommandPoolCreateInfo commandPoolInfo = VkCommandPoolCreateInfo{};
        commandPoolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.pNext                   = nullptr;
        commandPoolInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex        = familyIndex;

        VkResult result = vkCreateCommandPool(m_device, &commandPoolInfo, m_allocator, &item.pool);
        VK_CHECK_RESULT(result, "Failed creating command pool");

        VkCommandBufferAllocateInfo cmdAllocInfo = VkCommandBufferAllocateInfo{};
        cmdAllocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext                       = nullptr;
        cmdAllocInfo.commandPool                 = item.pool;
        cmdAllocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandBufferCount          = 1;

        VkCommandBuffer b = nullptr;
        result            = vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &item.buffer);
        VK_CHECK_RESULT(result, "Failed allocating command buffers");

        return m_cmdStreams.AddItem(item);
    }

    void VKBackend::DestroyCommandStream(uint32 handle)
    {
        auto& stream = m_cmdStreams.GetItemR(handle);
        if (!stream.isValid)
        {
            LOGE("Backend -> Command Stream to be destroyed is not valid!");
            return;
        }

        stream.isValid = false;
        vkDestroyCommandPool(m_device, stream.pool, m_allocator);
        m_cmdStreams.RemoveItem(handle);
    }

    void VKBackend::CloseCommandStreams(CommandStream** streams, uint32 streamCount)
    {

        for (uint32 i = 0; i < streamCount; i++)
        {
            auto  stream = streams[i];
            auto& sr     = m_cmdStreams.GetItemR(stream->m_gpuHandle);
            auto  buffer = sr.buffer;
            auto  pool   = sr.pool;

            if (stream->m_commandCount == 0)
                continue;

            vkResetCommandPool(m_device, pool, 0);

            VkCommandBufferBeginInfo beginInfo = VkCommandBufferBeginInfo{};
            beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.pNext                    = nullptr;
            beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            beginInfo.pInheritanceInfo         = nullptr;
            VkResult res                       = vkBeginCommandBuffer(buffer, &beginInfo);
            VK_CHECK_RESULT(res, "Failed beginning command buffer.");

            sr.boundShader = 0;

            for (uint32 i = 0; i < stream->m_commandCount; i++)
            {
                uint8* data = stream->m_commands[i];
                TypeID tid  = 0;
                LINAGX_MEMCPY(&tid, data, sizeof(TypeID));
                const size_t increment = sizeof(TypeID);
                uint8*       cmd       = data + increment;
                (this->*m_cmdFunctions[tid])(cmd, sr);
            }

            res = vkEndCommandBuffer(buffer);
            VK_CHECK_RESULT(res, "Failed ending command buffer!");
        }
    }

    void VKBackend::SubmitCommandStreams(const SubmitDesc& desc)
    {
        auto&                       frame = m_perFrameData[m_currentFrameIndex];
        LINAGX_VEC<VkCommandBuffer> _buffers;

        // Push all valid command buffers into a list.
        for (uint32 i = 0; i < desc.streamCount; i++)
        {
            auto stream = desc.streams[i];
            if (stream->m_commandCount == 0)
            {
                LOGE("Backend -> Can not execute stream as no commands were recorded!");
                continue;
            }

            auto& str = m_cmdStreams.GetItemR(stream->m_gpuHandle);
            _buffers.push_back(str.buffer);

            // Mark submitted intermediate resources.
            for (auto& inter : str.intermediateResources)
                m_killQueueIntermediateResources[inter] = PerformanceStats.totalFrames;
            str.intermediateResources.clear();
        }

        auto queue = m_queueData[desc.queue].queue;

        VkPipelineStageFlags             waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        LINAGX_VEC<VkSemaphore>          waitSemaphores;
        LINAGX_VEC<VkPipelineStageFlags> waitStages;
        LINAGX_VEC<VkSemaphore>          signalSemaphores;
        LINAGX_VEC<uint64>               waitSemaphoreValues;
        LINAGX_VEC<uint64>               signalSemaphoreValues;

        // This is for WITHIN the frame.
        // We wait for image acquired semaphores.
        // We signal 1 semaphore per submit for presentation.
        {
            if (desc.queue == QueueType::Graphics)
            {
                for (uint32 i = 0; i < m_imageAcqSemaphoresCount; i++)
                {
                    waitSemaphores.push_back(frame.imageAcquiredSemaphores[i]);
                    waitStages.push_back(waitStage);
                    waitSemaphoreValues.push_back(0); // ignored binary semaphore.
                }
            }

            if (m_imageAcqSemaphoresCount != 0)
            {
                signalSemaphores.push_back(frame.submitSemaphores[frame.submissionCount]);
                signalSemaphoreValues.push_back(0);
            }
        }

        // This is for USER signal mechanisms.
        {
            if (desc.useWait)
            {
                waitSemaphores.push_back(m_userSemaphores.GetItem(desc.waitSemaphore).ptr);
                waitSemaphoreValues.push_back(desc.waitValue);
                waitStages.push_back(waitStage);
            }

            if (desc.useSignal)
            {
                signalSemaphores.push_back(m_userSemaphores.GetItem(desc.signalSemaphore).ptr);
                signalSemaphoreValues.push_back(desc.signalValue);
            }
        }

        VkTimelineSemaphoreSubmitInfo timelineInfo;
        timelineInfo.sType                     = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        timelineInfo.pNext                     = NULL;
        timelineInfo.waitSemaphoreValueCount   = static_cast<uint32>(waitSemaphores.size());
        timelineInfo.pWaitSemaphoreValues      = waitSemaphoreValues.data();
        timelineInfo.signalSemaphoreValueCount = static_cast<uint32>(signalSemaphores.size());
        timelineInfo.pSignalSemaphoreValues    = signalSemaphoreValues.data();

        VkSubmitInfo submitInfo;
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext                = &timelineInfo;
        submitInfo.waitSemaphoreCount   = static_cast<uint32>(waitSemaphores.size());
        submitInfo.pWaitSemaphores      = waitSemaphores.data();
        submitInfo.signalSemaphoreCount = static_cast<uint32>(signalSemaphores.size());
        submitInfo.pSignalSemaphores    = signalSemaphores.data();
        submitInfo.commandBufferCount   = desc.streamCount;
        submitInfo.pCommandBuffers      = _buffers.data();
        submitInfo.pWaitDstStageMask    = waitStages.data();

        VkResult res = vkQueueSubmit(queue, 1, &submitInfo, frame.submitFences[frame.submissionCount]);
        frame.submissionCount++;

        LOGA((frame.submissionCount < m_initInfo.gpuLimits.maxSubmitsPerFrame), "Backend -> Exceeded maximum submissions per frame! Please increase the limit.");
        VK_CHECK_RESULT(res, "Failed submitting to queue!");
    }

    uint16 VKBackend::CreateFence()
    {
        VkFence           fence = nullptr;
        VkFenceCreateInfo info  = VkFenceCreateInfo{};
        info.sType              = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext              = nullptr;
        info.flags              = VK_FENCE_CREATE_SIGNALED_BIT;

        VkResult result = vkCreateFence(m_device, &info, m_allocator, &fence);
        VK_CHECK_RESULT(result, "Failed creating fence!");
        return m_fences.AddItem(fence);
    }

    void VKBackend::DestroyFence(uint16 handle)
    {
        auto fence = m_fences.GetItemR(handle);
        if (fence == nullptr)
        {
            LOGE("Backend -> Fence to be destroyed is not valid!");
            return;
        }

        vkDestroyFence(m_device, fence, m_allocator);

        fence = nullptr;
        m_fences.RemoveItem(handle);
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

        VkDebugUtilsMessageSeverityFlagsEXT severity = 0;

#ifndef NDEBUG
        severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
#endif
        builder.set_debug_messenger_severity(severity);
        builder.set_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

        auto res = builder.build();
        if (!res)
        {
            LOGE("Backend ->Vulkan builder failed!");
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

        if (m_initInfo.gpuFeatures.enableBindless)
        {
            features.shaderSampledImageArrayDynamicIndexing = true;
        }

        VkPhysicalDeviceVulkan12Features vk12Features = {};
        vk12Features.timelineSemaphore                = true;

        if (m_initInfo.gpuFeatures.enableBindless)
        {
            vk12Features.runtimeDescriptorArray          = true;
            vk12Features.descriptorBindingPartiallyBound = true;
        }

        // std::vector<const char*> deviceExtensions;
        // deviceExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_MULTIVIEW_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
        // deviceExtensions.push_back(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);

        vkb::PhysicalDeviceSelector selector{inst};
        vkb::PhysicalDevice         physicalDevice = selector.set_minimum_version(LGX_VK_MAJOR, LGX_VK_MINOR)
                                                 .set_required_features_12(vk12Features)
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

        // For using UPDATE_AFTER_BIND_BIT on material bindings - invalid after using VK12 Features
        // VkPhysicalDeviceDescriptorIndexingFeatures descFeatures    = VkPhysicalDeviceDescriptorIndexingFeatures{};
        // descFeatures.sType                                         = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        // descFeatures.pNext                                         = nullptr;
        // descFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
        // descFeatures.descriptorBindingSampledImageUpdateAfterBind  = VK_TRUE;
        // deviceBuilder.add_pNext(&descFeatures);

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature = VkPhysicalDeviceDynamicRenderingFeaturesKHR{};
        dynamic_rendering_feature.sType                                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamic_rendering_feature.dynamicRendering                            = VK_TRUE;

        deviceBuilder.add_pNext(&shaderDrawParamsFeature);
        deviceBuilder.add_pNext(&dynamic_rendering_feature);

        bool   hasDedicatedTransferQueue = physicalDevice.has_dedicated_transfer_queue();
        bool   hasDedicatedComputeQueue  = physicalDevice.has_dedicated_compute_queue();
        auto   queueFamilies             = physicalDevice.get_queue_families();
        uint32 transferQueueFamily       = 0;
        uint32 transferQueueIndex        = 0;
        uint32 graphicsQueueFamily       = 0;
        uint32 graphicsQueueIndex        = 0;
        uint32 computeQueueFamily        = 0;
        uint32 computeQueueIndex         = 0;

        std::vector<vkb::CustomQueueDescription> queueDescs;
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++)
        {
            uint32 count = 1;

            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueFamily = i;

                if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                {
                    transferQueueFamily = i;

                    if (queueFamilies[i].queueCount > count + 1)
                        count++;

                    transferQueueIndex = count - 1;
                }

                if (!hasDedicatedComputeQueue && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                {
                    computeQueueFamily = i;

                    if (queueFamilies[i].queueCount > count + 1)
                        count++;
                    computeQueueIndex = count - 1;
                }
            }

            queueDescs.push_back(vkb::CustomQueueDescription(i, count, std::vector<float>(count, 1.0f)));
        }

        deviceBuilder.custom_queue_setup(queueDescs);

        vkb::Device vkbDevice = deviceBuilder.build().value();
        m_device              = vkbDevice.device;
        m_gpu                 = physicalDevice.physical_device;

        g_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(m_device, "vkSetDebugUtilsObjectNameEXT"));

        // Queue support
        {
            if (hasDedicatedComputeQueue)
            {
                auto res           = vkbDevice.get_dedicated_queue_index(vkb::QueueType::compute);
                computeQueueFamily = res.value();
                computeQueueIndex  = 0;
            }

            m_graphicsQueueIndices = std::make_pair(graphicsQueueFamily, graphicsQueueIndex);
            m_transferQueueIndices = std::make_pair(transferQueueFamily, transferQueueIndex);
            m_computeQueueIndices  = std::make_pair(computeQueueFamily, computeQueueIndex);

            auto& graphics = m_queueData[QueueType::Graphics];
            auto& transfer = m_queueData[QueueType::Transfer];
            auto& compute  = m_queueData[QueueType::Compute];

            vkGetDeviceQueue(m_device, m_graphicsQueueIndices.first, m_graphicsQueueIndices.second, &graphics.queue);
            vkGetDeviceQueue(m_device, m_transferQueueIndices.first, m_transferQueueIndices.second, &transfer.queue);
            vkGetDeviceQueue(m_device, m_computeQueueIndices.first, m_computeQueueIndices.second, &compute.queue);

            VkSemaphoreCreateInfo info = VkSemaphoreCreateInfo{};
            info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            info.pNext                 = nullptr,
            info.flags                 = 0;

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
        }

        // Check format support
        {
            VkImageFormatProperties p;

            const uint32 last = static_cast<uint32>(Format::FORMAT_MAX);

            for (uint32 i = 0; i < last; i++)
            {
                const Format   lgxFormat = static_cast<Format>(i);
                const VkFormat vkFormat  = GetVKFormat(lgxFormat);
                VkResult       supported = vkGetPhysicalDeviceImageFormatProperties(m_gpu, vkFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_SAMPLE_COUNT_1_BIT, &p);
                if (supported == VK_SUCCESS)
                {
                    GPUInfo.supportedImageFormats.push_back(lgxFormat);
                }
            }

            for (auto& f : m_initInfo.checkForFormatSupport)
            {
                auto it = std::find_if(GPUInfo.supportedImageFormats.begin(), GPUInfo.supportedImageFormats.end(), [&](Format format) { return f == format; });
                if (it == GPUInfo.supportedImageFormats.end())
                    LOGE("Backend -> Required format is not supported by the GPU device! %d", static_cast<int>(f));
            }
        }

        // GPU props
        {
            VkPhysicalDeviceProperties gpuProps;
            vkGetPhysicalDeviceProperties(m_gpu, &gpuProps);
            VkPhysicalDeviceMemoryProperties gpuMemProps;
            vkGetPhysicalDeviceMemoryProperties(m_gpu, &gpuMemProps);
            m_minUniformBufferOffsetAlignment = gpuProps.limits.minUniformBufferOffsetAlignment;
            LOGT("Backend -> Selected GPU: %s - %f mb", gpuProps.deviceName, gpuMemProps.memoryHeaps->size / 1000000.0);
            GPUInfo.dedicatedVideoMemory = gpuMemProps.memoryHeaps->size;
            GPUInfo.deviceName           = gpuProps.deviceName;

            m_gpuProperties = gpuProps;

            // Iterate over memory heaps
            for (uint32_t typeIndex = 0; typeIndex < gpuMemProps.memoryTypeCount; ++typeIndex)
            {
                const auto& type = gpuMemProps.memoryTypes[typeIndex];

                // Check if the heap has host-mappable memory properties
                if (type.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && type.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                {
                    GPUInfo.totalCPUVisibleGPUMemorySize = gpuMemProps.memoryHeaps[type.heapIndex].size;
                }
            }
        }

        // Vma
        {
            VmaAllocatorCreateInfo allocatorInfo = {};
            allocatorInfo.physicalDevice         = m_gpu;
            allocatorInfo.device                 = m_device;
            allocatorInfo.instance               = m_vkInstance;
            vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
        }

        // Per frame data
        {
            VkFenceCreateInfo fenceInfo = VkFenceCreateInfo{};
            fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.pNext             = nullptr;
            fenceInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

            VkSemaphoreCreateInfo smpInfo = VkSemaphoreCreateInfo{};
            smpInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            smpInfo.pNext                 = nullptr;
            smpInfo.flags                 = 0;

            for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
            {
                VKBPerFrameData pfd = {};

                pfd.submitFences.resize(m_initInfo.gpuLimits.maxSubmitsPerFrame, nullptr);
                pfd.imageAcquiredSemaphores.resize(m_initInfo.gpuLimits.maxSubmitsPerFrame, nullptr);
                pfd.submitSemaphores.resize(m_initInfo.gpuLimits.maxSubmitsPerFrame, nullptr);

                for (uint32 j = 0; j < m_initInfo.gpuLimits.maxSubmitsPerFrame; j++)
                {
                    VkFence     fence      = nullptr;
                    VkSemaphore semaphore1 = nullptr;
                    VkSemaphore semaphore2 = nullptr;
                    VkResult    result     = vkCreateFence(m_device, &fenceInfo, m_allocator, &fence);
                    VK_CHECK_RESULT(result, "Failed creating fence!");

                    result = vkCreateSemaphore(m_device, &smpInfo, m_allocator, &semaphore1);
                    VK_CHECK_RESULT(result, "Failed creating semaphore.");
                    result = vkCreateSemaphore(m_device, &smpInfo, m_allocator, &semaphore2);
                    VK_CHECK_RESULT(result, "Failed creating semaphore.");

                    pfd.submitSemaphores[j]        = semaphore1;
                    pfd.imageAcquiredSemaphores[j] = semaphore2;
                    pfd.submitFences[j]            = fence;
                }

                vkResetFences(m_device, m_initInfo.gpuLimits.maxSubmitsPerFrame, &pfd.submitFences[0]);

                m_perFrameData.push_back(pfd);
            }
        }

        // Descriptors
        {
            LINAGX_VEC<VkDescriptorPoolSize> sizeInfos;

            uint32 totalSets = 0;

            LINAGX_MAP<DescriptorType, uint32> limits;
            limits[DescriptorType::CombinedImageSampler] = m_initInfo.gpuLimits.samplerLimit;
            limits[DescriptorType::SeparateSampler]      = m_initInfo.gpuLimits.samplerLimit;
            limits[DescriptorType::SeparateImage]        = m_initInfo.gpuLimits.textureLimit;
            limits[DescriptorType::UBO]                  = m_initInfo.gpuLimits.bufferLimit;
            limits[DescriptorType::SSBO]                 = m_initInfo.gpuLimits.bufferLimit;

            for (const auto& [dt, limit] : limits)
            {
                VkDescriptorPoolSize sizeInfo = VkDescriptorPoolSize{};
                sizeInfo.type                 = GetVKDescriptorType(dt);
                sizeInfo.descriptorCount      = limit;
                sizeInfos.push_back(sizeInfo);
                totalSets += limit;
            }

            VkDescriptorPoolCreateInfo info = VkDescriptorPoolCreateInfo{};
            info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            info.flags                      = 0;
            info.maxSets                    = totalSets;
            info.poolSizeCount              = static_cast<uint32>(sizeInfos.size());
            info.pPoolSizes                 = sizeInfos.data();

            VkResult res = vkCreateDescriptorPool(m_device, &info, m_allocator, &m_descriptorPool);
            VK_CHECK_RESULT(res, "Backend -> Could not create descriptor pool!");
        }
        // Command functions
        {
            BACKEND_BIND_COMMANDS(VKBackend);
        }

        return true;
    }

    void VKBackend::Shutdown()
    {
        for (const auto [id, frame] : m_killQueueIntermediateResources)
            DestroyResource(id);

        m_killQueueIntermediateResources.clear();

        vkDestroyDescriptorPool(m_device, m_descriptorPool, m_allocator);

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            auto& pfd = m_perFrameData[i];

            for (uint32 j = 0; j < m_initInfo.gpuLimits.maxSubmitsPerFrame; j++)
            {
                vkDestroyFence(m_device, pfd.submitFences[j], m_allocator);
                vkDestroySemaphore(m_device, pfd.submitSemaphores[j], m_allocator);
                vkDestroySemaphore(m_device, pfd.imageAcquiredSemaphores[j], m_allocator);
            }
        }

        for (auto& swp : m_swapchains)
        {
            LOGA(!swp.isValid, "Backend -> Some swapchains were not destroyed!");
        }

        for (auto& pp : m_shaders)
        {
            LOGA(!pp.isValid, "Backend -> Some shader pipelines were not destroyed!");
        }

        for (auto& txt : m_texture2Ds)
        {
            LOGA(!txt.isValid, "Backend -> Some textures were not destroyed!");
        }

        for (auto& str : m_cmdStreams)
        {
            LOGA(!str.isValid, "Backend -> Some command streams were not destroyed!");
        }

        for (auto& r : m_resources)
        {
            LOGA(!r.isValid, "Backend -> Some resources were not destroyed!");
        }

        for (auto& r : m_userSemaphores)
        {
            LOGA(!r.isValid, "Backend -> Some semaphores were not destroyed!");
        }

        for (auto& r : m_samplers)
        {
            LOGA(!r.isValid, "Backend -> Some samplers were not destroyed!");
        }

        for (auto& r : m_descriptorSets)
        {
            LOGA(!r.isValid, "Backend -> Some descriptor sets were not destroyed!");
        }

        vmaDestroyAllocator(m_vmaAllocator);
        vkDestroyDevice(m_device, m_allocator);
        vkb::destroy_debug_utils_messenger(m_vkInstance, m_debugMessenger);
        vkDestroyInstance(m_vkInstance, m_allocator);
    }

    void VKBackend::Join()
    {
        LINAGX_VEC<VkFence> fences;
        const uint64        timeout = static_cast<uint64>(5000000000);

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            auto& frame = m_perFrameData[i];
            for (uint32 i = 0; i < frame.submissionCount; i++)
                fences.push_back(frame.submitFences[i]);
        }

        vkDeviceWaitIdle(m_device);

        if (!fences.empty())
            vkWaitForFences(m_device, static_cast<uint32>(fences.size()), fences.data(), true, timeout);
    }

    void VKBackend::StartFrame(uint32 frameIndex)
    {
        m_currentFrameIndex  = frameIndex;
        auto&        frame   = m_perFrameData[frameIndex];
        const uint64 timeout = static_cast<uint64>(5000000000);

        // Wait for all submission fences for this frame.
        LINAGX_VEC<VkFence> fences;
        fences.resize(frame.submissionCount);

        for (uint32 i = 0; i < frame.submissionCount; i++)
            fences[i] = frame.submitFences[i];

        if (!fences.empty())
        {
            VkResult result = vkWaitForFences(m_device, static_cast<uint32>(fences.size()), &fences[0], true, timeout);
            VK_CHECK_RESULT(result, "Failed waiting for fences!");
        }

        m_imageAcqSemaphoresCount = 0;
        frame.submissionCount     = 0;

        // Acquire images for each swapchain
        uint8 i = 0;
        for (auto& swp : m_swapchains)
        {
            if (!swp.isValid || swp.width == 0 || swp.height == 0)
            {
                i++;
                continue;
            }

            VkResult result = vkAcquireNextImageKHR(m_device, swp.ptr, timeout, frame.imageAcquiredSemaphores[m_imageAcqSemaphoresCount], nullptr, &swp._imageIndex);
            m_imageAcqSemaphoresCount++;

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            {
                LOGA(false, "Backend -> Image is out of date or suboptimal, did you forget to call LinaGX::Renderer->RecreateSwapchain after a window resize?");
            }

            i++;
        }

        // reset
        if (!fences.empty())
        {
            VkResult result = vkResetFences(m_device, static_cast<uint32>(fences.size()), &fences[0]);
            VK_CHECK_RESULT(result, "Failed resetting fences!");
        }

        // Check for dangling intermediate resources.
        for (auto it = m_killQueueIntermediateResources.begin(); it != m_killQueueIntermediateResources.end();)
        {
            if (PerformanceStats.totalFrames > it->second + m_initInfo.framesInFlight + 1)
            {
                DestroyResource(it->first);
                it = m_killQueueIntermediateResources.erase(it);
            }
            else
                ++it;
        }
    }

    void VKBackend::Present(const PresentDesc& present)
    {
        auto&                   swp   = m_swapchains.GetItemR(present.swapchain);
        auto&                   frame = m_perFrameData[m_currentFrameIndex];
        LINAGX_VEC<VkSemaphore> waitSemaphores;

        if (swp.width == 0 || swp.height == 0)
            return;

        for (uint32 i = 0; i < frame.submissionCount; i++)
            waitSemaphores.push_back(frame.submitSemaphores[i]);

        VkPresentInfoKHR info   = VkPresentInfoKHR{};
        info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext              = nullptr;
        info.waitSemaphoreCount = static_cast<uint32>(waitSemaphores.size());
        info.pWaitSemaphores    = waitSemaphores.data();
        info.swapchainCount     = 1;
        info.pSwapchains        = &swp.ptr;
        info.pImageIndices      = &swp._imageIndex;

        VkResult result = vkQueuePresentKHR(m_queueData[QueueType::Graphics].queue, &info);

        // Check for swapchain recreation.
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            LOGA(false, "Backend -> Image is out of date or suboptimal, did you forget to call LinaGX::Renderer->RecreateSwapchain after a window resize?");
        }
    }

    void VKBackend::EndFrame()
    {
    }

    void VKBackend::CMD_BeginRenderPass(uint8* data, VKBCommandStream& stream)
    {
        CMDBeginRenderPass* begin = reinterpret_cast<CMDBeginRenderPass*>(data);

        VkImageView colorImageView, depthImageView = nullptr;
        VkImage     colorImage, depthImage         = nullptr;

        if (begin->isSwapchain)
        {
            const auto& swp = m_swapchains.GetItemR(begin->swapchain);
            colorImageView  = swp.views[swp._imageIndex];
            colorImage      = swp.imgs[swp._imageIndex];

            const auto& depthTxt = m_texture2Ds.GetItemR(swp.depthTextures[swp._imageIndex]);
            depthImageView       = depthTxt.imgView;
            depthImage           = depthTxt.img;
        }
        else
        {
            const auto& txt      = m_texture2Ds.GetItemR(begin->colorTexture);
            const auto& depthTxt = m_texture2Ds.GetItemR(begin->depthTexture);
            colorImageView       = txt.imgView;
            colorImage           = txt.img;
            depthImageView       = depthTxt.imgView;
            depthImage           = depthTxt.img;
        }

        VkClearValue clearValues[2];
        clearValues[0].color        = {begin->clearColor[0], begin->clearColor[1], begin->clearColor[2], begin->clearColor[3]};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderingAttachmentInfo colorAttachment = VkRenderingAttachmentInfo{};
        colorAttachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachment.pNext                     = nullptr;
        colorAttachment.imageView                 = colorImageView;
        colorAttachment.imageLayout               = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.resolveMode               = VK_RESOLVE_MODE_NONE;
        colorAttachment.resolveImageView          = nullptr;
        colorAttachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue                = clearValues[0];

        VkRenderingAttachmentInfo depthAttachment = VkRenderingAttachmentInfo{};
        depthAttachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthAttachment.pNext                     = nullptr;
        depthAttachment.imageView                 = depthImageView;
        depthAttachment.imageLayout               = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.resolveMode               = VK_RESOLVE_MODE_NONE;
        depthAttachment.resolveImageView          = nullptr;
        depthAttachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp                   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.clearValue                = clearValues[1];

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
        renderingInfo.pDepthAttachment     = &depthAttachment;
        renderingInfo.pStencilAttachment   = nullptr;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments    = &colorAttachment;

        auto buffer = stream.buffer;
        TransitionImageLayout(buffer, colorImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
        vkCmdBeginRendering(buffer, &renderingInfo);
        CMD_SetViewport((uint8*)&interVP, stream);
        CMD_SetScissors((uint8*)&interSC, stream);
    }

    void VKBackend::CMD_EndRenderPass(uint8* data, VKBCommandStream& stream)
    {
        CMDEndRenderPass* end    = reinterpret_cast<CMDEndRenderPass*>(data);
        auto              buffer = stream.buffer;
        vkCmdEndRendering(buffer);

        if (end->isSwapchain)
        {
            const auto& swp = m_swapchains.GetItemR(end->swapchain);
            TransitionImageLayout(buffer, swp.imgs[swp._imageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1);
        }
        else
        {
            const auto& txt = m_texture2Ds.GetItemR(end->texture);
            TransitionImageLayout(buffer, txt.img, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
        }
    }

    void VKBackend::CMD_SetViewport(uint8* data, VKBCommandStream& stream)
    {
        CMDSetViewport* cmd    = reinterpret_cast<CMDSetViewport*>(data);
        auto            buffer = stream.buffer;
        VkViewport      vp     = VkViewport{};
        vp.x                   = static_cast<float>(cmd->x);
        vp.y                   = Config.vulkanConfig.flipViewport ? static_cast<float>(cmd->height) : static_cast<float>(cmd->y);
        vp.width               = static_cast<float>(cmd->width);
        vp.height              = Config.vulkanConfig.flipViewport ? -static_cast<float>(cmd->height) : static_cast<float>(cmd->height);
        vp.minDepth            = cmd->minDepth;
        vp.maxDepth            = cmd->maxDepth;

        if (vp.width == 0.0)
            vp.width = 1.0f;

        if (vp.height == 0.0)
            vp.height = 1.0f;

        vkCmdSetViewport(buffer, 0, 1, &vp);
    }

    void VKBackend::CMD_SetScissors(uint8* data, VKBCommandStream& stream)
    {
        CMDSetViewport* cmd    = reinterpret_cast<CMDSetViewport*>(data);
        auto            buffer = stream.buffer;
        VkRect2D        rect   = VkRect2D{};
        rect.offset.x          = static_cast<int32>(cmd->x);
        rect.offset.y          = static_cast<int32>(cmd->y);
        rect.extent.width      = cmd->width;
        rect.extent.height     = cmd->height;
        vkCmdSetScissor(buffer, 0, 1, &rect);
    }

    void VKBackend::CMD_BindPipeline(uint8* data, VKBCommandStream& stream)
    {
        CMDBindPipeline* cmd    = reinterpret_cast<CMDBindPipeline*>(data);
        auto             buffer = stream.buffer;
        const auto&      shader = m_shaders.GetItemR(cmd->shader);
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.ptrPipeline);
        stream.boundShader = cmd->shader;
    }

    void VKBackend::CMD_DrawInstanced(uint8* data, VKBCommandStream& stream)
    {
        CMDDrawInstanced* cmd    = reinterpret_cast<CMDDrawInstanced*>(data);
        auto              buffer = stream.buffer;
        vkCmdDraw(buffer, cmd->vertexCountPerInstance, cmd->instanceCount, cmd->startVertexLocation, cmd->startInstanceLocation);
    }

    void VKBackend::CMD_DrawIndexedInstanced(uint8* data, VKBCommandStream& stream)
    {
        CMDDrawIndexedInstanced* cmd    = reinterpret_cast<CMDDrawIndexedInstanced*>(data);
        auto                     buffer = stream.buffer;
        vkCmdDrawIndexed(buffer, cmd->indexCountPerInstance, cmd->instanceCount, cmd->startIndexLocation, cmd->baseVertexLocation, cmd->startInstanceLocation);
    }

    void VKBackend::CMD_BindVertexBuffers(uint8* data, VKBCommandStream& stream)
    {
        CMDBindVertexBuffers* cmd    = reinterpret_cast<CMDBindVertexBuffers*>(data);
        auto                  buffer = stream.buffer;
        const auto&           res    = m_resources.GetItemR(cmd->resource);
        vkCmdBindVertexBuffers(buffer, cmd->slot, 1, &res.buffer, &cmd->offset);
    }

    void VKBackend::CMD_BindIndexBuffers(uint8* data, VKBCommandStream& stream)
    {
        CMDBindIndexBuffers* cmd    = reinterpret_cast<CMDBindIndexBuffers*>(data);
        auto                 buffer = stream.buffer;
        const auto&          res    = m_resources.GetItemR(cmd->resource);
        vkCmdBindIndexBuffer(buffer, res.buffer, cmd->offset, GetVKIndexType(cmd->indexFormat));
    }

    void VKBackend::CMD_CopyResource(uint8* data, VKBCommandStream& stream)
    {
        CMDCopyResource* cmd    = reinterpret_cast<CMDCopyResource*>(data);
        auto             buffer = stream.buffer;
        const auto&      srcRes = m_resources.GetItemR(cmd->source);
        const auto&      dstRes = m_resources.GetItemR(cmd->destination);

        VkBufferCopy region = VkBufferCopy{};
        region.size         = srcRes.size;
        region.srcOffset    = 0;
        region.dstOffset    = 0;

        vkCmdCopyBuffer(buffer, srcRes.buffer, dstRes.buffer, 1, &region);
    }

    void VKBackend::CMD_CopyBufferToTexture2D(uint8* data, VKBCommandStream& stream)
    {
        CMDCopyBufferToTexture2D* cmd    = reinterpret_cast<CMDCopyBufferToTexture2D*>(data);
        auto                      buffer = stream.buffer;

        uint32 totalDataSize = cmd->buffers[0].width * cmd->buffers[0].height * cmd->buffers[0].bytesPerPixel;
        for (uint32 i = 1; i < cmd->mipLevels; i++)
        {
            const auto& buf = cmd->buffers[i];
            totalDataSize += buf.width * buf.height * buf.bytesPerPixel;
        }

        ResourceDesc stagingDesc  = {};
        stagingDesc.size          = totalDataSize;
        stagingDesc.typeHintFlags = TH_None;
        stagingDesc.heapType      = ResourceHeap::StagingHeap;
        uint32 stagingHandle      = CreateResource(stagingDesc);
        stream.intermediateResources.push_back(stagingHandle);

        const auto& srcResource = m_resources.GetItemR(stagingHandle);
        const auto& dstTexture  = m_texture2Ds.GetItemR(cmd->destTexture);

        LINAGX_VEC<VkBufferImageCopy> regions;

        VkOffset3D imageOffset = {};
        imageOffset.x = imageOffset.y = imageOffset.z = 0;

        uint32 bufferOffset = 0;

        uint8* mapped = nullptr;
        MapResource(stagingHandle, mapped);

        for (uint32 i = 0; i < cmd->mipLevels; i++)
        {
            const auto& txtBuffer = cmd->buffers[i];

            VkExtent3D extent = {};
            extent.width      = txtBuffer.width;
            extent.height     = txtBuffer.height;
            extent.depth      = 1;

            VkImageSubresourceLayers subresLayers = {};
            subresLayers.aspectMask               = dstTexture.usage == Texture2DUsage::DepthStencilTexture ? (GetVKAspectFlags(dstTexture.depthStencilAspect)) : VK_IMAGE_ASPECT_COLOR_BIT;
            subresLayers.mipLevel                 = i;
            subresLayers.baseArrayLayer           = 0;
            subresLayers.layerCount               = 1;

            VkBufferImageCopy copy = VkBufferImageCopy{};
            copy.bufferOffset      = bufferOffset;
            copy.bufferRowLength   = 0;
            copy.bufferImageHeight = 0;
            copy.imageSubresource  = subresLayers;
            copy.imageOffset       = imageOffset;
            copy.imageExtent       = extent;

            regions.push_back(copy);

            const uint32 totalSz = txtBuffer.width * txtBuffer.height * txtBuffer.bytesPerPixel;
            std::memcpy(mapped + bufferOffset, txtBuffer.pixels, static_cast<size_t>(totalSz));
            bufferOffset += totalSz;
        }

        UnmapResource(stagingHandle);

        TransitionImageLayout(buffer, dstTexture.img, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmd->mipLevels);
        vkCmdCopyBufferToImage(buffer, srcResource.buffer, dstTexture.img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(regions.size()), regions.data());
        TransitionImageLayout(buffer, dstTexture.img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmd->mipLevels);
    }

    void VKBackend::CMD_BindDescriptorSets(uint8* data, VKBCommandStream& stream)
    {
        CMDBindDescriptorSets* cmd    = reinterpret_cast<CMDBindDescriptorSets*>(data);
        auto                   buffer = stream.buffer;
        const auto&            shader = m_shaders.GetItemR(stream.boundShader);

        LINAGX_VEC<VkDescriptorSet> sets;
        sets.resize(cmd->setCount);

        for (uint32 i = 0; i < cmd->setCount; i++)
            sets[i] = m_descriptorSets.GetItemR(cmd->descriptorSetHandles[i]).ptr;

        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.ptrLayout, cmd->firstSet, cmd->setCount, sets.data(), 0, nullptr);
    }

    void VKBackend::CMD_BindConstants(uint8* data, VKBCommandStream& stream)
    {
        CMDBindConstants* cmd    = reinterpret_cast<CMDBindConstants*>(data);
        auto              buffer = stream.buffer;
        const auto&       shader = m_shaders.GetItemR(stream.boundShader);

        uint32 stageFlags = 0;

        for (uint32 i = 0; i < cmd->stagesSize; i++)
            stageFlags |= GetVKShaderStage(cmd->stages[i]);

        vkCmdPushConstants(buffer, shader.ptrLayout, stageFlags, cmd->offset, cmd->size, cmd->data);
    }

    void VKBackend::TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32 mipLevels)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = oldLayout;
        barrier.newLayout                       = newLayout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if ((oldLayout == VK_IMAGE_LAYOUT_UNDEFINED || oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = 0;
            sourceStage           = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            destinationStage      = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage           = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            sourceStage           = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else
        {
            LOGA(false, "Backend -> Unsupported transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

} // namespace LinaGX
