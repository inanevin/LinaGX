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
#include "LinaGX/Core/Instance.hpp"
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
        case Format::R16G16B16A16_UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;
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

    VkStencilOp GetVKStencilOp(StencilOp op)
    {
        switch (op)
        {
        case StencilOp::Keep:
            return VkStencilOp::VK_STENCIL_OP_KEEP;
        case StencilOp::Zero:
            return VkStencilOp::VK_STENCIL_OP_ZERO;
        case StencilOp::Replace:
            return VkStencilOp::VK_STENCIL_OP_REPLACE;
        case StencilOp::IncrementClamp:
            return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOp::DecrementClamp:
            return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOp::Invert:
            return VkStencilOp::VK_STENCIL_OP_INVERT;
        case StencilOp::IncrementWrap:
            return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOp::DecrementWrap:
            return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            return VK_STENCIL_OP_KEEP;
        }
    }

    VkAttachmentLoadOp GetVKLoadOp(LoadOp op)
    {
        switch (op)
        {
        case LoadOp::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOp::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOp::DontCare:
        case LoadOp::None:
        default:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
    }

    VkAttachmentStoreOp GetVKStoreOp(StoreOp op)
    {
        switch (op)
        {
        case StoreOp::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case StoreOp::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        case StoreOp::None:
        default:
            return VK_ATTACHMENT_STORE_OP_NONE;
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

    VkDescriptorType GetVKDescriptorType(DescriptorType type, bool dynamic)
    {
        switch (type)
        {
        case LinaGX::DescriptorType::CombinedImageSampler:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case LinaGX::DescriptorType::SeparateSampler:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case LinaGX::DescriptorType::SeparateImage:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case LinaGX::DescriptorType::SSBO: {
            if (dynamic)
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            else
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
        case LinaGX::DescriptorType::UBO: {
            if (dynamic)
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            else
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        default: {
            LOGA(false, "Can not use!");
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
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
        VkResult res                 = vkWaitSemaphores(m_device, &waitInfo, timeout);
        VK_CHECK_RESULT(res, "Backend -> Failed waiting for semaphores!");
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

        VkSemaphoreCreateInfo smpInfo = VkSemaphoreCreateInfo{};
        smpInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        smpInfo.pNext                 = nullptr;
        smpInfo.flags                 = 0;

        swp.imageAcquiredSemaphores.resize(m_initInfo.framesInFlight);
        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            VkResult result = vkCreateSemaphore(m_device, &smpInfo, m_allocator, &swp.imageAcquiredSemaphores[i]);
            VK_CHECK_RESULT(result, "Failed creating semaphore.");
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

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
            vkDestroySemaphore(m_device, swp.imageAcquiredSemaphores[i], m_allocator);

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
        if (desc.width == 0 || desc.height == 0)
            return;

        Join();

        auto& swp       = m_swapchains.GetItemR(desc.swapchain);
        swp.width       = desc.width;
        swp.height      = desc.height;
        swp.presentMode = GetVKPresentMode(desc.vsyncMode);

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

        std::vector<VkImage>     imgs  = vkbSwapchain.get_images().value();
        std::vector<VkImageView> views = vkbSwapchain.get_image_views().value();
        swp.imgs.clear();
        swp.views.clear();

        for (VkImage img : imgs)
        {
            swp.imgs.push_back(img);
        }

        for (VkImageView view : views)
            swp.views.push_back(view);
    }

    void VKBackend::SetSwapchainActive(uint8 swp, bool isActive)
    {
        auto& swap    = m_swapchains.GetItemR(swp);
        swap.isActive = isActive;
    }

    bool VKBackend::CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob)
    {
        LOGA(false, "!!");
        return false;
    }

    bool CompareBindings(const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
    {
        return a.binding < b.binding;
    }

    uint16 VKBackend::CreateShader(const ShaderDesc& shaderDesc)
    {
        VKBShader shader = {};

        for (const auto& [stage, blob] : shaderDesc.stages)
        {
            if (stage == ShaderStage::Compute)
            {
                if (shaderDesc.stages.size() == 1)
                {
                    shader.isCompute = true;
                    break;
                }
                else
                {
                    LOGA(false, "Shader contains a compute stage but that is not the only stage, which is forbidden!");
                }
            }
        }

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

        auto getPerStageMaxDescriptors = [this](DescriptorType type) {
            switch (type)
            {
            case DescriptorType::UBO:
                return m_gpuProperties.limits.maxPerStageDescriptorSamplers;
            case DescriptorType::SSBO:
                return m_gpuProperties.limits.maxPerStageDescriptorStorageBuffers;
            case DescriptorType::CombinedImageSampler:
                return m_gpuProperties.limits.maxPerStageDescriptorSamplers;
            case DescriptorType::SeparateSampler:
                return m_gpuProperties.limits.maxPerStageDescriptorSamplers;
            case DescriptorType::SeparateImage:
                return m_gpuProperties.limits.maxPerStageDescriptorSampledImages;
            }
        };

        shader.layouts.resize(shaderDesc.layout.descriptorSetLayouts.size());

        uint32 set = 0;
        for (const auto& descriptorSet : shaderDesc.layout.descriptorSetLayouts)
        {
            LINAGX_VEC<VkDescriptorSetLayoutBinding> vkBindings;
            vkBindings.reserve(descriptorSet.bindings.size());

            bool                                 containsBindless = false;
            LINAGX_VEC<VkDescriptorBindingFlags> bindlessFlags;

            for (const auto& binding : descriptorSet.bindings)
            {
                VkDescriptorSetLayoutBinding vkBinding = VkDescriptorSetLayoutBinding{};
                vkBinding.binding                      = binding.binding;
                vkBinding.descriptorType               = GetVKDescriptorType(binding.type, false);
                vkBinding.descriptorCount              = binding.descriptorCount == 0 ? getPerStageMaxDescriptors(binding.type) : binding.descriptorCount;

                for (ShaderStage stg : binding.stages)
                    vkBinding.stageFlags |= GetVKShaderStage(stg);

                vkBindings.push_back(vkBinding);

                if (vkBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE && vkBinding.descriptorCount == m_gpuProperties.limits.maxPerStageDescriptorSampledImages)
                {
                    containsBindless = true;
                    bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
                }
                else if (vkBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER && vkBinding.descriptorCount == m_gpuProperties.limits.maxPerStageDescriptorSamplers)
                {
                    containsBindless = true;
                    bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
                }
                else if (vkBinding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && vkBinding.descriptorCount == m_gpuProperties.limits.maxPerStageDescriptorSamplers)
                {
                    containsBindless = true;
                    bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
                }
                else
                    bindlessFlags.push_back(0);
            }

            VkDescriptorSetLayoutCreateInfo setInfo = VkDescriptorSetLayoutCreateInfo{};
            setInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setInfo.flags                           = 0;
            setInfo.bindingCount                    = static_cast<uint32>(vkBindings.size());
            setInfo.pBindings                       = vkBindings.data();

            VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extInfo;
            extInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
            extInfo.pBindingFlags = bindlessFlags.data();
            extInfo.bindingCount  = setInfo.bindingCount;
            extInfo.pNext         = nullptr;

            if (containsBindless)
            {
                setInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
                setInfo.pNext = &extInfo;
                LOGA(m_initInfo.gpuFeatures.enableBindless, "Enable bindless feature in InitInfo to use bindless descriptors!");
            }

            VkDescriptorSetLayout layout = nullptr;
            VkResult              res    = vkCreateDescriptorSetLayout(m_device, &setInfo, m_allocator, &layout);
            VK_CHECK_RESULT(res, "Failed creating descriptor set layout.");

            LOGA(set < static_cast<uint32>(shader.layouts.size()), "Set index is bigger than or equal to shader layouts size, are you sure you setup your sets correctly?");
            shader.layouts[set] = layout;
            set++;
        }

        // Push constants
        LINAGX_VEC<VkPushConstantRange> constants;

        uint32 offset = 0;
        for (const auto& ct : shaderDesc.layout.constants)
        {
            VkPushConstantRange range = VkPushConstantRange{};
            range.size                = static_cast<uint32>(ct.size);
            range.offset              = offset;
            offset += range.size;

            for (ShaderStage stg : ct.stages)
                range.stageFlags |= GetVKShaderStage(stg);

            constants.push_back(range);
        }

        // Misc state
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = VkPipelineInputAssemblyStateCreateInfo{};
        inputAssembly.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.pNext                                  = nullptr;
        inputAssembly.topology                               = GetVKTopology(shaderDesc.topology);
        inputAssembly.primitiveRestartEnable                 = VK_FALSE;
        VkPipelineViewportStateCreateInfo viewportState      = VkPipelineViewportStateCreateInfo{};
        viewportState.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext                                  = nullptr;
        viewportState.viewportCount                          = 1;
        viewportState.pViewports                             = nullptr;
        viewportState.scissorCount                           = 1;
        viewportState.pScissors                              = nullptr;
        VkPipelineRasterizationStateCreateInfo raster        = VkPipelineRasterizationStateCreateInfo{};
        raster.sType                                         = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        raster.pNext                                         = nullptr;
        raster.depthClampEnable                              = VK_FALSE;
        raster.rasterizerDiscardEnable                       = VK_FALSE;
        raster.polygonMode                                   = GetVKPolygonMode(shaderDesc.polygonMode);
        raster.cullMode                                      = GetVKCullMode(shaderDesc.cullMode);
        raster.frontFace                                     = GetVKFrontFace(shaderDesc.frontFace);
        raster.depthBiasEnable                               = VK_FALSE;
        raster.depthBiasConstantFactor                       = 0.0f;
        raster.depthBiasClamp                                = 0.0f;
        raster.depthBiasSlopeFactor                          = 0.0f;
        raster.lineWidth                                     = 1.0f;
        VkPipelineMultisampleStateCreateInfo msaa            = VkPipelineMultisampleStateCreateInfo{};
        msaa.sType                                           = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        msaa.pNext                                           = nullptr;
        msaa.rasterizationSamples                            = VK_SAMPLE_COUNT_1_BIT;
        msaa.sampleShadingEnable                             = VK_FALSE;
        msaa.minSampleShading                                = 1.0f;
        msaa.pSampleMask                                     = nullptr;
        msaa.alphaToCoverageEnable                           = VK_FALSE;
        msaa.alphaToOneEnable                                = VK_FALSE;
        VkPipelineDepthStencilStateCreateInfo depthStencil   = VkPipelineDepthStencilStateCreateInfo{};
        depthStencil.sType                                   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.pNext                                   = nullptr;
        depthStencil.depthTestEnable                         = shaderDesc.depthStencilDesc.depthTest ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable                        = shaderDesc.depthStencilDesc.depthWrite ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp                          = GetVKCompareOp(shaderDesc.depthStencilDesc.depthCompare);
        depthStencil.depthBoundsTestEnable                   = VK_FALSE;
        depthStencil.minDepthBounds                          = 0.0f;
        depthStencil.maxDepthBounds                          = 1.0f;
        depthStencil.stencilTestEnable                       = shaderDesc.depthStencilDesc.stencilEnabled ? VK_TRUE : VK_FALSE;
        depthStencil.back.compareMask                        = shaderDesc.depthStencilDesc.stencilCompareMask;
        depthStencil.back.writeMask                          = shaderDesc.depthStencilDesc.stencilWriteMask;
        depthStencil.back.compareOp                          = GetVKCompareOp(shaderDesc.depthStencilDesc.backStencilState.compareOp);
        depthStencil.back.depthFailOp                        = GetVKStencilOp(shaderDesc.depthStencilDesc.backStencilState.depthFailOp);
        depthStencil.back.failOp                             = GetVKStencilOp(shaderDesc.depthStencilDesc.backStencilState.failOp);
        depthStencil.back.passOp                             = GetVKStencilOp(shaderDesc.depthStencilDesc.backStencilState.passOp);
        depthStencil.front.compareMask                       = shaderDesc.depthStencilDesc.stencilCompareMask;
        depthStencil.front.writeMask                         = shaderDesc.depthStencilDesc.stencilWriteMask;
        depthStencil.front.compareOp                         = GetVKCompareOp(shaderDesc.depthStencilDesc.frontStencilState.compareOp);
        depthStencil.front.depthFailOp                       = GetVKStencilOp(shaderDesc.depthStencilDesc.frontStencilState.depthFailOp);
        depthStencil.front.failOp                            = GetVKStencilOp(shaderDesc.depthStencilDesc.frontStencilState.failOp);
        depthStencil.front.passOp                            = GetVKStencilOp(shaderDesc.depthStencilDesc.frontStencilState.passOp);

        LINAGX_VEC<VkFormat>                            colorAttachmentFormats;
        LINAGX_VEC<VkPipelineColorBlendAttachmentState> blendAttachments;
        colorAttachmentFormats.reserve(shaderDesc.colorAttachments.size());
        blendAttachments.reserve(shaderDesc.colorAttachments.size());

        for (const auto& attachment : shaderDesc.colorAttachments)
        {
            VkPipelineColorBlendAttachmentState colorBlendAttachment = VkPipelineColorBlendAttachmentState{};
            colorBlendAttachment.blendEnable                         = attachment.blendAttachment.blendEnabled ? VK_TRUE : VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor                 = GetVKBlendFactor(attachment.blendAttachment.srcColorBlendFactor);
            colorBlendAttachment.dstColorBlendFactor                 = GetVKBlendFactor(attachment.blendAttachment.dstColorBlendFactor);
            colorBlendAttachment.colorBlendOp                        = GetVKBlendOp(attachment.blendAttachment.colorBlendOp);
            colorBlendAttachment.srcAlphaBlendFactor                 = GetVKBlendFactor(attachment.blendAttachment.srcAlphaBlendFactor);
            colorBlendAttachment.dstAlphaBlendFactor                 = GetVKBlendFactor(attachment.blendAttachment.dstAlphaBlendFactor);
            colorBlendAttachment.alphaBlendOp                        = GetVKBlendOp(attachment.blendAttachment.alphaBlendOp);
            colorBlendAttachment.colorWriteMask                      = 0;

            for (auto flag : attachment.blendAttachment.componentFlags)
                colorBlendAttachment.colorWriteMask |= GetVKColorComponentFlags(flag);

            colorAttachmentFormats.push_back(GetVKFormat(attachment.format));
            blendAttachments.push_back(colorBlendAttachment);
        }

        VkPipelineColorBlendStateCreateInfo colorBlending = VkPipelineColorBlendStateCreateInfo{};
        colorBlending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext                               = nullptr;
        colorBlending.logicOpEnable                       = shaderDesc.blendLogicOpEnabled ? VK_TRUE : VK_FALSE;
        colorBlending.logicOp                             = GetVKLogicOp(shaderDesc.blendLogicOp);
        colorBlending.attachmentCount                     = static_cast<uint32>(blendAttachments.size());
        colorBlending.pAttachments                        = blendAttachments.data();

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

        VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {};
        pipelineRenderingCreateInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipelineRenderingCreateInfo.colorAttachmentCount             = static_cast<uint32>(colorAttachmentFormats.size());
        pipelineRenderingCreateInfo.pColorAttachmentFormats          = colorAttachmentFormats.data();
        pipelineRenderingCreateInfo.depthAttachmentFormat            = GetVKFormat(shaderDesc.depthStencilDesc.depthStencilAttachmentFormat);

        // Compute only pipeline.
        if (shaderDesc.stages.size() == 1 && shaderDesc.stages.begin()->first == ShaderStage::Compute)
        {
            VkComputePipelineCreateInfo computeInfo = {};
            computeInfo.sType                       = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            computeInfo.pNext                       = nullptr;
            computeInfo.stage                       = shaderStages[0];
            computeInfo.layout                      = shader.ptrLayout;
            computeInfo.basePipelineHandle          = VK_NULL_HANDLE;
            res                                     = vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computeInfo, m_allocator, &shader.ptrPipeline);
        }
        else
        {
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
        }

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
        item.arrayLength        = txtDesc.arrayLength;

        VkImageCreateInfo imgCreateInfo = VkImageCreateInfo{};
        imgCreateInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imgCreateInfo.pNext             = nullptr;
        imgCreateInfo.imageType         = VK_IMAGE_TYPE_2D;
        imgCreateInfo.format            = GetVKFormat(txtDesc.format);
        imgCreateInfo.extent            = VkExtent3D{txtDesc.width, txtDesc.height, 1};
        imgCreateInfo.mipLevels         = txtDesc.mipLevels;
        imgCreateInfo.arrayLayers       = txtDesc.arrayLength;
        imgCreateInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
        imgCreateInfo.tiling            = txtDesc.usage == Texture2DUsage::ColorTextureDynamic ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
        imgCreateInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        imgCreateInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;

        if (txtDesc.usage == Texture2DUsage::DepthStencilTexture)
            imgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        else if (txtDesc.usage == Texture2DUsage::ColorTextureRenderTarget)
            imgCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        else
            imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (txtDesc.sampled)
            imgCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

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
        subResRange.layerCount              = txtDesc.arrayLength > 1 ? txtDesc.arrayLength : 1;

        VkImageViewCreateInfo viewInfo = VkImageViewCreateInfo{};
        viewInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext                 = nullptr;
        viewInfo.image                 = item.img;
        viewInfo.viewType              = txtDesc.arrayLength > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
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
        if (desc.typeHintFlags & TH_IndirectBuffer)
            bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
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
        info.anisotropyEnable    = m_supportsAnisotropy && desc.anisotropy != 0;
        info.maxAnisotropy       = static_cast<float>(desc.anisotropy);
        info.minLod              = desc.minLod;
        info.maxLod              = desc.maxLod;
        info.borderColor         = GetVKBorderColor(desc.borderColor);

        VkResult res = vkCreateSampler(m_device, &info, m_allocator, &item.ptr);
        VK_CHECK_RESULT(res, "Backend -> Could not create sampler!");
        VK_NAME_OBJECT(item.ptr, VK_OBJECT_TYPE_SAMPLER, desc.debugName, sinfo);
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
        item.bindings         = desc.bindings;

        LINAGX_VEC<VkDescriptorSetLayoutBinding> bindings;
        bool                                     containsBindlessTextures = false;
        LINAGX_VEC<VkDescriptorBindingFlags>     bindlessFlags;

        const uint32 bindingsCount = static_cast<uint32>(desc.bindings.size());

        for (uint32 i = 0; i < bindingsCount; i++)
        {
            const DescriptorBinding&     binding   = desc.bindings[i];
            VkDescriptorSetLayoutBinding vkBinding = {};
            vkBinding.binding                      = i;
            vkBinding.descriptorType               = GetVKDescriptorType(binding.type, binding.useDynamicOffset);
            vkBinding.descriptorCount              = binding.descriptorCount;

            if (binding.type == DescriptorType::SeparateImage && binding.unbounded)
            {
                containsBindlessTextures = true;
                bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
                vkBinding.descriptorCount = m_gpuProperties.limits.maxPerStageDescriptorSampledImages;
            }
            else if (binding.type == DescriptorType::SeparateSampler && binding.unbounded)
            {
                containsBindlessTextures = true;
                bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
                vkBinding.descriptorCount = m_gpuProperties.limits.maxPerStageDescriptorSamplers;
            }
            else if (binding.type == DescriptorType::CombinedImageSampler && binding.unbounded)
            {
                containsBindlessTextures = true;
                bindlessFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
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
            layoutInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
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
        auto& item = m_descriptorSets.GetItemR(desc.setHandle);
        LOGA(desc.binding < static_cast<uint32>(item.bindings.size()), "Backend -> Binding is not valid!");
        auto&        bindingData     = item.bindings[desc.binding];
        const uint32 descriptorCount = static_cast<uint32>(desc.buffers.size());
        LOGA(descriptorCount <= bindingData.descriptorCount, "Backend -> Error updating descriptor buffer as update count exceeds the maximum descriptor count for given binding!");
        LOGA(bindingData.type == DescriptorType::UBO || bindingData.type == DescriptorType::SSBO, "Backend -> You can only use DescriptorUpdateBuffer with descriptors of type UBO and SSBO! Use DescriptorUpdateImage()");

        LINAGX_VEC<VkDescriptorBufferInfo> bufferInfos;

        for (uint32 i = 0; i < descriptorCount; i++)
        {
            const auto&            res   = m_resources.GetItemR(desc.buffers[i]);
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
        write.descriptorCount      = descriptorCount;
        write.descriptorType       = GetVKDescriptorType(bindingData.type, bindingData.useDynamicOffset);
        write.pBufferInfo          = bufferInfos.data();
        vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
    }

    void VKBackend::DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc)
    {
        auto& item = m_descriptorSets.GetItemR(desc.setHandle);
        LOGA(desc.binding < static_cast<uint32>(item.bindings.size()), "Backend -> Binding is not valid!");
        auto&        bindingData        = item.bindings[desc.binding];
        const uint32 txtDescriptorCount = static_cast<uint32>(desc.textures.size());
        const uint32 smpDescriptorCount = static_cast<uint32>(desc.samplers.size());
        LOGA(txtDescriptorCount <= bindingData.descriptorCount && smpDescriptorCount <= bindingData.descriptorCount, "Backend -> Error updateing descriptor buffer as update count exceeds the maximum descriptor count for given binding!");
        LOGA(bindingData.type == DescriptorType::CombinedImageSampler || bindingData.type == DescriptorType::SeparateSampler || bindingData.type == DescriptorType::SeparateImage, "Backend -> You can only use DescriptorUpdateImage with descriptors of type combined image sampler, separate image or separate sampler! Use DescriptorUpdateBuffer()");

        LINAGX_VEC<VkDescriptorImageInfo> imgInfos;

        VkDescriptorType descriptorType = GetVKDescriptorType(bindingData.type, bindingData.useDynamicOffset);

        uint32 usedCount = 0;
        if (bindingData.type == DescriptorType::CombinedImageSampler || bindingData.type == DescriptorType::SeparateImage)
            usedCount = txtDescriptorCount;
        else
            usedCount = smpDescriptorCount;

        for (uint32 i = 0; i < usedCount; i++)
        {
            VkDescriptorImageInfo imgInfo = VkDescriptorImageInfo{};

            if (descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
                imgInfo.sampler = m_samplers.GetItemR(desc.samplers[i]).ptr;

            if (descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
            {
                imgInfo.imageView   = m_texture2Ds.GetItemR(desc.textures[i]).imgView;
                imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            imgInfos.push_back(imgInfo);
        }

        VkWriteDescriptorSet write = {};
        write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext                = nullptr;
        write.dstSet               = m_descriptorSets.GetItemR(desc.setHandle).ptr;
        write.dstBinding           = desc.binding;
        write.descriptorCount      = usedCount;
        write.descriptorType       = descriptorType;
        write.pImageInfo           = imgInfos.data();
        vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
    }

    uint16 VKBackend::CreatePipelineLayout(const PipelineLayoutDesc& desc)
    {
        VKBPipelineLayout item = {};
        item.isValid           = true;

        LINAGX_VEC<VkDescriptorSetLayout> setLayouts;
        LINAGX_VEC<VkPushConstantRange>   constants;

        setLayouts.reserve(desc.descriptorSets.size());
        for (auto set : desc.descriptorSets)
        {
            auto& dcSet = m_descriptorSets.GetItemR(set);
            setLayouts.push_back(dcSet.layout);
        }

        uint32 offset = 0;

        for (const auto& ct : desc.constantRanges)
        {
            VkPushConstantRange range = VkPushConstantRange{};
            range.size                = static_cast<uint32>(ct.size);
            range.offset              = offset;
            offset += range.size;

            for (ShaderStage stg : ct.stages)
                range.stageFlags |= GetVKShaderStage(stg);

            constants.push_back(range);
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = VkPipelineLayoutCreateInfo{};
        pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pNext                      = nullptr;
        pipelineLayoutInfo.flags                      = 0;
        pipelineLayoutInfo.setLayoutCount             = static_cast<uint32>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts                = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount     = static_cast<uint32>(constants.size());
        pipelineLayoutInfo.pPushConstantRanges        = constants.data();
        VkResult res                                  = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, m_allocator, &item.ptr);

        return m_pipelineLayouts.AddItem(item);
    }

    void VKBackend::DestroyPipelineLayout(uint16 layout)
    {
        auto& lyt = m_pipelineLayouts.GetItemR(layout);
        if (!lyt.isValid)
        {
            LOGE("Backend -> Pipeline Layout to be destroyed is not valid!");
            return;
        }

        vkDestroyPipelineLayout(m_device, lyt.ptr, m_allocator);

        m_pipelineLayouts.RemoveItem(layout);
    }

    uint32 VKBackend::CreateCommandStream(CommandType cmdType)
    {
        VKBCommandStream item = {};
        item.isValid          = true;

        const uint32 familyIndex = m_queueData[cmdType == CommandType::Secondary ? CommandType::Graphics : cmdType].familyIndex;

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
        cmdAllocInfo.level                       = cmdType == CommandType::Secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
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

        for (const auto [id, frame] : stream.intermediateResources)
            DestroyResource(id);

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
                uint8*        data = stream->m_commands[i];
                LINAGX_TYPEID tid  = 0;
                LINAGX_MEMCPY(&tid, data, sizeof(LINAGX_TYPEID));
                const size_t increment = sizeof(LINAGX_TYPEID);
                uint8*       cmd       = data + increment;
                (this->*m_cmdFunctions[tid])(cmd, sr);
            }

            res = vkEndCommandBuffer(buffer);
            VK_CHECK_RESULT(res, "Failed ending command buffer!");
        }
    }

    void VKBackend::SubmitCommandStreams(const SubmitDesc& desc)
    {
        auto& queue    = m_queues.GetItemR(desc.targetQueue);
        auto& queuePfd = queue.pfd[m_currentFrameIndex];
        auto  flag     = m_flagsPerQueue.at(queue.queue);

        LINAGX_VEC<VkSubmitInfo> submitInfos;
        submitInfos.resize(desc.streamCount);

        if (desc.isMultithreaded)
        {
            // spinlock
            while (flag->test_and_set(std::memory_order_acquire))
            {
            }
        }

        queue.wasSubmitted[m_currentFrameIndex] = true;

        auto&                       frame = m_perFrameData[m_currentFrameIndex];
        LINAGX_VEC<VkCommandBuffer> _buffers;
        LINAGX_VEC<uint8>           writesToSwapchains;

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
            LOGA(str.type != CommandType::Secondary, "Backend -> Can not submit command streams of type Secondary directly to the queues! Use CMDExecuteSecondary instead!");

            _buffers.push_back(str.buffer);

            if (!str.swapchainWrites.empty())
                writesToSwapchains.insert(writesToSwapchains.end(), str.swapchainWrites.begin(), str.swapchainWrites.end());

            str.swapchainWrites.clear();
        }

        VkPipelineStageFlags             waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        LINAGX_VEC<VkSemaphore>          waitSemaphores;
        LINAGX_VEC<VkPipelineStageFlags> waitStages;
        LINAGX_VEC<VkSemaphore>          signalSemaphores;
        LINAGX_VEC<uint64>               waitSemaphoreValues;
        LINAGX_VEC<uint64>               signalSemaphoreValues;

        if (queue.type == CommandType::Compute && (m_supportsDedicatedComputeQueue || m_supportsSeparateComputeQueue))
            waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        else if (queue.type == CommandType::Transfer && (m_supportsDedicatedTransferQueue || m_supportsSeparateTransferQueue))
            waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        // If its a graphics queue, we will have to wait on it during StartFrame() for the next frame-in-flight availability.
        // Thus, we need to signal its stored semaphore value.
        if (queue.type == CommandType::Graphics)
        {
            queue.frameSemaphoreValue++;
            queuePfd.storedStartFrameSemaphoreValue = queue.frameSemaphoreValue;
            signalSemaphores.push_back(queuePfd.startFrameWaitSemaphore);
            signalSemaphoreValues.push_back(queuePfd.storedStartFrameSemaphoreValue);
        }

        // If graphics queue, we need to signal a binary semaphore, so that we can wait on it during presentation.
        // Also need to wait for all images to be acquired.
        if (queue.type == CommandType::Graphics && !writesToSwapchains.empty())
        {
            for (auto swp : writesToSwapchains)
            {
                auto& swap = m_swapchains.GetItemR(swp);

                waitSemaphores.push_back(swap.imageAcquiredSemaphores[m_currentFrameIndex]);
                waitStages.push_back(waitStage);
                waitSemaphoreValues.push_back(0); // ignored binary semaphore.
                swap._submittedQueue          = desc.targetQueue;
                swap._submittedSemaphoreIndex = queuePfd.submitSemaphoreIndex;
            }

            signalSemaphores.push_back(queuePfd.submitSemaphoreBuffer[queuePfd.submitSemaphoreIndex]);
            signalSemaphoreValues.push_back(0);
            queuePfd.submitSemaphoreIndex++;
        }

        // This is for USER signal mechanisms.
        {
            if (desc.useWait)
            {
                for (uint32 i = 0; i < desc.waitCount; i++)
                {
                    waitSemaphores.push_back(m_userSemaphores.GetItem(desc.waitSemaphores[i]).ptr);
                    waitSemaphoreValues.push_back(desc.waitValues[i]);
                    waitStages.push_back(waitStage);
                }
            }

            if (desc.useSignal)
            {
                for (uint32 i = 0; i < desc.signalCount; i++)
                {
                    signalSemaphores.push_back(m_userSemaphores.GetItem(desc.signalSemaphores[i]).ptr);
                    signalSemaphoreValues.push_back(desc.signalValues[i]);
                }
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

        LOGA((frame.submissionCount < m_initInfo.gpuLimits.maxSubmitsPerFrame + 1), "Backend -> Exceeded maximum submissions per frame! Please increase the limit.");

        VkResult res = vkQueueSubmit(queue.queue, 1, &submitInfo, nullptr);

        if (desc.isMultithreaded)
            flag->clear(std::memory_order_release);

        VK_CHECK_RESULT(res, "Failed submitting to queue!");
    }

    uint8 VKBackend::CreateQueue(const QueueDesc& desc)
    {
        VkQueue targetQueue = nullptr;

        if (desc.type == CommandType::Transfer)
            targetQueue = m_queueData[CommandType::Transfer].queues[0];
        else if (desc.type == CommandType::Compute)
            targetQueue = m_queueData[CommandType::Compute].queues[0];
        else
        {
            auto& gfx = m_queueData[CommandType::Graphics];

            if (gfx.createRequestCount < static_cast<uint32>(gfx.queues.size()))
            {
                targetQueue = gfx.queues[gfx.createRequestCount];
                gfx.createRequestCount++;
            }
            else
            {
                targetQueue = gfx.queues.back();
            }
        }

        VKBQueue item;
        item.isValid = true;
        item.type    = desc.type;

        VkSemaphoreTypeCreateInfo timelineCreateInfo;
        timelineCreateInfo.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        timelineCreateInfo.pNext         = NULL;
        timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        timelineCreateInfo.initialValue  = 0;

        VkSemaphoreCreateInfo info = VkSemaphoreCreateInfo{};
        info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        info.pNext                 = &timelineCreateInfo;
        info.flags                 = 0;

        item.pfd.resize(m_initInfo.framesInFlight);

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            info.pNext   = &timelineCreateInfo;
            auto&    pfd = item.pfd[i];
            VkResult res = vkCreateSemaphore(m_device, &info, m_allocator, &pfd.startFrameWaitSemaphore);
            VK_CHECK_RESULT(res, "Failed creating semaphore.");

            info.pNext                     = nullptr;
            const uint32 submitSemaphoreSz = m_initInfo.gpuLimits.maxSubmitsPerFrame / 2;
            pfd.submitSemaphoreBuffer.resize(submitSemaphoreSz);

            for (uint32 j = 0; j < submitSemaphoreSz; j++)
            {
                res = vkCreateSemaphore(m_device, &info, m_allocator, &pfd.submitSemaphoreBuffer[j]);
                VK_CHECK_RESULT(res, "Failed creating semaphore.");
            }
        }

        item.queue = targetQueue;
        item.wasSubmitted.resize(m_initInfo.framesInFlight);

        return m_queues.AddItem(item);
    }

    void VKBackend::DestroyQueue(uint8 queue)
    {
        auto& item = m_queues.GetItemR(queue);
        if (!item.isValid)
        {
            LOGE("Backend -> Queue to be destroyed is not valid!");
            return;
        }

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            auto& pfd = item.pfd[i];

            vkDestroySemaphore(m_device, pfd.startFrameWaitSemaphore, m_allocator);

            const uint32 submitSemaphoreSz = m_initInfo.gpuLimits.maxSubmitsPerFrame / 2;
            for (uint32 j = 0; j < submitSemaphoreSz; j++)
                vkDestroySemaphore(m_device, pfd.submitSemaphoreBuffer[j], m_allocator);
        }

        m_queues.RemoveItem(queue);
    }

    uint8 VKBackend::GetPrimaryQueue(CommandType type)
    {
        LOGA(type != CommandType::Secondary, "Backend -> No queues of type Secondary exists, use either Graphics, Transfer or Compute!");
        return m_primaryQueues[type];
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
        features.samplerAnisotropy = true;
        features.multiDrawIndirect = true;

        if (m_initInfo.gpuFeatures.enableBindless)
        {
            features.shaderSampledImageArrayDynamicIndexing = true;
        }

        VkPhysicalDeviceVulkan12Features vk12Features = {};
        vk12Features.timelineSemaphore                = true;

        if (m_initInfo.gpuFeatures.enableBindless)
        {
            vk12Features.runtimeDescriptorArray                        = true;
            vk12Features.descriptorBindingPartiallyBound               = true;
            vk12Features.descriptorBindingSampledImageUpdateAfterBind  = true;
            vk12Features.descriptorBindingStorageBufferUpdateAfterBind = true;
            vk12Features.descriptorBindingUniformBufferUpdateAfterBind = true;
        }

        // Check for feature support
        {
            uint32_t                     deviceCount = 0;
            LINAGX_VEC<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(inst, &deviceCount, nullptr);
            LINAGX_VEC<bool> unsupportState(deviceCount);

            uint32 i = 0;
            for (const auto& device : devices)
            {
                VkPhysicalDeviceFeatures supportedFeatures;
                vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

                VkPhysicalDeviceVulkan12Features supportedVulkan12Features = {};
                supportedVulkan12Features.sType                            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
                VkPhysicalDeviceProperties2 deviceProperties2              = {};
                deviceProperties2.sType                                    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
                deviceProperties2.pNext                                    = &supportedVulkan12Features;
                vkGetPhysicalDeviceProperties2(device, &deviceProperties2);

                if (!supportedFeatures.samplerAnisotropy || !supportedFeatures.multiDrawIndirect || !supportedVulkan12Features.timelineSemaphore)
                    unsupportState[i] = true;

                if (m_initInfo.gpuFeatures.enableBindless)
                {
                    if (!supportedFeatures.shaderSampledImageArrayDynamicIndexing || !supportedVulkan12Features.runtimeDescriptorArray || !supportedVulkan12Features.descriptorBindingPartiallyBound || !supportedVulkan12Features.descriptorBindingSampledImageUpdateAfterBind || !supportedVulkan12Features.descriptorBindingUniformBufferUpdateAfterBind || !supportedVulkan12Features.descriptorBindingStorageBufferUpdateAfterBind)
                        unsupportState[i] = true;
                }

                i++;
            }

            bool foundOne = false;
            for (const auto& s : unsupportState)
            {
                if (!s)
                {
                    foundOne = true;
                    break;
                }
            }

            if (!foundOne)
            {
                LOGA(false, "No devices were found that supports all the features!");
            }
        }

        vkb::PhysicalDeviceSelector      selector{inst};
        vkb::Result<vkb::PhysicalDevice> phyRes = selector.set_minimum_version(LGX_VK_MAJOR, LGX_VK_MINOR)
                                                      .set_required_features_12(vk12Features)
                                                      .defer_surface_initialization()
                                                      .prefer_gpu_device_type(targetDeviceType)
                                                      .allow_any_gpu_device_type(false)
                                                      .set_required_features(features)
                                                      .select(vkb::DeviceSelectionMode::partially_and_fully_suitable);

        vkb::PhysicalDevice physicalDevice;

        // Disable not-crucial features if not supported.
        if (!phyRes.has_value())
        {
            features.multiDrawIndirect  = false;
            m_supportsMultiDrawIndirect = false;
            physicalDevice              = selector.select(vkb::DeviceSelectionMode::partially_and_fully_suitable).value();
        }
        else
        {
            m_supportsMultiDrawIndirect = true;
            physicalDevice              = phyRes.value();
        }

        // create the final Vulkan device
        vkb::DeviceBuilder                           deviceBuilder{physicalDevice};
        VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParamsFeature;
        shaderDrawParamsFeature.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        shaderDrawParamsFeature.pNext                = nullptr;
        shaderDrawParamsFeature.shaderDrawParameters = VK_TRUE;

        VkPhysicalDeviceMultiDrawFeaturesEXT mt;
        mt.sType     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT;
        mt.multiDraw = true;
        deviceBuilder.add_pNext(&mt);

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

        std::vector<VkQueueFamilyProperties>     queueFamilies = physicalDevice.get_queue_families();
        std::vector<vkb::CustomQueueDescription> queueDescs;

        m_supportsDedicatedTransferQueue               = false;
        m_supportsDedicatedComputeQueue                = false;
        m_supportsSeparateTransferQueue                = false;
        m_supportsSeparateComputeQueue                 = false;
        uint32             dedicatedTransferQueueIndex = 0;
        uint32             dedicatedComputeQueueIndex  = 0;
        uint32             separateTransferQueueIndex  = 0;
        uint32             separateComputeQueueIndex   = 0;
        LINAGX_VEC<uint32> graphicsQueueFamilies;
        LINAGX_VEC<uint32> computeQueueFamilies;
        LINAGX_VEC<uint32> transferQueueFamilies;

        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                graphicsQueueFamilies.push_back(i);

            if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                computeQueueFamilies.push_back(i);

            if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                transferQueueFamilies.push_back(i);
        }

        for (const auto& i : transferQueueFamilies)
        {
            if (!(queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !(queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
            {
                m_supportsDedicatedTransferQueue = true;
                dedicatedTransferQueueIndex      = i;
            }

            if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                m_supportsSeparateTransferQueue = true;
                separateTransferQueueIndex      = i;
            }
        }

        for (const auto& i : computeQueueFamilies)
        {
            if (!(queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
            {
                m_supportsDedicatedComputeQueue = true;
                dedicatedComputeQueueIndex      = i;
                break;
            }
            if ((queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                m_supportsSeparateComputeQueue = true;
                separateComputeQueueIndex      = i;
            }
        }

        LINAGX_MAP<uint32, uint32> queueIndicesAndCounts;

        if (m_supportsDedicatedTransferQueue)
            queueIndicesAndCounts[dedicatedTransferQueueIndex] = 1;
        else if (m_supportsSeparateTransferQueue)
            queueIndicesAndCounts[separateTransferQueueIndex]++;
        else
            queueIndicesAndCounts[transferQueueFamilies[0]]++;

        if (m_supportsDedicatedComputeQueue)
            queueIndicesAndCounts[dedicatedComputeQueueIndex] = 1;
        else if (m_supportsSeparateComputeQueue)
            queueIndicesAndCounts[separateComputeQueueIndex]++;
        else
            queueIndicesAndCounts[computeQueueFamilies[0]]++;

        queueIndicesAndCounts[graphicsQueueFamilies[0]] += 1 + m_initInfo.gpuFeatures.extraGraphicsQueueCount;

        for (auto& pair : queueIndicesAndCounts)
        {
            // Set actual counts.
            pair.second = Min(queueFamilies[pair.first].queueCount, pair.second);

            // request.
            queueDescs.push_back(vkb::CustomQueueDescription(pair.first, pair.second, std::vector<float>(pair.second, 1.0f)));
        }

        deviceBuilder.custom_queue_setup(queueDescs);

        vkb::Device vkbDevice = deviceBuilder.build().value();
        m_device              = vkbDevice.device;
        m_gpu                 = physicalDevice.physical_device;

        auto& gfx      = m_queueData[CommandType::Graphics];
        auto& transfer = m_queueData[CommandType::Transfer];
        auto& compute  = m_queueData[CommandType::Compute];

        transfer.queues.resize(1);
        compute.queues.resize(1);

        LINAGX_MAP<uint32, uint32> queueIndicesAndOccupiedQueues;

        if (m_supportsDedicatedTransferQueue)
        {
            vkGetDeviceQueue(m_device, dedicatedTransferQueueIndex, 0, &transfer.queues[0]);
            transfer.familyIndex = dedicatedTransferQueueIndex;
        }
        else if (m_supportsSeparateTransferQueue)
        {
            vkGetDeviceQueue(m_device, separateTransferQueueIndex, 0, &transfer.queues[0]);
            queueIndicesAndOccupiedQueues[separateTransferQueueIndex]++;
            transfer.familyIndex = separateTransferQueueIndex;
        }
        else
        {
            vkGetDeviceQueue(m_device, transferQueueFamilies[0], 0, &transfer.queues[0]);
            queueIndicesAndOccupiedQueues[transferQueueFamilies[0]]++;
            transfer.familyIndex = transferQueueFamilies[0];
        }

        if (m_supportsDedicatedComputeQueue)
        {
            vkGetDeviceQueue(m_device, dedicatedComputeQueueIndex, 0, &compute.queues[0]);
            compute.familyIndex = dedicatedComputeQueueIndex;
        }
        else if (m_supportsSeparateComputeQueue)
        {
            if (queueIndicesAndOccupiedQueues[separateComputeQueueIndex] < queueFamilies[separateComputeQueueIndex].queueCount)
            {
                vkGetDeviceQueue(m_device, separateComputeQueueIndex, queueIndicesAndOccupiedQueues[separateComputeQueueIndex], &compute.queues[0]);
                queueIndicesAndOccupiedQueues[separateComputeQueueIndex]++;
            }
            else
            {
                vkGetDeviceQueue(m_device, separateComputeQueueIndex, queueIndicesAndOccupiedQueues[separateComputeQueueIndex] - 1, &compute.queues[0]);
            }

            compute.familyIndex = separateComputeQueueIndex;
        }
        else
        {
            if (queueIndicesAndOccupiedQueues[computeQueueFamilies[0]] < queueFamilies[computeQueueFamilies[0]].queueCount)
            {
                vkGetDeviceQueue(m_device, computeQueueFamilies[0], queueIndicesAndOccupiedQueues[computeQueueFamilies[0]], &compute.queues[0]);
                queueIndicesAndOccupiedQueues[computeQueueFamilies[0]]++;
            }
            else
            {
                vkGetDeviceQueue(m_device, computeQueueFamilies[0], queueIndicesAndOccupiedQueues[computeQueueFamilies[0]] - 1, &compute.queues[0]);
            }

            compute.familyIndex = computeQueueFamilies[0];
        }

        gfx.queues.resize(m_initInfo.gpuFeatures.extraGraphicsQueueCount + 1);
        gfx.familyIndex = graphicsQueueFamilies[0];

        for (uint32 i = 0; i < m_initInfo.gpuFeatures.extraGraphicsQueueCount + 1; i++)
        {
            if (queueIndicesAndOccupiedQueues[graphicsQueueFamilies[0]] < queueFamilies[graphicsQueueFamilies[0]].queueCount)
            {
                vkGetDeviceQueue(m_device, graphicsQueueFamilies[0], queueIndicesAndOccupiedQueues[graphicsQueueFamilies[0]], &gfx.queues[i]);
                queueIndicesAndOccupiedQueues[graphicsQueueFamilies[0]]++;
            }
            else
            {
                vkGetDeviceQueue(m_device, graphicsQueueFamilies[0], queueIndicesAndOccupiedQueues[graphicsQueueFamilies[0]] - 1, &gfx.queues[i]);
            }
        }

        for (const auto& [type, data] : m_queueData)
        {
            for (auto q : data.queues)
            {
                auto& ptr = m_flagsPerQueue[q];

                if (ptr == nullptr)
                    ptr = new std::atomic_flag();
            }
        }

        g_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(m_device, "vkSetDebugUtilsObjectNameEXT"));

        // Queue support
        {

            QueueDesc descGfx, descTransfer, descCompute;
            descGfx.type                           = CommandType::Graphics;
            descTransfer.type                      = CommandType::Transfer;
            descCompute.type                       = CommandType::Compute;
            descGfx.debugName                      = "Primary Graphics Queue";
            descTransfer.debugName                 = "Primary Transfer Queue";
            descCompute.debugName                  = "Primary Compute Queue";
            m_primaryQueues[CommandType::Graphics] = CreateQueue(descGfx);
            m_primaryQueues[CommandType::Transfer] = CreateQueue(descTransfer);
            m_primaryQueues[CommandType::Compute]  = CreateQueue(descCompute);
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
                    GPUInfo.totalCPUVisibleGPUMemorySize = Max(GPUInfo.totalCPUVisibleGPUMemorySize, gpuMemProps.memoryHeaps[type.heapIndex].size);
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
            for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
            {
                VKBPerFrameData pfd = {};
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
                sizeInfo.type                 = GetVKDescriptorType(dt, false);
                sizeInfo.descriptorCount      = limit;
                sizeInfos.push_back(sizeInfo);
                totalSets += limit;
            }

            VkDescriptorPoolSize sizeInfoUboDynamic = VkDescriptorPoolSize{};
            sizeInfoUboDynamic.type                 = GetVKDescriptorType(DescriptorType::UBO, true);
            sizeInfoUboDynamic.descriptorCount      = limits[DescriptorType::UBO] / 2;
            sizeInfos.push_back(sizeInfoUboDynamic);

            VkDescriptorPoolSize sizeInfoSSBODynamic = VkDescriptorPoolSize{};
            sizeInfoSSBODynamic.type                 = GetVKDescriptorType(DescriptorType::UBO, true);
            sizeInfoSSBODynamic.descriptorCount      = limits[DescriptorType::SSBO] / 2;
            sizeInfos.push_back(sizeInfoSSBODynamic);

            VkDescriptorPoolCreateInfo info = VkDescriptorPoolCreateInfo{};
            info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            info.flags                      = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
            info.maxSets                    = m_initInfo.gpuLimits.maxDescriptorSets;
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
        for (const auto& [q, flag] : m_flagsPerQueue)
            delete flag;

        DestroyQueue(GetPrimaryQueue(CommandType::Graphics));
        DestroyQueue(GetPrimaryQueue(CommandType::Transfer));
        DestroyQueue(GetPrimaryQueue(CommandType::Compute));

        vkDestroyDescriptorPool(m_device, m_descriptorPool, m_allocator);

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            auto& pfd = m_perFrameData[i];
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

        for (auto& q : m_queues)
        {
            LOGA(!q.isValid, "Backend -> Some queues were not destroyed!");
        }

        for (auto& l : m_pipelineLayouts)
        {
            LOGA(!l.isValid, "Backend -> Some pipeline layouts were not destroyed!");
        }

        vmaDestroyAllocator(m_vmaAllocator);
        vkDestroyDevice(m_device, m_allocator);
        vkb::destroy_debug_utils_messenger(m_vkInstance, m_debugMessenger);
        vkDestroyInstance(m_vkInstance, m_allocator);
    }

    void VKBackend::Join()
    {
        const uint64 timeout = static_cast<uint64>(5000000000);

        LINAGX_VEC<VkSemaphore> waitSemaphores;
        LINAGX_VEC<uint64>      waitSemaphoreValues;

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            for (auto& q : m_queues)
            {
                if (!q.isValid || q.type != CommandType::Graphics || q.wasSubmitted[i])
                    continue;

                q.wasSubmitted[i] = false;

                auto sem = q.pfd[i].startFrameWaitSemaphore;

                if (std::find_if(waitSemaphores.begin(), waitSemaphores.end(), [sem](VkSemaphore s) { return s == sem; }) == waitSemaphores.end())
                {
                    waitSemaphores.push_back(sem);
                    waitSemaphoreValues.push_back(q.pfd[i].storedStartFrameSemaphoreValue);
                }
            }
        }

        if (!waitSemaphores.empty())
        {
            VkSemaphoreWaitInfo waitInfo = VkSemaphoreWaitInfo{};
            waitInfo.sType               = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
            waitInfo.pNext               = nullptr;
            waitInfo.flags               = 0;
            waitInfo.semaphoreCount      = static_cast<uint32>(waitSemaphores.size());
            waitInfo.pSemaphores         = waitSemaphores.data();
            waitInfo.pValues             = waitSemaphoreValues.data();
            vkWaitSemaphores(m_device, &waitInfo, timeout);
        }

        vkDeviceWaitIdle(m_device);
    }

    void VKBackend::StartFrame(uint32 frameIndex)
    {
        m_currentFrameIndex = frameIndex;
        auto& frame         = m_perFrameData[frameIndex];

        // Wait for the queues to finish operations from last time they were used.
        LINAGX_VEC<VkSemaphore> waitSemaphores;
        LINAGX_VEC<uint64>      waitSemaphoreValues;

        for (auto& q : m_queues)
        {
            if (!q.isValid || q.type != CommandType::Graphics || !q.wasSubmitted[m_currentFrameIndex])
                continue;

            q.wasSubmitted[m_currentFrameIndex]             = false;
            q.pfd[m_currentFrameIndex].submitSemaphoreIndex = 0;

            auto sem = q.pfd[m_currentFrameIndex].startFrameWaitSemaphore;

            if (std::find_if(waitSemaphores.begin(), waitSemaphores.end(), [sem](VkSemaphore s) { return s == sem; }) == waitSemaphores.end())
            {
                waitSemaphores.push_back(sem);
                waitSemaphoreValues.push_back(q.pfd[m_currentFrameIndex].storedStartFrameSemaphoreValue);
            }
        }

        const uint64 timeout = static_cast<uint64>(5000000000);

        if (!waitSemaphores.empty())
        {
            VkSemaphoreWaitInfo waitInfo = VkSemaphoreWaitInfo{};
            waitInfo.sType               = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
            waitInfo.pNext               = nullptr;
            waitInfo.flags               = 0;
            waitInfo.semaphoreCount      = static_cast<uint32>(waitSemaphores.size());
            waitInfo.pSemaphores         = waitSemaphores.data();
            waitInfo.pValues             = waitSemaphoreValues.data();
            VkResult res                 = vkWaitSemaphores(m_device, &waitInfo, timeout);
            VK_CHECK_RESULT(res, "Backend -> Failed waiting for semaphores!");
        }

        frame.submissionCount = 0;

        // Acquire images for each swapchain
        const uint8 swpSize = m_swapchains.GetNextFreeID();
        for (uint8 i = 0; i < swpSize; i++)
        {
            auto& swp = m_swapchains.GetItemR(i);

            if (!swp.isValid || swp.width == 0 || swp.height == 0 || !swp.isActive)
            {
                continue;
            }

            VkResult result = vkAcquireNextImageKHR(m_device, swp.ptr, timeout, swp.imageAcquiredSemaphores[m_currentFrameIndex], nullptr, &swp._imageIndex);
            if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
            {
                swp.gotImage = true;
            }
            else
            {
                LOGE("Backend -> Failed acquiring image for swapchain!");
            }

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            {
                LOGA(false, "Backend -> Image is out of date or suboptimal, did you forget to call LinaGX::Instance->RecreateSwapchain after a window resize?");
            }
        }

        const uint32 cmdSize = m_cmdStreams.GetNextFreeID();
        for (uint32 i = 0; i < cmdSize; i++)
        {
            auto& cs = m_cmdStreams.GetItemR(i);

            if (!cs.isValid)
                continue;

            for (auto it = cs.intermediateResources.begin(); it != cs.intermediateResources.end();)
            {
                if (PerformanceStats.totalFrames > it->second + m_initInfo.framesInFlight + 1)
                {
                    DestroyResource(it->first);
                    it = cs.intermediateResources.erase(it);
                }
                else
                    ++it;
            }
        }
    }

    void VKBackend::Present(const PresentDesc& present)
    {
        LINAGX_VEC<VkSwapchainKHR> swaps;
        LINAGX_VEC<uint32>         imageIndices;
        LINAGX_VEC<VkSemaphore>    waitSemaphores;
        swaps.reserve(present.swapchainCount);
        imageIndices.reserve(present.swapchainCount);
        waitSemaphores.reserve(present.swapchainCount);

        auto& frame = m_perFrameData[m_currentFrameIndex];

        for (uint32 i = 0; i < present.swapchainCount; i++)
        {
            const auto& swp = m_swapchains.GetItemR(present.swapchains[i]);

            if (swp.width == 0 || swp.height == 0 || !swp.gotImage)
                continue;

            if (!swp.isActive)
            {
                LOGE("Trying to present an inactive swapchain!");
                continue;
            }

            swaps.push_back(swp.ptr);
            imageIndices.push_back(swp._imageIndex);
            waitSemaphores.push_back(m_queues.GetItemR(swp._submittedQueue).pfd[m_currentFrameIndex].submitSemaphoreBuffer[swp._submittedSemaphoreIndex]);
        }

        if (swaps.empty())
            return;

        VkPresentInfoKHR info   = VkPresentInfoKHR{};
        info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext              = nullptr;
        info.waitSemaphoreCount = static_cast<uint32>(waitSemaphores.size());
        info.pWaitSemaphores    = waitSemaphores.data();
        info.swapchainCount     = static_cast<uint32>(swaps.size());
        info.pSwapchains        = swaps.data();
        info.pImageIndices      = imageIndices.data();

        VkResult result = vkQueuePresentKHR(m_queues.GetItemR(GetPrimaryQueue(CommandType::Graphics)).queue, &info);

        // Check for swapchain recreation.
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            LOGA(false, "Backend -> Image is out of date or suboptimal, did you forget to call LinaGX::Instance->RecreateSwapchain after a window resize?");
        }
    }

    void VKBackend::EndFrame()
    {
    }

    void VKBackend::CMD_BeginRenderPass(uint8* data, VKBCommandStream& stream)
    {
        CMDBeginRenderPass* begin = reinterpret_cast<CMDBeginRenderPass*>(data);

        VkImageView depthStencilView = nullptr;

        if (begin->depthStencilAttachment.useDepth || begin->depthStencilAttachment.useStencil)
        {
            const auto& depthStencilTxt = m_texture2Ds.GetItemR(begin->depthStencilAttachment.texture);
            depthStencilView            = depthStencilTxt.imgView;
        }

        LINAGX_VEC<VkRenderingAttachmentInfo> colorAttachments;
        colorAttachments.resize(begin->colorAttachmentCount);

        LINAGX_VEC<VkImageView> imageViews;
        LINAGX_VEC<VkImage>     images;
        imageViews.resize(begin->colorAttachmentCount);
        images.resize(begin->colorAttachmentCount);

        stream.lastRPImages.clear();
        stream.lastRPImages.resize(begin->colorAttachmentCount);

        for (uint32 i = 0; i < begin->colorAttachmentCount; i++)
        {
            const auto&               att = begin->colorAttachments[i];
            VkRenderingAttachmentInfo info;

            if (att.isSwapchain)
            {
                const auto& swp        = m_swapchains.GetItemR(static_cast<uint8>(att.texture));
                imageViews[i]          = swp.views[swp._imageIndex];
                images[i]              = swp.imgs[swp._imageIndex];
                stream.lastRPImages[i] = {images[i], true};
                stream.swapchainWrites.push_back(static_cast<uint8>(att.texture));
            }
            else
            {
                const auto& txt        = m_texture2Ds.GetItemR(att.texture);
                imageViews[i]          = txt.imgView;
                images[i]              = txt.img;
                stream.lastRPImages[i] = {images[i], false};
            }

            info.sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            info.pNext              = nullptr;
            info.imageView          = imageViews[i];
            info.imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            info.resolveMode        = VK_RESOLVE_MODE_NONE;
            info.resolveImageView   = nullptr;
            info.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            info.loadOp             = GetVKLoadOp(att.loadOp);
            info.storeOp            = GetVKStoreOp(att.storeOp);
            info.clearValue.color   = {att.clearColor.x, att.clearColor.y, att.clearColor.z, att.clearColor.w};

            colorAttachments[i] = info;
        }

        VkRenderingAttachmentInfo depthAttachment = VkRenderingAttachmentInfo{};
        depthAttachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthAttachment.pNext                     = nullptr;
        depthAttachment.imageView                 = depthStencilView;
        depthAttachment.imageLayout               = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.resolveMode               = VK_RESOLVE_MODE_NONE;
        depthAttachment.resolveImageView          = nullptr;
        depthAttachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.loadOp                    = GetVKLoadOp(begin->depthStencilAttachment.depthLoadOp);
        depthAttachment.storeOp                   = GetVKStoreOp(begin->depthStencilAttachment.depthStoreOp);
        depthAttachment.clearValue.depthStencil   = {begin->depthStencilAttachment.useDepth ? begin->depthStencilAttachment.clearDepth : 0.0f, begin->depthStencilAttachment.useStencil ? begin->depthStencilAttachment.clearStencil : 0};

        VkRenderingAttachmentInfo stencilAttachment = VkRenderingAttachmentInfo{};
        stencilAttachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        stencilAttachment.pNext                     = nullptr;
        stencilAttachment.imageView                 = depthStencilView;
        stencilAttachment.imageLayout               = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        stencilAttachment.resolveMode               = VK_RESOLVE_MODE_NONE;
        stencilAttachment.resolveImageView          = nullptr;
        stencilAttachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        stencilAttachment.loadOp                    = GetVKLoadOp(begin->depthStencilAttachment.stencilLoadOp);
        stencilAttachment.storeOp                   = GetVKStoreOp(begin->depthStencilAttachment.stencilStoreOp);
        stencilAttachment.clearValue.depthStencil   = {begin->depthStencilAttachment.useDepth ? begin->depthStencilAttachment.clearDepth : 0.0f, begin->depthStencilAttachment.useStencil ? begin->depthStencilAttachment.clearStencil : 0};

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
        renderingInfo.pDepthAttachment     = begin->depthStencilAttachment.useDepth ? &depthAttachment : VK_NULL_HANDLE;
        renderingInfo.pStencilAttachment   = begin->depthStencilAttachment.useStencil ? &stencilAttachment : VK_NULL_HANDLE;
        renderingInfo.colorAttachmentCount = begin->colorAttachmentCount;
        renderingInfo.pColorAttachments    = colorAttachments.data();

        auto buffer = stream.buffer;

        for (auto img : images)
            TransitionImageLayout(buffer, img, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);

        vkCmdBeginRendering(buffer, &renderingInfo);
        CMD_SetViewport((uint8*)&interVP, stream);
        CMD_SetScissors((uint8*)&interSC, stream);
    }

    void VKBackend::CMD_EndRenderPass(uint8* data, VKBCommandStream& stream)
    {
        CMDEndRenderPass* end    = reinterpret_cast<CMDEndRenderPass*>(data);
        auto              buffer = stream.buffer;
        vkCmdEndRendering(buffer);

        for (const auto& imgData : stream.lastRPImages)
        {
            if (imgData.isSwapchain)
                TransitionImageLayout(buffer, imgData.ptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, 1);
            else
                TransitionImageLayout(buffer, imgData.ptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1);
        }

        stream.lastRPImages.clear();
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
        vkCmdBindPipeline(buffer, shader.isCompute ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS, shader.ptrPipeline);
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

    void VKBackend::CMD_DrawIndexedIndirect(uint8* data, VKBCommandStream& stream)
    {
        CMDDrawIndexedIndirect* cmd       = reinterpret_cast<CMDDrawIndexedIndirect*>(data);
        auto                    buffer    = stream.buffer;
        auto&                   indBuffer = m_resources.GetItemR(cmd->indirectBuffer);

        if (m_supportsMultiDrawIndirect)
            vkCmdDrawIndexedIndirect(buffer, indBuffer.buffer, sizeof(uint32), cmd->count, cmd->stride);
        else
        {
            for (uint32 i = 0; i < cmd->count; i++)
                vkCmdDrawIndexedIndirect(buffer, indBuffer.buffer, sizeof(uint32) + sizeof(IndexedIndirectCommand) * i, 1, sizeof(IndexedIndirectCommand));
        }
    }

    void VKBackend::CMD_DrawIndirect(uint8* data, VKBCommandStream& stream)
    {
        CMDDrawIndirect* cmd       = reinterpret_cast<CMDDrawIndirect*>(data);
        auto             buffer    = stream.buffer;
        auto&            indBuffer = m_resources.GetItemR(cmd->indirectBuffer);

        if (m_supportsMultiDrawIndirect)
            vkCmdDrawIndexedIndirect(buffer, indBuffer.buffer, sizeof(uint32), cmd->count, cmd->stride);
        else
        {
            for (uint32 i = 0; i < cmd->count; i++)
                vkCmdDrawIndexedIndirect(buffer, indBuffer.buffer, sizeof(uint32) + sizeof(IndexedIndirectCommand) * i, 1, sizeof(IndexedIndirectCommand));
        }
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
        vkCmdBindIndexBuffer(buffer, res.buffer, cmd->offset, GetVKIndexType(cmd->indexType));
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

        ResourceDesc stagingDesc                    = {};
        stagingDesc.size                            = totalDataSize;
        stagingDesc.typeHintFlags                   = TH_None;
        stagingDesc.heapType                        = ResourceHeap::StagingHeap;
        uint32 stagingHandle                        = CreateResource(stagingDesc);
        stream.intermediateResources[stagingHandle] = PerformanceStats.totalFrames;

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
            subresLayers.baseArrayLayer           = cmd->destinationSlice;
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

        TransitionImageLayout(buffer, dstTexture.img, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmd->mipLevels, dstTexture.arrayLength);
        vkCmdCopyBufferToImage(buffer, srcResource.buffer, dstTexture.img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(regions.size()), regions.data());
        TransitionImageLayout(buffer, dstTexture.img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmd->mipLevels, dstTexture.arrayLength);
    }

    void VKBackend::CMD_BindDescriptorSets(uint8* data, VKBCommandStream& stream)
    {
        CMDBindDescriptorSets* cmd    = reinterpret_cast<CMDBindDescriptorSets*>(data);
        auto                   buffer = stream.buffer;

        VkPipelineLayout layout = nullptr;

        if (cmd->layoutSource == DescriptorSetsLayoutSource::LastBoundShader)
            layout = m_shaders.GetItemR(stream.boundShader).ptrLayout;
        else if (cmd->layoutSource == DescriptorSetsLayoutSource::CustomShader)
            layout = m_shaders.GetItemR(cmd->customLayoutShader).ptrLayout;
        else
            layout = m_pipelineLayouts.GetItemR(cmd->customLayout).ptr;

        LINAGX_VEC<VkDescriptorSet> sets;
        sets.resize(cmd->setCount);

        for (uint32 i = 0; i < cmd->setCount; i++)
            sets[i] = m_descriptorSets.GetItemR(cmd->descriptorSetHandles[i]).ptr;

        vkCmdBindDescriptorSets(buffer, cmd->isCompute ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS, layout, cmd->firstSet, cmd->setCount, sets.data(), cmd->dynamicOffsetCount, cmd->dynamicOffsets);
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

    void VKBackend::CMD_Dispatch(uint8* data, VKBCommandStream& stream)
    {
        CMDDispatch* cmd    = reinterpret_cast<CMDDispatch*>(data);
        auto         buffer = stream.buffer;
        vkCmdDispatch(buffer, cmd->groupSizeX, cmd->groupSizeY, cmd->groupSizeZ);
    }

    void VKBackend::CMD_ComputeBarrier(uint8* data, VKBCommandStream& stream)
    {
        CMDComputeBarrier* cmd    = reinterpret_cast<CMDComputeBarrier*>(data);
        auto               buffer = stream.buffer;

        VkMemoryBarrier memoryBarrier{};
        memoryBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;

        vkCmdPipelineBarrier(buffer,
                             VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // srcStageMask
                             VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,  // dstStageMask
                             0,                                    // dependencyFlags
                             1, &memoryBarrier,                    // memoryBarrierCount, pMemoryBarriers
                             0, nullptr,                           // bufferMemoryBarrierCount, pBufferMemoryBarriers
                             0, nullptr);                          // imageMemoryBarrierCount, pImageMemoryBarriers
    }

    void VKBackend::CMD_ExecuteSecondaryStream(uint8* data, VKBCommandStream& stream)
    {
        CMDExecuteSecondaryStream* cmd       = reinterpret_cast<CMDExecuteSecondaryStream*>(data);
        auto                       buffer    = stream.buffer;
        auto                       bufferSec = m_cmdStreams.GetItemR(cmd->secondaryStream->m_gpuHandle).buffer;
        vkCmdExecuteCommands(stream.buffer, 1, &bufferSec);
    }

    void VKBackend::TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32 mipLevels, uint32 arraySize)
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
        barrier.subresourceRange.layerCount     = arraySize;

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
            barrier.dstAccessMask = (m_supportsDedicatedComputeQueue || m_supportsSeparateTransferQueue) ? 0 : VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = (m_supportsDedicatedComputeQueue || m_supportsSeparateTransferQueue) ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
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
