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

namespace LinaGX
{
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

        m_backend = Backend::CreateBackend(info.api);

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
        SPIRVUtility::Initialize();
        return true;
    }

    void Renderer::Shutdown()
    {
        SPIRVUtility::Shutdown();
    }

    bool Renderer::CompileShader(ShaderStage stage, const char* text, CompiledShaderBlob& outCompiledBlob)
    {
        if (m_initInfo.api == BackendAPI::DX12)
        {
            CompiledShaderBlob spirvBlob;
            if (SPIRVUtility::GLSL2SPV(stage, text, spirvBlob))
            {
                //SPIRVUtility::SPV2HLSL(stage, spirvBlob);

                return m_backend->CompileSPVBlob(stage, spirvBlob, outCompiledBlob);
            }
            else
                return false;
        }
        else if (m_initInfo.api == BackendAPI::Metal)
        {
        }
        else if (m_initInfo.api == BackendAPI::Vulkan)
        {
            return SPIRVUtility::GLSL2SPV(stage, text, outCompiledBlob);
        }

        return false;
    }

} // namespace LinaGX
