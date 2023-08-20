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

#include "LinaGX/Platform/Metal/MTLBackend.hpp"
#include "LinaGX/Utility/PlatformUtility.hpp"
#include "LinaGX/Core/Commands.hpp"
#include "LinaGX/Core/Instance.hpp"
#include "LinaGX/Core/CommandStream.hpp"

namespace LinaGX
{


uint16 MTLBackend::CreateUserSemaphore() {
    MTLUserSemaphore item = {};
    item.isValid = true;
    
    return m_userSemaphores.AddItem(item);
}

void MTLBackend::DestroyUserSemaphore(uint16 handle) {
    auto& semaphore = m_userSemaphores.GetItemR(handle);
    
    if (!semaphore.isValid)
    {
        LOGE("Backend -> Semaphore to be destroyed is not valid!");
        return;
    }

    m_userSemaphores.RemoveItem(handle);
}

void MTLBackend::WaitForUserSemaphore(uint16 handle, uint64 value) {
        ;
}

uint8 MTLBackend::CreateSwapchain(const SwapchainDesc &desc) {
    MTLSwapchain item = {};
    item.isValid = true;
    
    return m_swapchains.AddItem(item);
}

void MTLBackend::DestroySwapchain(uint8 handle) {
    auto& swp = m_swapchains.GetItemR(handle);
    if (!swp.isValid)
    {
        LOGE("Backend -> Swapchain to be destroyed is not valid!");
        return;
    }
    
    m_swapchains.RemoveItem(handle);
}

void MTLBackend::RecreateSwapchain(const SwapchainRecreateDesc &desc) {
        ;
}

void MTLBackend::SetSwapchainActive(uint8 swp, bool isActive) {
        ;
}

bool MTLBackend::CompileShader(ShaderStage stage, const std::string &source, DataBlob &outBlob) {
        ;
}

uint16 MTLBackend::CreateShader(const ShaderDesc &shaderDesc) {
    MTLShader item = {};
    item.isValid = true;
    
    return m_shaders.AddItem(item);
}

void MTLBackend::DestroyShader(uint16 handle) {
    auto& shader = m_shaders.GetItemR(handle);
    if (!shader.isValid)
    {
        LOGE("Backend -> Shader to be destroyed is not valid!");
        return;
    }
    
    m_shaders.RemoveItem(handle);
}

uint32 MTLBackend::CreateTexture2D(const Texture2DDesc &desc) {
    MTLTexture2D item = {};
    item.isValid = true;
    
    return m_texture2Ds.AddItem(item);
}

void MTLBackend::DestroyTexture2D(uint32 handle) {
    auto& txt = m_texture2Ds.GetItemR(handle);
    if (!txt.isValid)
    {
        LOGE("Backend -> Texture to be destroyed is not valid!");
        return;
    }
    
    m_texture2Ds.RemoveItem(handle);
}

uint32 MTLBackend::CreateSampler(const SamplerDesc &desc) {
    MTLSampler item ={};
    item.isValid = true;
    
    return m_samplers.AddItem(item);
}

void MTLBackend::DestroySampler(uint32 handle) {
    auto& item = m_samplers.GetItemR(handle);
    if (!item.isValid)
    {
        LOGE("Backend -> Sampler to be destroyed is not valid!");
        return;
    }
    
    m_samplers.RemoveItem(handle);
}

uint32 MTLBackend::CreateResource(const ResourceDesc &desc) {
    MTLResource item = {};
    item.isValid = true;
    
    return m_resources.AddItem(item);
}

void MTLBackend::MapResource(uint32 handle, uint8 *&ptr) {
        ;
}

void MTLBackend::UnmapResource(uint32 handle) {
            ;
}

void MTLBackend::DestroyResource(uint32 handle) {
    auto& res = m_resources.GetItemR(handle);
    if (!res.isValid)
    {
        LOGE("Backend -> Resource to be destroyed is not valid!");
        return;
    }
    
    m_resources.RemoveItem(handle);
}

uint16 MTLBackend::CreateDescriptorSet(const DescriptorSetDesc &desc) {
    MTLDescriptorSet item = {};
    item.isValid = true;
    
    return m_descriptorSets.AddItem(item);
}

void MTLBackend::DestroyDescriptorSet(uint16 handle) {
    auto& item = m_descriptorSets.GetItemR(handle);
    if (!item.isValid)
    {
        LOGE("Backend -> Descriptor set to be destroyed is not valid!");
        return;
    }

    m_descriptorSets.RemoveItem(handle);
}

void MTLBackend::DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc &desc) {
        ;
}

void MTLBackend::DescriptorUpdateImage(const DescriptorUpdateImageDesc &desc) {
        ;
}

uint32 MTLBackend::CreateCommandStream(QueueType cmdType) {
    MTLCommandStream item = {};
    item.isValid = true;
    
    return m_cmdStreams.AddItem(item);
}

void MTLBackend::DestroyCommandStream(uint32 handle) {
    auto& stream = m_cmdStreams.GetItemR(handle);
    if (!stream.isValid)
    {
        LOGE("Backend -> Command Stream to be destroyed is not valid!");
        return;
    }
    
    m_cmdStreams.RemoveItem(handle);
}

void MTLBackend::CloseCommandStreams(CommandStream **streams, uint32 streamCount) {
        ;
}

void MTLBackend::SubmitCommandStreams(const SubmitDesc &desc) {
            ;
}

uint8 MTLBackend::CreateQueue(const QueueDesc &desc) {
    MTLQueue item = {};
    item.isValid = true;
    
    return m_queues.AddItem(item);
}

void MTLBackend::DestroyQueue(uint8 queue) {
    auto& item = m_queues.GetItemR(queue);
    if (!item.isValid)
    {
        LOGE("Backend -> Queue to be destroyed is not valid!");
        return;
    }
    
    m_queues.RemoveItem(queue);
}

uint8 MTLBackend::GetPrimaryQueue(QueueType type) {
    return 0;
}


bool MTLBackend::Initialize(const InitInfo &initInfo) {
        
}

void MTLBackend::Shutdown() {
            
    for (auto& swp : m_swapchains)
    {
        LOGA(!swp.isValid, "Backend -> Some swapchains were not destroyed!");
    }

    for (auto& shader : m_shaders)
    {
        LOGA(!shader.isValid, "Backend -> Some shaders were not destroyed!");
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
}

void MTLBackend::Join() {
        
}

void MTLBackend::StartFrame(uint32 frameIndex) {
        
}

void MTLBackend::Present(const PresentDesc &present) {
            
}

void MTLBackend::EndFrame() {
            
}


void MTLBackend::CMD_BeginRenderPass(uint8 *data, MTLCommandStream &stream) {
    CMDBeginRenderPass* begin = reinterpret_cast<CMDBeginRenderPass*>(data);
}

void MTLBackend::CMD_EndRenderPass(uint8 *data, MTLCommandStream &stream) {
    CMDEndRenderPass* end  = reinterpret_cast<CMDEndRenderPass*>(data);

}

void MTLBackend::CMD_SetViewport(uint8 *data, MTLCommandStream &stream) {
    CMDSetViewport* cmd  = reinterpret_cast<CMDSetViewport*>(data);
}

void MTLBackend::CMD_SetScissors(uint8 *data, MTLCommandStream &stream) {
    CMDSetScissors* cmd  = reinterpret_cast<CMDSetScissors*>(data);
}

void MTLBackend::CMD_BindPipeline(uint8 *data, MTLCommandStream &stream) {
    CMDBindPipeline* cmd    = reinterpret_cast<CMDBindPipeline*>(data);
}

void MTLBackend::CMD_DrawInstanced(uint8 *data, MTLCommandStream &stream) {
    CMDDrawInstanced* cmd  = reinterpret_cast<CMDDrawInstanced*>(data);
}

void MTLBackend::CMD_DrawIndexedInstanced(uint8 *data, MTLCommandStream &stream) {
    CMDDrawIndexedInstanced* cmd  = reinterpret_cast<CMDDrawIndexedInstanced*>(data);
}

void MTLBackend::CMD_DrawIndexedIndirect(uint8 *data, MTLCommandStream &stream) {
    CMDDrawIndexedIndirect* cmd    = reinterpret_cast<CMDDrawIndexedIndirect*>(data);
}

void MTLBackend::CMD_BindVertexBuffers(uint8 *data, MTLCommandStream &stream) {
    CMDBindVertexBuffers* cmd      = reinterpret_cast<CMDBindVertexBuffers*>(data);
}

void MTLBackend::CMD_BindIndexBuffers(uint8 *data, MTLCommandStream &stream) {
    CMDBindIndexBuffers*    cmd  = reinterpret_cast<CMDBindIndexBuffers*>(data);
}

void MTLBackend::CMD_CopyResource(uint8 *data, MTLCommandStream &stream) {
    CMDCopyResource* cmd     = reinterpret_cast<CMDCopyResource*>(data);
}

void MTLBackend::CMD_CopyBufferToTexture2D(uint8 *data, MTLCommandStream &stream) {
    CMDCopyBufferToTexture2D* cmd        = reinterpret_cast<CMDCopyBufferToTexture2D*>(data);
}

void MTLBackend::CMD_BindDescriptorSets(uint8 *data, MTLCommandStream &stream) {
    CMDBindDescriptorSets* cmd    = reinterpret_cast<CMDBindDescriptorSets*>(data);
}

void MTLBackend::CMD_BindConstants(uint8 *data, MTLCommandStream &stream) {
    CMDBindConstants* cmd    = reinterpret_cast<CMDBindConstants*>(data);
}

void MTLBackend::CMD_Dispatch(uint8 *data, MTLCommandStream &stream) {
    CMDDispatch* cmd  = reinterpret_cast<CMDDispatch*>(data);
}

void MTLBackend::CMD_ComputeBarrier(uint8 *data, MTLCommandStream &stream) {
    CMDComputeBarrier* cmd  = reinterpret_cast<CMDComputeBarrier*>(data);
}

} // namespace LinaVG

