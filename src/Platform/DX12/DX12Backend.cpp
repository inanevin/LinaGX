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

#include "LinaGX/Platform/DX12/DX12Backend.hpp"
#include "LinaGX/Platform/DX12/SDK/D3D12MemAlloc.h"
#include "LinaGX/Platform/DX12/DX12HeapGPU.hpp"
#include "LinaGX/Platform/DX12/DX12HeapStaging.hpp"
#include "LinaGX/Utility/PlatformUtility.hpp"
#include "LinaGX/Core/Commands.hpp"
#include "LinaGX/Core/Instance.hpp"
#include "LinaGX/Core/CommandStream.hpp"

#ifdef LINAGX_PLATFORM_WINDOWS
#include "nvapi/nvapi.h"
#endif

LINAGX_DISABLE_VC_WARNING(6387);

using Microsoft::WRL::ComPtr;

namespace LinaGX
{
#ifndef NDEBUG
#define NAME_DX12_OBJECT_CSTR(x, NAME)       \
    auto wcharConverted = CharToWChar(NAME); \
    x->SetName(wcharConverted);              \
    delete[] wcharConverted;

#define NAME_DX12_OBJECT(x, NAME) x->SetName(NAME)
#else
#define NAME_DX12_OBJECT_CSTR(x, NAME)
#define NAME_DX12_OBJECT(x, NAME)
#endif

#define DX12_THROW(exception, ...) \
    LOGE(__VA_ARGS__);             \
    throw exception;

    DWORD msgCallback = 0;

    DXGI_FORMAT GetDXFormat(Format format)
    {
        switch (format)
        {
        case Format::UNDEFINED:
            return DXGI_FORMAT_UNKNOWN;
        case Format::B8G8R8A8_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case Format::B8G8R8A8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case Format::R32G32B32_SFLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case Format::R32G32B32_SINT:
            return DXGI_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32A32_SFLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Format::R32G32B32A32_SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case Format::R16G16B16A16_SFLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case Format::R16G16B16A16_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case Format::R32G32_SFLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
        case Format::R32G32_SINT:
            return DXGI_FORMAT_R32G32_SINT;
        case Format::D32_SFLOAT:
            return DXGI_FORMAT_D32_FLOAT;
        case Format::R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case Format::R8G8_UNORM:
            return DXGI_FORMAT_R8G8_UNORM;
        case Format::R8_UNORM:
            return DXGI_FORMAT_R8_UNORM;
        case Format::R8_UINT:
            return DXGI_FORMAT_R8_UINT;
        case Format::R16_SFLOAT:
            return DXGI_FORMAT_R16_FLOAT;
        case Format::R16_SINT:
            return DXGI_FORMAT_R16_SINT;
        case Format::R32_SFLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case Format::R32_SINT:
            return DXGI_FORMAT_R32_SINT;
        case Format::R32_UINT:
            return DXGI_FORMAT_R32_UINT;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    DXGI_FORMAT GetDXIndexType(IndexType type)
    {
        switch (type)
        {
        case IndexType::Uint16:
            return DXGI_FORMAT_R16_UINT;
        case IndexType::Uint32:
            return DXGI_FORMAT_R32_UINT;
        default:
            return DXGI_FORMAT_R32_UINT;
        }
    }

    D3D12_BLEND GetDXBlend(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::Zero:
            return D3D12_BLEND_ZERO;
        case BlendFactor::One:
            return D3D12_BLEND_ONE;
        case BlendFactor::SrcColor:
            return D3D12_BLEND_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:
            return D3D12_BLEND_INV_SRC_COLOR;
        case BlendFactor::DstColor:
            return D3D12_BLEND_DEST_COLOR;
        case BlendFactor::OneMinusDstColor:
            return D3D12_BLEND_INV_DEST_COLOR;
        case BlendFactor::SrcAlpha:
            return D3D12_BLEND_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return D3D12_BLEND_DEST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:
            return D3D12_BLEND_INV_DEST_ALPHA;
        default:
            return D3D12_BLEND_ZERO;
        }
    }

    D3D12_BLEND_OP GetDXBlendOp(BlendOp op)
    {
        switch (op)
        {
        case BlendOp::Add:
            return D3D12_BLEND_OP_ADD;
        case BlendOp::Subtract:
            return D3D12_BLEND_OP_SUBTRACT;
        case BlendOp::ReverseSubtract:
            return D3D12_BLEND_OP_REV_SUBTRACT;
        case BlendOp::Min:
            return D3D12_BLEND_OP_MIN;
        case BlendOp::Max:
            return D3D12_BLEND_OP_MAX;
        default:
            return D3D12_BLEND_OP_ADD;
        }
    }

    D3D12_LOGIC_OP GetDXLogicOp(LogicOp op)
    {
        switch (op)
        {
        case LogicOp::Clear:
            return D3D12_LOGIC_OP_CLEAR;
        case LogicOp::And:
            return D3D12_LOGIC_OP_AND;
        case LogicOp::AndReverse:
            return D3D12_LOGIC_OP_AND_REVERSE;
        case LogicOp::Copy:
            return D3D12_LOGIC_OP_COPY;
        case LogicOp::AndInverted:
            return D3D12_LOGIC_OP_AND_INVERTED;
        case LogicOp::NoOp:
            return D3D12_LOGIC_OP_NOOP;
        case LogicOp::XOR:
            return D3D12_LOGIC_OP_XOR;
        case LogicOp::OR:
            return D3D12_LOGIC_OP_OR;
        case LogicOp::NOR:
            return D3D12_LOGIC_OP_NOR;
        case LogicOp::Equivalent:
            return D3D12_LOGIC_OP_EQUIV;
        default:
            return D3D12_LOGIC_OP_CLEAR;
        }
    }

    D3D12_CULL_MODE GetDXCullMode(CullMode cm)
    {
        switch (cm)
        {
        case CullMode::None:
            return D3D12_CULL_MODE_NONE;
        case CullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        case CullMode::Back:
            return D3D12_CULL_MODE_BACK;
        default:
            return D3D12_CULL_MODE_NONE;
        }
    }

    D3D12_COMPARISON_FUNC GetDXCompareOp(CompareOp op)
    {
        switch (op)
        {
        case CompareOp::Never:
            return D3D12_COMPARISON_FUNC_NEVER;
        case CompareOp::Less:
            return D3D12_COMPARISON_FUNC_LESS;
        case CompareOp::Equal:
            return D3D12_COMPARISON_FUNC_EQUAL;
        case CompareOp::LEqual:
            return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case CompareOp::Greater:
            return D3D12_COMPARISON_FUNC_GREATER;
        case CompareOp::NotEqual:
            return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case CompareOp::GEqual:
            return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case CompareOp::Always:
            return D3D12_COMPARISON_FUNC_ALWAYS;
        default:
            return D3D12_COMPARISON_FUNC_ALWAYS;
        }
    }

    D3D12_STENCIL_OP GetDXStencilOp(StencilOp op)
    {
        switch (op)
        {
        case StencilOp::Keep:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
        case StencilOp::Zero:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_ZERO;
        case StencilOp::Replace:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_REPLACE;
        case StencilOp::IncrementClamp:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR_SAT;
        case StencilOp::DecrementClamp:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR_SAT;
        case StencilOp::Invert:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INVERT;
        case StencilOp::IncrementWrap:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR;
        case StencilOp::DecrementWrap:
            return D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR;
        default:
            return D3D12_STENCIL_OP_KEEP;
        }
    }

    D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE GetDXLoadOp(LoadOp op)
    {
        switch (op)
        {
        case LoadOp::Load:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
        case LoadOp::Clear:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        case LoadOp::DontCare:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        case LoadOp::None:
        default:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
        }
    }

    D3D12_RENDER_PASS_ENDING_ACCESS_TYPE GetDXStoreOp(StoreOp op)
    {
        switch (op)
        {
        case StoreOp::Store:
            return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        case StoreOp::DontCare:
            return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        case StoreOp::None:
        default:
            return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
        }
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetDXTopologyType(Topology tp)
    {
        switch (tp)
        {
        case Topology::PointList:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case Topology::LineList:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case Topology::LineStrip:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case Topology::TriangleList:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case Topology::TriangleStrip:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case Topology::TriangleFan:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case Topology::TriangleListAdjacency:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case Topology::TriangleStripAdjacency:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        }
    }

    D3D12_PRIMITIVE_TOPOLOGY GetDXTopology(Topology topology)
    {
        switch (topology)
        {
        case Topology::PointList:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case Topology::LineList:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case Topology::LineStrip:
            return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case Topology::TriangleFan:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLEFAN;
        case Topology::TriangleStrip:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        case Topology::TriangleList:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case Topology::TriangleListAdjacency:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
        case Topology::TriangleStripAdjacency:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
        default:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        }
    }

    D3D12_SHADER_VISIBILITY GetDXVisibility(ShaderStage stg)
    {
        switch (stg)
        {
        case ShaderStage::Vertex:
            return D3D12_SHADER_VISIBILITY_VERTEX;
        case ShaderStage::Fragment:
            return D3D12_SHADER_VISIBILITY_PIXEL;
        case ShaderStage::Geometry:
            return D3D12_SHADER_VISIBILITY_GEOMETRY;
        case ShaderStage::Tesellation:
            return D3D12_SHADER_VISIBILITY_HULL;
        default:
            return D3D12_SHADER_VISIBILITY_ALL;
        }
    }

    D3D12_COMMAND_LIST_TYPE GetDXCommandType(CommandType type)
    {
        switch (type)
        {
        case CommandType::Graphics:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandType::Transfer:
            return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandType::Compute:
            return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        case CommandType::Secondary:
            return D3D12_COMMAND_LIST_TYPE_BUNDLE;
        default:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        }
    }

    D3D12_TEXTURE_ADDRESS_MODE GetDXAddressMode(SamplerAddressMode mode)
    {
        switch (mode)
        {
        case SamplerAddressMode::Repeat:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case SamplerAddressMode::MirroredRepeat:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case SamplerAddressMode::MirrorClampToEdge:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        case SamplerAddressMode::ClampToBorder:
            return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        case SamplerAddressMode::ClampToEdge:
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        default:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }
    }

    D3D12_FILTER GetDXFilter(Filter minFilter, Filter magFilter)
    {
        if (minFilter == Filter::Anisotropic || magFilter == Filter::Anisotropic)
            return D3D12_FILTER_ANISOTROPIC;

        if (minFilter == Filter::Linear && magFilter == Filter::Linear)
            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        else if (minFilter == Filter::Linear && magFilter == Filter::Nearest)
            return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        else if (minFilter == Filter::Nearest && magFilter == Filter::Nearest)
            return D3D12_FILTER_MIN_MAG_MIP_POINT;
        else if (minFilter == Filter::Nearest && magFilter == Filter::Linear)
            return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;

        return D3D12_FILTER_ANISOTROPIC;
    }

    D3D12_COLOR_WRITE_ENABLE GetDXColorWriteMask(ColorComponentFlags flag)
    {
        switch (flag)
        {
        case ColorComponentFlags::R:
            return D3D12_COLOR_WRITE_ENABLE_RED;
        case ColorComponentFlags::G:
            return D3D12_COLOR_WRITE_ENABLE_GREEN;
        case ColorComponentFlags::B:
            return D3D12_COLOR_WRITE_ENABLE_BLUE;
        case ColorComponentFlags::A:
            return D3D12_COLOR_WRITE_ENABLE_ALPHA;
        default:
            return D3D12_COLOR_WRITE_ENABLE_ALL;
        }
    }

    D3D12_RESOURCE_STATES GetDXTextureBarrierState(TextureBarrierState state, bool usedOutsideFragment)
    {
        switch (state)
        {
        case LinaGX::TextureBarrierState::ColorAttachment:
        case LinaGX::TextureBarrierState::DepthStencilAttachment:
        case LinaGX::TextureBarrierState::DepthAttachment:
        case LinaGX::TextureBarrierState::StencilAttachment:
            return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case LinaGX::TextureBarrierState::ShaderRead: {
            if (usedOutsideFragment)
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            else
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        }
        case LinaGX::TextureBarrierState::Present:
            return D3D12_RESOURCE_STATE_PRESENT;
        case LinaGX::TextureBarrierState::TransferSource:
            return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case LinaGX::TextureBarrierState::TransferDestination:
            return D3D12_RESOURCE_STATE_COPY_DEST;
        default:
            return D3D12_RESOURCE_STATE_COMMON;
        }
    }
    D3D12_RESOURCE_STATES GetDXResourceBarrierState(ResourceBarrierState state)
    {
        switch (state)
        {
        case LinaGX::ResourceBarrierState::TransferRead:
            return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case LinaGX::ResourceBarrierState::TransferWrite:
            return D3D12_RESOURCE_STATE_COPY_DEST;
        default:
            return D3D12_RESOURCE_STATE_COMMON;
        }
    }
    void FillBorderColor(BorderColor bc, float* color)
    {
        switch (bc)
        {
        case LinaGX::BorderColor::BlackTransparent:
            color[0] = color[1] = color[2] = color[3] = 0.0f;
            break;
        case LinaGX::BorderColor::BlackOpaque:
            color[0] = color[1] = color[2] = 0.0f;
            color[3]                       = 1.0f;
            break;
        case LinaGX::BorderColor::WhiteOpaque:
            color[0] = color[1] = color[2] = color[3] = 1.0f;
            break;
        default:
            break;
        }
    }

    void MessageCallback(D3D12_MESSAGE_CATEGORY messageType, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID messageId, LPCSTR pDesc, void* pContext)
    {
        if (pDesc != NULL)
        {
            if (severity == D3D12_MESSAGE_SEVERITY_MESSAGE)
            {
                LOGT("Backend -> %s", pDesc);
            }
            else if (severity == D3D12_MESSAGE_SEVERITY_INFO)
            {
                // LOGV("Backend -> %s", pDesc);
            }
            else
            {
                LOGE("Backend -> %s", pDesc);
            }
        }
    }

    void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, PreferredGPUType gpuType)
    {
        *ppAdapter = nullptr;

        ComPtr<IDXGIAdapter1> adapter;

        const unsigned int NVIDIA_VENDOR_ID = 0x10DE;
        const unsigned int AMD_VENDOR_ID    = 0x1002;
        const unsigned int INTEL_VENDOR_ID  = 0x8086;

        auto findDevice = [&](bool agressive) {
            ComPtr<IDXGIFactory6> factory6;
            if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
            {
                for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, gpuType == PreferredGPUType::Discrete ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER, IID_PPV_ARGS(&adapter))); ++adapterIndex)
                {
                    DXGI_ADAPTER_DESC1 desc;
                    adapter->GetDesc1(&desc);

                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                        continue;

                    if (agressive && desc.VendorId != NVIDIA_VENDOR_ID && desc.VendorId != AMD_VENDOR_ID && gpuType == PreferredGPUType::Discrete)
                        continue;

                    if (agressive && (desc.VendorId == NVIDIA_VENDOR_ID || desc.VendorId == AMD_VENDOR_ID) && gpuType == PreferredGPUType::Integrated)
                        continue;

                    // Check to see whether the adapter supports Direct3D 12, but don't create the
                    // actual device yet.
                    if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                    {
                        char* buf = WCharToChar(desc.Description);
                        LOGT("Backend -> Selected hardware adapter %s, dedicated video memory %f mb", buf, desc.DedicatedVideoMemory * 0.000001);
                        GPUInfo.dedicatedVideoMemory = static_cast<uint64>(desc.DedicatedVideoMemory);
                        GPUInfo.deviceName           = buf;
                        delete[] buf;
                        break;
                    }
                }
            }
        };

        findDevice(true);

        if (adapter.Get() == nullptr)
            findDevice(false);

        if (adapter.Get() == nullptr)
        {
            LOGE("Backend -> Failed finding a suitable device!");
            return;
        }

        *ppAdapter = adapter.Detach();
    }

    uint16 DX12Backend::CreateUserSemaphore()
    {
        DX12UserSemaphore item = {};
        item.isValid           = true;

        try
        {
            ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&item.ptr)));
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend-> Exception when creating a fence! {0}", e.what());
        }

        return m_userSemaphores.AddItem(item);
    }

    void DX12Backend::DestroyUserSemaphore(uint16 handle)
    {
        auto& semaphore = m_userSemaphores.GetItemR(handle);
        if (!semaphore.isValid)
        {
            LOGE("Backend -> Semaphore to be destroyed is not valid!");
            return;
        }

        semaphore.ptr.Reset();
        m_userSemaphores.RemoveItem(handle);
    }

    void DX12Backend::WaitForUserSemaphore(uint16 handle, uint64 value)
    {
        WaitForFences(m_userSemaphores.GetItemR(handle).ptr.Get(), value);
    }

    uint8 DX12Backend::CreateSwapchain(const SwapchainDesc& desc)
    {
        DXGI_FORMAT swapFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

        // DXGI flip swapchains can only be b8g8r8a8_unorm, r16g16b16a16_float or r8g8b8a8_unorm.
        if (desc.format == Format::R16G16B16A16_SFLOAT)
            swapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        else if (desc.format == Format::R8G8B8A8_UNORM)
            swapFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        else if (desc.format == Format::R8G8B8A8_SRGB)
            swapFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

        DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
        swapchainDesc.BufferCount           = m_initInfo.backbufferCount;
        swapchainDesc.Width                 = static_cast<UINT>(desc.width);
        swapchainDesc.Height                = static_cast<UINT>(desc.height);
        swapchainDesc.Format                = swapFormat;
        swapchainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchainDesc.SampleDesc.Count      = 1;
        // swapchainDesc.Flags                 = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

        ComPtr<IDXGISwapChain1> swapchain;

        if (m_allowTearing)
            swapchainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        try
        {
            const auto& primaryGraphics = m_queues.GetItemR(desc.queue);
            ThrowIfFailed(m_factory->CreateSwapChainForHwnd(primaryGraphics.queue.Get(), // Swap chain needs the queue so that it can force a flush on it.
                                                            (HWND)desc.window,
                                                            &swapchainDesc,
                                                            nullptr,
                                                            nullptr,
                                                            &swapchain));

            // ThrowIfFailed(swapchain->SetFullscreenState(desc.isFullscreen, nullptr));
            // m_factory->MakeWindowAssociation((HWND)desc.window, DXGI_MWA_NO_ALT_ENTER);
            // ThrowIfFailed(swapchain->ResizeBuffers(m_initInfo.backbufferCount, desc.width, desc.height, swapchainDesc.Format, swapchainDesc.Flags));

            DX12Swapchain swp = {};
            swp.isValid       = true;
            swp.width         = desc.width;
            swp.height        = desc.height;
            swp.vsync         = desc.vsyncMode;
            swp.format        = desc.format;
            swp.depthFormat   = desc.depthFormat;
            swp.isFullscreen  = desc.isFullscreen;
            ThrowIfFailed(swapchain.As(&swp.ptr));

            LOGT("Backend -> Successfuly created swapchain with size %d x %d", desc.width, desc.height);

            // Create RT and depth textures for swapchain.
            {
                for (uint32 i = 0; i < m_initInfo.backbufferCount; i++)
                {
                    DX12Texture2D color      = {};
                    color.isSwapchainTexture = true;
                    color.descriptor         = m_rtvHeap->GetNewHeapHandle();
                    color.isValid            = true;

                    try
                    {
                        ThrowIfFailed(swp.ptr->GetBuffer(i, IID_PPV_ARGS(&color.rawRes)));
                    }
                    catch (HrException e)
                    {
                        DX12_THROW(e, "Backend -> Exception while creating render target for swapchain! %s", e.what());
                    }

                    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
                    rtvDesc.Format                        = GetDXFormat(desc.format);
                    rtvDesc.ViewDimension                 = D3D12_RTV_DIMENSION_TEXTURE2D;
                    m_device->CreateRenderTargetView(color.rawRes.Get(), &rtvDesc, {color.descriptor.GetCPUHandle()});
                    swp.colorTextures.push_back(m_textures.AddItem(color));
                }
            }

            return m_swapchains.AddItem(swp);
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when creating a swapchain! %s", e.what());
        }

        return 0;
    }

    void DX12Backend::DestroySwapchain(uint8 handle)
    {
        auto& swp = m_swapchains.GetItemR(handle);
        if (!swp.isValid)
        {
            LOGE("Backend -> Swapchain to be destroyed is not valid!");
            return;
        }

        try
        {
            // ThrowIfFailed(swp.ptr->SetFullscreenState(FALSE, nullptr));
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Setting swapchain fullscreen state!");
        }

        for (auto t : swp.colorTextures)
            DestroyTexture(t);

        swp.isValid = false;
        swp.ptr.Reset();
        m_swapchains.RemoveItem(handle);
    }

    void DX12Backend::RecreateSwapchain(const SwapchainRecreateDesc& desc)
    {
        if (desc.width == 0 || desc.height == 0)
            return;

        Join();

        auto&                swp     = m_swapchains.GetItemR(desc.swapchain);
        DXGI_SWAP_CHAIN_DESC swpDesc = {};
        swp.ptr->GetDesc(&swpDesc);

        try
        {
            // if (!m_allowTearing)
            //     ThrowIfFailed(swp.ptr->SetFullscreenState(desc.isFullscreen, nullptr));

            for (uint32 i = 0; i < m_initInfo.backbufferCount; i++)
                DestroyTexture(swp.colorTextures[i]);

            ThrowIfFailed(swp.ptr->ResizeBuffers(m_initInfo.backbufferCount, desc.width, desc.height, swpDesc.BufferDesc.Format, swpDesc.Flags));

            swp.width       = desc.width;
            swp.height      = desc.height;
            swp._imageIndex = swp.ptr->GetCurrentBackBufferIndex();

            for (uint32 i = 0; i < m_initInfo.backbufferCount; i++)
            {
                DX12Texture2D color      = {};
                color.isSwapchainTexture = true;
                color.descriptor         = m_rtvHeap->GetNewHeapHandle();
                color.isValid            = true;

                try
                {
                    ThrowIfFailed(swp.ptr->GetBuffer(i, IID_PPV_ARGS(&color.rawRes)));
                }
                catch (HrException e)
                {
                    DX12_THROW(e, "Backend -> Exception while creating render target for swapchain! %s", e.what());
                }

                D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
                rtvDesc.Format                        = GetDXFormat(swp.format);
                rtvDesc.ViewDimension                 = D3D12_RTV_DIMENSION_TEXTURE2D;
                m_device->CreateRenderTargetView(color.rawRes.Get(), &rtvDesc, {color.descriptor.GetCPUHandle()});
                swp.colorTextures[i] = m_textures.AddItem(color);
            }
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Failed resizing swapchain! %s", e.what());
        }
    }

    void DX12Backend::SetSwapchainActive(uint8 swp, bool isActive)
    {
        m_swapchains.GetItemR(swp).isActive = isActive;
    }

    bool DX12Backend::CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob)
    {
        try
        {
            Microsoft::WRL::ComPtr<IDxcCompiler3> idxcCompiler;

            HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&idxcCompiler));
            if (FAILED(hr))
            {
                LOGE("Backend -> Failed to create DXC compiler");
                return false;
            }

            ComPtr<IDxcUtils> utils;
            hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));

            if (FAILED(hr))
            {
                LOGE("Backend -> Failed to create DXC utils.");
                return false;
            }

            UINT32                   codePage = CP_UTF8;
            ComPtr<IDxcBlobEncoding> sourceBlob;
            const char*              shaderSource = source.c_str();
            m_idxcLib->CreateBlobWithEncodingFromPinned((const BYTE*)shaderSource, static_cast<UINT>(source.size()), codePage, &sourceBlob);

            const wchar_t* targetProfile = L"vs_6_0";
            if (stage == ShaderStage::Fragment)
                targetProfile = L"ps_6_0";
            else if (stage == ShaderStage::Compute)
                targetProfile = L"cs_6_0";
            else if (stage == ShaderStage::Geometry)
                targetProfile = L"gs_6_0";
            else if (stage == ShaderStage::Tesellation)
                targetProfile = L"hs_6_0";

            DxcBuffer sourceBuffer;
            sourceBuffer.Ptr      = sourceBlob->GetBufferPointer();
            sourceBuffer.Size     = sourceBlob->GetBufferSize();
            sourceBuffer.Encoding = 0;

            LINAGX_VEC<LPCWSTR> arguments;

            arguments.push_back(L"-T");
            arguments.push_back(targetProfile);
            arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
            arguments.push_back(L"-HV 2021");                 //-WX

            // We will still get reflection info, just with a smaller binary.
            arguments.push_back(L"-Qstrip_debug");
            arguments.push_back(L"-Qstrip_reflect");

#ifndef NDEBUG
            arguments.push_back(DXC_ARG_DEBUG); //-Zi
#else
            arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

            ComPtr<IDxcResult> result;
            ThrowIfFailed(idxcCompiler->Compile(&sourceBuffer, arguments.data(), static_cast<uint32>(arguments.size()), NULL, IID_PPV_ARGS(result.GetAddressOf())));

            ComPtr<IDxcBlobUtf8> errors;
            ComPtr<IDxcBlobWide> outputName;
            result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr);

            if (errors && errors->GetStringLength() > 0)
            {
                LOGE("Backend -> %s", (char*)errors->GetStringPointer());
            }

            result->GetStatus(&hr);

            if (FAILED(hr))
            {
                if (result)
                {
                    ComPtr<IDxcBlobEncoding> errorsBlob;
                    hr = result->GetErrorBuffer(&errorsBlob);
                    if (SUCCEEDED(hr) && errorsBlob)
                    {
                        LOGE("Backend -> Shader compilation failed: %s", (const char*)errorsBlob->GetBufferPointer());
                        return false;
                    }
                }
            }

            ComPtr<IDxcBlob> code;
            result->GetResult(&code);

            const SIZE_T sz = code->GetBufferSize();
            outBlob.size    = code->GetBufferSize();
            outBlob.ptr     = new uint8[sz];
            LINAGX_MEMCPY(outBlob.ptr, code->GetBufferPointer(), outBlob.size);
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when compiling shader! %s", e.what());
            return false;
        }

        return true;
    }

    void ProcessDescriptorSetDesc(const DescriptorSetDesc& desc, uint32 setIndex, LINAGX_VEC<CD3DX12_ROOT_PARAMETER1>& rootParameters, LINAGX_VEC<CD3DX12_DESCRIPTOR_RANGE1>& ranges, uint32& rangeCounter, LINAGX_VEC<DX12RootParamInfo>& rootParamInfos)
    {
        uint32 bindingIndex = 0;

        for (const auto& binding : desc.bindings)
        {
            CD3DX12_ROOT_PARAMETER1 param      = CD3DX12_ROOT_PARAMETER1{};
            CD3DX12_ROOT_PARAMETER1 param2     = CD3DX12_ROOT_PARAMETER1{};
            D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
            if (binding.stages.size() == 1)
                visibility = GetDXVisibility(binding.stages[0]);

            DX12RootParamInfo paramInfo = {};
            paramInfo.rootParameter     = static_cast<uint32>(rootParameters.size());
            paramInfo.binding           = bindingIndex;
            paramInfo.set               = setIndex;
            paramInfo.elementSize       = binding.descriptorCount;
            paramInfo.reflectionType    = binding.type;
            paramInfo.isWritable        = binding.isWritable;
            rootParamInfos.push_back(paramInfo);

            auto flags = binding.unbounded ? D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE : D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

            if (binding.type == DescriptorType::UBO)
            {
                if (binding.descriptorCount > 1)
                {
                    ranges[rangeCounter].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, binding.descriptorCount, bindingIndex, setIndex, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
                    param.InitAsDescriptorTable(1, &ranges[rangeCounter], visibility);
                    rangeCounter++;
                }
                else
                    param.InitAsConstantBufferView(bindingIndex, setIndex, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, visibility);

                rootParameters.push_back(param);
            }
            else if (binding.type == DescriptorType::CombinedImageSampler)
            {
                ranges[rangeCounter].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, binding.unbounded ? UINT_MAX : binding.descriptorCount, bindingIndex, setIndex, flags);
                param.InitAsDescriptorTable(1, &ranges[rangeCounter], visibility);
                rangeCounter++;

                ranges[rangeCounter].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, binding.unbounded ? UINT_MAX : binding.descriptorCount, bindingIndex, setIndex, flags);
                param2.InitAsDescriptorTable(1, &ranges[rangeCounter], visibility);
                rangeCounter++;

                rootParameters.push_back(param);
                rootParameters.push_back(param2);
            }
            else if (binding.type == DescriptorType::SeparateImage)
            {
                ranges[rangeCounter].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, binding.unbounded ? UINT_MAX : binding.descriptorCount, bindingIndex, setIndex, flags);
                param.InitAsDescriptorTable(1, &ranges[rangeCounter], visibility);
                rangeCounter++;

                rootParameters.push_back(param);
            }
            else if (binding.type == DescriptorType::SeparateSampler)
            {
                ranges[rangeCounter].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, binding.unbounded ? UINT_MAX : binding.descriptorCount, bindingIndex, setIndex, flags);
                param.InitAsDescriptorTable(1, &ranges[rangeCounter], visibility);
                rangeCounter++;

                rootParameters.push_back(param);
            }
            else
            {
                ranges[rangeCounter].Init(!binding.isWritable ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV : D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, bindingIndex, setIndex, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
                param.InitAsDescriptorTable(1, &ranges[rangeCounter], visibility);
                rangeCounter++;

                rootParameters.push_back(param);
            }

            bindingIndex++;
        }
    }
    uint16 DX12Backend::CreateShader(const ShaderDesc& shaderDesc)
    {
        DX12Shader shader = {};
        shader.isValid    = true;
        shader.topology   = shaderDesc.topology;

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

        // Root signature.
        if (!shaderDesc.useCustomPipelineLayout)
        {
            LINAGX_VEC<CD3DX12_ROOT_PARAMETER1>   rootParameters;
            LINAGX_VEC<CD3DX12_DESCRIPTOR_RANGE1> ranges;
            ranges.resize(shaderDesc.layout.totalDescriptors);
            uint32 rangeCounter = 0;

            uint32 setIndex = 0;

            for (const auto& descriptorSet : shaderDesc.layout.descriptorSetLayouts)
            {
                DescriptorSetDesc copyDesc;

                for (const auto& b : descriptorSet.bindings)
                {
                    DescriptorBinding copyBinding;
                    copyBinding.descriptorCount = b.descriptorCount;
                    copyBinding.isWritable      = b.isWritable;
                    copyBinding.stages          = b.stages;
                    copyBinding.type            = b.type;
                    copyBinding.unbounded       = b.descriptorCount == 0;
                    copyDesc.bindings.push_back(copyBinding);
                }

                ProcessDescriptorSetDesc(copyDesc, setIndex, rootParameters, ranges, rangeCounter, shader.layout.rootParams);
                setIndex++;
            }

            uint32 drawIDRootParamIndex = 0;
            if (shaderDesc.layout.hasGLDrawID)
            {
                CD3DX12_ROOT_PARAMETER1 param      = CD3DX12_ROOT_PARAMETER1{};
                D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_VERTEX;
                param.InitAsConstants(1, shaderDesc.layout.drawIDBinding, 0, D3D12_SHADER_VISIBILITY_VERTEX);
                drawIDRootParamIndex = static_cast<uint32>(rootParameters.size());
                rootParameters.push_back(param);
            }

            auto containsStage = [](LINAGX_VEC<ShaderStage>& vec, ShaderStage target) {
                for (auto stg : vec)
                {
                    if (stg == target)
                        return true;
                }

                return false;
            };

            if (!shaderDesc.layout.constants.empty())
            {
                shader.layout.constantsSpace   = shaderDesc.layout.constantsSet;
                shader.layout.constantsBinding = shaderDesc.layout.constantsBinding;

                CD3DX12_ROOT_PARAMETER1 param = CD3DX12_ROOT_PARAMETER1{};

                LINAGX_VEC<ShaderStage> stages;

                for (const auto& ct : shaderDesc.layout.constants)
                {
                    for (auto stg : ct.stages)
                    {
                        if (!containsStage)
                            stages.push_back(stg);
                    }
                }

                D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
                if (stages.size() == 1)
                    visibility = GetDXVisibility(stages[0]);

                DX12RootParamInfo paramInfo = {};
                paramInfo.rootParameter     = static_cast<uint32>(rootParameters.size());
                paramInfo.reflectionType    = DescriptorType::ConstantBlock;
                paramInfo.set               = shader.layout.constantsSpace;
                paramInfo.binding           = shader.layout.constantsBinding;
                shader.layout.rootParams.push_back(paramInfo);

                uint32 totalConstants = 0;
                for (const auto& ct : shaderDesc.layout.constants)
                {
                    for (const auto& mem : ct.members)
                        totalConstants += static_cast<uint32>(mem.size) / sizeof(uint32);
                }

                param.InitAsConstants(totalConstants, shader.layout.constantsBinding, shader.layout.constantsSpace, visibility);
                rootParameters.push_back(param);
            }

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig(static_cast<uint32>(rootParameters.size()), rootParameters.data(), 0, NULL, shader.isCompute ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
            ComPtr<ID3DBlob>                      signatureBlob = nullptr;
            ComPtr<ID3DBlob>                      errorBlob     = nullptr;

            HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSig, &signatureBlob, &errorBlob);

            if (FAILED(hr))
            {
                LOGA(false, "Backend -> Failed serializing root signature! %s", (char*)errorBlob->GetBufferPointer());
                return 0;
            }

            hr = m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&shader.layout.rootSig));

            if (FAILED(hr))
            {
                LOGA(false, "Backend -> Failed creating root signature!");
                return 0;
            }

            // Indirect signature.
            if (shaderDesc.layout.hasGLDrawID)
            {
                try
                {
                    D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2]     = {};
                    argumentDescs[0].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
                    argumentDescs[0].Constant.RootParameterIndex      = drawIDRootParamIndex;
                    argumentDescs[0].Constant.DestOffsetIn32BitValues = 0;
                    argumentDescs[0].Constant.Num32BitValuesToSet     = 1;
                    argumentDescs[1].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

                    D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
                    commandSignatureDesc.pArgumentDescs               = argumentDescs;
                    commandSignatureDesc.NumArgumentDescs             = _countof(argumentDescs);
                    commandSignatureDesc.ByteStride                   = sizeof(IndexedIndirectCommand);

                    ThrowIfFailed(m_device->CreateCommandSignature(&commandSignatureDesc, shader.layout.rootSig.Get(), IID_PPV_ARGS(&shader.layout.indirectIndexedSig)));

                    D3D12_INDIRECT_ARGUMENT_DESC argumentDescs2[2]    = {};
                    argumentDescs[0].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
                    argumentDescs[0].Constant.RootParameterIndex      = drawIDRootParamIndex;
                    argumentDescs[0].Constant.DestOffsetIn32BitValues = 0;
                    argumentDescs[0].Constant.Num32BitValuesToSet     = 1;
                    argumentDescs[1].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

                    D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc2 = {};
                    commandSignatureDesc.pArgumentDescs                = argumentDescs2;
                    commandSignatureDesc.NumArgumentDescs              = _countof(argumentDescs2);
                    commandSignatureDesc.ByteStride                    = sizeof(IndirectCommand);

                    ThrowIfFailed(m_device->CreateCommandSignature(&commandSignatureDesc2, shader.layout.rootSig.Get(), IID_PPV_ARGS(&shader.layout.indirectDrawSig)));
                }
                catch (HrException e)
                {
                    DX12_THROW(e, "Backend -> Exception when creating command signature! %s", e.what());
                    return 0;
                }
            }
        }
        else
        {
            auto& lyt     = m_pipelineLayouts.GetItemR(shaderDesc.customPipelineLayout);
            shader.layout = lyt;
        }

        // done with it.
        if (shader.isCompute)
        {
            const auto& blob = shaderDesc.stages.begin()->second;

            D3D12_COMPUTE_PIPELINE_STATE_DESC cpsd = {};
            cpsd.pRootSignature                    = shader.layout.rootSig.Get();
            cpsd.CS                                = {blob.ptr, blob.size};
            cpsd.NodeMask                          = 0;
            m_device->CreateComputePipelineState(&cpsd, IID_PPV_ARGS(&shader.pso));
            return m_shaders.AddItem(shader);
        }

        LINAGX_VEC<D3D12_INPUT_ELEMENT_DESC> inputLayout;

        uint32 i = 0;
        for (const auto& input : shaderDesc.layout.vertexInputs)
        {
            D3D12_INPUT_ELEMENT_DESC desc = D3D12_INPUT_ELEMENT_DESC{};
            desc.SemanticName             = "TEXCOORD";
            desc.SemanticIndex            = i++;
            desc.Format                   = GetDXFormat(input.format);
            desc.InputSlot                = 0;
            desc.AlignedByteOffset        = static_cast<uint32>(input.offset);
            desc.InputSlotClass           = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            desc.InstanceDataStepRate     = 0;
            inputLayout.push_back(desc);
        }

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

        if (!inputLayout.empty())
            psoDesc.InputLayout = {&inputLayout[0], static_cast<UINT>(inputLayout.size())};

        D3D12_BLEND_DESC blendDesc       = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        blendDesc.AlphaToCoverageEnable  = shaderDesc.alphaToCoverage;
        blendDesc.IndependentBlendEnable = false;

        const uint32 attachmentCount = static_cast<uint32>(shaderDesc.colorAttachments.size());

        for (uint32 i = 0; i < attachmentCount; i++)
        {
            const auto& att                          = shaderDesc.colorAttachments[i];
            blendDesc.RenderTarget[i].BlendEnable    = att.blendAttachment.blendEnabled;
            blendDesc.RenderTarget[i].SrcBlend       = GetDXBlend(att.blendAttachment.srcColorBlendFactor);
            blendDesc.RenderTarget[i].DestBlend      = GetDXBlend(att.blendAttachment.dstColorBlendFactor);
            blendDesc.RenderTarget[i].SrcBlendAlpha  = GetDXBlend(att.blendAttachment.srcAlphaBlendFactor);
            blendDesc.RenderTarget[i].DestBlendAlpha = GetDXBlend(att.blendAttachment.dstAlphaBlendFactor);
            blendDesc.RenderTarget[i].BlendOp        = GetDXBlendOp(att.blendAttachment.colorBlendOp);
            blendDesc.RenderTarget[i].BlendOpAlpha   = GetDXBlendOp(att.blendAttachment.alphaBlendOp);
            blendDesc.RenderTarget[i].LogicOpEnable  = shaderDesc.blendLogicOpEnabled;
            blendDesc.RenderTarget[i].LogicOp        = GetDXLogicOp(shaderDesc.blendLogicOp);

            blendDesc.RenderTarget[i].RenderTargetWriteMask = 0;
            for (auto mask : att.blendAttachment.componentFlags)
                blendDesc.RenderTarget[i].RenderTargetWriteMask |= GetDXColorWriteMask(mask);

            psoDesc.RTVFormats[i] = GetDXFormat(att.format);
        }

        const D3D12_DEPTH_STENCILOP_DESC backStencil  = {GetDXStencilOp(shaderDesc.depthStencilDesc.backStencilState.failOp), GetDXStencilOp(shaderDesc.depthStencilDesc.backStencilState.depthFailOp), GetDXStencilOp(shaderDesc.depthStencilDesc.backStencilState.passOp), GetDXCompareOp(shaderDesc.depthStencilDesc.backStencilState.compareOp)};
        const D3D12_DEPTH_STENCILOP_DESC frontStencil = {GetDXStencilOp(shaderDesc.depthStencilDesc.frontStencilState.failOp), GetDXStencilOp(shaderDesc.depthStencilDesc.frontStencilState.depthFailOp), GetDXStencilOp(shaderDesc.depthStencilDesc.frontStencilState.passOp), GetDXCompareOp(shaderDesc.depthStencilDesc.frontStencilState.compareOp)};

        psoDesc.pRootSignature                        = shader.layout.rootSig.Get();
        psoDesc.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState                            = blendDesc;
        psoDesc.DepthStencilState.DepthEnable         = shaderDesc.depthStencilDesc.depthTest;
        psoDesc.DepthStencilState.DepthWriteMask      = shaderDesc.depthStencilDesc.depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        psoDesc.DepthStencilState.DepthFunc           = GetDXCompareOp(shaderDesc.depthStencilDesc.depthCompare);
        psoDesc.DepthStencilState.StencilEnable       = shaderDesc.depthStencilDesc.stencilEnabled;
        psoDesc.DepthStencilState.StencilReadMask     = shaderDesc.depthStencilDesc.stencilCompareMask;
        psoDesc.DepthStencilState.StencilWriteMask    = shaderDesc.depthStencilDesc.stencilWriteMask;
        psoDesc.DepthStencilState.FrontFace           = backStencil;
        psoDesc.DepthStencilState.BackFace            = frontStencil;
        psoDesc.SampleMask                            = UINT_MAX;
        psoDesc.PrimitiveTopologyType                 = GetDXTopologyType(shaderDesc.topology);
        psoDesc.NumRenderTargets                      = attachmentCount;
        psoDesc.SampleDesc.Count                      = 1;
        psoDesc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode              = GetDXCullMode(shaderDesc.cullMode);
        psoDesc.RasterizerState.FrontCounterClockwise = shaderDesc.frontFace == FrontFace::CCW;
        psoDesc.DSVFormat                             = (shaderDesc.depthStencilDesc.depthWrite || shaderDesc.depthStencilDesc.stencilEnabled) ? GetDXFormat(shaderDesc.depthStencilDesc.depthStencilAttachmentFormat) : DXGI_FORMAT_UNKNOWN;

        for (const auto& [stg, blob] : shaderDesc.stages)
        {
            const void*  byteCode = (void*)blob.ptr;
            const SIZE_T length   = static_cast<SIZE_T>(blob.size);

            if (stg == ShaderStage::Vertex)
            {
                psoDesc.VS.pShaderBytecode = byteCode;
                psoDesc.VS.BytecodeLength  = length;
            }
            else if (stg == ShaderStage::Fragment)
            {
                psoDesc.PS.pShaderBytecode = byteCode;
                psoDesc.PS.BytecodeLength  = length;
            }
            else if (stg == ShaderStage::Geometry)
            {
                psoDesc.GS.pShaderBytecode = byteCode;
                psoDesc.GS.BytecodeLength  = length;
            }
            else if (stg == ShaderStage::Tesellation)
            {
                psoDesc.HS.pShaderBytecode = byteCode;
                psoDesc.HS.BytecodeLength  = length;
            }
            else if (stg == ShaderStage::Compute)
            {
                LOGA(false, "!!");
            }
        }
        try
        {
            ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&shader.pso)));
            NAME_DX12_OBJECT_CSTR(shader.pso, shaderDesc.debugName);
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when creating PSO! %s", e.what());
            return 0;
        }

        return m_shaders.AddItem(shader);
    }

    void DX12Backend::DestroyShader(uint16 handle)
    {
        auto& shader = m_shaders.GetItemR(handle);
        if (!shader.isValid)
        {
            LOGE("Backend -> Shader to be destroyed is not valid!");
            return;
        }

        shader.isValid = false;
        shader.pso.Reset();
        m_shaders.RemoveItem(handle);
    }

    uint32 DX12Backend::CreateTexture(const TextureDesc& txtDesc)
    {
        DX12Texture2D item = {};
        item.isValid       = true;
        item.desc          = txtDesc;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment           = 0;
        resourceDesc.Width               = txtDesc.width;
        resourceDesc.Height              = txtDesc.height;
        resourceDesc.DepthOrArraySize    = txtDesc.arrayLength;
        resourceDesc.MipLevels           = txtDesc.mipLevels;
        resourceDesc.SampleDesc.Count    = 1;
        resourceDesc.SampleDesc.Quality  = 0;
        resourceDesc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
        resourceDesc.Format              = GetDXFormat(txtDesc.format);

        auto allocationInfo    = m_device->GetResourceAllocationInfo(0, 1, &resourceDesc);
        item.requiredAlignment = allocationInfo.Alignment;

        D3D12MA::ALLOCATION_DESC allocationDesc = {};
        allocationDesc.HeapType                 = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
        D3D12_CLEAR_VALUE*    clear = NULL;
        const float           cc[]{0, 0, 0, 1};
        auto                  depthClear = CD3DX12_CLEAR_VALUE(GetDXFormat(txtDesc.format), txtDesc.depthClear, txtDesc.stencilClear);
        auto                  colorClear = CD3DX12_CLEAR_VALUE(GetDXFormat(txtDesc.format), cc);

        if (txtDesc.usage == TextureUsage::DepthStencilTexture)
        {
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            state              = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            clear              = &depthClear;

            if (!txtDesc.sampled)
                resourceDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        }
        else if (txtDesc.usage == TextureUsage::ColorTextureRenderTarget)
        {
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            state              = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

            if (txtDesc.sampledOutsideFragment)
                state |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

            clear = &colorClear;
        }
        else if (txtDesc.usage == TextureUsage::ColorTexture)
        {
            state = D3D12_RESOURCE_STATE_COMMON;

            if (txtDesc.sampledOutsideFragment)
                state |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        }

        item.state = state;

        try
        {
            ThrowIfFailed(m_dx12Allocator->CreateResource(&allocationDesc, &resourceDesc, state, clear, &item.allocation, IID_NULL, NULL));
            NAME_DX12_OBJECT_CSTR(item.allocation->GetResource(), txtDesc.debugName);
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when creating a texture resource! %s", e.what());
        }

        if (txtDesc.usage == TextureUsage::ColorTexture || txtDesc.usage == TextureUsage::ColorTextureDynamic)
        {
            // Texture descriptor + SRV
            item.descriptor                         = m_textureHeap->GetNewHeapHandle();
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format                          = GetDXFormat(txtDesc.format);
            if (txtDesc.arrayLength > 1)
            {
                srvDesc.Texture2DArray.ArraySize       = txtDesc.arrayLength;
                srvDesc.Texture2DArray.MipLevels       = txtDesc.mipLevels;
                srvDesc.Texture2DArray.FirstArraySlice = 0;
                srvDesc.ViewDimension                  = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            }
            else
            {
                srvDesc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MipLevels = txtDesc.mipLevels;
            }

            m_device->CreateShaderResourceView(item.allocation->GetResource(), &srvDesc, {item.descriptor.GetCPUHandle()});
        }
        else if (txtDesc.usage == TextureUsage::DepthStencilTexture)
        {
            // DS descriptor + DSV
            item.descriptor                                = m_dsvHeap->GetNewHeapHandle();
            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
            depthStencilDesc.Format                        = GetDXFormat(txtDesc.format);
            depthStencilDesc.ViewDimension                 = D3D12_DSV_DIMENSION_TEXTURE2D;
            depthStencilDesc.Flags                         = D3D12_DSV_FLAG_NONE;
            m_device->CreateDepthStencilView(item.allocation->GetResource(), &depthStencilDesc, {item.descriptor.GetCPUHandle()});

            item.descriptor2                        = m_textureHeap->GetNewHeapHandle();
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format                          = DXGI_FORMAT_R32_FLOAT;
            srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels             = 1;
            m_device->CreateShaderResourceView(item.allocation->GetResource(), &srvDesc, {item.descriptor2.GetCPUHandle()});
        }
        else if (txtDesc.usage == TextureUsage::ColorTextureRenderTarget)
        {
            // Texture descriptor + RT descriptor + SRV + RTV
            item.descriptor                    = m_rtvHeap->GetNewHeapHandle();
            D3D12_RENDER_TARGET_VIEW_DESC desc = {};
            desc.Format                        = GetDXFormat(txtDesc.format);
            desc.ViewDimension                 = D3D12_RTV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice            = 0;
            m_device->CreateRenderTargetView(item.allocation->GetResource(), &desc, {item.descriptor.GetCPUHandle()});

            item.descriptor2                        = m_textureHeap->GetNewHeapHandle();
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format                          = GetDXFormat(txtDesc.format);
            srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels             = 1;
            m_device->CreateShaderResourceView(item.allocation->GetResource(), &srvDesc, {item.descriptor2.GetCPUHandle()});
        }

        return m_textures.AddItem(item);
    }

    void DX12Backend::DestroyTexture(uint32 handle)
    {
        auto& txt = m_textures.GetItemR(handle);
        if (!txt.isValid)
        {
            LOGE("Backend -> Texture to be destroyed is not valid!");
            return;
        }

        if (txt.isSwapchainTexture)
        {
            m_rtvHeap->FreeHeapHandle(txt.descriptor);
            txt.rawRes.Reset();
        }
        else
        {
            if (txt.desc.usage == TextureUsage::DepthStencilTexture)
                m_dsvHeap->FreeHeapHandle(txt.descriptor);
            else if (txt.desc.usage == TextureUsage::ColorTexture || txt.desc.usage == TextureUsage::ColorTextureDynamic)
                m_textureHeap->FreeHeapHandle(txt.descriptor);
            else if (txt.desc.usage == TextureUsage::ColorTextureRenderTarget)
            {
                m_rtvHeap->FreeHeapHandle(txt.descriptor);
                m_textureHeap->FreeHeapHandle(txt.descriptor2);
            }
        }

        if (txt.allocation != NULL)
        {
            txt.allocation->Release();
            txt.allocation = NULL;
        }

        txt.isValid = false;
        m_textures.RemoveItem(handle);
    }

    uint32 DX12Backend::CreateSampler(const SamplerDesc& desc)
    {
        DX12Sampler item = {};
        item.isValid     = true;

        D3D12_SAMPLER_DESC samplerDesc;
        samplerDesc.AddressU       = GetDXAddressMode(desc.mode);
        samplerDesc.AddressV       = GetDXAddressMode(desc.mode);
        samplerDesc.AddressW       = GetDXAddressMode(desc.mode);
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE;
        samplerDesc.Filter         = GetDXFilter(desc.minFilter, desc.magFilter);
        samplerDesc.MinLOD         = desc.minLod;
        samplerDesc.MaxLOD         = desc.maxLod;
        samplerDesc.MaxAnisotropy  = desc.anisotropy;
        samplerDesc.MipLODBias     = static_cast<FLOAT>(desc.mipLodBias);
        FillBorderColor(desc.borderColor, samplerDesc.BorderColor);

        item.descriptor = m_samplerHeap->GetNewHeapHandle();

        m_device->CreateSampler(&samplerDesc, {item.descriptor.GetCPUHandle()});

        return m_samplers.AddItem(item);
    }

    void DX12Backend::DestroySampler(uint32 handle)
    {
        auto& item = m_samplers.GetItemR(handle);
        if (!item.isValid)
        {
            LOGE("Backend -> Sampler to be destroyed is not valid!");
            return;
        }

        m_samplerHeap->FreeHeapHandle(item.descriptor);
        m_samplers.RemoveItem(handle);
    }

    uint32 DX12Backend::CreateResource(const ResourceDesc& desc)
    {
        const uint64 alignedSize = ALIGN_SIZE_POW(desc.size, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        uint64       finalSize   = desc.size;

        if (desc.typeHintFlags & TH_ConstantBuffer)
            finalSize = alignedSize;

        DX12Resource item  = {};
        item.heapType      = desc.heapType;
        item.isValid       = true;
        item.size          = finalSize;
        item.isGPUWritable = desc.isGPUWritable;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment           = 0;
        resourceDesc.Width               = finalSize;
        resourceDesc.Height              = 1;
        resourceDesc.DepthOrArraySize    = 1;
        resourceDesc.MipLevels           = 1;
        resourceDesc.Format              = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count    = 1;
        resourceDesc.SampleDesc.Quality  = 0;
        resourceDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

        if (item.isGPUWritable)
        {
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        ResourceHeap usedHeapType = desc.heapType;

        if (usedHeapType == ResourceHeap::CPUVisibleGPUMemory)
        {
            LOGA(GPUInfo.totalCPUVisibleGPUMemorySize != 0, "Backend -> Trying to create a host-visible gpu resource meanwhile current gpu doesn't support this!");

            NV_RESOURCE_PARAMS nvResourceParams = {};
            nvResourceParams.NVResourceFlags    = NV_D3D12_RESOURCE_FLAG_CPUVISIBLE_VIDMEM;
            auto         hp                     = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            NvAPI_Status result                 = NvAPI_D3D12_CreateCommittedResource(m_device.Get(), &hp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, NULL, &nvResourceParams, IID_PPV_ARGS(&item.cpuVisibleResource), NULL);
            LOGA(result == NvAPI_Status::NVAPI_OK, "Backend -> Host-visible gpu resource creation failed! Maybe exceeded total size?");
            NAME_DX12_OBJECT_CSTR(item.cpuVisibleResource, desc.debugName);
        }

        D3D12_RESOURCE_STATES    state          = D3D12_RESOURCE_STATE_GENERIC_READ;
        D3D12MA::ALLOCATION_DESC allocationDesc = {};

        if (usedHeapType == ResourceHeap::StagingHeap)
        {
            allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

            if (desc.typeHintFlags & TH_VertexBuffer || desc.typeHintFlags & TH_ConstantBuffer)
                state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            else if (desc.typeHintFlags & TH_IndexBuffer)
                state = D3D12_RESOURCE_STATE_INDEX_BUFFER;
            else
                state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        }
        else
        {
            allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
            state                   = D3D12_RESOURCE_STATE_COPY_DEST;
        }

        try
        {
            if (!item.cpuVisibleResource)
                ThrowIfFailed(m_dx12Allocator->CreateResource(&allocationDesc, &resourceDesc, state, NULL, &item.allocation, IID_NULL, NULL));

            item.descriptor = m_bufferHeap->GetNewHeapHandle();
            item.state      = state;

            if (desc.typeHintFlags & TH_ConstantBuffer)
            {
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbv;
                cbv.BufferLocation = GetGPUResource(item)->GetGPUVirtualAddress();
                cbv.SizeInBytes    = static_cast<UINT>(finalSize);
                m_device->CreateConstantBufferView(&cbv, {item.descriptor.GetCPUHandle()});
            }
            else if (desc.typeHintFlags & TH_StorageBuffer)
            {
                D3D12_SHADER_RESOURCE_VIEW_DESC srv;
                srv.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srv.Format                     = DXGI_FORMAT_R32_TYPELESS;
                srv.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
                srv.Buffer.FirstElement        = 0;
                srv.Buffer.NumElements         = static_cast<UINT>(finalSize / 4);
                srv.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_RAW;
                srv.Buffer.StructureByteStride = 0;
                m_device->CreateShaderResourceView(GetGPUResource(item), &srv, {item.descriptor.GetCPUHandle()});
            }

            if (desc.isGPUWritable && desc.heapType != ResourceHeap::StagingHeap)
            {
                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
                uavDesc.Format                           = DXGI_FORMAT_R32_TYPELESS;
                uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
                uavDesc.Buffer.FirstElement              = 0;
                uavDesc.Buffer.NumElements               = static_cast<UINT>(finalSize / 4);
                uavDesc.Buffer.StructureByteStride       = 0;
                uavDesc.Buffer.CounterOffsetInBytes      = 0;
                uavDesc.Buffer.Flags                     = D3D12_BUFFER_UAV_FLAG_RAW;

                item.additionalDescriptor = m_bufferHeap->GetNewHeapHandle();
                m_device->CreateUnorderedAccessView(GetGPUResource(item), NULL, &uavDesc, {item.additionalDescriptor.GetCPUHandle()});
            }
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when creating a resource! {0}", e.what());
        }

        NAME_DX12_OBJECT_CSTR(GetGPUResource(item), desc.debugName);

        return m_resources.AddItem(item);
    }

    void DX12Backend::MapResource(uint32 handle, uint8*& ptr)
    {
        auto& item = m_resources.GetItemR(handle);

        if (item.heapType == ResourceHeap::CPUVisibleGPUMemory)
        {
            try
            {
                CD3DX12_RANGE readRange(0, 0);
                ThrowIfFailed(item.cpuVisibleResource->Map(0, &readRange, reinterpret_cast<void**>(&ptr)));
            }
            catch (HrException e)
            {
                DX12_THROW(e, "Backend -> Failed mapping resource! {0}", e.what());
            }
        }
        else if (item.heapType == ResourceHeap::GPUOnly)
        {
            LOGA(false, "Backend -> Can not map GPU Only resource!");
        }
        else if (item.heapType == ResourceHeap::StagingHeap)
        {
            try
            {
                CD3DX12_RANGE readRange(0, 0);
                ThrowIfFailed(item.allocation->GetResource()->Map(0, &readRange, reinterpret_cast<void**>(&ptr)));
            }
            catch (HrException e)
            {
                DX12_THROW(e, "Backend -> Failed mapping resource! {0}", e.what());
            }
        }

        item.isMapped = true;
    }

    void DX12Backend::UnmapResource(uint32 handle)
    {
        auto& item = m_resources.GetItemR(handle);

        if (!item.isMapped)
            return;

        item.isMapped = false;

        if (item.heapType == ResourceHeap::StagingHeap)
        {
            CD3DX12_RANGE readRange(0, 0);
            item.allocation->GetResource()->Unmap(0, &readRange);
        }
        else if (item.heapType == ResourceHeap::CPUVisibleGPUMemory)
        {
            CD3DX12_RANGE readRange(0, 0);
            item.cpuVisibleResource->Unmap(0, &readRange);
        }
    }

    void DX12Backend::DestroyResource(uint32 handle)
    {
        auto& res = m_resources.GetItemR(handle);
        if (!res.isValid)
        {
            LOGE("Backend -> Resource to be destroyed is not valid!");
            return;
        }

        UnmapResource(handle);

        if (res.heapType == ResourceHeap::StagingHeap || res.heapType == ResourceHeap::GPUOnly)
        {
            res.allocation->Release();
            res.allocation = nullptr;
        }
        else if (res.heapType == ResourceHeap::CPUVisibleGPUMemory)
        {
            res.cpuVisibleResource.Reset();
        }

        m_resources.RemoveItem(handle);
    }

    uint16 DX12Backend::CreateDescriptorSet(const DescriptorSetDesc& desc)
    {
        DX12DescriptorSet item = {};
        item.isValid           = true;

        const uint32 bindingCount = static_cast<uint32>(desc.bindings.size());

        for (uint32 i = 0; i < bindingCount; i++)
        {
            const DescriptorBinding& binding = desc.bindings[i];

            DX12DescriptorBinding dx12Binding = {};
            dx12Binding.lgxBinding            = binding;
            dx12Binding.binding               = i;
            dx12Binding.stages                = binding.stages;

            if (binding.type == DescriptorType::CombinedImageSampler)
            {
                dx12Binding.gpuPointer           = m_gpuHeapBuffer->GetHeapHandleBlock(binding.descriptorCount);
                dx12Binding.additionalGpuPointer = m_gpuHeapSampler->GetHeapHandleBlock(binding.descriptorCount);
            }
            else if (binding.type == DescriptorType::SeparateSampler)
                dx12Binding.gpuPointer = m_gpuHeapSampler->GetHeapHandleBlock(binding.descriptorCount);
            else
                dx12Binding.gpuPointer = m_gpuHeapBuffer->GetHeapHandleBlock(binding.descriptorCount);

            item.bindings.push_back(dx12Binding);
        }

        return m_descriptorSets.AddItem(item);
    }

    void DX12Backend::DestroyDescriptorSet(uint16 handle)
    {
        auto& item = m_descriptorSets.GetItemR(handle);
        if (!item.isValid)
        {
            LOGE("Backend -> Descriptor set to be destroyed is not valid!");
            return;
        }

        m_descriptorSets.RemoveItem(handle);
    }

    void DX12Backend::DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc)
    {
        auto& item = m_descriptorSets.GetItemR(desc.setHandle);
        LOGA(desc.binding < static_cast<uint32>(item.bindings.size()), "Backend -> Binding is not valid!");
        auto&        bindingData     = item.bindings[desc.binding];
        const uint32 descriptorCount = static_cast<uint32>(desc.buffers.size());
        LOGA(descriptorCount <= bindingData.lgxBinding.descriptorCount, "Backend -> Error updating descriptor buffer as update count exceeds the maximum descriptor count for given binding!");
        LOGA(bindingData.lgxBinding.type == DescriptorType::UBO || bindingData.lgxBinding.type == DescriptorType::SSBO, "Backend -> You can only use DescriptorUpdateBuffer with descriptors of type UBO and SSBO! Use DescriptorUpdateImage()");

        LINAGX_VEC<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
        LINAGX_VEC<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
        auto&                                   descriptorSet = m_descriptorSets.GetItemR(desc.setHandle);

        for (auto& binding : descriptorSet.bindings)
        {
            if (binding.binding == desc.binding)
            {
                for (uint32 i = 0; i < descriptorCount; i++)
                {
                    const auto& res = m_resources.GetItemR(desc.buffers[i]);

                    if (bindingData.lgxBinding.type == DescriptorType::UBO && descriptorCount == 1)
                    {
                        DescriptorHandle handle = {};
                        handle.SetGPUHandle(res.allocation->GetResource()->GetGPUVirtualAddress());
                        binding.gpuPointer = handle;
                        return;
                    }
                    else
                    {
                        if (desc.isWriteAccess)
                            srcDescriptors.push_back({res.additionalDescriptor.GetCPUHandle()});
                        else
                            srcDescriptors.push_back({res.descriptor.GetCPUHandle()});

                        destDescriptors.push_back({binding.gpuPointer.GetCPUHandle() + i * m_gpuHeapBuffer->GetDescriptorSize()});
                    }
                }
            }
        }

        m_device->CopyDescriptors(descriptorCount, destDescriptors.data(), NULL, descriptorCount, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    void DX12Backend::DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc)
    {
        auto& item = m_descriptorSets.GetItemR(desc.setHandle);
        LOGA(desc.binding < static_cast<uint32>(item.bindings.size()), "Backend -> Binding is not valid!");
        auto&        bindingData        = item.bindings[desc.binding];
        const uint32 txtDescriptorCount = static_cast<uint32>(desc.textures.size());
        const uint32 smpDescriptorCount = static_cast<uint32>(desc.samplers.size());
        LOGA(txtDescriptorCount <= bindingData.lgxBinding.descriptorCount && smpDescriptorCount <= bindingData.lgxBinding.descriptorCount, "Backend -> Error updateing descriptor buffer as update count exceeds the maximum descriptor count for given binding!");
        LOGA(bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler || bindingData.lgxBinding.type == DescriptorType::SeparateSampler || bindingData.lgxBinding.type == DescriptorType::SeparateImage, "Backend -> You can only use DescriptorUpdateImage with descriptors of type combined image sampler, separate image or separate sampler! Use DescriptorUpdateBuffer()");

        LINAGX_VEC<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptorsTxt;
        LINAGX_VEC<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptorsSampler;
        LINAGX_VEC<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptorsTxt;
        LINAGX_VEC<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptorsSampler;
        destDescriptorsTxt.resize(txtDescriptorCount);
        destDescriptorsSampler.resize(smpDescriptorCount);
        srcDescriptorsTxt.resize(txtDescriptorCount);
        srcDescriptorsSampler.resize(smpDescriptorCount);

        uint32 usedCount = 0;
        if (bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler || bindingData.lgxBinding.type == DescriptorType::SeparateImage)
            usedCount = txtDescriptorCount;
        else
            usedCount = smpDescriptorCount;

        auto& descriptorSet = m_descriptorSets.GetItemR(desc.setHandle);

        for (const auto& binding : descriptorSet.bindings)
        {
            if (binding.binding == desc.binding)
            {
                for (uint32 i = 0; i < usedCount; i++)
                {
                    if (bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler || bindingData.lgxBinding.type == DescriptorType::SeparateImage)
                    {
                        const auto& res                    = m_textures.GetItemR(desc.textures[i]);
                        const auto& srcResDescriptorHandle = (res.desc.usage == TextureUsage::ColorTextureRenderTarget || res.desc.usage == TextureUsage::DepthStencilTexture) ? res.descriptor2 : res.descriptor;

                        D3D12_CPU_DESCRIPTOR_HANDLE srcHandleTxt;
                        srcHandleTxt.ptr     = srcResDescriptorHandle.GetCPUHandle();
                        srcDescriptorsTxt[i] = srcHandleTxt;

                        D3D12_CPU_DESCRIPTOR_HANDLE txtHandle;
                        txtHandle.ptr         = binding.gpuPointer.GetCPUHandle() + i * m_gpuHeapBuffer->GetDescriptorSize();
                        destDescriptorsTxt[i] = txtHandle;
                    }

                    if (bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler || bindingData.lgxBinding.type == DescriptorType::SeparateSampler)
                    {
                        const auto& sampler                    = m_samplers.GetItemR(desc.samplers[i]);
                        const auto& srcSamplerDescriptorHandle = sampler.descriptor;

                        D3D12_CPU_DESCRIPTOR_HANDLE srcHandleSampler;
                        srcHandleSampler.ptr     = srcSamplerDescriptorHandle.GetCPUHandle();
                        srcDescriptorsSampler[i] = srcHandleSampler;

                        D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle;

                        if (bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler)
                            samplerHandle.ptr = binding.additionalGpuPointer.GetCPUHandle() + i * m_samplerHeap->GetDescriptorSize();
                        else
                            samplerHandle.ptr = binding.gpuPointer.GetCPUHandle() + i * m_samplerHeap->GetDescriptorSize();

                        destDescriptorsSampler[i] = samplerHandle;
                    }
                }
                break;
            }
        }

        if (bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler)
        {
            m_device->CopyDescriptors(usedCount, destDescriptorsTxt.data(), NULL, usedCount, srcDescriptorsTxt.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            m_device->CopyDescriptors(usedCount, destDescriptorsSampler.data(), NULL, usedCount, srcDescriptorsSampler.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        }
        else if (bindingData.lgxBinding.type == DescriptorType::SeparateImage)
        {
            m_device->CopyDescriptors(usedCount, destDescriptorsTxt.data(), NULL, usedCount, srcDescriptorsTxt.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
        else if (bindingData.lgxBinding.type == DescriptorType::SeparateSampler)
        {
            m_device->CopyDescriptors(usedCount, destDescriptorsSampler.data(), NULL, usedCount, srcDescriptorsSampler.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        }
    }

    uint16 DX12Backend::CreatePipelineLayout(const PipelineLayoutDesc& desc)
    {
        DX12PipelineLayout item = {};
        item.isValid            = true;

        LINAGX_VEC<CD3DX12_ROOT_PARAMETER1>   rootParameters;
        LINAGX_VEC<CD3DX12_DESCRIPTOR_RANGE1> ranges;

        uint32 totalDescriptors = 0;
        for (const auto& setDesc : desc.descriptorSetDescriptions)
        {
            for (auto b : setDesc.bindings)
                totalDescriptors += b.unbounded ? 1 : b.descriptorCount;
        }

        ranges.resize(totalDescriptors);
        uint32 rangeCounter = 0;
        uint32 setIndex     = 0;

        for (const auto& setDesc : desc.descriptorSetDescriptions)
        {
            ProcessDescriptorSetDesc(setDesc, setIndex, rootParameters, ranges, rangeCounter, item.rootParams);
            setIndex++;
        }

        uint32 drawIDRootParamIndex = 0;
        if (desc.indirectDrawEnabled)
        {
            uint32 maxBinding = 0;

            for (const auto& setDesc : desc.descriptorSetDescriptions)
            {
                for (const auto& binding : setDesc.bindings)
                {
                    if (binding.type == DescriptorType::UBO)
                        maxBinding++;
                }

                break;
            }

            CD3DX12_ROOT_PARAMETER1 param      = CD3DX12_ROOT_PARAMETER1{};
            D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_VERTEX;
            param.InitAsConstants(1, maxBinding, 0, D3D12_SHADER_VISIBILITY_VERTEX);
            drawIDRootParamIndex = static_cast<uint32>(rootParameters.size());
            rootParameters.push_back(param);
        }

        auto containsStage = [](LINAGX_VEC<ShaderStage>& vec, ShaderStage target) {
            for (auto stg : vec)
            {
                if (stg == target)
                    return true;
            }

            return false;
        };

        if (!desc.constantRanges.empty())
        {
            item.constantsSpace = static_cast<uint32>(desc.descriptorSetDescriptions.size());
            // item.constantsBinding = desc.descriptorSets.empty() ? 0 : static_cast<uint32>(m_descriptorSets.GetItemR(desc.descriptorSets.at(0)).bindings.size());
            item.constantsBinding = 0;

            CD3DX12_ROOT_PARAMETER1 param = CD3DX12_ROOT_PARAMETER1{};

            LINAGX_VEC<ShaderStage> stages;

            for (const auto& ct : desc.constantRanges)
            {
                for (auto stg : ct.stages)
                {
                    if (!containsStage)
                        stages.push_back(stg);
                }
            }

            D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
            if (stages.size() == 1)
                visibility = GetDXVisibility(stages[0]);

            DX12RootParamInfo paramInfo = {};
            paramInfo.rootParameter     = static_cast<uint32>(rootParameters.size());
            paramInfo.reflectionType    = DescriptorType::ConstantBlock;
            paramInfo.set               = item.constantsSpace;
            paramInfo.binding           = item.constantsBinding;
            item.rootParams.push_back(paramInfo);

            uint32 totalConstants = 0;
            for (const auto& ct : desc.constantRanges)
                totalConstants += ct.size / sizeof(uint32);

            param.InitAsConstants(totalConstants, item.constantsBinding, item.constantsSpace, visibility);
            rootParameters.push_back(param);
        }
        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig(static_cast<uint32>(rootParameters.size()), rootParameters.data(), 0, NULL, desc.isCompute ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        ComPtr<ID3DBlob>                      signatureBlob = nullptr;
        ComPtr<ID3DBlob>                      errorBlob     = nullptr;

        HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSig, &signatureBlob, &errorBlob);

        if (FAILED(hr))
        {
            LOGA(false, "Backend -> Failed serializing root signature! %s", (char*)errorBlob->GetBufferPointer());
            return 0;
        }

        hr = m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&item.rootSig));

        if (FAILED(hr))
        {
            LOGA(false, "Backend -> Failed creating root signature!");
            return 0;
        }

        if (desc.indirectDrawEnabled)
        {
            try
            {
                D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2]     = {};
                argumentDescs[0].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
                argumentDescs[0].Constant.RootParameterIndex      = drawIDRootParamIndex;
                argumentDescs[0].Constant.DestOffsetIn32BitValues = 0;
                argumentDescs[0].Constant.Num32BitValuesToSet     = 1;
                argumentDescs[1].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

                D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
                commandSignatureDesc.pArgumentDescs               = argumentDescs;
                commandSignatureDesc.NumArgumentDescs             = _countof(argumentDescs);
                commandSignatureDesc.ByteStride                   = sizeof(IndexedIndirectCommand);

                ThrowIfFailed(m_device->CreateCommandSignature(&commandSignatureDesc, item.rootSig.Get(), IID_PPV_ARGS(&item.indirectIndexedSig)));

                D3D12_INDIRECT_ARGUMENT_DESC argumentDescs2[2]    = {};
                argumentDescs[0].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
                argumentDescs[0].Constant.RootParameterIndex      = drawIDRootParamIndex;
                argumentDescs[0].Constant.DestOffsetIn32BitValues = 0;
                argumentDescs[0].Constant.Num32BitValuesToSet     = 1;
                argumentDescs[1].Type                             = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

                D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc2 = {};
                commandSignatureDesc.pArgumentDescs                = argumentDescs2;
                commandSignatureDesc.NumArgumentDescs              = _countof(argumentDescs2);
                commandSignatureDesc.ByteStride                    = sizeof(IndirectCommand);

                ThrowIfFailed(m_device->CreateCommandSignature(&commandSignatureDesc2, item.rootSig.Get(), IID_PPV_ARGS(&item.indirectDrawSig)));
            }
            catch (HrException e)
            {
                DX12_THROW(e, "Backend -> Exception when creating command signature! %s", e.what());
                return 0;
            }
        }

        return m_pipelineLayouts.AddItem(item);
    }

    void DX12Backend::DestroyPipelineLayout(uint16 layout)
    {
        auto& lyt = m_pipelineLayouts.GetItemR(layout);
        if (!lyt.isValid)
        {
            LOGE("Backend -> Pipeline Layout to be destroyed is not valid!");
            return;
        }

        lyt.rootSig.Reset();

        m_pipelineLayouts.RemoveItem(layout);
    }

    void DX12Backend::DX12Exception(HrException e)
    {
        LOGE("Backend -> Exception: %s", e.what());
        _ASSERT(false);
    }

    ID3D12Resource* DX12Backend::GetGPUResource(const DX12Resource& res)
    {
        if (res.heapType == ResourceHeap::CPUVisibleGPUMemory)
            return res.cpuVisibleResource.Get();
        else
            return res.allocation->GetResource();
    }

    uint16 DX12Backend::CreateFence()
    {
        const uint16 handle = m_fences.AddItem(ComPtr<ID3D12Fence>());
        auto&        fence  = m_fences.GetItemR(handle);

        try
        {
            ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend-> Exception when creating a fence! {0}", e.what());
        }

        return handle;
    }

    void DX12Backend::DestroyFence(uint16 handle)
    {
        auto& fence = m_fences.GetItemR(handle);
        fence.Reset();
        m_fences.RemoveItem(handle);
    }

    void DX12Backend::WaitForFences(ID3D12Fence* fence, uint64 frameFenceValue)
    {
        const UINT64 lastFence = fence->GetCompletedValue();

        if (lastFence < frameFenceValue)
        {
            try
            {
                HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                if (eventHandle == nullptr)
                {
                    ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
                }
                else
                {
                    ThrowIfFailed(fence->SetEventOnCompletion(frameFenceValue, eventHandle));
                    WaitForSingleObject(eventHandle, INFINITE);
                    CloseHandle(eventHandle);
                }
            }
            catch (HrException e)
            {
                LOGE("Backend -> Exception when waiting for a fence! {0}", e.what());
            }
        }
    }

    bool DX12Backend::Initialize(const InitInfo& initInfo)
    {
        m_initInfo = initInfo;

        try
        {
            {
                UINT dxgiFactoryFlags = 0;

#ifndef NDEBUG
                ComPtr<ID3D12Debug> debugController;
                if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
                {
                    debugController->EnableDebugLayer();

                    // Enable additional debug layers.
                    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
                }
                else
                {
                    LOGE("Backend -> Failed enabling debug layers!");
                }
#endif

                ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

                ComPtr<IDXGIFactory5> factory5;
                HRESULT               facres       = m_factory.As(&factory5);
                BOOL                  allowTearing = FALSE;
                if (SUCCEEDED(facres))
                {
                    facres = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
                }

                m_allowTearing = SUCCEEDED(facres) && allowTearing && Config.dx12Config.allowTearing;
            }

            // Choose gpu & create device
            {
                if (initInfo.gpu == PreferredGPUType::CPU)
                {
                    ComPtr<IDXGIAdapter> warpAdapter;
                    ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
                    ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
                }
                else
                {
                    GetHardwareAdapter(m_factory.Get(), &m_adapter, initInfo.gpu);

                    ThrowIfFailed(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
                }
            }

#ifndef NDEBUG
            // Dbg callback
            {
                ID3D12InfoQueue1* infoQueue = nullptr;
                if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
                {
                    infoQueue->RegisterMessageCallback(&MessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &msgCallback);
                }
            }
#endif

            // Allocator
            {
                D3D12MA::ALLOCATOR_DESC allocatorDesc;
                allocatorDesc.pDevice              = m_device.Get();
                allocatorDesc.PreferredBlockSize   = 0;
                allocatorDesc.Flags                = D3D12MA::ALLOCATOR_FLAG_NONE;
                allocatorDesc.pAdapter             = m_adapter.Get();
                allocatorDesc.pAllocationCallbacks = NULL;
                ThrowIfFailed(D3D12MA::CreateAllocator(&allocatorDesc, &m_dx12Allocator));
            }

            // DXC
            {
                HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_idxcLib));
                if (FAILED(hr))
                {
                    LOGE("Backend -> Failed to create DXC library!");
                }
            }

            // Queue
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

            // Heaps
            {
                m_bufferHeap     = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_initInfo.gpuLimits.bufferLimit);
                m_textureHeap    = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_initInfo.gpuLimits.textureLimit);
                m_dsvHeap        = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_initInfo.gpuLimits.textureLimit);
                m_rtvHeap        = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_initInfo.gpuLimits.textureLimit);
                m_samplerHeap    = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_initInfo.gpuLimits.samplerLimit);
                m_gpuHeapBuffer  = new DX12HeapGPU(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_initInfo.gpuLimits.bufferLimit, true);
                m_gpuHeapSampler = new DX12HeapGPU(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_initInfo.gpuLimits.samplerLimit, true);
            }

            // Command functions
            {
                BACKEND_BIND_COMMANDS(DX12Backend);
            }

            // Per frame
            {
                for (uint32 i = 0; i < initInfo.framesInFlight; i++)
                {
                    DX12PerFrameData pfd = {};
                    m_perFrameData.push_back(pfd);
                }
            }

            // GPU Support
            {
                NvU64        totalBytes = 0, freeBytes = 0;
                NvAPI_Status result = NvAPI_D3D12_QueryCpuVisibleVidmem(m_device.Get(), &totalBytes, &freeBytes);
                if (result == NvAPI_Status::NVAPI_OK)
                {
                    GPUInfo.totalCPUVisibleGPUMemorySize = freeBytes;
                }

                const uint32 last = static_cast<uint32>(Format::FORMAT_MAX);

                for (uint32 i = 0; i < last; i++)
                {
                    const Format lgxFormat = static_cast<Format>(i);

                    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {GetDXFormat(lgxFormat), D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE};

                    ThrowIfFailed(m_device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport)));

                    if ((formatSupport.Support1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D) != 0)
                        GPUInfo.supportedImageFormats.push_back(lgxFormat);
                }

                for (auto& f : m_initInfo.checkForFormatSupport)
                {
                    auto it = std::find_if(GPUInfo.supportedImageFormats.begin(), GPUInfo.supportedImageFormats.end(), [&](Format format) { return f == format; });
                    if (it == GPUInfo.supportedImageFormats.end())
                        LOGE("Backend -> Required format is not supported by the GPU device! %d", static_cast<int>(f));
                }
            }

            GPUInfo.minConstantBufferOffsetAlignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when pre-initializating renderer! %s", e.what());
        }

        return true;
    }

    void DX12Backend::Shutdown()
    {
        DestroyQueue(GetPrimaryQueue(CommandType::Graphics));
        DestroyQueue(GetPrimaryQueue(CommandType::Transfer));
        DestroyQueue(GetPrimaryQueue(CommandType::Compute));

        for (auto& swp : m_swapchains)
        {
            LOGA(!swp.isValid, "Backend -> Some swapchains were not destroyed!");
        }

        for (auto& shader : m_shaders)
        {
            LOGA(!shader.isValid, "Backend -> Some shaders were not destroyed!");
        }

        for (auto& txt : m_textures)
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
            LOGA(!r.isValid, "Backend ->Some semaphores were not destroyed!");
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

#ifndef NDEBUG
        ID3D12InfoQueue1* infoQueue = nullptr;
        if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
        {
            infoQueue->UnregisterMessageCallback(msgCallback);
        }
#endif
        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            auto& pfd = m_perFrameData[i];
        }

        delete m_gpuHeapBuffer;
        delete m_gpuHeapSampler;

        m_dx12Allocator->Release();
        m_idxcLib.Reset();
        m_device.Reset();

        delete m_bufferHeap;
        delete m_textureHeap;
        delete m_dsvHeap;
        delete m_rtvHeap;
        delete m_samplerHeap;
    }

    void DX12Backend::Join()
    {
        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            const auto& frame = m_perFrameData[i];
            for (auto& q : m_queues)
            {
                if (!q.isValid || q.type != CommandType::Graphics)
                    continue;

                WaitForFences(q.frameFences[i].Get(), q.storedFenceValues[i]);
            }
        }
    }

    uint32 DX12Backend::CreateCommandStream(CommandType cmdType)
    {
        DX12CommandStream item = {};
        item.isValid           = true;
        item.type              = cmdType;

        try
        {
            ThrowIfFailed(m_device->CreateCommandAllocator(GetDXCommandType(cmdType), IID_PPV_ARGS(item.allocator.GetAddressOf())));
            ThrowIfFailed(m_device->CreateCommandList(0, GetDXCommandType(cmdType), item.allocator.Get(), nullptr, IID_PPV_ARGS(item.list.GetAddressOf())));
            ThrowIfFailed(item.list->Close());
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when creating a command allocator! %s", e.what());
        }

        return m_cmdStreams.AddItem(item);
    }

    void DX12Backend::StartFrame(uint32 frameIndex)
    {
        m_submissionPerFrame = 0;
        m_currentFrameIndex  = frameIndex;

        for (auto& q : m_queues)
        {
            if (!q.isValid || q.type != CommandType::Graphics)
                continue;

            WaitForFences(q.frameFences[m_currentFrameIndex].Get(), q.storedFenceValues[m_currentFrameIndex]);
        }

        const uint32 next = m_cmdStreams.GetNextFreeID();
        for (uint32 i = 0; i < next; i++)
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

            for (auto it = cs.adjustedBuffers.begin(); it != cs.adjustedBuffers.end();)
            {
                if (PerformanceStats.totalFrames > it->second + m_initInfo.framesInFlight + 1)
                {
                    LINAGX_FREE(it->first);
                    it = cs.adjustedBuffers.erase(it);
                }
                else
                    ++it;
            }
        }
    }

    void DX12Backend::DestroyCommandStream(uint32 handle)
    {
        auto& stream = m_cmdStreams.GetItemR(handle);
        if (!stream.isValid)
        {
            LOGE("Backend -> Command Stream to be destroyed is not valid!");
            return;
        }

        for (const auto& [id, frame] : stream.intermediateResources)
            DestroyResource(id);

        stream.isValid = false;
        stream.list.Reset();
        stream.allocator.Reset();

        m_cmdStreams.RemoveItem(handle);
    }

    void DX12Backend::CloseCommandStreams(CommandStream** streams, uint32 streamCount)
    {
        for (uint32 i = 0; i < streamCount; i++)
        {
            auto  stream    = streams[i];
            auto& sr        = m_cmdStreams.GetItemR(stream->m_gpuHandle);
            auto  list      = sr.list;
            auto  allocator = sr.allocator;

            if (stream->m_commandCount == 0)
                continue;

            try
            {
                ThrowIfFailed(allocator->Reset());
                ThrowIfFailed(list->Reset(allocator.Get(), nullptr));
                sr.boundShader        = 0;
                sr.boundRootSignature = nullptr;

                if (sr.type == CommandType::Graphics || sr.type == CommandType::Compute)
                {
                    ID3D12DescriptorHeap* heaps[] = {m_gpuHeapBuffer->GetHeap(), m_gpuHeapSampler->GetHeap()};
                    list->SetDescriptorHeaps(_countof(heaps), heaps);
                }
            }
            catch (HrException e)
            {
                DX12_THROW(e, "Backend-> Exception when resetting a command list! %s", e.what());
            }

            for (uint32 i = 0; i < stream->m_commandCount; i++)
            {
                uint8*        data = stream->m_commands[i];
                LINAGX_TYPEID tid  = 0;
                LINAGX_MEMCPY(&tid, data, sizeof(LINAGX_TYPEID));
                const size_t increment = sizeof(LINAGX_TYPEID);
                uint8*       cmd       = data + increment;
                (this->*m_cmdFunctions[tid])(cmd, sr);
                // TODO: tids -> consecutive into arr
            }

            try
            {
                ThrowIfFailed(list->Close());
            }
            catch (HrException e)
            {
                DX12_THROW(e, "Backend -> Exception when closing a command list! %s", e.what());
            }
        }
    }

    void DX12Backend::SubmitCommandStreams(const SubmitDesc& desc)
    {
        auto& queue = m_queues.GetItemR(desc.targetQueue);

        try
        {
            LINAGX_VEC<ID3D12CommandList*> _lists;

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
                _lists.push_back(str.list.Get());
            }

            if (desc.useWait)
            {
                for (uint32 i = 0; i < desc.waitCount; i++)
                    queue.queue->Wait(m_userSemaphores.GetItem(desc.waitSemaphores[i]).ptr.Get(), desc.waitValues[i]);
            }

            ID3D12CommandList* const* data = _lists.data();
            queue.queue->ExecuteCommandLists(desc.streamCount, data);

            if (desc.useSignal)
            {
                for (uint32 i = 0; i < desc.signalCount; i++)
                    queue.queue->Signal(m_userSemaphores.GetItem(desc.signalSemaphores[i]).ptr.Get(), desc.signalValues[i]);
            }

            m_submissionPerFrame.store(m_submissionPerFrame + 1);
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend -> Exception when executing operations on the queue! %s", e.what());
        }
    }

    uint8 DX12Backend::CreateQueue(const QueueDesc& desc)
    {
        DX12Queue item;
        item.isValid = true;
        item.type    = desc.type;

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type                     = GetDXCommandType(desc.type);
        ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&item.queue)));
        NAME_DX12_OBJECT_CSTR(item.queue, desc.debugName);

        item.frameFences.resize(m_initInfo.framesInFlight);
        item.storedFenceValues.resize(m_initInfo.framesInFlight);
        // item.inUse = new std::atomic_flag();

        try
        {
            for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
                ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&item.frameFences[i])));
        }
        catch (HrException e)
        {
            DX12_THROW(e, "Backend-> Exception when creating a fence! {0}", e.what());
        }

        return m_queues.AddItem(item);
    }

    void DX12Backend::DestroyQueue(uint8 queue)
    {
        auto& item = m_queues.GetItemR(queue);
        if (!item.isValid)
        {
            LOGE("Backend -> Queue to be destroyed is not valid!");
            return;
        }

        // delete item.inUse;

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
            item.frameFences[i].Reset();

        item.queue.Reset();

        m_queues.RemoveItem(queue);
    }

    uint8 DX12Backend::GetPrimaryQueue(CommandType type)
    {
        LOGA(type != CommandType::Secondary, "Backend -> No queues of type Secondary exists, use either Graphics, Transfer or Compute!");
        return m_primaryQueues[type];
    }

    void DX12Backend::Present(const PresentDesc& present)
    {
        for (uint32 i = 0; i < present.swapchainCount; i++)
        {
            auto& swp = m_swapchains.GetItemR(present.swapchains[i]);

            if (swp.width == 0 || swp.height == 0)
                continue;

            if (!swp.isActive)
            {
                LOGE("Trying to present an inactive swapchain!");
                continue;
            }

            try
            {
                UINT   flags    = (m_allowTearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;
                uint32 interval = 0;

                if (swp.vsync == VsyncMode::EveryVBlank)
                {
                    interval = 1;
                    flags    = 0;
                }
                else if (swp.vsync == VsyncMode::EverySecondVBlank)
                {
                    interval = 2;
                    flags    = 0;
                }

                ThrowIfFailed(swp.ptr->Present(interval, flags));
                swp._imageIndex = swp.ptr->GetCurrentBackBufferIndex();

                // DXGI_FRAME_STATISTICS FrameStatistics;
                // swp.ptr->GetFrameStatistics(&FrameStatistics);
                //
                // if (FrameStatistics.PresentCount > m_previousPresentCount)
                // {
                //     if (m_previousRefreshCount > 0 && (FrameStatistics.PresentRefreshCount - m_previousRefreshCount) > (FrameStatistics.PresentCount - m_previousPresentCount))
                //     {
                //         ++m_glitchCount;
                //         interval = 0;
                //     }
                // }
                //
                // m_previousPresentCount = FrameStatistics.PresentCount;
                // m_previousRefreshCount = FrameStatistics.SyncRefreshCount;
            }
            catch (HrException& e)
            {
                DX12_THROW(e, "Backend -> Present engine error! {0}", e.what());
            }
        }
    }

    void DX12Backend::EndFrame()
    {
        LOGA((m_submissionPerFrame < m_initInfo.gpuLimits.maxSubmitsPerFrame), "Backend -> Exceeded maximum submissions per frame! Please increase the limit.");

        // Increase & signal, we'll wait for this value next time we are starting this frame index.
        for (auto& q : m_queues)
        {
            if (!q.isValid || q.type != CommandType::Graphics)
                continue;

            q.frameFenceValue++;
            q.storedFenceValues[m_currentFrameIndex] = q.frameFenceValue;
            q.queue->Signal(q.frameFences[m_currentFrameIndex].Get(), q.frameFenceValue);
        }
    }

    void DX12Backend::CMD_BeginRenderPass(uint8* data, DX12CommandStream& stream)
    {
        CMDBeginRenderPass* begin = reinterpret_cast<CMDBeginRenderPass*>(data);

        LINAGX_VEC<D3D12_RENDER_PASS_RENDER_TARGET_DESC> colorAttDescs;
        colorAttDescs.resize(begin->colorAttachmentCount);

        for (uint32 i = 0; i < begin->colorAttachmentCount; i++)
        {
            const auto& att = begin->colorAttachments[i];

            if (att.isSwapchain)
            {
                const auto&  swp    = m_swapchains.GetItemR(static_cast<uint8>(att.texture));
                const uint32 handle = swp.colorTextures[swp._imageIndex];
                const auto&  txt    = m_textures.GetItemR(handle);

                CD3DX12_CLEAR_VALUE cv;
                cv.Format   = GetDXFormat(txt.desc.format);
                cv.Color[0] = att.clearColor.x;
                cv.Color[1] = att.clearColor.y;
                cv.Color[2] = att.clearColor.z;
                cv.Color[3] = att.clearColor.w;
                D3D12_RENDER_PASS_BEGINNING_ACCESS colorBegin{GetDXLoadOp(att.loadOp), {cv}};
                D3D12_RENDER_PASS_ENDING_ACCESS    colorEnd{GetDXStoreOp(att.storeOp), {}};

                colorAttDescs[i] = {txt.descriptor.GetCPUHandle(), colorBegin, colorEnd};
            }
            else
            {
                auto& txt = m_textures.GetItemR(att.texture);

                CD3DX12_CLEAR_VALUE cv;
                cv.Format   = GetDXFormat(txt.desc.format);
                cv.Color[0] = att.clearColor.x;
                cv.Color[1] = att.clearColor.y;
                cv.Color[2] = att.clearColor.z;
                cv.Color[3] = att.clearColor.w;
                D3D12_RENDER_PASS_BEGINNING_ACCESS colorBegin{GetDXLoadOp(att.loadOp), {cv}};
                D3D12_RENDER_PASS_ENDING_ACCESS    colorEnd{GetDXStoreOp(att.storeOp), {}};
                colorAttDescs[i] = {txt.descriptor.GetCPUHandle(), colorBegin, colorEnd};
            }
        }

        if (begin->depthStencilAttachment.useDepth || begin->depthStencilAttachment.useStencil)
        {
            const auto&                          depthTxt = m_textures.GetItemR(begin->depthStencilAttachment.texture);
            CD3DX12_CLEAR_VALUE                  clearDepthStencil{GetDXFormat(depthTxt.desc.format), begin->depthStencilAttachment.useDepth ? begin->depthStencilAttachment.clearDepth : 0.0f, begin->depthStencilAttachment.useStencil ? static_cast<UINT8>(begin->depthStencilAttachment.clearStencil) : (uint8)0};
            D3D12_RENDER_PASS_BEGINNING_ACCESS   depthBegin{GetDXLoadOp(begin->depthStencilAttachment.depthLoadOp), {clearDepthStencil}};
            D3D12_RENDER_PASS_BEGINNING_ACCESS   stencilBegin{GetDXLoadOp(begin->depthStencilAttachment.stencilLoadOp), {clearDepthStencil}};
            D3D12_RENDER_PASS_ENDING_ACCESS      depthEnd{GetDXStoreOp(begin->depthStencilAttachment.depthStoreOp), {}};
            D3D12_RENDER_PASS_ENDING_ACCESS      stencilEnd{GetDXStoreOp(begin->depthStencilAttachment.stencilStoreOp), {}};
            D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilDesc{depthTxt.descriptor.GetCPUHandle(), depthBegin, depthBegin, depthEnd, depthEnd};
            stream.list->BeginRenderPass(begin->colorAttachmentCount, colorAttDescs.data(), &depthStencilDesc, D3D12_RENDER_PASS_FLAG_NONE);
        }
        else
            stream.list->BeginRenderPass(begin->colorAttachmentCount, colorAttDescs.data(), NULL, D3D12_RENDER_PASS_FLAG_NONE);

        CMDSetViewport interVP = {};
        CMDSetScissors interSC = {};
        interVP.x              = begin->viewport.x;
        interVP.y              = begin->viewport.x;
        interVP.width          = begin->viewport.width;
        interVP.height         = begin->viewport.height;
        interVP.minDepth       = 0.0f;
        interVP.maxDepth       = 1.0f;
        interSC.x              = begin->scissors.x;
        interSC.y              = begin->scissors.y;
        interSC.width          = begin->scissors.width;
        interSC.height         = begin->scissors.height;
        CMD_SetViewport((uint8*)&interVP, stream);
        CMD_SetScissors((uint8*)&interSC, stream);
    }

    void DX12Backend::CMD_EndRenderPass(uint8* data, DX12CommandStream& stream)
    {
        CMDEndRenderPass* end = reinterpret_cast<CMDEndRenderPass*>(data);
        stream.list->EndRenderPass();
        stream.boundShader        = 0;
        stream.boundRootSignature = nullptr;
    }

    void DX12Backend::CMD_SetViewport(uint8* data, DX12CommandStream& stream)
    {
        CMDSetViewport* cmd = reinterpret_cast<CMDSetViewport*>(data);
        D3D12_VIEWPORT  vp;
        vp.MinDepth = cmd->minDepth;
        vp.MaxDepth = cmd->maxDepth;
        vp.Height   = static_cast<float>(cmd->height);
        vp.Width    = static_cast<float>(cmd->width);
        vp.TopLeftX = static_cast<float>(cmd->x);
        vp.TopLeftY = static_cast<float>(cmd->y);
        stream.list->RSSetViewports(1, &vp);
    }

    void DX12Backend::CMD_SetScissors(uint8* data, DX12CommandStream& stream)
    {
        CMDSetScissors* cmd = reinterpret_cast<CMDSetScissors*>(data);
        D3D12_RECT      sc;
        sc.left   = static_cast<LONG>(cmd->x);
        sc.top    = static_cast<LONG>(cmd->y);
        sc.right  = static_cast<LONG>(cmd->x + cmd->width);
        sc.bottom = static_cast<LONG>(cmd->y + cmd->height);
        stream.list->RSSetScissorRects(1, &sc);
    }

    DX12RootParamInfo* FindRootParam(LINAGX_VEC<DX12RootParamInfo>* rootParams, DescriptorType type, uint32 binding, uint32 set)
    {
        const uint32 sz = static_cast<uint32>(rootParams->size());
        for (uint32 i = 0; i < sz; i++)
        {
            auto& p = rootParams->at(i);
            if (p.reflectionType == type && p.binding == binding && p.set == set)
                return &p;
        }

        return nullptr;
    }

    void DX12Backend::BindDescriptorSets(DX12CommandStream& stream, DX12Shader& shader)
    {
        for (auto& [index, setData] : stream.boundDescriptorSets)
        {
            if (!setData.isDirty)
                continue;

            setData.isDirty = false;

            const auto& set = m_descriptorSets.GetItemR(setData.handle);

            uint32 dynamicOffsetIndexCounter = 0;

            for (const auto& binding : set.bindings)
            {
                DX12RootParamInfo* param = FindRootParam(&shader.layout.rootParams, binding.lgxBinding.type, binding.binding, index);

                if (param == nullptr)
                    continue;

                uint64 handle  = binding.gpuPointer.GetGPUHandle();
                uint64 handle2 = binding.additionalGpuPointer.GetGPUHandle();

                if (binding.lgxBinding.useDynamicOffset && (binding.lgxBinding.type == DescriptorType::UBO || binding.lgxBinding.type == DescriptorType::SSBO))
                {
                    const uint64 offset = static_cast<uint64>(setData.boundDynamicOffsets[dynamicOffsetIndexCounter++]);
                    dynamicOffsetIndexCounter++;
                    handle += offset;
                }

                if (binding.lgxBinding.type == DescriptorType::UBO && binding.lgxBinding.descriptorCount == 1)
                {
                    if (shader.isCompute)
                        stream.list->SetComputeRootConstantBufferView(param->rootParameter, handle);
                    else
                        stream.list->SetGraphicsRootConstantBufferView(param->rootParameter, handle);
                }
                else if (binding.lgxBinding.type == DescriptorType::CombinedImageSampler)
                {
                    if (shader.isCompute)
                    {
                        stream.list->SetComputeRootDescriptorTable(param->rootParameter, {handle});
                        stream.list->SetComputeRootDescriptorTable(param->rootParameter + 1, {handle2});
                    }
                    else
                    {
                        stream.list->SetGraphicsRootDescriptorTable(param->rootParameter, {handle});
                        stream.list->SetGraphicsRootDescriptorTable(param->rootParameter + 1, {handle2});
                    }
                }
                else
                {
                    if (shader.isCompute)
                        stream.list->SetComputeRootDescriptorTable(param->rootParameter, {handle});
                    else
                        stream.list->SetGraphicsRootDescriptorTable(param->rootParameter, {handle});
                }
            }
        }
    }

    void DX12Backend::CMD_BindPipeline(uint8* data, DX12CommandStream& stream)
    {
        CMDBindPipeline* cmd    = reinterpret_cast<CMDBindPipeline*>(data);
        auto&            shader = m_shaders.GetItemR(cmd->shader);

        if (!shader.isCompute)
        {
            if (stream.boundRootSignature != shader.layout.rootSig.Get())
                stream.list->SetGraphicsRootSignature(shader.layout.rootSig.Get());

            stream.list->IASetPrimitiveTopology(GetDXTopology(shader.topology));
            stream.list->SetPipelineState(shader.pso.Get());
        }
        else
        {
            if (stream.boundRootSignature != shader.layout.rootSig.Get())
                stream.list->SetComputeRootSignature(shader.layout.rootSig.Get());

            stream.list->SetPipelineState(shader.pso.Get());
        }

        stream.boundRootSignature = shader.layout.rootSig.Get();
        stream.boundShader        = cmd->shader;

        for (auto& set : stream.boundDescriptorSets)
            set.second.isDirty = true;

        BindDescriptorSets(stream, shader);
    }

    void DX12Backend::CMD_DrawInstanced(uint8* data, DX12CommandStream& stream)
    {
        CMDDrawInstanced* cmd = reinterpret_cast<CMDDrawInstanced*>(data);
        stream.list->DrawInstanced(cmd->vertexCountPerInstance, cmd->instanceCount, cmd->startVertexLocation, cmd->startInstanceLocation);
    }

    void DX12Backend::CMD_DrawIndexedInstanced(uint8* data, DX12CommandStream& stream)
    {
        CMDDrawIndexedInstanced* cmd = reinterpret_cast<CMDDrawIndexedInstanced*>(data);
        stream.list->DrawIndexedInstanced(cmd->indexCountPerInstance, cmd->instanceCount, cmd->startIndexLocation, cmd->baseVertexLocation, cmd->startInstanceLocation);
    }

    void DX12Backend::CMD_DrawIndexedIndirect(uint8* data, DX12CommandStream& stream)
    {
        CMDDrawIndexedIndirect* cmd    = reinterpret_cast<CMDDrawIndexedIndirect*>(data);
        auto&                   shader = m_shaders.GetItemR(stream.boundShader);
        auto                    buffer = GetGPUResource(m_resources.GetItemR(cmd->indirectBuffer));
        stream.list->ExecuteIndirect(shader.layout.indirectIndexedSig.Get(), cmd->count, buffer, 0, NULL, 0);
    }

    void DX12Backend::CMD_DrawIndirect(uint8* data, DX12CommandStream& stream)
    {
        CMDDrawIndirect* cmd    = reinterpret_cast<CMDDrawIndirect*>(data);
        auto&            shader = m_shaders.GetItemR(stream.boundShader);
        auto             buffer = GetGPUResource(m_resources.GetItemR(cmd->indirectBuffer));
        stream.list->ExecuteIndirect(shader.layout.indirectDrawSig.Get(), cmd->count, buffer, 0, NULL, 0);
    }

    void DX12Backend::CMD_BindVertexBuffers(uint8* data, DX12CommandStream& stream)
    {
        CMDBindVertexBuffers* cmd      = reinterpret_cast<CMDBindVertexBuffers*>(data);
        const auto&           resource = m_resources.GetItemR(cmd->resource);

        D3D12_VERTEX_BUFFER_VIEW view;
        view.BufferLocation = GetGPUResource(resource)->GetGPUVirtualAddress();
        view.SizeInBytes    = static_cast<uint32>(resource.size);
        view.StrideInBytes  = static_cast<uint32>(cmd->vertexSize);
        stream.list->IASetVertexBuffers(cmd->slot, 1, &view);
    }

    void DX12Backend::CMD_BindIndexBuffers(uint8* data, DX12CommandStream& stream)
    {
        CMDBindIndexBuffers*    cmd = reinterpret_cast<CMDBindIndexBuffers*>(data);
        const auto&             res = m_resources.GetItemR(cmd->resource);
        D3D12_INDEX_BUFFER_VIEW view;
        view.BufferLocation = GetGPUResource(res)->GetGPUVirtualAddress();
        view.SizeInBytes    = static_cast<uint32>(res.size);
        view.Format         = GetDXIndexType(cmd->indexType);
        stream.list->IASetIndexBuffer(&view);
    }

    void DX12Backend::CMD_CopyResource(uint8* data, DX12CommandStream& stream)
    {
        CMDCopyResource* cmd     = reinterpret_cast<CMDCopyResource*>(data);
        const auto&      srcRes  = m_resources.GetItemR(cmd->source);
        const auto&      destRes = m_resources.GetItemR(cmd->destination);
        stream.list->CopyResource(GetGPUResource(destRes), GetGPUResource(srcRes));
    }

    uint64 AlignUp(uint64 value, uint64 alignment)
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }

    void DX12Backend::CMD_CopyBufferToTexture2D(uint8* data, DX12CommandStream& stream)
    {
        CMDCopyBufferToTexture2D* cmd        = reinterpret_cast<CMDCopyBufferToTexture2D*>(data);
        const auto&               dstTexture = m_textures.GetItemR(cmd->destTexture);

        // Find correct size for staging resource.
        uint64 ogDataSize = 0;

        for (uint32 i = 0; i < cmd->mipLevels; i++)
        {
            const auto& buf = cmd->buffers[i];

            // Calculate the rowPitch
            const uint64 mipRowPitch = (buf.width * buf.bytesPerPixel + (D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1)) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1);

            // Calculate mip size
            const uint64 mipSz = mipRowPitch * buf.height;

            // Align the mip size
            const uint64 alignedMipSize = AlignUp(mipSz, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

            ogDataSize += alignedMipSize;
        }

        // Create staging.
        ResourceDesc stagingDesc                    = {};
        stagingDesc.size                            = ogDataSize;
        stagingDesc.typeHintFlags                   = TH_None;
        stagingDesc.heapType                        = ResourceHeap::StagingHeap;
        uint32 stagingHandle                        = CreateResource(stagingDesc);
        stream.intermediateResources[stagingHandle] = PerformanceStats.totalFrames;

        LINAGX_VEC<D3D12_SUBRESOURCE_DATA> allData;

        auto calcTd = [&](void* data, uint32 width, uint32 height, uint32 bytesPerPixel) {
            D3D12_SUBRESOURCE_DATA textureData = {};
            textureData.pData                  = data;
            textureData.RowPitch               = static_cast<LONG_PTR>((width * bytesPerPixel + (D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1)) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1));
            textureData.SlicePitch             = textureData.RowPitch * static_cast<LONG_PTR>(height);
            allData.push_back(textureData);
        };

        for (uint32 i = 0; i < cmd->mipLevels; i++)
        {
            const auto& buffer                   = cmd->buffers[i];
            void*       adjustedData             = AdjustBufferPitch(buffer.pixels, buffer.width, buffer.height, buffer.bytesPerPixel, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
            stream.adjustedBuffers[adjustedData] = PerformanceStats.totalFrames;
            calcTd(adjustedData, buffer.width, buffer.height, buffer.bytesPerPixel);
        }

        const auto& srcRes = m_resources.GetItemR(stagingHandle);
        UpdateSubresources(stream.list.Get(), dstTexture.allocation->GetResource(), GetGPUResource(srcRes), 0, static_cast<uint32>(allData.size()) * cmd->destinationSlice, static_cast<uint32>(allData.size()), allData.data());
    }

    void DX12Backend::CMD_BindDescriptorSets(uint8* data, DX12CommandStream& stream)
    {
        CMDBindDescriptorSets* cmd = reinterpret_cast<CMDBindDescriptorSets*>(data);

        uint32 dynamicOffsetCounter = 0;

        for (uint32 i = 0; i < cmd->setCount; i++)
        {
            const uint32             targetSetIndex = i + cmd->firstSet;
            const DX12DescriptorSet& set            = m_descriptorSets.GetItemR(cmd->descriptorSetHandles[i]);
            DX12BoundDescriptorSet   data           = {cmd->descriptorSetHandles[i], true};

            for (const auto& b : set.bindings)
            {
                if (b.lgxBinding.useDynamicOffset)
                    data.boundDynamicOffsets.push_back(cmd->dynamicOffsets[dynamicOffsetCounter++]);
            }

            stream.boundDescriptorSets[targetSetIndex] = data;
        }

        if (stream.boundRootSignature != nullptr)
            BindDescriptorSets(stream, m_shaders.GetItemR(stream.boundShader));
    }

    void DX12Backend::CMD_BindConstants(uint8* data, DX12CommandStream& stream)
    {
        CMDBindConstants* cmd    = reinterpret_cast<CMDBindConstants*>(data);
        auto&             shader = m_shaders.GetItemR(stream.boundShader);

        DX12RootParamInfo* param = FindRootParam(&shader.layout.rootParams, DescriptorType::ConstantBlock, shader.layout.constantsBinding, shader.layout.constantsSpace);
        stream.list->SetGraphicsRoot32BitConstants(param->rootParameter, cmd->size / sizeof(uint32), cmd->data, cmd->offset / sizeof(uint32));
    }

    void DX12Backend::CMD_Dispatch(uint8* data, DX12CommandStream& stream)
    {
        CMDDispatch* cmd = reinterpret_cast<CMDDispatch*>(data);
        stream.list->Dispatch(cmd->groupSizeX, cmd->groupSizeY, cmd->groupSizeZ);
    }

    void DX12Backend::CMD_ComputeBarrier(uint8* data, DX12CommandStream& stream)
    {
        CMDComputeBarrier* cmd = reinterpret_cast<CMDComputeBarrier*>(data);
    }

    void DX12Backend::CMD_ExecuteSecondaryStream(uint8* data, DX12CommandStream& stream)
    {
        CMDExecuteSecondaryStream* cmd    = reinterpret_cast<CMDExecuteSecondaryStream*>(data);
        auto                       bundle = m_cmdStreams.GetItemR(cmd->secondaryStream->m_gpuHandle).list.Get();
        stream.list->ExecuteBundle(bundle);
    }

    void DX12Backend::CMD_Barrier(uint8* data, DX12CommandStream& stream)
    {
        CMDBarrier* cmd = reinterpret_cast<CMDBarrier*>(data);

        LINAGX_VEC<CD3DX12_RESOURCE_BARRIER> barriers;
        barriers.reserve(cmd->textureBarrierCount + cmd->resourceBarrierCount);

        for (uint32 i = 0; i < cmd->textureBarrierCount; i++)
        {
            const auto& barrier = cmd->textureBarriers[i];

            uint32 txtIndex = barrier.texture;

            if (barrier.isSwapchain)
            {
                auto& swp = m_swapchains.GetItemR(static_cast<uint8>(barrier.texture));
                txtIndex  = swp.colorTextures[swp._imageIndex];
            }

            auto& txt      = m_textures.GetItemR(txtIndex);
            auto  newState = GetDXTextureBarrierState(barrier.toState, txt.desc.sampledOutsideFragment);

            // Will decay to common & promoted on first usage.
            if (stream.type == CommandType::Transfer && newState != D3D12_RESOURCE_STATE_COPY_DEST && newState != D3D12_RESOURCE_STATE_COPY_SOURCE)
                continue;

            auto dxRes = txt.rawRes.Get() != nullptr ? txt.rawRes.Get() : txt.allocation->GetResource();
            auto dxBar = CD3DX12_RESOURCE_BARRIER::Transition(dxRes, txt.state, newState);
            txt.state  = newState;
            barriers.push_back(dxBar);
        }

        for (uint32 i = 0; i < cmd->resourceBarrierCount; i++)
        {
            const auto& barrier  = cmd->resourceBarriers[i];
            auto&       res      = m_resources.GetItemR(barrier.resource);
            auto        dxRes    = res.cpuVisibleResource.Get() != nullptr ? res.cpuVisibleResource.Get() : res.allocation->GetResource();
            auto        newState = GetDXResourceBarrierState(barrier.toState);

            // Will decay to common & promoted on first usage.
            if (stream.type == CommandType::Transfer && newState != D3D12_RESOURCE_STATE_COPY_DEST && newState != D3D12_RESOURCE_STATE_COPY_SOURCE)
                continue;

            auto dxBar = CD3DX12_RESOURCE_BARRIER::Transition(dxRes, res.state, newState);
            res.state  = newState;
            barriers.push_back(dxBar);
        }

        if (!barriers.empty())
            stream.list->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
    }

} // namespace LinaGX

LINAGX_RESTORE_VC_WARNING()
