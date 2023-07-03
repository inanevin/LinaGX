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
        uint32                                  width       = 0;
        uint32                                  height      = 0;
        uint32                                  _imageIndex = 0;
        VsyncMode                               vsync       = VsyncMode::None;
    };

    struct DX12RootParamInfo
    {
        uint32         rootParameter  = 0;
        uint32         binding        = 0;
        uint32         set            = 0;
        uint32         elementSize    = 1;
        DescriptorType reflectionType = DescriptorType::UBO;
    };

    struct DX12Shader
    {
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso      = NULL;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig  = NULL;
        Topology                                    topology = Topology::TriangleList;
        bool                                        isValid  = false;
        LINAGX_VEC<DX12RootParamInfo>               rootParams;

        DX12RootParamInfo* FindRootParam(DescriptorType type, uint32 binding, uint32 set)
        {
            for (auto& p : rootParams)
            {
                if (p.reflectionType == type && p.binding == binding && p.set == set)
                    return &p;
            }

            return nullptr;
        }
    };

    struct DX12Texture2D
    {
        DescriptorHandle                       descriptor        = {};
        DescriptorHandle                       descriptor2       = {};
        Microsoft::WRL::ComPtr<ID3D12Resource> rawRes            = NULL;
        uint64                                 requiredAlignment = 0;
        D3D12MA::Allocation*                   allocation        = NULL;

        Texture2DDesc desc               = {};
        bool          isValid            = false;
        bool          isSwapchainTexture = false;
    };

    struct DX12Sampler
    {
        bool             isValid    = false;
        DescriptorHandle descriptor = {};
    };

    struct DX12CommandStream
    {
        bool                                                           isValid = false;
        QueueType                                                      type    = QueueType::Graphics;
        LINAGX_VEC<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>     allocators;
        LINAGX_VEC<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>> lists;
        LINAGX_VEC<DX12HeapGPU*>                                       samplerHeaps;
        LINAGX_VEC<DX12HeapGPU*>                                       bufferHeaps;
        uint32                                                         boundShader;
        uint32                                                         _cbvSize     = 0;
        uint32                                                         _extraSize   = 0;
        uint32                                                         _textureSize = 0;
        uint32                                                         _samplerSize = 0;
    };

    struct DX12PerFrameData
    {
        uint64 storedFenceGraphics = 0;
        uint64 storedFenceTransfer = 0;
    };

    struct DX12QueueData
    {
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
        Microsoft::WRL::ComPtr<ID3D12Fence>        frameFence;
        uint64                                     frameFenceValue = 0;
    };

    struct DX12Resource
    {
        bool                                   isValid            = false;
        D3D12MA::Allocation*                   allocation         = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> cpuVisibleResource = nullptr;
        ResourceHeap                           heapType           = ResourceHeap::StagingHeap;
        uint64                                 size               = 0;
        bool                                   isMapped           = false;
    };

    struct DX12UserSemaphore
    {
        bool                                isValid = false;
        Microsoft::WRL::ComPtr<ID3D12Fence> ptr     = nullptr;
    };

    struct DX12DescriptorSet
    {
        bool isValid = false;
    };

    class DX12Backend : public Backend
    {
    private:
        typedef void (DX12Backend::*CommandFunction)(uint8*, DX12CommandStream& stream);

    public:
        DX12Backend(Renderer* renderer)
            : Backend(renderer){};
        virtual ~DX12Backend(){};

        virtual uint16 CreateUserSemaphore() override;
        virtual void   DestroyUserSemaphore(uint16 handle) override;
        virtual void   WaitForUserSemaphore(uint16 handle, uint64 value) override;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual void   RecreateSwapchain(const SwapchainRecreateDesc& desc) override;
        virtual bool   CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob) override;
        virtual uint16 CreateShader(const ShaderDesc& shaderDesc) override;
        virtual void   DestroyShader(uint16 handle) override;
        virtual uint32 CreateTexture2D(const Texture2DDesc& desc) override;
        virtual void   DestroyTexture2D(uint32 handle) override;
        virtual uint32 CreateSampler(const SamplerDesc& desc) override;
        virtual void   DestroySampler(uint32 handle) override;
        virtual uint32 CreateResource(const ResourceDesc& desc) override;
        virtual void   MapResource(uint32 handle, uint8*& ptr) override;
        virtual void   UnmapResource(uint32 handle) override;
        virtual void   DestroyResource(uint32 handle) override;
        virtual uint16 CreateDescriptorSet(const DescriptorSetDesc& desc) override;
        virtual void   DestroyDescriptorSet(uint16 handle) override;
        virtual void   DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc) override;
        virtual void   DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc) override;
        virtual uint32 CreateCommandStream(QueueType cmdType) override;
        virtual void   DestroyCommandStream(uint32 handle) override;
        virtual void   CloseCommandStreams(CommandStream** streams, uint32 streamCount) override;
        virtual void   SubmitCommandStreams(const SubmitDesc& desc) override;

        void            DX12Exception(HrException e);
        ID3D12Resource* GetGPUResource(const DX12Resource& res);

        ID3D12Device* GetDevice()
        {
            return m_device.Get();
        }

    private:
        uint16 CreateFence();
        void   DestroyFence(uint16 handle);
        void   WaitForFences(ID3D12Fence* fence, uint64 frameFenceValue);

    public:
        virtual bool Initialize(const InitInfo& initInfo) override;
        virtual void Shutdown() override;
        virtual void Join() override;
        virtual void StartFrame(uint32 frameIndex) override;
        virtual void Present(const PresentDesc& present) override;
        virtual void EndFrame() override;

    private:
        void CMD_BeginRenderPass(uint8* data, DX12CommandStream& stream);
        void CMD_EndRenderPass(uint8* data, DX12CommandStream& stream);
        void CMD_SetViewport(uint8* data, DX12CommandStream& stream);
        void CMD_SetScissors(uint8* data, DX12CommandStream& stream);
        void CMD_BindPipeline(uint8* data, DX12CommandStream& stream);
        void CMD_DrawInstanced(uint8* data, DX12CommandStream& stream);
        void CMD_DrawIndexedInstanced(uint8* data, DX12CommandStream& stream);
        void CMD_BindVertexBuffers(uint8* data, DX12CommandStream& stream);
        void CMD_BindIndexBuffers(uint8* data, DX12CommandStream& stream);
        void CMD_CopyResource(uint8* data, DX12CommandStream& stream);
        void CMD_CopyBufferToTexture2D(uint8* data, DX12CommandStream& stream);
        void CMD_BindDescriptorSets(uint8* data, DX12CommandStream& stream);

    private:
        D3D12MA::Allocator*                   m_dx12Allocator = nullptr;
        Microsoft::WRL::ComPtr<IDxcLibrary>   m_idxcLib;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter      = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Device>  m_device       = nullptr;
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory      = nullptr;
        bool                                  m_allowTearing = false;

        DX12HeapStaging*                                    m_rtvHeap        = nullptr;
        DX12HeapStaging*                                    m_bufferHeap     = nullptr;
        DX12HeapStaging*                                    m_textureHeap    = nullptr;
        DX12HeapStaging*                                    m_dsvHeap        = nullptr;
        DX12HeapStaging*                                    m_samplerHeap    = nullptr;
        IDList<uint8, DX12Swapchain>                        m_swapchains     = {10};
        IDList<uint16, DX12Shader>                          m_shaders        = {20};
        IDList<uint32, DX12Texture2D>                       m_texture2Ds     = {50};
        IDList<uint32, DX12CommandStream>                   m_cmdStreams     = {50};
        IDList<uint16, Microsoft::WRL::ComPtr<ID3D12Fence>> m_fences         = {20};
        IDList<uint32, DX12Resource>                        m_resources      = {100};
        IDList<uint16, DX12UserSemaphore>                   m_userSemaphores = {20};
        IDList<uint32, DX12Sampler>                         m_samplers       = {100};
        IDList<uint16, DX12DescriptorSet>                   m_descriptorSets = {20};

        LINAGX_MAP<TypeID, CommandFunction> m_cmdFunctions;
        uint32                              m_currentFrameIndex    = 0;
        uint32                              m_currentImageIndex    = 0;
        uint32                              m_previousRefreshCount = 0;
        uint32                              m_previousPresentCount = 0;
        uint32                              m_glitchCount          = 0;

        LINAGX_VEC<DX12PerFrameData>         m_perFrameData;
        LINAGX_MAP<QueueType, DX12QueueData> m_queueData;

        InitInfo m_initInfo           = {};
        uint32   m_submissionPerFrame = 0;
    };

} // namespace LinaGX

#endif
