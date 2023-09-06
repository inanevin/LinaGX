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

#ifndef LINAGX_METAL_BACKEND_HPP
#define LINAGX_METAL_BACKEND_HPP

#include "LinaGX/Core/Backend.hpp"
#include <atomic>

namespace LinaGX
{

struct MTLTexture2D
{
    bool isValid = false;
    void* ptr = nullptr;
};

struct MTLCommandStream
{
    bool isValid = false;
    bool currentRenderPassUseDepth = false;
    QueueType type = QueueType::Graphics;
    void* currentBuffer = nullptr;
    void* currentEncoder = nullptr;
    void* currentBlitEncoder = nullptr;
    void* currentComputeEncoder = nullptr;
    void* currentEncoderDepthStencil = nullptr;
    void* indirectCommandBuffer = nullptr;
    uint32 indirectCommandBufferMaxCommands = 0;
    LINAGX_VEC<void*> allBlitEncoders;
    LINAGX_VEC<void*> allRenderEncoders;
    LINAGX_VEC<void*> allComputeEncoders;
    LINAGX_VEC<uint8> writtenSwapchains;
    uint32 currentShader = 0;
    uint32 currentIndexBuffer = 0;
    uint8 indexBufferType = 0;
    bool currentShaderIsCompute = false;
    LINAGX_MAP<uint32, uint64> intermediateResources;
    LINAGX_MAP<uint32, uint16> boundDescriptorSets;
};

struct MTLSwapchain
{
    bool isValid = false;
    bool isActive = true;
    void* layer = nullptr;
    void* _currentDrawable = nullptr;
    void* window = nullptr;
    uint32 width = 0;
    uint32 height = 0;
    uint32 _currentDrawableIndex = 0;
};

struct MTLArgEncoder
{
    void* encoder = nullptr;
    void* encoderSecondary = nullptr;
};

struct MTLShader
{
    bool isValid = false;
    bool isCompute = false;
    PolygonMode    polygonMode           = PolygonMode::Fill;
    CullMode       cullMode              = CullMode::None;
    FrontFace      frontFace             = FrontFace::CW;
    Topology topology = Topology::TriangleList;
    void* pso = nullptr;
    void* dsso = nullptr;
    void* cso = nullptr;
    ShaderLayout layout = {};
};

struct MTLFence
{
    bool isValid = false;
};


struct MTLResource
{
    bool isValid = false;
    void* ptr = nullptr;
    ResourceHeap heapType = ResourceHeap::StagingHeap;
    size_t size = 0;
};

struct MTLUserSemaphore
{
    bool isValid = false;
    void* semaphore = nullptr;
    uint64 reachValue = 0;
};

struct MTLSampler
{
    bool isValid = false;
    void* ptr = nullptr;
};

struct MTLDescriptorSetPerStageData
{
    uint32 localBufferID = 0;
    uint32 localTextureID = 0;
    uint32 localSamplerID = 0;
};

struct MTLBinding
{
    DescriptorBinding lgxBinding;
    LINAGX_VEC<uint32> resources;
    LINAGX_VEC<uint32> additionalResources;
    void* argBuffer = nullptr;
    void* argBufferSecondary = nullptr;
    void* argEncoder = nullptr;
    void* argEncoderSecondary = nullptr;
    uint32 localBufferID = 0;
    uint32 localBufferIDSecondary = 0;
};

struct MTLDescriptorSet
{
    bool isValid = false;
    void* buffer = nullptr;
    DescriptorSetDesc desc = {};
    LINAGX_VEC<MTLBinding> bindings;
    uint32 localBufferID = 0;
    uint32 localTextureID = 0;
    uint32 localSamplerID = 0;
};

struct MTLQueue
{
    bool isValid = false;
    void* queue = nullptr;
    QueueType type = QueueType::Graphics;
};

struct MTLPerFrameData
{
    uint64 submits = 0;
    uint64 reachedSubmits = 0;
};


class MTLBackend : public Backend
{
private:
    typedef void (MTLBackend::*CommandFunction)(uint8*, MTLCommandStream& stream);
    
public:
    MTLBackend(Instance* renderer)
    : Backend(renderer){};
    virtual ~MTLBackend(){};
    
    virtual uint16 CreateUserSemaphore() override;
    virtual void   DestroyUserSemaphore(uint16 handle) override;
    virtual void   WaitForUserSemaphore(uint16 handle, uint64 value) override;
    virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
    virtual void   DestroySwapchain(uint8 handle) override;
    virtual void   RecreateSwapchain(const SwapchainRecreateDesc& desc) override;
    virtual void   SetSwapchainActive(uint8 swp, bool isActive) override;
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
    virtual uint8  CreateQueue(const QueueDesc& desc) override;
    virtual void   DestroyQueue(uint8 queue) override;
    virtual uint8  GetPrimaryQueue(QueueType type) override;
    
    
public:
    virtual bool Initialize(const InitInfo& initInfo) override;
    virtual void Shutdown() override;
    virtual void Join() override;
    virtual void StartFrame(uint32 frameIndex) override;
    virtual void Present(const PresentDesc& present) override;
    virtual void EndFrame() override;
    
private:
    void CMD_BeginRenderPass(uint8* data, MTLCommandStream& stream);
    void CMD_EndRenderPass(uint8* data, MTLCommandStream& stream);
    void CMD_SetViewport(uint8* data, MTLCommandStream& stream);
    void CMD_SetScissors(uint8* data, MTLCommandStream& stream);
    void CMD_BindPipeline(uint8* data, MTLCommandStream& stream);
    void CMD_DrawInstanced(uint8* data, MTLCommandStream& stream);
    void CMD_DrawIndexedInstanced(uint8* data, MTLCommandStream& stream);
    void CMD_DrawIndexedIndirect(uint8* data, MTLCommandStream& stream);
    void CMD_BindVertexBuffers(uint8* data, MTLCommandStream& stream);
    void CMD_BindIndexBuffers(uint8* data, MTLCommandStream& stream);
    void CMD_CopyResource(uint8* data, MTLCommandStream& stream);
    void CMD_CopyBufferToTexture2D(uint8* data, MTLCommandStream& stream);
    void CMD_BindDescriptorSets(uint8* data, MTLCommandStream& stream);
    void CMD_BindConstants(uint8* data, MTLCommandStream& stream);
    void CMD_Dispatch(uint8* data, MTLCommandStream& stream);
    void CMD_ComputeBarrier(uint8* data, MTLCommandStream& stream);
    
private:
    
    
    
    IDList<uint8,  MTLSwapchain>                        m_swapchains     = {10};
    IDList<uint16, MTLShader>                          m_shaders        = {20};
    IDList<uint32, MTLTexture2D>                       m_texture2Ds     = {50};
    IDList<uint32, MTLCommandStream>                   m_cmdStreams     = {50};
    IDList<uint16, MTLFence> m_fences         = {20};
    IDList<uint32, MTLResource>                        m_resources      = {100};
    IDList<uint16, MTLUserSemaphore>                   m_userSemaphores = {20};
    IDList<uint32, MTLSampler>                         m_samplers       = {100};
    IDList<uint16, MTLDescriptorSet>                   m_descriptorSets = {20};
    IDList<uint8,  MTLQueue>                            m_queues         = {5};
    
    uint32                                     m_currentFrameIndex    = 0;
    uint32                                     m_currentImageIndex    = 0;
    
    
    LINAGX_VEC<MTLPerFrameData> m_perFrameData;
    
    InitInfo            m_initInfo           = {};
    std::atomic<uint32> m_submissionPerFrame = 0;
    void* m_device = nullptr;
    
    uint8 m_primaryQueues[3];
    
    LINAGX_MAP<LINAGX_TYPEID, CommandFunction> m_cmdFunctions;
    std::atomic_flag m_submissionFlag;
    bool m_frameOnGoing = false;
};


} // namespace LinaGX

#endif
