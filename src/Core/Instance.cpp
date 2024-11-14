/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2023-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "LinaGX/Core/Instance.hpp"
#include "LinaGX/Core/Backend.hpp"
#include "LinaGX/Utility/SPIRVUtility.hpp"
#include "LinaGX/Core/CommandStream.hpp"
#include "LinaGX/Common/Math.hpp"
#include "LinaGX/Common/CommonConfig.hpp"

#ifdef LINAGX_PLATFORM_WINDOWS
#ifndef LINAGX_DISABLE_VK
#include "LinaGX/Platform/Vulkan/VKBackend.hpp"
#endif

#ifndef LINAGX_DISABLE_DX12
#include "LinaGX/Platform/DX12/DX12Backend.hpp"
#endif
#endif

#ifdef LINAGX_PLATFORM_APPLE
#include "LinaGX/Platform/Metal/MTLBackend.hpp"
#endif

namespace LinaGX
{

#define LGX_CONDITIONAL_LOCK(condition, mtx) auto conditionalScope = condition ? std::unique_lock<std::mutex>(mtx) : std::unique_lock<std::mutex>()

    Instance::~Instance()
    {
        Shutdown();
    }

    bool Instance::Initialize()
    {
#ifdef LINAGX_PLATFORM_APPLE

        if (Config.api != BackendAPI::Metal)
        {
            LOGE("Backend API needs to be Metal for Apple platforms!");
            return false;
        }
#endif

#ifdef LINAGX_PLATFORM_WINDOWS

#ifdef LINAGX_DISABLE_DX12
        if (Config.api == BackendAPI::DX12)
        {
            LOGE("Trying to use DX12 but it's disabled by project configuration!");
            return false;
        }
#endif

#ifdef LINAGX_DISABLE_VK
        if (Config.api == BackendAPI::Vulkan)
        {
            LOGE("Trying to use Vulkan but it's disabled by project configuration!");
            return false;
        }
#endif

#endif

        m_backend = Backend::CreateBackend();

        if (m_backend == nullptr)
        {
            LOGE("Instance -> Failed creating backend!");
            return false;
        }

        if (!m_backend->Initialize())
        {
            LOGE("Instance -> Failed initializing backend!");
            return false;
        }

        LOGT("Instance -> Successfuly initialized.");

        m_windowManager.Initialize();
        SPIRVUtility::Initialize();
        return true;
    }

    void Instance::Join()
    {
        m_backend->Join();
    }

    void Instance::Shutdown()
    {
        m_backend->Join();
        m_windowManager.Shutdown();
        SPIRVUtility::Shutdown();
        m_backend->Shutdown();
        delete m_backend;
    }

    void Instance::StartFrame()
    {
        m_backend->StartFrame(m_currentFrameIndex);
    }

    void Instance::CloseCommandStreams(CommandStream** streams, uint32 streamCount)
    {
        m_backend->CloseCommandStreams(streams, streamCount);
    }

    void Instance::SubmitCommandStreams(const SubmitDesc& desc)
    {
        m_backend->SubmitCommandStreams(desc);

        for (uint32 i = 0; i < desc.streamCount; i++)
            desc.streams[i]->Reset();
    }

    void Instance::EndFrame()
    {
        m_backend->EndFrame();
        m_currentFrameIndex = (m_currentFrameIndex + 1) % Config.framesInFlight;
        PerformanceStats.totalFrames++;
        m_windowManager.EndFrame();
    }

    void Instance::Present(const PresentDesc& present)
    {
        m_backend->Present(present);
    }

    FormatSupportInfo Instance::GetFormatSupport(Format format)
    {
        for (const auto& fmt : GPUInfo.supportedTexture2DFormats)
        {
            if (fmt.format == format)
                return fmt;
        }

        return {Format::UNDEFINED};
    }

    uint16 Instance::CreateUserSemaphore()
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateUserSemaphore();
    }

    void Instance::DestroyUserSemaphore(uint16 handle)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroyUserSemaphore(handle);
    }

    void Instance::WaitForUserSemaphore(uint16 handle, uint64 value)
    {
        m_backend->WaitForUserSemaphore(handle, value);
    }

    uint8 Instance::CreateSwapchain(const SwapchainDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateSwapchain(desc);
    }

    void Instance::DestroySwapchain(uint8 handle)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroySwapchain(handle);
    }

    void Instance::RecreateSwapchain(const SwapchainRecreateDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->RecreateSwapchain(desc);
    }

    void Instance::SetSwapchainActive(uint8 swp, bool isActive)
    {
        m_backend->SetSwapchainActive(swp, isActive);
    }

    bool Instance::CompileShader(LINAGX_VEC<ShaderCompileData>& compileData, ShaderLayout& outLayout)
    {
        for (ShaderCompileData& data : compileData)
        {
            DataBlob spv = {};
            if (!SPIRVUtility::GLSL2SPV(data.stage, data.text, data.includePath, spv, outLayout, Config.api))
                return false;

            data.outBlob = spv;
        }

        SPIRVUtility::PostFillReflection(outLayout, Config.api);

        if (Config.api == BackendAPI::DX12)
        {
            LINAGX_VEC<LINAGX_STRING> outHLSLs;
            int                       idx = 0;

            for (ShaderCompileData& data : compileData)
            {
                outHLSLs.push_back("");
                const bool res = SPIRVUtility::SPV2HLSL(data.stage, data.outBlob, outHLSLs[idx], outLayout);

                delete[] data.outBlob.ptr;

                if (!res)
                    return false;

                data.outBlob = {};
                idx++;
            }

            idx = 0;
            for (ShaderCompileData& data : compileData)
            {
                LINAGX_STRING hlsl = "";

                // gl_DrawID is not supported in HLSL
                // we need to add a custom constant buffer declaration.
                if (data.stage == ShaderStage::Vertex && outLayout.hasGLDrawID)
                {
                    uint32 maxBinding = 0;

                    for (const auto& dcSetLayout : outLayout.descriptorSetLayouts)
                    {
                        for (const auto& binding : dcSetLayout.bindings)
                        {
                            if (binding.type == DescriptorType::UBO)
                                maxBinding++;
                        }

                        break;
                    }

                    outLayout.drawIDBinding = maxBinding;

                    hlsl = "\ncbuffer DrawIDBuffer : register(b" + LINAGX_TOSTRING(outLayout.drawIDBinding);
                    hlsl += +") \n{\n\tuint LGX_DRAW_ID; \n}; \n\n";
                }

                hlsl += outHLSLs[idx];
                idx++;

#ifdef LINAGX_PLATFORM_WINDOWS
#ifndef LINAGX_DISABLE_DX12
                if (!DX12Backend::CompileShader(data.stage, hlsl, data.outBlob))
                    return false;
#endif
#endif
            }
        }
        else if (Config.api == BackendAPI::Metal)
        {
            for (ShaderCompileData& data : compileData)
            {
                LINAGX_STRING outMSL = "";

                const bool res = SPIRVUtility::SPV2MSL(data.stage, data.outBlob, outMSL, outLayout);

                delete[] data.outBlob.ptr;

                if (!res)
                    return false;

                data.outBlob = {};

#ifdef LINAGX_PLATFORM_APPLE
                if (!MTLBackend::CompileShader(data.stage, outMSL, data.outBlob))
                    return false;
#endif
            }
        }

        return true;
    }

    uint16 Instance::CreateShader(const ShaderDesc& shaderDesc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateShader(shaderDesc);
    }

    void Instance::DestroyShader(uint16 handle)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroyShader(handle);
    }

    CommandStream* Instance::CreateCommandStream(const CommandStreamDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        const uint32   strHandle = m_backend->CreateCommandStream(desc);
        CommandStream* stream    = new CommandStream(m_backend, desc, strHandle);
        m_backend->SetCommandStreamImpl(strHandle, stream);
        m_commandStreams.push_back(stream);
        return stream;
    }

    void Instance::DestroyCommandStream(CommandStream* stream)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        delete stream;
    }

    uint32 Instance::CreateTexture(const TextureDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateTexture(desc);
    }

    void Instance::DestroyTexture(uint32 handle)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroyTexture(handle);
    }

    uint32 Instance::CreateSampler(const SamplerDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateSampler(desc);
    }

    void Instance::DestroySampler(uint32 handle)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroySampler(handle);
    }

    uint32 Instance::CreateResource(const ResourceDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateResource(desc);
    }

    void Instance::DestroyResource(uint32 handle)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroyResource(handle);
    }

    void Instance::MapResource(uint32 resource, uint8*& ptr)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->MapResource(resource, ptr);
    }

    void Instance::UnmapResource(uint32 resource)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->UnmapResource(resource);
    }

    uint16 Instance::CreateDescriptorSet(const DescriptorSetDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateDescriptorSet(desc);
    }

    void Instance::DestroyDescriptorSet(uint16 handle)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroyDescriptorSet(handle);
    }

    void Instance::DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc)
    {
        m_backend->DescriptorUpdateBuffer(desc);
    }

    void Instance::DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc)
    {
        m_backend->DescriptorUpdateImage(desc);
    }

    uint16 Instance::CreatePipelineLayout(const PipelineLayoutDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreatePipelineLayout(desc);
    }

    void Instance::DestroyPipelineLayout(uint16 layout)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroyPipelineLayout(layout);
    }

    uint8 Instance::CreateQueue(const QueueDesc& desc)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        return m_backend->CreateQueue(desc);
    }

    void Instance::DestroyQueue(uint8 queue)
    {
        LGX_CONDITIONAL_LOCK(Config.mutexLockCreationDeletion, m_globalMtx);
        m_backend->DestroyQueue(queue);
    }

    uint8 Instance::GetPrimaryQueue(CommandType type)
    {
        return m_backend->GetPrimaryQueue(type);
    }

    uint32 Instance::VKQueryFeatureSupport(PreferredGPUType gpuType)
    {
#ifdef LINAGX_PLATFORM_WINDOWS
#ifndef LINAGX_DISABLE_VK
        return VKBackend::QueryFeatureSupport(gpuType);
#else
        return 0;
#endif
#endif
        return 0;
    }

    void Instance::BufferIndexedIndirectCommandData(uint8* buffer, size_t padding, uint32 drawID, uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance)
    {
        if (Config.api == BackendAPI::DX12)
        {
            DX12IndexedIndirectCommand cmd = {};
            cmd.LGX_DrawID                 = drawID;
            cmd.indexCountPerInstance      = indexCount;
            cmd.instanceCount              = instanceCount;
            cmd.startIndexLocation         = firstIndex;
            cmd.baseVertexLocation         = vertexOffset;
            cmd.startInstanceLocation      = firstInstance;

            LINAGX_MEMCPY(buffer + padding, &cmd, sizeof(DX12IndexedIndirectCommand));
            return;
        }

        IndexedIndirectCommand cmd = {};
        cmd.indexCountPerInstance  = indexCount;
        cmd.instanceCount          = instanceCount;
        cmd.startIndexLocation     = firstIndex;
        cmd.baseVertexLocation     = vertexOffset;
        cmd.startInstanceLocation  = firstInstance;

        LINAGX_MEMCPY(buffer + padding, &cmd, sizeof(IndexedIndirectCommand));
    }

    void Instance::BufferIndirectCommandData(uint8* buffer, size_t padding, uint32 drawID, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 firstInstance)
    {
        if (Config.api == BackendAPI::DX12)
        {
            DX12IndirectCommand cmd = {};
            cmd.LGX_DrawID          = drawID;
            cmd.vertexCount         = vertexCount,
            cmd.instanceCount       = instanceCount,
            cmd.vertexStart         = vertexOffset,
            cmd.baseInstance        = firstInstance,

            LINAGX_MEMCPY(buffer + padding, &cmd, sizeof(DX12IndirectCommand));
            return;
        }

        IndirectCommand cmd = {};
        cmd.vertexCount     = vertexCount,
        cmd.instanceCount   = instanceCount,
        cmd.vertexStart     = vertexOffset,
        cmd.baseInstance    = firstInstance,

        LINAGX_MEMCPY(buffer + padding, &cmd, sizeof(IndirectCommand));
    }

    size_t Instance::GetIndexedIndirectCommandSize()
    {
        if (Config.api == BackendAPI::DX12)
            return sizeof(DX12IndexedIndirectCommand);

        return sizeof(IndexedIndirectCommand);
    }

    size_t Instance::GetIndirectCommandSize()
    {
        if (Config.api == BackendAPI::DX12)
            return sizeof(DX12IndirectCommand);

        return sizeof(IndirectCommand);
    }
} // namespace LinaGX
