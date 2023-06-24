/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

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

#include "App.hpp"
#include "LinaGX.hpp"
#include <iostream>
#include <cstdarg>

namespace LinaGX::Examples
{
    LinaGX::Renderer* renderer = nullptr;

    void LogError(const char* err, ...)
    {
        va_list args;
        va_start(args, err);

        std::cout << "\033[1;31m";
        std::cout << "LinaGX: ";
        vprintf(err, args);
        std::cout << "\033[0m" << std::endl;
        va_end(args);
    }

    void LogInfo(const char* info, ...)
    {
        va_list args;
        va_start(args, info);
        std::cout << "\033[32mLinaGX: ";
        vprintf(info, args);
        std::cout << std::endl;
        va_end(args);
    }

    void App::Initialize()
    {
        LinaGX::Config.logLevel      = LogLevel::Verbose;
        LinaGX::Config.errorCallback = LogError;
        LinaGX::Config.infoCallback  = LogInfo;

        LinaGX::InitInfo initInfo;
        initInfo.api = BackendAPI::Vulkan;
        initInfo.gpu = PreferredGPUType::Integrated;

        renderer = new LinaGX::Renderer();
        renderer->Initialize(initInfo);

        m_wm.CreateAppWindow(initInfo.api, 800, 600, "LinaGX Examples - Introduction");
        WindowManager::CloseCallback = [&]() {
            m_isRunning = false;
        };

        const LINAGX_STRING vtxShader  = Internal::ReadFileContentsAsString("Resources/Shaders/SimpleShader_vert.glsl");
        const LINAGX_STRING fragShader = Internal::ReadFileContentsAsString("Resources/Shaders/SimpleShader_frag.glsl");

        ShaderLayout       outLayout  = {};
        CompiledShaderBlob vertexBlob = {};
        CompiledShaderBlob fragBlob   = {};
        renderer->CompileShader(ShaderStage::Vertex, vtxShader.c_str(), "Resources/Shaders/Include", vertexBlob, outLayout);
        renderer->CompileShader(ShaderStage::Pixel, fragShader.c_str(), "Resources/Shaders/Include", fragBlob, outLayout);

        ShaderDesc shaderDesc = {
            .layout = outLayout,
        };

        LINAGX_MAP<ShaderStage, CompiledShaderBlob> stages        = {{ShaderStage::Vertex, vertexBlob}, {ShaderStage::Pixel, fragBlob}};
        uint16                                      shaderProgram = renderer->GenerateShader(stages, shaderDesc);

        auto swp = renderer->CreateSwapchain({
            .x               = 0,
            .y               = 0,
            .width           = 500,
            .height          = 500,
            .window          = m_wm.GetOSWindow(),
            .osHandle        = m_wm.GetOSHandle(),
            .format          = Format::R8G8B8A8_UNORM,
            .backBufferCount = 3,
        });

        renderer->DestroySwapchain(swp);
        renderer->DestroyShader(shaderProgram);
    }

    void App::Run()
    {
        m_isRunning = true;

        while (m_isRunning)
        {
            m_wm.Poll();
        }

        Shutdown();
    }

    void App::Shutdown()
    {
        renderer->Shutdown();
        delete renderer;
        m_wm.Shutdown();
    }

} // namespace LinaGX::Examples
