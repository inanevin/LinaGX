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

#include "Platform/DX12/DX12Backend.hpp"
#include "Platform/DX12/SDK/D3D12MemAlloc.h"
#include "Platform/DX12/DX12HeapGPU.hpp"
#include "Platform/DX12/DX12HeapStaging.hpp"
#include "Core/Commands.hpp"
#include "Core/Renderer.hpp"
#include "Core/CommandStream.hpp"
#include <nvapi/nvapi.h>

LINAGX_DISABLE_VC_WARNING(6387);

using Microsoft::WRL::ComPtr;

namespace LinaGX
{
#ifndef NDEBUG
#define NAME_DX12_OBJECT_CSTR(x, NAME) x->SetName(L#NAME)
#define NAME_DX12_OBJECT(x, NAME)      x->SetName(NAME)
#else
#define NAME_D3D12_OBJECT(x)
#define NAME_D3D12_OBJECT_INDEXED(x, n)
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
            return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return D3D12_BLEND_INV_DEST_ALPHA;
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
        case ShaderStage::Pixel:
            return D3D12_SHADER_VISIBILITY_PIXEL;
        case ShaderStage::Geometry:
            return D3D12_SHADER_VISIBILITY_GEOMETRY;
        case ShaderStage::Tesellation:
            return D3D12_SHADER_VISIBILITY_HULL;
        default:
            return D3D12_SHADER_VISIBILITY_ALL;
        }
    }

    D3D12_COMMAND_LIST_TYPE GetDXCommandType(QueueType type)
    {
        switch (type)
        {
        case QueueType::Graphics:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case QueueType::Transfer:
            return D3D12_COMMAND_LIST_TYPE_COPY;
        case QueueType::Compute:
            return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        default:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        }
    }

    D3D12_TEXTURE_ADDRESS_MODE GetAddressMode(SamplerAddressMode mode)
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

    D3D12_FILTER GetFilter(Filter minFilter, Filter magFilter)
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

    void MessageCallback(D3D12_MESSAGE_CATEGORY messageType, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID messageId, LPCSTR pDesc, void* pContext)
    {
        if (pDesc != NULL)
        {
            if (severity == D3D12_MESSAGE_SEVERITY_MESSAGE)
            {
                LOGT("DX12Backend -> %s", pDesc);
            }
            else if (severity == D3D12_MESSAGE_SEVERITY_INFO)
            {
                // LOGV("DX12Backend -> %s", pDesc);
            }
            else
            {
                LOGE("DX12Backend -> %s", pDesc);
            }
        }
    }

    void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, PreferredGPUType gpuType)
    {
        *ppAdapter = nullptr;

        ComPtr<IDXGIAdapter1> adapter;

        ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, gpuType == PreferredGPUType::Discrete ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER, IID_PPV_ARGS(&adapter))); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    continue;

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    char* buf = Internal::WCharToChar(desc.Description);
                    LOGT("DX12Backend -> Selected hardware adapter %s, dedicated video memory %f mb", buf, desc.DedicatedVideoMemory * 0.000001);
                    GPUInfo.dedicatedVideoMemory = static_cast<uint64>(desc.DedicatedVideoMemory);
                    GPUInfo.deviceName           = buf;
                    delete[] buf;
                    break;
                }
            }
        }

        if (adapter.Get() == nullptr)
        {
            for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    continue;

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    char* buf = Internal::WCharToChar(desc.Description);
                    LOGT("DX12Backend -> Selected hardware adapter %s, dedicated video memory %f mb", buf, desc.DedicatedVideoMemory * 0.000001);
                    GPUInfo.dedicatedVideoMemory = static_cast<uint64>(desc.DedicatedVideoMemory);
                    GPUInfo.deviceName           = buf;
                    delete[] buf;
                    break;
                }
            }
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
            DX12_THROW(e, "DX12Backend-> Exception when creating a fence! {0}", e.what());
        }

        return m_userSemaphores.AddItem(item);
    }

    void DX12Backend::DestroyUserSemaphore(uint16 handle)
    {
        auto& semaphore = m_userSemaphores.GetItemR(handle);
        if (!semaphore.isValid)
        {
            LOGE("VKBackend -> Semaphore to be destroyed is not valid!");
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
        DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
        swapchainDesc.BufferCount           = m_initInfo.backbufferCount;
        swapchainDesc.Width                 = static_cast<UINT>(desc.width);
        swapchainDesc.Height                = static_cast<UINT>(desc.height);
        swapchainDesc.Format                = GetDXFormat(m_initInfo.rtSwapchainFormat);
        swapchainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchainDesc.SampleDesc.Count      = 1;

        ComPtr<IDXGISwapChain1> swapchain;
        swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

        if (m_allowTearing)
            swapchainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        try
        {
            ThrowIfFailed(m_factory->CreateSwapChainForHwnd(m_queueData[QueueType::Graphics].queue.Get(), // Swap chain needs the queue so that it can force a flush on it.
                                                            (HWND)desc.window,
                                                            &swapchainDesc,
                                                            nullptr,
                                                            nullptr,
                                                            &swapchain));

            DX12Swapchain swp = {};
            swp.isValid       = true;
            ThrowIfFailed(swapchain.As(&swp.ptr));

            LOGT("DX12Backend -> Successfuly created swapchain with size %d x %d", desc.width, desc.height);

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
                        DX12_THROW(e, "DX12Backend -> Exception while creating render target for swapchain! %s", e.what());
                    }

                    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
                    rtvDesc.Format                        = GetDXFormat(m_initInfo.rtSwapchainFormat);
                    rtvDesc.ViewDimension                 = D3D12_RTV_DIMENSION_TEXTURE2D;
                    m_device->CreateRenderTargetView(color.rawRes.Get(), &rtvDesc, {color.descriptor.GetCPUHandle()});
                    swp.colorTextures.push_back(m_texture2Ds.AddItem(color));

                    Texture2DDesc depthDesc = {};
                    depthDesc.format        = m_initInfo.rtDepthFormat;
                    depthDesc.width         = desc.width;
                    depthDesc.height        = desc.height;
                    depthDesc.mipLevels     = 1;
                    depthDesc.usage         = Texture2DUsage::DepthStencilTexture;
                    swp.depthTextures.push_back(CreateTexture2D(depthDesc));
                }
            }

            return m_swapchains.AddItem(swp);
        }
        catch (HrException e)
        {
            DX12_THROW(e, "DX12Backend -> Exception when creating a swapchain! %s", e.what());
        }

        return 0;
    }

    void DX12Backend::DestroySwapchain(uint8 handle)
    {
        auto& swp = m_swapchains.GetItemR(handle);
        if (!swp.isValid)
        {
            LOGE("VKBackend -> Swapchain to be destroyed is not valid!");
            return;
        }

        for (auto t : swp.colorTextures)
            DestroyTexture2D(t);

        for (auto t : swp.depthTextures)
            DestroyTexture2D(t);

        swp.isValid = false;
        swp.ptr.Reset();
        m_swapchains.RemoveItem(handle);
    }

    bool DX12Backend::CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob)
    {
        try
        {
            Microsoft::WRL::ComPtr<IDxcCompiler3> idxcCompiler;

            HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&idxcCompiler));
            if (FAILED(hr))
            {
                LOGE("DX12Backend -> Failed to create DXC compiler");
                return false;
            }

            ComPtr<IDxcUtils> utils;
            hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));

            if (FAILED(hr))
            {
                LOGE("DX12Backend -> Failed to create DXC utils.");
                return false;
            }

            UINT32                   codePage = CP_UTF8;
            ComPtr<IDxcBlobEncoding> sourceBlob;
            const char*              shaderSource = source.c_str();
            m_idxcLib->CreateBlobWithEncodingFromPinned((const BYTE*)shaderSource, static_cast<UINT>(source.size()), codePage, &sourceBlob);

            const wchar_t* targetProfile = L"vs_6_0";
            if (stage == ShaderStage::Pixel)
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
                LOGE("DX12Backend -> %s", (char*)errors->GetStringPointer());
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
                        LOGE("DX12Backend -> Shader compilation failed: %s", (const char*)errorsBlob->GetBufferPointer());
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
            DX12_THROW(e, "DX12Backend -> Exception when compiling shader! %s", e.what());
            return false;
        }

        return true;
    }

    uint16 DX12Backend::CreateShader(const ShaderDesc& shaderDesc)
    {
        DX12Shader shader = {};
        shader.isValid    = true;
        shader.topology   = shaderDesc.topology;

        // Root signature.
        {
            CD3DX12_DESCRIPTOR_RANGE1 descRange[3] = {};

            // Textures & Samplers & Materials
            descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
            descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
            descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 2, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

            LINAGX_VEC<CD3DX12_ROOT_PARAMETER1> rootParameters;

            for (const auto& ubo : shaderDesc.layout.ubos)
            {
                CD3DX12_ROOT_PARAMETER1 param      = CD3DX12_ROOT_PARAMETER1{};
                D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
                if (ubo.stages.size() == 1)
                    visibility = GetDXVisibility(ubo.stages[0]);

                CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
                ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, ubo.binding, ubo.set);
                param.InitAsDescriptorTable(1, ranges, visibility);
                rootParameters.push_back(param);
            }

            for (const auto& t2d : shaderDesc.layout.combinedImageSamplers)
            {
                CD3DX12_ROOT_PARAMETER1 param = CD3DX12_ROOT_PARAMETER1{};

                D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
                if (t2d.stages.size() == 1)
                    visibility = GetDXVisibility(t2d.stages[0]);

                CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
                ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, t2d.binding, t2d.set, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
                ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, t2d.binding, t2d.set, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
                param.InitAsDescriptorTable(1, ranges, visibility);
                rootParameters.push_back(param);
            }

            for (const auto& sampler : shaderDesc.layout.samplers)
            {
                CD3DX12_ROOT_PARAMETER1 param = CD3DX12_ROOT_PARAMETER1{};

                D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
                if (sampler.stages.size() == 1)
                    visibility = GetDXVisibility(sampler.stages[0]);

                CD3DX12_DESCRIPTOR_RANGE1 range[1];
                range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, sampler.binding, sampler.set, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
                param.InitAsDescriptorTable(1, range, visibility);
                rootParameters.push_back(param);
            }

            for (const auto& ssbo : shaderDesc.layout.ssbos)
            {
                CD3DX12_ROOT_PARAMETER1 param      = CD3DX12_ROOT_PARAMETER1{};
                D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
                if (ssbo.stages.size() == 1)
                    visibility = GetDXVisibility(ssbo.stages[0]);

                CD3DX12_DESCRIPTOR_RANGE1 range[1];
                range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, ssbo.binding, ssbo.set, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
                param.InitAsDescriptorTable(1, range, visibility);
                rootParameters.push_back(param);
            }

            if (shaderDesc.layout.constantBlock.size != 0)
            {
                const auto&             ct         = shaderDesc.layout.constantBlock;
                CD3DX12_ROOT_PARAMETER1 param      = CD3DX12_ROOT_PARAMETER1{};
                D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
                if (ct.stages.size() == 1)
                    visibility = GetDXVisibility(ct.stages[0]);

                param.InitAsConstants(static_cast<uint32>(ct.members.size()), 0, 0, visibility);
                rootParameters.push_back(param);
            }

            CD3DX12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
            samplerDesc[0].Init(0, D3D12_FILTER_ANISOTROPIC);

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig(static_cast<uint32>(rootParameters.size()), rootParameters.data(), 1, samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
            ComPtr<ID3DBlob>                      signatureBlob = nullptr;
            ComPtr<ID3DBlob>                      errorBlob     = nullptr;

            HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSig, &signatureBlob, &errorBlob);

            if (FAILED(hr))
            {
                LOGA(false, "DX12Backend -> Failed serializing root signature! %s", (char*)errorBlob->GetBufferPointer());
                return 0;
            }

            hr = m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&shader.rootSig));

            if (FAILED(hr))
            {
                LOGA(false, "DX12Backend -> Failed creating root signature!");
                return 0;
            }
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

        D3D12_BLEND_DESC blendDesc                      = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        blendDesc.AlphaToCoverageEnable                 = false;
        blendDesc.IndependentBlendEnable                = false;
        blendDesc.RenderTarget[0].BlendEnable           = shaderDesc.blendAttachment.blendEnabled;
        blendDesc.RenderTarget[0].SrcBlend              = GetDXBlend(shaderDesc.blendAttachment.srcColorBlendFactor);
        blendDesc.RenderTarget[0].DestBlend             = GetDXBlend(shaderDesc.blendAttachment.dstColorBlendFactor);
        blendDesc.RenderTarget[0].SrcBlendAlpha         = GetDXBlend(shaderDesc.blendAttachment.srcAlphaBlendFactor);
        blendDesc.RenderTarget[0].DestBlendAlpha        = GetDXBlend(shaderDesc.blendAttachment.dstAlphaBlendFactor);
        blendDesc.RenderTarget[0].BlendOp               = GetDXBlendOp(shaderDesc.blendAttachment.colorBlendOp);
        blendDesc.RenderTarget[0].BlendOpAlpha          = GetDXBlendOp(shaderDesc.blendAttachment.alphaBlendOp);
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[0].LogicOpEnable         = shaderDesc.blendLogicOpEnabled;
        blendDesc.RenderTarget[0].LogicOp               = GetDXLogicOp(shaderDesc.blendLogicOp);

        const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = {D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS};
        psoDesc.pRootSignature                            = shader.rootSig.Get();
        psoDesc.RasterizerState                           = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState                                = blendDesc;
        psoDesc.DepthStencilState.DepthEnable             = shaderDesc.depthTest;
        psoDesc.DepthStencilState.DepthWriteMask          = shaderDesc.depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        psoDesc.DepthStencilState.DepthFunc               = GetDXCompareOp(shaderDesc.depthCompare);
        psoDesc.DepthStencilState.StencilEnable           = FALSE;
        psoDesc.DepthStencilState.StencilReadMask         = D3D12_DEFAULT_STENCIL_READ_MASK;
        psoDesc.DepthStencilState.StencilWriteMask        = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        psoDesc.DepthStencilState.FrontFace               = defaultStencilOp;
        psoDesc.DepthStencilState.BackFace                = defaultStencilOp;
        psoDesc.SampleMask                                = UINT_MAX;
        psoDesc.PrimitiveTopologyType                     = GetDXTopologyType(shaderDesc.topology);
        psoDesc.NumRenderTargets                          = 1;
        psoDesc.SampleDesc.Count                          = 1;
        psoDesc.RasterizerState.FillMode                  = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode                  = GetDXCullMode(shaderDesc.cullMode);
        psoDesc.RasterizerState.FrontCounterClockwise     = shaderDesc.frontFace == FrontFace::CCW;
        psoDesc.RTVFormats[0]                             = GetDXFormat(m_initInfo.rtSwapchainFormat);
        psoDesc.DSVFormat                                 = GetDXFormat(m_initInfo.rtDepthFormat);

        for (const auto& [stg, blob] : shaderDesc.stages)
        {
            const void*  byteCode = (void*)blob.ptr;
            const SIZE_T length   = static_cast<SIZE_T>(blob.size);

            if (stg == ShaderStage::Vertex)
            {
                psoDesc.VS.pShaderBytecode = byteCode;
                psoDesc.VS.BytecodeLength  = length;
            }
            else if (stg == ShaderStage::Pixel)
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
        }
        catch (HrException e)
        {
            DX12_THROW(e, "DX12Backend -> Exception when creating PSO! %s", e.what());
            return 0;
        }

        return m_shaders.AddItem(shader);
    }

    void DX12Backend::DestroyShader(uint16 handle)
    {
        auto& shader = m_shaders.GetItemR(handle);
        if (!shader.isValid)
        {
            LOGE("DX12Backend -> Shader to be destroyed is not valid!");
            return;
        }

        shader.isValid = false;
        shader.pso.Reset();
        m_shaders.RemoveItem(handle);
    }

    uint32 DX12Backend::CreateTexture2D(const Texture2DDesc& txtDesc)
    {
        DX12Texture2D item = {};
        item.usage         = txtDesc.usage;
        item.isValid       = true;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment           = 0;
        resourceDesc.Width               = txtDesc.width;
        resourceDesc.Height              = txtDesc.height;
        resourceDesc.DepthOrArraySize    = 1;
        resourceDesc.MipLevels           = txtDesc.mipLevels;
        resourceDesc.SampleDesc.Count    = 1;
        resourceDesc.SampleDesc.Quality  = 0;
        resourceDesc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
        resourceDesc.Format              = GetDXFormat(txtDesc.format);

        uint32 mostDetailedSize = txtDesc.width * txtDesc.height * txtDesc.channels;
        item.requiredAlignment  = txtDesc.usage == Texture2DUsage::ColorTexture ? (mostDetailedSize < D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT ? D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT) : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

        D3D12MA::ALLOCATION_DESC allocationDesc = {};
        allocationDesc.HeapType                 = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
        D3D12_CLEAR_VALUE*    clear = NULL;
        const float           cc[]{0, 0, 0, 1};
        auto                  depthClear = CD3DX12_CLEAR_VALUE(GetDXFormat(txtDesc.format), 1.0f, 0);
        auto                  colorClear = CD3DX12_CLEAR_VALUE(GetDXFormat(txtDesc.format), cc);

        if (txtDesc.usage == Texture2DUsage::DepthStencilTexture)
        {
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
            state              = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            clear              = &depthClear;
        }
        else if (txtDesc.usage == Texture2DUsage::ColorTextureRenderTarget)
        {
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            state              = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            clear              = &colorClear;
        }
        else if (txtDesc.usage == Texture2DUsage::ColorTexture)
        {
            state = D3D12_RESOURCE_STATE_COMMON;
        }

        try
        {
            ThrowIfFailed(m_dx12Allocator->CreateResource(&allocationDesc, &resourceDesc, state, clear, &item.allocation, IID_NULL, NULL));
        }
        catch (HrException e)
        {
            DX12_THROW(e, "DX12Backend -> Exception when creating a texture resource! %s", e.what());
        }

#ifndef NDEBUG
        item.allocation->GetResource()->SetName(txtDesc.debugName);
#endif

        if (txtDesc.usage == Texture2DUsage::ColorTexture || txtDesc.usage == Texture2DUsage::ColorTextureDynamic)
        {
            // Texture descriptor + SRV
            item.descriptor                         = m_textureHeap->GetNewHeapHandle();
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format                          = GetDXFormat(txtDesc.format);
            srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels             = txtDesc.mipLevels;
            m_device->CreateShaderResourceView(item.allocation->GetResource(), &srvDesc, {item.descriptor.GetCPUHandle()});
        }
        else if (txtDesc.usage == Texture2DUsage::DepthStencilTexture)
        {
            // DS descriptor + DSV
            item.descriptor                                = m_dsvHeap->GetNewHeapHandle();
            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
            depthStencilDesc.Format                        = GetDXFormat(txtDesc.format);
            depthStencilDesc.ViewDimension                 = D3D12_DSV_DIMENSION_TEXTURE2D;
            depthStencilDesc.Flags                         = D3D12_DSV_FLAG_NONE;
            m_device->CreateDepthStencilView(item.allocation->GetResource(), &depthStencilDesc, {item.descriptor.GetCPUHandle()});
        }
        else if (txtDesc.usage == Texture2DUsage::ColorTextureRenderTarget)
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

        return m_texture2Ds.AddItem(item);
    }

    void DX12Backend::DestroyTexture2D(uint32 handle)
    {
        auto& txt = m_texture2Ds.GetItemR(handle);
        if (!txt.isValid)
        {
            LOGE("DX12Backend -> Texture to be destroyed is not valid!");
            return;
        }

        if (txt.isSwapchainTexture)
        {
            m_rtvHeap->FreeHeapHandle(txt.descriptor);
            txt.rawRes.Reset();
        }
        else
        {
            if (txt.usage == Texture2DUsage::DepthStencilTexture)
                m_dsvHeap->FreeHeapHandle(txt.descriptor);
            else if (txt.usage == Texture2DUsage::ColorTexture || txt.usage == Texture2DUsage::ColorTextureDynamic)
                m_textureHeap->FreeHeapHandle(txt.descriptor);
            else if (txt.usage == Texture2DUsage::ColorTextureRenderTarget)
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
        m_texture2Ds.RemoveItem(handle);
    }

    uint32 DX12Backend::CreateResource(const ResourceDesc& desc)
    {
        DX12Resource item = {};
        item.heapType     = desc.heapType;
        item.isValid      = true;
        item.size         = desc.size;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment           = 0;
        resourceDesc.Width               = desc.size;
        resourceDesc.Height              = 1;
        resourceDesc.DepthOrArraySize    = 1;
        resourceDesc.MipLevels           = 1;
        resourceDesc.Format              = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count    = 1;
        resourceDesc.SampleDesc.Quality  = 0;
        resourceDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

        ResourceHeap usedHeapType = desc.heapType;

        if (usedHeapType == ResourceHeap::CPUVisibleGPUMemory)
        {
            LOGA(GPUInfo.totalCPUVisibleGPUMemorySize != 0, "DX12Backend -> Trying to create a host-visible gpu resource meanwhile current gpu doesn't support this!");

            NV_RESOURCE_PARAMS nvResourceParams = {};
            nvResourceParams.NVResourceFlags    = NV_D3D12_RESOURCE_FLAG_CPUVISIBLE_VIDMEM;
            auto         hp                     = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            NvAPI_Status result                 = NvAPI_D3D12_CreateCommittedResource(m_device.Get(), &hp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, NULL, &nvResourceParams, IID_PPV_ARGS(&item.cpuVisibleResource), NULL);
            LOGA(result == NvAPI_Status::NVAPI_OK, "DX12Backend -> Host-visible gpu resource creation failed! Maybe exceeded total size?");
            NAME_DX12_OBJECT(item.cpuVisibleResource, desc.debugName);
            return m_resources.AddItem(item);
        }

        D3D12_RESOURCE_STATES    state          = D3D12_RESOURCE_STATE_GENERIC_READ;
        D3D12MA::ALLOCATION_DESC allocationDesc = {};

        if (usedHeapType == ResourceHeap::StagingHeap)
        {
            allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

            if (desc.typeHintFlags & LGX_VertexBuffer || desc.typeHintFlags & LGX_ConstantBuffer)
                state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            else if (desc.typeHintFlags & LGX_IndexBuffer)
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
            ThrowIfFailed(m_dx12Allocator->CreateResource(&allocationDesc, &resourceDesc, state, NULL, &item.allocation, IID_NULL, NULL));
        }
        catch (HrException e)
        {
            DX12_THROW(e, "DX12Backend -> Exception when creating a resource! {0}", e.what());
        }

        NAME_DX12_OBJECT(item.allocation->GetResource(), desc.debugName);

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
                DX12_THROW(e, "DX12Backend -> Failed mapping resource! {0}", e.what());
            }
        }
        else if (item.heapType == ResourceHeap::GPUOnly)
        {
            LOGA(false, "DX12Backend -> Can not map GPU Only resource!");
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
                DX12_THROW(e, "DX12Backend -> Failed mapping resource! {0}", e.what());
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
            LOGE("VKBackend -> Resource to be destroyed is not valid!");
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

    void DX12Backend::DX12Exception(HrException e)
    {
        LOGE("DX12Backend -> Exception: %s", e.what());
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
            DX12_THROW(e, "DX12Backend-> Exception when creating a fence! {0}", e.what());
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
                LOGE("DX12Backend -> Exception when waiting for a fence! {0}", e.what());
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

                // Enable the debug layer (requires the Graphics Tools "optional feature").
                // NOTE: Enabling the debug layer after device creation will invalidate the active device.

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
                    LOGE("DX12Backend -> Failed enabling debug layers!");
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

                m_allowTearing = SUCCEEDED(facres) && allowTearing;
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
                    LOGE("DX12Backend -> Failed to create DXC library!");
                }
            }

            // Queue
            {
                auto& graphics = m_queueData[QueueType::Graphics];
                auto& transfer = m_queueData[QueueType::Transfer];
                auto& compute  = m_queueData[QueueType::Compute];

                D3D12_COMMAND_QUEUE_DESC queueDesc = {};
                queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
                queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
                ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&graphics.queue)));

                queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
                ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&transfer.queue)));

                queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
                ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&compute.queue)));

                NAME_DX12_OBJECT(graphics.queue, L"Direct Queue");
                NAME_DX12_OBJECT(transfer.queue, L"Transfer Queue");
                NAME_DX12_OBJECT(compute.queue, L"Compute Queue");

                try
                {
                    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphics.frameFence)));
                    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&transfer.frameFence)));
                    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&compute.frameFence)));
                }
                catch (HrException e)
                {
                    DX12_THROW(e, "DX12Backend-> Exception when creating a fence! {0}", e.what());
                }
            }

            // Heaps
            {
                m_bufferHeap  = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_initInfo.gpuLimits.bufferLimit);
                m_textureHeap = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_initInfo.gpuLimits.textureLimit);
                m_dsvHeap     = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_initInfo.gpuLimits.textureLimit);
                m_rtvHeap     = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_initInfo.gpuLimits.textureLimit);
                m_samplerHeap = new DX12HeapStaging(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_initInfo.gpuLimits.samplerLimit);
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
            }
        }
        catch (HrException e)
        {
            DX12_THROW(e, "DX12Backend -> Exception when pre-initializating renderer! %s", e.what());
        }

        return true;
    }

    void DX12Backend::Shutdown()
    {
        for (auto& [queue, data] : m_queueData)
        {
            data.frameFence.Reset();
        }

        for (auto& swp : m_swapchains)
        {
            LOGA(!swp.isValid, "DX12Backend -> Some swapchains were not destroyed!");
        }

        for (auto& shader : m_shaders)
        {
            LOGA(!shader.isValid, "DX12Backend -> Some shaders were not destroyed!");
        }

        for (auto& txt : m_texture2Ds)
        {
            LOGA(!txt.isValid, "DX12Backend -> Some textures were not destroyed!");
        }

        for (auto& str : m_cmdStreams)
        {
            LOGA(!str.isValid, "DX12Backend -> Some command streams were not destroyed!");
        }

        for (auto& r : m_resources)
        {
            LOGA(!r.isValid, "DX12Backend -> Some resources were not destroyed!");
        }

        for (auto& r : m_userSemaphores)
        {
            LOGA(!r.isValid, "VKBackend -> Some semaphores were not destroyed!");
        }

        ID3D12InfoQueue1* infoQueue = nullptr;
        if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
        {
            infoQueue->UnregisterMessageCallback(msgCallback);
        }

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
            WaitForFences(m_queueData[QueueType::Graphics].frameFence.Get(), frame.storedFenceGraphics);
            WaitForFences(m_queueData[QueueType::Transfer].frameFence.Get(), frame.storedFenceTransfer);
        }
    }

    uint32 DX12Backend::CreateCommandStream(QueueType cmdType)
    {
        DX12CommandStream item = {};
        item.isValid           = true;
        item.type              = cmdType;

        try
        {
            item.allocators.resize(m_initInfo.framesInFlight);
            item.lists.resize(m_initInfo.framesInFlight);

            for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
            {
                ThrowIfFailed(m_device->CreateCommandAllocator(GetDXCommandType(cmdType), IID_PPV_ARGS(item.allocators[i].GetAddressOf())));
                ThrowIfFailed(m_device->CreateCommandList(0, GetDXCommandType(cmdType), item.allocators[i].Get(), nullptr, IID_PPV_ARGS(item.lists[i].GetAddressOf())));
                ThrowIfFailed(item.lists[i]->Close());
            }
        }
        catch (HrException e)
        {
            DX12_THROW(e, "DX12Backend -> Exception when creating a command allocator! %s", e.what());
        }

        return m_cmdStreams.AddItem(item);
    }

    void DX12Backend::StartFrame(uint32 frameIndex)
    {
        m_submissionPerFrame = 0;
        m_currentFrameIndex  = frameIndex;
        auto& frame          = m_perFrameData[m_currentFrameIndex];
        WaitForFences(m_queueData[QueueType::Graphics].frameFence.Get(), frame.storedFenceGraphics);
        WaitForFences(m_queueData[QueueType::Transfer].frameFence.Get(), frame.storedFenceTransfer);
    }

    void DX12Backend::DestroyCommandStream(uint32 handle)
    {
        auto& stream = m_cmdStreams.GetItemR(handle);
        if (!stream.isValid)
        {
            LOGE("VKBackend -> Command Stream to be destroyed is not valid!");
            return;
        }
        stream.isValid = false;

        for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
        {
            stream.lists[i].Reset();
            stream.allocators[i].Reset();
        }

        m_cmdStreams.RemoveItem(handle);
    }

    void DX12Backend::CloseCommandStreams(CommandStream** streams, uint32 streamCount)
    {
        for (uint32 i = 0; i < streamCount; i++)
        {
            auto  stream    = streams[i];
            auto& sr        = m_cmdStreams.GetItemR(stream->m_gpuHandle);
            auto  list      = sr.lists[m_currentFrameIndex];
            auto  allocator = sr.allocators[m_currentFrameIndex];

            if (stream->m_commandCount == 0)
                continue;

            try
            {
                ThrowIfFailed(allocator->Reset());
                ThrowIfFailed(list->Reset(allocator.Get(), nullptr));
            }
            catch (HrException e)
            {
                DX12_THROW(e, "DX12Backend-> Exception when resetting a command list! %s", e.what());
            }

            for (uint32 i = 0; i < stream->m_commandCount; i++)
            {
                uint8* data = stream->m_commands[i];
                TypeID tid  = 0;
                LINAGX_MEMCPY(&tid, data, sizeof(TypeID));
                const size_t increment = sizeof(TypeID);
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
                DX12_THROW(e, "DX12Backend -> Exception when closing a command list! %s", e.what());
            }
        }
    }

    void DX12Backend::ExecuteCommandStreams(const ExecuteDesc& desc)
    {
        try
        {
            LINAGX_VEC<ID3D12CommandList*> _lists;

            for (uint32 i = 0; i < desc.streamCount; i++)
            {
                auto stream = desc.streams[i];
                if (stream->m_commandCount == 0)
                {
                    LOGE("DX12Backend -> Can not execute stream as no commands were recorded!");
                    continue;
                }

                const auto& str = m_cmdStreams.GetItemR(stream->m_gpuHandle);
                _lists.push_back(str.lists[m_currentFrameIndex].Get());
            }

            auto queue = m_queueData[desc.queue].queue;

            if (desc.useWait)
                queue->Wait(m_userSemaphores.GetItem(desc.waitSemaphore).ptr.Get(), desc.waitValue);

            ID3D12CommandList* const* data = _lists.data();
            queue->ExecuteCommandStreams(desc.streamCount, data);

            if (desc.useSignal)
                queue->Signal(m_userSemaphores.GetItem(desc.signalSemaphore).ptr.Get(), desc.signalValue);

            m_submissionPerFrame++;
            LOGA((m_submissionPerFrame < m_initInfo.gpuLimits.maxSubmitsPerFrame), "DX12Backend -> Exceeded maximum submissions per frame! Please increase the limit.");
        }
        catch (HrException e)
        {
            DX12_THROW(e, "DX12Backend -> Exception when executing operations on the queue! %s", e.what());
        }
    }

    void DX12Backend::Present(const PresentDesc& present)
    {
        auto& swp = m_swapchains.GetItemR(present.swapchain);

        try
        {
            UINT   flags    = m_allowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0;
            uint32 interval = 0;

            if (present.vsync == VsyncMode::EveryVBlank)
            {
                interval = 1;
                flags    = 0;
            }
            else if (present.vsync == VsyncMode::EverySecondVBlank)
            {
                interval = 2;
                flags    = 0;
            }

            ThrowIfFailed(swp.ptr->Present(interval, flags));
            swp._imageIndex = swp.ptr->GetCurrentBackBufferIndex();

            DXGI_FRAME_STATISTICS FrameStatistics;
            swp.ptr->GetFrameStatistics(&FrameStatistics);

            if (FrameStatistics.PresentCount > m_previousPresentCount)
            {
                if (m_previousRefreshCount > 0 && (FrameStatistics.PresentRefreshCount - m_previousRefreshCount) > (FrameStatistics.PresentCount - m_previousPresentCount))
                {
                    ++m_glitchCount;
                    interval = 0;
                }
            }

            m_previousPresentCount = FrameStatistics.PresentCount;
            m_previousRefreshCount = FrameStatistics.SyncRefreshCount;
        }
        catch (HrException& e)
        {
            DX12_THROW(e, "DX12Backend -> Present engine error! {0}", e.what());
        }
    }

    void DX12Backend::EndFrame()
    {
        auto& frame = m_perFrameData[m_currentFrameIndex];

        // Increase & signal, we'll wait for this value next time we are starting this frame index.
        auto& graphics = m_queueData[QueueType::Graphics];
        auto& transfer = m_queueData[QueueType::Transfer];

        graphics.frameFenceValue++;
        transfer.frameFenceValue++;
        frame.storedFenceGraphics = graphics.frameFenceValue;
        frame.storedFenceTransfer = transfer.frameFenceValue;

        graphics.queue->Signal(graphics.frameFence.Get(), graphics.frameFenceValue);
        transfer.queue->Signal(transfer.frameFence.Get(), transfer.frameFenceValue);
    }

    void DX12Backend::CMD_BeginRenderPass(uint8* data, const DX12CommandStream& stream)
    {
        CMDBeginRenderPass* begin = reinterpret_cast<CMDBeginRenderPass*>(data);
        auto                list  = stream.lists[m_currentFrameIndex];

        int32 txtIndex   = begin->colorTexture;
        int32 depthIndex = begin->depthTexture;

        if (begin->isSwapchain)
        {
            const auto& swp = m_swapchains.GetItemR(begin->swapchain);
            txtIndex        = swp.colorTextures[swp._imageIndex];
            depthIndex      = swp.depthTextures[swp._imageIndex];
        }

        const auto& colorTxt = m_texture2Ds.GetItemR(txtIndex);
        const auto& depthTxt = m_texture2Ds.GetItemR(depthIndex);

        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(begin->isSwapchain ? colorTxt.rawRes.Get() : colorTxt.allocation->GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        list->ResourceBarrier(1, &transition);

        CD3DX12_CLEAR_VALUE clearValue{GetDXFormat(begin->isSwapchain ? m_initInfo.rtSwapchainFormat : m_initInfo.rtColorFormat), begin->clearColor};
        CD3DX12_CLEAR_VALUE clearDepth{GetDXFormat(m_initInfo.rtDepthFormat), 1.0f, 0};

        D3D12_RENDER_PASS_BEGINNING_ACCESS   colorBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
        D3D12_RENDER_PASS_ENDING_ACCESS      colorEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
        D3D12_RENDER_PASS_RENDER_TARGET_DESC colorDesc{colorTxt.descriptor.GetCPUHandle(), colorBegin, colorEnd};

        D3D12_RENDER_PASS_BEGINNING_ACCESS   depthBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearDepth}};
        D3D12_RENDER_PASS_ENDING_ACCESS      depthEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
        D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthDesc{depthTxt.descriptor.GetCPUHandle(), depthBegin, depthBegin, depthEnd, depthEnd};

        list->BeginRenderPass(1, &colorDesc, &depthDesc, D3D12_RENDER_PASS_FLAG_NONE);

        CMDSetViewport interVP = {};
        CMDSetScissors interSC = {};
        interVP.x              = begin->viewport.x;
        interVP.y              = begin->viewport.x;
        interVP.width          = begin->viewport.width;
        interVP.height         = begin->viewport.height;
        interSC.x              = begin->scissors.x;
        interSC.y              = begin->scissors.y;
        interSC.width          = begin->scissors.width;
        interSC.height         = begin->scissors.height;
        CMD_SetViewport((uint8*)&interVP, stream);
        CMD_SetScissors((uint8*)&interSC, stream);
    }

    void DX12Backend::CMD_EndRenderPass(uint8* data, const DX12CommandStream& stream)
    {
        CMDEndRenderPass* end  = reinterpret_cast<CMDEndRenderPass*>(data);
        auto              list = stream.lists[m_currentFrameIndex];
        list->EndRenderPass();

        if (end->isSwapchain)
        {
            const auto& swp        = m_swapchains.GetItemR(end->swapchain);
            const auto& txt        = m_texture2Ds.GetItemR(swp.colorTextures[swp._imageIndex]);
            auto        transition = CD3DX12_RESOURCE_BARRIER::Transition(txt.rawRes.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            list->ResourceBarrier(1, &transition);
        }
    }

    void DX12Backend::CMD_SetViewport(uint8* data, const DX12CommandStream& stream)
    {
        CMDSetViewport* cmd  = reinterpret_cast<CMDSetViewport*>(data);
        auto            list = stream.lists[m_currentFrameIndex];
        D3D12_VIEWPORT  vp;
        vp.MinDepth = cmd->minDepth;
        vp.MaxDepth = cmd->maxDepth;
        vp.Height   = static_cast<float>(cmd->height);
        vp.Width    = static_cast<float>(cmd->width);
        vp.TopLeftX = static_cast<float>(cmd->x);
        vp.TopLeftY = static_cast<float>(cmd->y);
        list->RSSetViewports(1, &vp);
    }

    void DX12Backend::CMD_SetScissors(uint8* data, const DX12CommandStream& stream)
    {
        CMDSetScissors* cmd  = reinterpret_cast<CMDSetScissors*>(data);
        auto            list = stream.lists[m_currentFrameIndex];
        D3D12_RECT      sc;
        sc.left   = static_cast<LONG>(cmd->x);
        sc.top    = static_cast<LONG>(cmd->y);
        sc.right  = static_cast<LONG>(cmd->x + cmd->width);
        sc.bottom = static_cast<LONG>(cmd->y + cmd->height);
        list->RSSetScissorRects(1, &sc);
    }

    void DX12Backend::CMD_BindPipeline(uint8* data, const DX12CommandStream& stream)
    {
        CMDBindPipeline* cmd    = reinterpret_cast<CMDBindPipeline*>(data);
        auto             list   = stream.lists[m_currentFrameIndex];
        const auto&      shader = m_shaders.GetItemR(cmd->shader);
        list->SetGraphicsRootSignature(shader.rootSig.Get());
        list->IASetPrimitiveTopology(GetDXTopology(shader.topology));
        list->SetPipelineState(shader.pso.Get());
    }

    void DX12Backend::CMD_DrawInstanced(uint8* data, const DX12CommandStream& stream)
    {
        CMDDrawInstanced* cmd  = reinterpret_cast<CMDDrawInstanced*>(data);
        auto              list = stream.lists[m_currentFrameIndex];
        list->DrawInstanced(cmd->vertexCountPerInstance, cmd->instanceCount, cmd->startVertexLocation, cmd->startInstanceLocation);
    }

    void DX12Backend::CMD_DrawIndexedInstanced(uint8* data, const DX12CommandStream& stream)
    {
        CMDDrawIndexedInstanced* cmd  = reinterpret_cast<CMDDrawIndexedInstanced*>(data);
        auto                     list = stream.lists[m_currentFrameIndex];
        list->DrawIndexedInstanced(cmd->indexCountPerInstance, cmd->instanceCount, cmd->startIndexLocation, cmd->baseVertexLocation, cmd->startInstanceLocation);
    }

    void DX12Backend::CMD_BindVertexBuffers(uint8* data, const DX12CommandStream& stream)
    {
        CMDBindVertexBuffers* cmd      = reinterpret_cast<CMDBindVertexBuffers*>(data);
        auto                  list     = stream.lists[m_currentFrameIndex];
        const auto&           resource = m_resources.GetItemR(cmd->resource);

        D3D12_VERTEX_BUFFER_VIEW view;
        view.BufferLocation = GetGPUResource(resource)->GetGPUVirtualAddress();
        view.SizeInBytes    = static_cast<uint32>(resource.size);
        view.StrideInBytes  = static_cast<uint32>(cmd->vertexSize);
        list->IASetVertexBuffers(cmd->slot, 1, &view);
    }

    void DX12Backend::CMD_BindIndexBuffers(uint8* data, const DX12CommandStream& stream)
    {
        CMDBindIndexBuffers*    cmd  = reinterpret_cast<CMDBindIndexBuffers*>(data);
        auto                    list = stream.lists[m_currentFrameIndex];
        const auto&             res  = m_resources.GetItemR(cmd->resource);
        D3D12_INDEX_BUFFER_VIEW view;
        view.BufferLocation = GetGPUResource(res)->GetGPUVirtualAddress();
        view.SizeInBytes    = static_cast<uint32>(res.size);
        view.Format         = GetDXIndexType(cmd->indexFormat);
        list->IASetIndexBuffer(&view);
    }

    void DX12Backend::CMD_CopyResource(uint8* data, const DX12CommandStream& stream)
    {
        CMDCopyResource* cmd     = reinterpret_cast<CMDCopyResource*>(data);
        auto             list    = stream.lists[m_currentFrameIndex];
        const auto&      srcRes  = m_resources.GetItemR(cmd->source);
        const auto&      destRes = m_resources.GetItemR(cmd->destination);
        list->CopyResource(GetGPUResource(destRes), GetGPUResource(srcRes));
    }

} // namespace LinaGX

LINAGX_RESTORE_VC_WARNING()