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
#include "Introduction.hpp"

namespace LinaGX::Examples
{
    LinaGX::Renderer*      renderer  = nullptr;
    LinaGX::CommandStream* stream    = nullptr;
    uint8                  swapchain = 0;

    void Introduction::Initialize()
    {
        App::Initialize();

        LinaGX::Config.logLevel      = LogLevel::Verbose;
        LinaGX::Config.errorCallback = LogError;
        LinaGX::Config.infoCallback  = LogInfo;

        LinaGX::InitInfo initInfo;
        initInfo.api             = BackendAPI::DX12;
        initInfo.gpu             = PreferredGPUType::Integrated;
        initInfo.framesInFlight  = 1;
        initInfo.backbufferCount = 2;

        renderer = new LinaGX::Renderer();
        renderer->Initialize(initInfo);

        const LINAGX_STRING vtxShader  = Internal::ReadFileContentsAsString("Resources/Shaders/SimpleShader_vert.glsl");
        const LINAGX_STRING fragShader = Internal::ReadFileContentsAsString("Resources/Shaders/SimpleShader_frag.glsl");

        ShaderLayout outLayout  = {};
        DataBlob     vertexBlob = {};
        DataBlob     fragBlob   = {};
        renderer->CompileShader(ShaderStage::Vertex, vtxShader.c_str(), "Resources/Shaders/Include", vertexBlob, outLayout);
        renderer->CompileShader(ShaderStage::Pixel, fragShader.c_str(), "Resources/Shaders/Include", fragBlob, outLayout);

        ShaderDesc shaderDesc = {
            .layout = outLayout,
        };

        LINAGX_MAP<ShaderStage, DataBlob> stages        = {{ShaderStage::Vertex, vertexBlob}, {ShaderStage::Pixel, fragBlob}};
        uint16                            shaderProgram = renderer->CreateShader(stages, shaderDesc);

        swapchain = renderer->CreateSwapchain({
            .x           = 0,
            .y           = 0,
            .width       = 500,
            .height      = 500,
            .window      = m_wm.GetOSWindow(),
            .osHandle    = m_wm.GetOSHandle(),
            .format      = Format::B8G8R8A8_UNORM,
            .depthFormat = Format::D32_SFLOAT,
        });

        renderer->DestroyShader(shaderProgram);

        LINAGX_FREE(vertexBlob.ptr);
        LINAGX_FREE(fragBlob.ptr);

        stream = renderer->CreateCommandStream(10, CommandType::Graphics);
    }

    void Introduction::Shutdown()
    {
        delete stream;

        renderer->DestroySwapchain(swapchain);
        renderer->Shutdown();
        delete renderer;
        App::Shutdown();
    }

    void Introduction::OnTick()
    {
        renderer->StartFrame();

        CMDBeginRenderPassSwapchain* beginRenderPass = stream->AddCommand<CMDBeginRenderPassSwapchain>();
        beginRenderPass->swapchain                   = swapchain;
        beginRenderPass->clearColor[0]               = 0.5f;
        beginRenderPass->clearColor[1]               = 0.2f;
        beginRenderPass->clearColor[2]               = 0.2f;
        beginRenderPass->clearColor[3]               = 1.0f;

        CMDEndRenderPass* end = stream->AddCommand<CMDEndRenderPass>();
        end->isSwapchain      = true;
        end->swapchain        = swapchain;

        renderer->Flush();
        renderer->Present({.swapchain = swapchain});
        renderer->EndFrame();
    }

} // namespace LinaGX::Examples
