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

#include "LinaGX/Core/Instance.hpp"
#include "LinaGX/Core/Backend.hpp"
#include "LinaGX/Utility/SPIRVUtility.hpp"
#include "LinaGX/Core/CommandStream.hpp"
#include "LinaGX/Common/Math.hpp"

namespace LinaGX
{
    Instance::~Instance()
    {
        Shutdown();
    }

    bool Instance::Initialize(const InitInfo& info)
    {
#ifdef LINAGX_PLATFORM_APPLE

        if (initInfo.api != BackendAPI::Metal && initInfo.api != BackendAPI::OpenGL)
        {
            if (initInfo.errorCallback)
                initInfo.errorCallback("LinaGX Error: Backend API needs to be either Metal or OpenGL on MacOS platform!");
            return false;
        }
#endif

        m_backend = Backend::CreateBackend(info.api, this);

        if (m_backend == nullptr)
        {
            LOGE("Instance -> Failed creating backend!");
            return false;
        }

        if (!m_backend->Initialize(info))
        {
            LOGE("Instance -> Failed initializing backend!");
            return false;
        }

        LOGT("Instance -> Successfuly initialized.");

        m_initInfo = info;
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
        m_currentFrameIndex = (m_currentFrameIndex + 1) % m_initInfo.framesInFlight;
        PerformanceStats.totalFrames++;
    }

    void Instance::Present(const PresentDesc& present)
    {
        m_backend->Present(present);
    }

    uint16 Instance::CreateUserSemaphore()
    {
        return m_backend->CreateUserSemaphore();
    }

    void Instance::DestroyUserSemaphore(uint16 handle)
    {
        m_backend->DestroyUserSemaphore(handle);
    }

    void Instance::WaitForUserSemaphore(uint16 handle, uint64 value)
    {
        m_backend->WaitForUserSemaphore(handle, value);
    }

    uint8 Instance::CreateSwapchain(const SwapchainDesc& desc)
    {
        return m_backend->CreateSwapchain(desc);
    }

    void Instance::DestroySwapchain(uint8 handle)
    {
        m_backend->DestroySwapchain(handle);
    }

    void Instance::RecreateSwapchain(const SwapchainRecreateDesc& desc)
    {
        m_backend->RecreateSwapchain(desc);
    }

    // bool Instance::CompileShader(ShaderStage stage, const char* text, const char* includePath, DataBlob& outCompiledBlob, ShaderLayout& outLayout)
    // {
    //     DataBlob spv = {};
    //     if (!SPIRVUtility::GLSL2SPV(stage, text, includePath, spv, outLayout))
    //         return false;
    //
    //     if (m_initInfo.api == BackendAPI::DX12)
    //     {
    //         LINAGX_STRING outHLSL = "";
    //
    //         const bool res = SPIRVUtility::SPV2HLSL(stage, spv, outHLSL, outLayout);
    //
    //         LINAGX_FREE(spv.ptr);
    //
    //         if (!res)
    //             return false;
    //
    //         return m_backend->CompileShader(stage, outHLSL, outCompiledBlob);
    //     }
    //     else if (m_initInfo.api == BackendAPI::Metal)
    //     {
    //         LINAGX_STRING outMSL = "";
    //
    //         const bool res = SPIRVUtility::SPV2MSL(stage, spv, outMSL, outLayout);
    //
    //         LINAGX_FREE(spv.ptr);
    //
    //         if (!res)
    //             return false;
    //
    //         return m_backend->CompileShader(stage, outMSL, outCompiledBlob);
    //     }
    //     else if (m_initInfo.api == BackendAPI::Vulkan)
    //     {
    //         // Already SPV, assign & return.
    //         outCompiledBlob = spv;
    //         return true;
    //     }
    //
    //     return false;
    // }

    bool Instance::CompileShader(const LINAGX_MAP<ShaderStage, ShaderCompileData>& compileData, LINAGX_MAP<ShaderStage, DataBlob>& outCompiledBlobs, ShaderLayout& outLayout)
    {
        for (const auto& [stage, data] : compileData)
        {
            DataBlob spv = {};
            if (!SPIRVUtility::GLSL2SPV(stage, data.text, data.includePath, spv, outLayout, m_initInfo.api))
                return false;

            outCompiledBlobs[stage] = spv;
        }

        // If contains push constants
        // Make sure we assign it to maxium set's maximum binding+1
        if (outLayout.constantBlock.size != 0)
        {
            uint32 maxSet = 0;
            for (const auto& [set, bindings] : outLayout.setsAndBindings)
            {
                if (set > maxSet)
                    maxSet = set;
            }

            uint32 maxBinding = 0;
            for (const auto& binding : outLayout.setsAndBindings.at(maxSet))
            {
                if (binding > maxBinding)
                    maxBinding = binding;
            }

            outLayout.constantBlock.set     = maxSet;
            outLayout.constantBlock.binding = maxBinding + 1;
        }

        if (m_initInfo.api == BackendAPI::DX12)
        {
            LINAGX_MAP<ShaderStage, LINAGX_STRING> outHLSLs;

            for (auto& [stage, spv] : outCompiledBlobs)
            {
                const bool res = SPIRVUtility::SPV2HLSL(stage, spv, outHLSLs[stage], outLayout);

                LINAGX_FREE(spv.ptr);

                if (!res)
                    return false;

                spv = {};
            }

            for (auto& [stage, blob] : outCompiledBlobs)
            {
                LINAGX_STRING hlsl = "";

                // gl_DrawID is not supported in HLSL
                // we need to add a custom constant buffer declaration.
                if (stage == ShaderStage::Vertex && outLayout.hasGLDrawID)
                {
                    uint32 maxBinding = 0;

                    for (const auto& ubo : outLayout.ubos)
                    {
                        if (ubo.set == 0)
                            maxBinding = Max(maxBinding, ubo.binding);
                    }

                    outLayout.drawIDBinding = maxBinding + 1;

                    hlsl = "\n cbuffer DrawIDBuffer : register(b" + LINAGX_TOSTRING(outLayout.drawIDBinding);
                    hlsl += +") \n  {\n uint LGX_DRAW_ID; \n }; \n";
                }

                hlsl += outHLSLs[stage];

                if (!m_backend->CompileShader(stage, hlsl, blob))
                    return false;
            }
        }
        else if (m_initInfo.api == BackendAPI::Metal)
        {
            for (auto& [stage, spv] : outCompiledBlobs)
            {
                LINAGX_STRING outMSL = "";

                const bool res = SPIRVUtility::SPV2MSL(stage, spv, outMSL, outLayout);

                LINAGX_FREE(spv.ptr);

                if (!res)
                    return false;

                spv = {};

                if (!m_backend->CompileShader(stage, outMSL, spv))
                    return false;
            }
        }

        return true;
    }

    uint16 Instance::CreateShader(const ShaderDesc& shaderDesc)
    {
        return m_backend->CreateShader(shaderDesc);
    }

    void Instance::DestroyShader(uint16 handle)
    {
        m_backend->DestroyShader(handle);
    }

    CommandStream* Instance::CreateCommandStream(uint32 commandCount, QueueType type)
    {
        CommandStream* stream = new CommandStream(m_backend, type, commandCount, m_backend->CreateCommandStream(type));
        m_commandStreams.push_back(stream);
        return stream;
    }

    void Instance::DestroyCommandStream(CommandStream* stream)
    {
        delete stream;
    }

    uint32 Instance::CreateTexture2D(const Texture2DDesc& desc)
    {
        return m_backend->CreateTexture2D(desc);
    }

    void Instance::DestroyTexture2D(uint32 handle)
    {
        m_backend->DestroyTexture2D(handle);
    }

    uint32 Instance::CreateSampler(const SamplerDesc& desc)
    {
        return m_backend->CreateSampler(desc);
    }

    void Instance::DestroySampler(uint32 handle)
    {
        m_backend->DestroySampler(handle);
    }

    uint32 Instance::CreateResource(const ResourceDesc& desc)
    {
        return m_backend->CreateResource(desc);
    }

    void Instance::DestroyResource(uint32 handle)
    {
        m_backend->DestroyResource(handle);
    }

    void Instance::MapResource(uint32 resource, uint8*& ptr)
    {
        m_backend->MapResource(resource, ptr);
    }

    void Instance::UnmapResource(uint32 resource)
    {
        m_backend->UnmapResource(resource);
    }

    uint16 Instance::CreateDescriptorSet(const DescriptorSetDesc& desc)
    {
        return m_backend->CreateDescriptorSet(desc);
    }

    void Instance::DestroyDescriptorSet(uint16 handle)
    {
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

} // namespace LinaGX
