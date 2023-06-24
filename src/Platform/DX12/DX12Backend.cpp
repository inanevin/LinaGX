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

    DWORD msgCallback = 0;

    DXGI_FORMAT GetDXGIFormat(Format format)
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
        case Format::R32G32B32A32_SFLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Format::R32G32_SFLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
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
        default:
            return DXGI_FORMAT_UNKNOWN;
        }

        return DXGI_FORMAT_UNKNOWN;
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
            for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, gpuType == PreferredGPUType::Discrete ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter))); ++adapterIndex)
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
                    delete[] buf;
                    break;
                }
            }
        }

        *ppAdapter = adapter.Detach();
    }

    bool DX12Backend::Initialize(const InitInfo& initInfo)
    {
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
                D3D12_COMMAND_QUEUE_DESC queueDesc = {};
                queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
                queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
                ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_queueDirect)));

                NAME_DX12_OBJECT(m_queueDirect, L"Direct Queue");
            }
        }
        catch (HrException e)
        {
            LOGE("DX12Backend -> Exception when pre-initializating renderer! %s", e.what());
            DX12Exception(e);
        }

        return true;
    }

    void DX12Backend::Shutdown()
    {
        for (auto& swp : m_swapchains)
        {
            LOGA(!swp.isValid, "DX12Backend -> Some swapchains were not destroyed!");
        }

        ID3D12InfoQueue1* infoQueue = nullptr;
        if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
        {
            infoQueue->UnregisterMessageCallback(msgCallback);
        }

        m_dx12Allocator->Release();
        m_idxcLib.Reset();
        m_device.Reset();
    }

    bool DX12Backend::CompileShader(ShaderStage stage, const LINAGX_STRING& source, CompiledShaderBlob& outBlob)
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
            LOGE("DX12Backend -> Exception when compiling shader! %s", e.what());
            DX12Exception(e);
            return false;
        }

        return true;
    }

    uint8 DX12Backend::CreateSwapchain(const SwapchainDesc& desc)
    {
        DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
        swapchainDesc.BufferCount           = desc.backBufferCount;
        swapchainDesc.Width                 = static_cast<UINT>(desc.width);
        swapchainDesc.Height                = static_cast<UINT>(desc.height);
        swapchainDesc.Format                = GetDXGIFormat(desc.format);
        swapchainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchainDesc.SampleDesc.Count      = 1;

        ComPtr<IDXGISwapChain1> swapchain;
        swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

        if (m_allowTearing)
            swapchainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        try
        {
            ThrowIfFailed(m_factory->CreateSwapChainForHwnd(m_queueDirect.Get(), // Swap chain needs the queue so that it can force a flush on it.
                                                            (HWND)desc.window,
                                                            &swapchainDesc,
                                                            nullptr,
                                                            nullptr,
                                                            &swapchain));

            DX12Swapchain swp = {};
            ThrowIfFailed(swapchain.As(&swp.ptr));

            LOGT("DX12Backend -> Successfuly created swapchain with size %d x %d", desc.width, desc.height);

            swp.isValid = true;
            return m_swapchains.AddItem(swp);
        }
        catch (HrException e)
        {
            LOGE("DX12Backend -> Exception when creating a swapchain! %s", e.what());
            DX12Exception(e);
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

        swp.isValid = false;
        swp.ptr.Reset();
    }

    uint16 DX12Backend::GenerateShader(const LINAGX_MAP<ShaderStage, CompiledShaderBlob>& stages, const ShaderDesc& shaderDesc)
    {
        LINAGX_VEC<D3D12_INPUT_ELEMENT_DESC> inputLayout;
        /*
        for (const auto& input : shaderDesc.layout.vertexInputs)
        {
            D3D12_INPUT_ELEMENT_DESC desc = D3D12_INPUT_ELEMENT_DESC{};
            desc.SemanticName =
        }
        // Define the vertex input layout.
        {
            if (ppType == PipelineType::Standard)
            {
                inputLayout.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
                inputLayout.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
                inputLayout.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
                inputLayout.push_back({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
            }
            else if (ppType == PipelineType::GUI)
            {
                inputLayout.push_back({"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
                inputLayout.push_back({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
                inputLayout.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
            }
            else if (ppType == PipelineType::NoVertex)
            {
                // no input.
            }
        }

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

        if (!inputLayout.empty())
            psoDesc.InputLayout = {&inputLayout[0], static_cast<UINT>(inputLayout.size())};

        D3D12_BLEND_DESC blendDesc                      = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        blendDesc.AlphaToCoverageEnable                 = false;
        blendDesc.IndependentBlendEnable                = false;
        blendDesc.RenderTarget[0].BlendEnable           = true;
        blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[0].LogicOpEnable         = false;
        blendDesc.RenderTarget[0].LogicOp               = D3D12_LOGIC_OP_COPY;

        const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = {D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS};
        psoDesc.pRootSignature                            = root;
        psoDesc.RasterizerState                           = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState                                = blendDesc;
        psoDesc.DepthStencilState.DepthEnable             = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask          = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc               = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        psoDesc.DepthStencilState.StencilEnable           = FALSE;
        psoDesc.DepthStencilState.StencilReadMask         = D3D12_DEFAULT_STENCIL_READ_MASK;
        psoDesc.DepthStencilState.StencilWriteMask        = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        psoDesc.DepthStencilState.FrontFace               = defaultStencilOp;
        psoDesc.DepthStencilState.BackFace                = defaultStencilOp;
        psoDesc.SampleMask                                = UINT_MAX;
        psoDesc.PrimitiveTopologyType                     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets                          = 1;
        psoDesc.RTVFormats[0]                             = GetFormat(DEFAULT_RT_FORMAT);
        psoDesc.DSVFormat                                 = GetFormat(DEFAULT_DEPTH_FORMAT);
        psoDesc.SampleDesc.Count                          = 1;
        psoDesc.RasterizerState.FillMode                  = D3D12_FILL_MODE_SOLID;

        if (ppType == PipelineType::Standard)
        {
            psoDesc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
            psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
        }
        else if (ppType == PipelineType::NoVertex)
        {
            psoDesc.RasterizerState.CullMode              = D3D12_CULL_MODE_NONE;
            psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
        }
        else if (ppType == PipelineType::GUI)
        {
            psoDesc.RasterizerState.CullMode              = D3D12_CULL_MODE_NONE;
            psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
            psoDesc.DepthStencilState.DepthEnable         = FALSE;
        }

        for (const auto& [stg, title] : stages)
        {
            const auto&  bc       = shader->GetCompiledCode(stg);
            const void*  byteCode = (void*)bc.data;
            const SIZE_T length   = static_cast<SIZE_T>(bc.dataSize);

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
            else if (stg == ShaderStage::TesellationControl || stg == ShaderStage::TesellationEval)
            {
                psoDesc.HS.pShaderBytecode = byteCode;
                psoDesc.HS.BytecodeLength  = length;
            }
        }

        try
        {
            ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso)));
        }
        catch (HrException e)
        {
            LINA_CRITICAL("[Renderer] -> Exception when creating PSO! {0}", e.what());
            renderer->DX12Exception(e);
        }
        */
        return 0;
    }

    void DX12Backend::DX12Exception(HrException e)
    {
        LOGE("DX12Backend -> Exception: %s", e.what());
        _ASSERT(false);
    }

} // namespace LinaGX
