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

#include "Core/Renderer.hpp"
#include "Core/Backend.hpp"
#include "Utility/SPIRVUtility.hpp"
#include "Core/CommandStream.hpp"

namespace LinaGX
{
    Renderer::~Renderer()
    {
        Shutdown();
    }

    bool Renderer::Initialize(const InitInfo& info)
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
            LOGE("Renderer -> Failed creating backend!");
            return false;
        }

        if (!m_backend->Initialize(info))
        {
            LOGE("Renderer -> Failed initializing backend!");
            return false;
        }

        LOGT("Renderer -> Successfuly initialized.");

        m_initInfo = info;
        m_windowManager.Initialize();
        SPIRVUtility::Initialize();
        return true;
    }

    void Renderer::Join()
    {
        m_backend->Join();
    }

    void Renderer::Shutdown()
    {
        m_backend->Join();
        m_windowManager.Shutdown();
        SPIRVUtility::Shutdown();
        m_backend->Shutdown();
    }

    void Renderer::StartFrame()
    {
        m_backend->StartFrame(m_currentFrameIndex);
    }

    void Renderer::CloseCommandStreams(CommandStream** streams, uint32 streamCount)
    {
        m_backend->CloseCommandStreams(streams, streamCount);
    }

    void Renderer::ExecuteCommandStreams(const ExecuteDesc& desc)
    {
        m_backend->ExecuteCommandStreams(desc);

        for (uint32 i = 0; i < desc.streamCount; i++)
            desc.streams[i]->Reset();
    }

    void Renderer::EndFrame()
    {
        m_backend->EndFrame();
        m_currentFrameIndex = (m_currentFrameIndex + 1) % m_initInfo.framesInFlight;
        PerformanceStats.totalFrames++;
    }

    void Renderer::Present(const PresentDesc& present)
    {
        m_backend->Present(present);
    }

    uint16 Renderer::CreateUserSemaphore()
    {
        return m_backend->CreateUserSemaphore();
    }

    void Renderer::DestroyUserSemaphore(uint16 handle)
    {
        m_backend->DestroyUserSemaphore(handle);
    }

    void Renderer::WaitForUserSemaphore(uint16 handle, uint64 value)
    {
        m_backend->WaitForUserSemaphore(handle, value);
    }

    uint8 Renderer::CreateSwapchain(const SwapchainDesc& desc)
    {
        return m_backend->CreateSwapchain(desc);
    }

    void Renderer::DestroySwapchain(uint8 handle)
    {
        m_backend->DestroySwapchain(handle);
    }

    void Renderer::RecreateSwapchain(const SwapchainRecreateDesc& desc)
    {
        m_backend->RecreateSwapchain(desc);
    }

    bool Renderer::CompileShader(ShaderStage stage, const char* text, const char* includePath, DataBlob& outCompiledBlob, ShaderLayout& outLayout)
    {
        DataBlob spv = {};
        if (!SPIRVUtility::GLSL2SPV(stage, text, includePath, spv, outLayout))
            return false;

        if (m_initInfo.api == BackendAPI::DX12)
        {
            LINAGX_STRING outHLSL = "";

            const bool res = SPIRVUtility::SPV2HLSL(stage, spv, outHLSL);

            LINAGX_FREE(spv.ptr);

            if (!res)
                return false;

            return m_backend->CompileShader(stage, outHLSL, outCompiledBlob);
        }
        else if (m_initInfo.api == BackendAPI::Metal)
        {
            LINAGX_STRING outMSL = "";

            const bool res = SPIRVUtility::SPV2MSL(stage, spv, outMSL);

            LINAGX_FREE(spv.ptr);

            if (!res)
                return false;

            return m_backend->CompileShader(stage, outMSL, outCompiledBlob);
        }
        else if (m_initInfo.api == BackendAPI::Vulkan)
        {
            // Already SPV, assign & return.
            outCompiledBlob = spv;
            return true;
        }

        return false;
    }

    uint16 Renderer::CreateShader(const ShaderDesc& shaderDesc)
    {
        return m_backend->CreateShader(shaderDesc);
    }

    void Renderer::DestroyShader(uint16 handle)
    {
        m_backend->DestroyShader(handle);
    }

    CommandStream* Renderer::CreateCommandStream(uint32 commandCount, QueueType type)
    {
        CommandStream* stream = new CommandStream(m_backend, type, commandCount, m_backend->CreateCommandStream(type));
        m_commandStreams.push_back(stream);
        return stream;
    }

    void Renderer::DestroyCommandStream(CommandStream* stream)
    {
        delete stream;
    }

    uint32 Renderer::CreateTexture2D(const Texture2DDesc& desc)
    {
        return m_backend->CreateTexture2D(desc);
    }

    uint32 Renderer::CreateResource(const ResourceDesc& desc)
    {
        return m_backend->CreateResource(desc);
    }

    void Renderer::DestroyResource(uint32 handle)
    {
        m_backend->DestroyResource(handle);
    }

    void Renderer::MapResource(uint32 resource, uint8*& ptr)
    {
        m_backend->MapResource(resource, ptr);
    }

    void Renderer::UnmapResource(uint32 resource)
    {
        m_backend->UnmapResource(resource);
    }

} // namespace LinaGX
