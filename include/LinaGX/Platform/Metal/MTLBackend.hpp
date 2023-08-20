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

namespace LinaGX
{

struct MTLTexture2D
{
    bool isValid = false;
};

struct MTLCommandStream
{
    bool isValid = false;
};

struct MTLSwapchain
{
    bool isValid = false;
};

struct MTLShader
{
    bool isValid = false;
};

struct MTLFence
{
    bool isValid = false;
};


struct MTLResource
{
    bool isValid = false;
};

struct MTLUserSemaphore
{
    bool isValid = false;
};

struct MTLSampler
{
    bool isValid = false;
};

struct MTLDescriptorSet
{
    bool isValid = false;
};

struct MTLQueue
{
    bool isValid = false;
};

struct MTLPerFrameData
{

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
};


} // namespace LinaGX

#endif
