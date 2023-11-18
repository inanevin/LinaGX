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
#include "LinaGX/LinaGX.hpp"
#include <iostream>
#include <cstdarg>
#include "Triangle.hpp"

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID   0
#define FRAMES_IN_FLIGHT 2
#define BACK_BUFFER      2

    LinaGX::Instance* _lgx       = nullptr;
    uint8             _swapchain = 0;
    Window*           _window    = nullptr;

    // Shaders.
    uint16 _shaderProgram = 0;

    struct Vertex
    {
        float position[3];
        float color[3];
    };

    struct PerFrameData
    {
        LinaGX::CommandStream* stream = nullptr;
    };

    PerFrameData _pfd[FRAMES_IN_FLIGHT];

    void Example::Initialize()
    {
        App::Initialize();

        //******************* CONFIGURATION & INITIALIZATION
        {
            BackendAPI api = BackendAPI::DX12;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::Config.api             = api;
            LinaGX::Config.gpu             = PreferredGPUType::Integrated;
            LinaGX::Config.framesInFlight  = FRAMES_IN_FLIGHT;
            LinaGX::Config.backbufferCount = BACK_BUFFER;
            LinaGX::Config.gpuLimits       = {};
            LinaGX::Config.logLevel        = LogLevel::Verbose;
            LinaGX::Config.errorCallback   = LogError;
            LinaGX::Config.infoCallback    = LogInfo;

            _lgx = new LinaGX::Instance();
            _lgx->Initialize();
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Triangle", 0, 0, 800, 600, WindowStyle::WindowedApplication);
            _window->SetCallbackClose([this]() { Quit(); });
        }

        //******************* SHADER CREATION
        {
            // Compile shaders.
            const std::string                         vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/vert.glsl");
            const std::string                         fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/frag.glsl");
            ShaderLayout                              outLayout  = {};
            ShaderCompileData                         dataVertex = {vtxShader, "Resources/Shaders/Include"};
            ShaderCompileData                         dataFrag   = {fragShader, "Resources/Shaders/Include"};
            std::unordered_map<ShaderStage, DataBlob> outCompiledBlobs;
            _lgx->CompileShader({{ShaderStage::Vertex, dataVertex}, {ShaderStage::Fragment, dataFrag}}, outCompiledBlobs, outLayout);
            // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.

            // Create shader program with vertex & fragment stages.
            LinaGX::ShaderColorAttachment colorAttachment = {
                .format = Format::B8G8R8A8_UNORM,
            };

            ShaderDesc shaderDesc = {
                .stages           = {{ShaderStage::Vertex, outCompiledBlobs[ShaderStage::Vertex]}, {ShaderStage::Fragment, outCompiledBlobs[ShaderStage::Fragment]}},
                .colorAttachments = {colorAttachment},
                .layout           = outLayout,
                .polygonMode      = PolygonMode::Fill,
                .cullMode         = CullMode::None,
                .frontFace        = FrontFace::CCW,
                .topology         = Topology::TriangleList,
            };
            _shaderProgram = _lgx->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            for (auto& [stg, blob] : outCompiledBlobs)
                free(blob.ptr);
        }

        //*******************  MISC
        {
            // Create a swapchain for main window.
            _swapchain = _lgx->CreateSwapchain({
                .format       = Format::B8G8R8A8_UNORM,
                .x            = 0,
                .y            = 0,
                .width        = _window->GetSize().x,
                .height       = _window->GetSize().y,
                .window       = _window->GetWindowHandle(),
                .osHandle     = _window->GetOSHandle(),
                .isFullscreen = false,
            });

            // We need to re-create the swapchain (thus it's images) if window size changes!
            _window->SetCallbackSizeChanged([&](const LGXVector2ui& newSize) {
                LGXVector2ui          monitor    = _window->GetMonitorSize();
                SwapchainRecreateDesc resizeDesc = {
                    .swapchain    = _swapchain,
                    .width        = newSize.x,
                    .height       = newSize.y,
                    .isFullscreen = newSize.x == monitor.x && newSize.y == monitor.y,
                };
                _lgx->RecreateSwapchain(resizeDesc);
            });

            // Create command stream to record draw calls.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
                _pfd[i].stream = _lgx->CreateCommandStream({CommandType::Graphics, 10, 512, 512, 64});
        }
    } // namespace LinaGX::Examples

    void Example::Shutdown()
    {
        // First get rid of the window.
        _lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _lgx->Join();

        // Get rid of resources
        _lgx->DestroySwapchain(_swapchain);
        _lgx->DestroyShader(_shaderProgram);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            _lgx->DestroyCommandStream(_pfd[i].stream);

        // Terminate renderer & shutdown app.
        delete _lgx;
        App::Shutdown();
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        _lgx->TickWindowSystem();

        // Let LinaGX know we are starting a new frame.
        _lgx->StartFrame();

        auto& currentFrame = _pfd[_lgx->GetCurrentFrameIndex()];

        // Barrier to Color Attachment
        {
            LinaGX::CMDBarrier* barrier              = currentFrame.stream->AddCommand<LinaGX::CMDBarrier>();
            barrier->srcStageFlags                   = LinaGX::PSF_TopOfPipe;
            barrier->dstStageFlags                   = LinaGX::PSF_ColorAttachment;
            barrier->textureBarrierCount             = 1;
            barrier->textureBarriers                 = currentFrame.stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
            barrier->textureBarriers->srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
            barrier->textureBarriers->dstAccessFlags = LinaGX::AF_ColorAttachmentRead;
            barrier->textureBarriers->isSwapchain    = true;
            barrier->textureBarriers->texture        = static_cast<uint32>(_swapchain);
            barrier->textureBarriers->toState        = LinaGX::TextureBarrierState::ColorAttachment;
        }

        // Render pass begin
        {
            Viewport                          viewport = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y, .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect                      sc       = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y};
            LinaGX::RenderPassColorAttachment att      = {};
            att.isSwapchain                            = true;
            att.texture                                = static_cast<uint32>(_swapchain);
            att.clearColor                             = {0.3f, 0.3f, 0.3f, 1.0f};
            CMDBeginRenderPass* beginRenderPass        = currentFrame.stream->AddCommand<CMDBeginRenderPass>();
            beginRenderPass->colorAttachmentCount      = 1;
            beginRenderPass->colorAttachments          = currentFrame.stream->EmplaceAuxMemory<LinaGX::RenderPassColorAttachment>(att);
            beginRenderPass->viewport                  = viewport;
            beginRenderPass->scissors                  = sc;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = currentFrame.stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgram;
        }

        // Draw the triangle
        {
            CMDDrawInstanced* drawInstanced       = currentFrame.stream->AddCommand<CMDDrawInstanced>();
            drawInstanced->vertexCountPerInstance = 3;
            drawInstanced->instanceCount          = 1;
            drawInstanced->startInstanceLocation  = 0;
            drawInstanced->startVertexLocation    = 0;
        }

        // End render pass
        {
            CMDEndRenderPass* end = currentFrame.stream->AddCommand<CMDEndRenderPass>();
        }

        // Barrier to Present
        {
            LinaGX::CMDBarrier* barrier              = currentFrame.stream->AddCommand<LinaGX::CMDBarrier>();
            barrier->srcStageFlags                   = LinaGX::PSF_ColorAttachment;
            barrier->dstStageFlags                   = LinaGX::PSF_BottomOfPipe;
            barrier->textureBarrierCount             = 1;
            barrier->textureBarriers                 = currentFrame.stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
            barrier->textureBarriers->srcAccessFlags = LinaGX::AF_ColorAttachmentWrite;
            barrier->textureBarriers->dstAccessFlags = 0;
            barrier->textureBarriers->isSwapchain    = true;
            barrier->textureBarriers->texture        = static_cast<uint32>(_swapchain);
            barrier->textureBarriers->toState        = LinaGX::TextureBarrierState::Present;
        }

        // This does the actual *recording* of every single command stream alive.
        _lgx->CloseCommandStreams(&currentFrame.stream, 1);

        // Submit work on gpu.
        _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &currentFrame.stream, .streamCount = 1});

        // Present main swapchain.
        _lgx->Present({.swapchains = &_swapchain, .swapchainCount = 1});

        // Let LinaGX know we are finalizing this frame.
        _lgx->EndFrame();
    }

} // namespace LinaGX::Examples
