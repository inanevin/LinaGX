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

#ifndef LINAGX_DX12Backend_HPP
#define LINAGX_DX12Backend_HPP

#include "Core/Backend.hpp"
#include "Platform/DX12/DX12Common.hpp"

namespace D3D12MA
{
    class Allocator;
    class Allocation;
} // namespace D3D12MA

namespace LinaGX
{
    class DX12HeapStaging;
    class DX12HeapGPU;

    struct DX12Swapchain
    {
        Microsoft::WRL::ComPtr<IDXGISwapChain3> ptr     = NULL;
        bool                                    isValid = false;
        LINAGX_VEC<uint32>                      colorTextures;
        LINAGX_VEC<uint32>                      depthTextures;
        uint32                                  _imageIndex = 0;
    };

    struct DX12Shader
    {
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso      = NULL;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig  = NULL;
        Topology                                    topology = Topology::TriangleList;
        bool                                        isValid  = false;
    };

    struct DX12Texture2D
    {
        DescriptorHandle                       descriptor        = {};
        DescriptorHandle                       descriptor2       = {};
        Microsoft::WRL::ComPtr<ID3D12Resource> rawRes            = NULL;
        uint32                                 requiredAlignment = 0;
        D3D12MA::Allocation*                   allocation        = NULL;

        Texture2DUsage usage              = Texture2DUsage::ColorTexture;
        bool           isValid            = false;
        bool           isSwapchainTexture = false;
    };

    struct DX12CommandStream
    {
        bool                                                           isValid = false;
        CommandType                                                    type    = CommandType::Graphics;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator>                 allocator;
        LINAGX_VEC<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>> lists;
    };

    struct DX12PerFrameData
    {
        uint64 storedFenceGraphics = 0;
        uint64 storedFenceTransfer = 0;
    };

    struct DX12FenceControl
    {
        uint16 fence      = 0;
        uint64 fenceValue = 0;
    };

    class DX12Backend : public Backend
    {
    private:
        typedef void (DX12Backend::*CommandFunction)(void*, const DX12CommandStream& stream);

    public:
        DX12Backend(Renderer* renderer)
            : Backend(renderer){};
        virtual ~DX12Backend(){};

        virtual uint32 CreateCommandStream(CommandType cmdType) override;
        virtual void   DestroyCommandStream(uint32 handle) override;
        virtual bool   CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob) override;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual uint16 CreateShader(const LINAGX_MAP<ShaderStage, DataBlob>& stages, const ShaderDesc& shaderDesc) override;
        virtual void   DestroyShader(uint16 handle) override;
        virtual uint32 CreateTexture2D(const Texture2DDesc& desc) override;
        virtual void   DestroyTexture2D(uint32 handle) override;
        void           DX12Exception(HrException e);

        ID3D12Device* GetDevice()
        {
            return m_device.Get();
        }

    private:
        uint16 CreateFence();
        void   DestroyFence(uint16 handle);
        void   WaitForFences(uint16 fenceHandle, uint64 frameFenceValue);

    public:
        virtual bool Initialize(const InitInfo& initInfo) override;
        virtual void Shutdown() override;
        virtual void Join() override;
        virtual void StartFrame(uint32 frameIndex) override;
        virtual void FlushCommandStreams() override;
        virtual void Present(const PresentDesc& present) override;
        virtual void EndFrame() override;

    private:
        void CMD_BeginRenderPass(void* data, const DX12CommandStream& stream);
        void CMD_EndRenderPass(void* data, const DX12CommandStream& stream);
        void CMD_SetViewport(void* data, const DX12CommandStream& stream);
        void CMD_SetScissors(void* data, const DX12CommandStream& stream);
        void CMD_BindPipeline(void* data, const DX12CommandStream& stream);
        void CMD_DrawInstanced(void* data, const DX12CommandStream& stream);
        void CMD_DrawIndexedInstanced(void* data, const DX12CommandStream& stream);

    private:
        D3D12MA::Allocator*                        m_dx12Allocator = nullptr;
        Microsoft::WRL::ComPtr<IDxcLibrary>        m_idxcLib;
        Microsoft::WRL::ComPtr<IDXGIAdapter1>      m_adapter = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Device>       m_device  = nullptr;
        Microsoft::WRL::ComPtr<IDXGIFactory4>      m_factory = nullptr;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queueDirect;
        bool                                       m_allowTearing = false;

        DX12HeapStaging*                                    m_rtvHeap     = nullptr;
        DX12HeapStaging*                                    m_bufferHeap  = nullptr;
        DX12HeapStaging*                                    m_textureHeap = nullptr;
        DX12HeapStaging*                                    m_dsvHeap     = nullptr;
        DX12HeapStaging*                                    m_samplerHeap = nullptr;
        IDList<uint8, DX12Swapchain>                        m_swapchains  = {10};
        IDList<uint16, DX12Shader>                          m_shaders     = {20};
        IDList<uint32, DX12Texture2D>                       m_texture2Ds  = {50};
        IDList<uint32, DX12CommandStream>                   m_cmdStreams  = {50};
        IDList<uint16, Microsoft::WRL::ComPtr<ID3D12Fence>> m_fences      = {20};

        LINAGX_MAP<TypeID, CommandFunction> m_cmdFunctions;
        uint32                              m_currentFrameIndex    = 0;
        uint32                              m_currentImageIndex    = 0;
        uint32                              m_previousRefreshCount = 0;
        uint32                              m_previousPresentCount = 0;
        uint32                              m_glitchCount          = 0;

        LINAGX_VEC<DX12PerFrameData> m_perFrameData;
        DX12FenceControl             m_fenceControlGraphics = {};
        DX12FenceControl             m_fenceControlTransfer = {};

        InitInfo m_initInfo = {};
    };
} // namespace LinaGX

#endif
