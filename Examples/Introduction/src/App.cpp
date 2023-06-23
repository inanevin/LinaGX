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

    const char* vtxShader = R"GLSL(
#version 330 core

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = vec4(position, 1.0);
}
)GLSL";

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
        std::cout << "LinaGX: ";
        vprintf(info, args);
        std::cout << std::endl;
        va_end(args);
    }

    void App::Initialize()
    {
        LinaGX::Config.errorCallback = LogError;
        LinaGX::Config.infoCallback  = LogInfo;

        LinaGX::InitInfo initInfo;
        initInfo.api = BackendAPI::Vulkan;
        initInfo.gpu = PreferredGPUType::Integrated;

        m_wm.CreateWindow(initInfo.api, 800, 600, "LinaGX Examples - Introduction");

        renderer = new LinaGX::Renderer();
        renderer->Initialize(initInfo);

        CompiledShaderBlob outBlob;
        renderer->CompileShader(ShaderStage::Vertex, vtxShader, outBlob);
    }

    void App::Run()
    {
        while (true)
        {
            m_wm.Poll();
        }
    }

    void App::Shutdown()
    {
        renderer->Shutdown();
        delete renderer;
        m_wm.Shutdown();
    }

} // namespace LinaGX::Examples
