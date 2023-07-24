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
#include "Triangle.hpp"

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID 0

    LinaGX::Renderer*      _renderer  = nullptr;
    LinaGX::CommandStream* _stream    = nullptr;
    uint8                  _swapchain = 0;
    Window*                _window    = nullptr;

    // Shaders.
    uint16 _shaderProgram = 0;

    struct Vertex
    {
        float position[3];
        float color[3];
    };

    void Example::Initialize()
    {
        App::Initialize();

        //******************* CONFIGURATION & INITIALIZATION
        {
            LinaGX::Config.logLevel      = LogLevel::Verbose;
            LinaGX::Config.errorCallback = LogError;
            LinaGX::Config.infoCallback  = LogInfo;

            BackendAPI api = BackendAPI::DX12;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::InitInfo initInfo = InitInfo{
                .api                   = api,
                .gpu                   = PreferredGPUType::Integrated,
                .framesInFlight        = 2,
                .backbufferCount       = 2,
                .checkForFormatSupport = {Format::B8G8R8A8_UNORM, Format::D32_SFLOAT},
            };

            _renderer = new LinaGX::Renderer();
            _renderer->Initialize(initInfo);
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _renderer->CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Triangle", 0, 0, 800, 600, WindowStyle::Windowed);
            _window->SetCallbackClose([this]() { m_isRunning = false; });
        }

        //******************* SHADER CREATION
        {
            // Compile shaders.
            const std::string vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/vert.glsl");
            const std::string fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/frag.glsl");
            ShaderLayout      outLayout  = {};
            DataBlob          vertexBlob = {};
            DataBlob          fragBlob   = {};
            _renderer->CompileShader(ShaderStage::Vertex, vtxShader.c_str(), "Resources/Shaders/Include", vertexBlob, outLayout);
            _renderer->CompileShader(ShaderStage::Fragment, fragShader.c_str(), "Resources/Shaders/Include", fragBlob, outLayout);

            // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.

            // Create shader program with vertex & fragment stages.
            ShaderDesc shaderDesc = {
                .stages                = {{ShaderStage::Vertex, vertexBlob}, {ShaderStage::Fragment, fragBlob}},
                .colorAttachmentFormat = Format::B8G8R8A8_UNORM,
                .layout                = outLayout,
                .polygonMode           = PolygonMode::Fill,
                .cullMode              = CullMode::None,
                .frontFace             = FrontFace::CCW,
                .topology              = Topology::TriangleList,
                .blendAttachment       = {.componentFlags = ColorComponentFlags::RGBA},
            };
            _shaderProgram = _renderer->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            free(vertexBlob.ptr);
            free(fragBlob.ptr);
        }

        //*******************  MISC
        {
            // Create a swapchain for main window.
            _swapchain = _renderer->CreateSwapchain({
                .format       = Format::B8G8R8A8_UNORM,
                .x            = 0,
                .y            = 0,
                .width        = _window->GetWidth(),
                .height       = _window->GetHeight(),
                .window       = _window->GetWindowHandle(),
                .osHandle     = _window->GetOSHandle(),
                .isFullscreen = false,
                .vsyncMode    = VsyncMode::None,
            });

            // We need to re-create the swapchain (thus it's images) if window size changes!
            _window->SetCallbackSizeChanged([&](uint32 w, uint32 h) {
                uint32 monitorW, monitorH = 0;
                _window->GetMonitorSize(monitorW, monitorH);

                SwapchainRecreateDesc resizeDesc = {
                    .swapchain    = _swapchain,
                    .width        = w,
                    .height       = h,
                    .isFullscreen = w == monitorW && h == monitorH,
                };

                _renderer->RecreateSwapchain(resizeDesc);
            });

            // Create command stream to record draw calls.
            _stream = _renderer->CreateCommandStream(10, QueueType::Graphics);
        }
    } // namespace LinaGX::Examples

    void Example::Shutdown()
    {
        // First get rid of the window.
        _renderer->DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _renderer->Join();

        // Get rid of resources
        _renderer->DestroySwapchain(_swapchain);
        _renderer->DestroyShader(_shaderProgram);
        _renderer->DestroyCommandStream(_stream);

        // Terminate renderer & shutdown app.
        delete _renderer;
        App::Shutdown();
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        _renderer->PollWindow();

        // Let LinaGX know we are starting a new frame.
        _renderer->StartFrame();

        // Render pass begin
        {
            Viewport            viewport        = {.x = 0, .y = 0, .width = _window->GetWidth(), .height = _window->GetHeight(), .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect        sc              = {.x = 0, .y = 0, .width = _window->GetWidth(), .height = _window->GetHeight()};
            CMDBeginRenderPass* beginRenderPass = _stream->AddCommand<CMDBeginRenderPass>();
            beginRenderPass->isSwapchain        = true;
            beginRenderPass->swapchain          = _swapchain;
            beginRenderPass->clearColor[0]      = 0.79f;
            beginRenderPass->clearColor[1]      = 0.4f;
            beginRenderPass->clearColor[2]      = 1.0f;
            beginRenderPass->clearColor[3]      = 1.0f;
            beginRenderPass->viewport           = viewport;
            beginRenderPass->scissors           = sc;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = _stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgram;
        }

        // Draw the triangle
        {
            CMDDrawInstanced* drawInstanced       = _stream->AddCommand<CMDDrawInstanced>();
            drawInstanced->vertexCountPerInstance = 3;
            drawInstanced->instanceCount          = 1;
            drawInstanced->startInstanceLocation  = 0;
            drawInstanced->startVertexLocation    = 0;
        }

        // End render pass
        {
            CMDEndRenderPass* end = _stream->AddCommand<CMDEndRenderPass>();
            end->isSwapchain      = true;
            end->swapchain        = _swapchain;
        }

        // This does the actual *recording* of every single command stream alive.
        _renderer->CloseCommandStreams(&_stream, 1);

        // Submit work on gpu.
        _renderer->SubmitCommandStreams({.streams = &_stream, .streamCount = 1});

        // Present main swapchain.
        _renderer->Present({.swapchain = _swapchain});

        // Let LinaGX know we are finalizing this frame.
        _renderer->EndFrame();
    }

} // namespace LinaGX::Examples
