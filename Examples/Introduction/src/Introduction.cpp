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

#include "App.hpp"
#include "LinaGX.hpp"
#include <iostream>
#include <cstdarg>
#include "Introduction.hpp"

namespace LinaGX::Examples
{
    LinaGX::Renderer*      renderer      = nullptr;
    LinaGX::CommandStream* stream        = nullptr;
    uint8                  swapchain     = 0;
    uint16                 shaderProgram = 0;

#define MAIN_WINDOW_ID 0

    struct Vertex
    {
        float position[3];
        float color[4];
    };

    void Introduction::Initialize()
    {
        App::Initialize();

        LinaGX::Config.logLevel          = LogLevel::Verbose;
        LinaGX::Config.errorCallback     = LogError;
        LinaGX::Config.infoCallback      = LogInfo;
        LinaGX::Config.rtSwapchainFormat = Format::B8G8R8A8_UNORM;
        LinaGX::Config.rtColorFormat     = Format::R8G8B8A8_SRGB;
        LinaGX::Config.rtDepthFormat     = Format::D32_SFLOAT;

        LinaGX::InitInfo initInfo;
        initInfo.api             = BackendAPI::Vulkan;
        initInfo.gpu             = PreferredGPUType::Integrated;
        initInfo.framesInFlight  = 2;
        initInfo.backbufferCount = 2;

        renderer = new LinaGX::Renderer();
        renderer->Initialize(initInfo);
        auto window = renderer->CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Introduction", 0, 0, 800, 600, WindowStyle::Windowed);

        const LINAGX_STRING vtxShader  = Internal::ReadFileContentsAsString("Resources/Shaders/SimpleShader_vert.glsl");
        const LINAGX_STRING fragShader = Internal::ReadFileContentsAsString("Resources/Shaders/SimpleShader_frag.glsl");

        ShaderLayout outLayout  = {};
        DataBlob     vertexBlob = {};
        DataBlob     fragBlob   = {};
        renderer->CompileShader(ShaderStage::Vertex, vtxShader.c_str(), "Resources/Shaders/Include", vertexBlob, outLayout);
        renderer->CompileShader(ShaderStage::Pixel, fragShader.c_str(), "Resources/Shaders/Include", fragBlob, outLayout);

        ShaderDesc shaderDesc = {
            .layout          = outLayout,
            .polygonMode     = PolygonMode::Fill,
            .cullMode        = CullMode::None,
            .frontFace       = FrontFace::CCW,
            .topology        = Topology::TriangleList,
            .blendAttachment = {.componentFlags = ColorComponentFlags::RGBA},
        };

        LINAGX_MAP<ShaderStage, DataBlob> stages = {{ShaderStage::Vertex, vertexBlob}, {ShaderStage::Pixel, fragBlob}};
        shaderProgram                            = renderer->CreateShader(stages, shaderDesc);
        LINAGX_FREE(vertexBlob.ptr);
        LINAGX_FREE(fragBlob.ptr);

        swapchain = renderer->CreateSwapchain({
            .x        = 0,
            .y        = 0,
            .width    = 800,
            .height   = 600,
            .window   = window->GetWindowHandle(),
            .osHandle = window->GetOSHandle(),
        });

        stream = renderer->CreateCommandStream(10, CommandType::Graphics);
    }

    void Introduction::Shutdown()
    {
        renderer->DestroyApplicationWindow(MAIN_WINDOW_ID);

        renderer->Join();

        delete stream;
        renderer->DestroySwapchain(swapchain);
        renderer->DestroyShader(shaderProgram);

        renderer->Shutdown();
        delete renderer;
        App::Shutdown();
    }

    void Introduction::OnTick()
    {
        renderer->PollWindow();
        renderer->StartFrame();
        return;
        // Render pass begin
        {
            Viewport            viewport        = {.x = 0, .y = 0, .width = 800, .height = 600, .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect        sc              = {.x = 0, .y = 0, .width = 800, .height = 600};
            CMDBeginRenderPass* beginRenderPass = stream->AddCommand<CMDBeginRenderPass>();
            beginRenderPass->swapchain          = swapchain;
            beginRenderPass->clearColor[0]      = 0.5f;
            beginRenderPass->clearColor[1]      = 0.2f;
            beginRenderPass->clearColor[2]      = 0.4f;
            beginRenderPass->clearColor[3]      = 1.0f;
            beginRenderPass->viewport           = viewport;
            beginRenderPass->scissors           = sc;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = shaderProgram;
        }

        // Draw the triangle
        {
            CMDDrawInstanced* drawInstanced       = stream->AddCommand<CMDDrawInstanced>();
            drawInstanced->vertexCountPerInstance = 3;
            drawInstanced->instanceCount          = 1;
            drawInstanced->startInstanceLocation  = 0;
            drawInstanced->startVertexLocation    = 0;
        }

        // End render pass
        {
            CMDEndRenderPass* end = stream->AddCommand<CMDEndRenderPass>();
            end->isSwapchain      = true;
            end->swapchain        = swapchain;
        }

        renderer->Flush();
        renderer->Present({.swapchain = swapchain});
        renderer->EndFrame();
    }

} // namespace LinaGX::Examples
