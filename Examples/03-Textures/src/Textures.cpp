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
#include "Textures.hpp"

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID 0

    LinaGX::Renderer* _renderer  = nullptr;
    uint8             _swapchain = 0;
    Window*           _window     = nullptr;

    // Shaders.
    uint16 _shaderProgram = 0;

    // Streams.
    LinaGX::CommandStream* _stream     = nullptr;
    LinaGX::CommandStream* _copyStream = nullptr;

    // Resources
    uint32 _vertexBufferStaging = 0;
    uint32 _vertexBufferGPU     = 0;
    uint32 _indexBufferStaging  = 0;
    uint32 _indexBufferGPU      = 0;
    uint32 _textureGPU          = 0;
    uint32 _sampler             = 0;
    uint16 _descriptorSet0      = 0;

    // Syncronization
    uint16 _copySemaphore      = 0;
    uint64 _copySemaphoreValue = 0;

    struct Vertex
    {
        float position[3];
        float uv[2];
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
                .api               = api,
                .gpu               = PreferredGPUType::Integrated,
                .framesInFlight    = 2,
                .backbufferCount   = 2,
                .rtSwapchainFormat = Format::B8G8R8A8_UNORM,
                .rtDepthFormat     = Format::D32_SFLOAT,
            };

            _renderer = new LinaGX::Renderer();
            _renderer->Initialize(initInfo);
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _renderer->CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Textures", 0, 0, 800, 800, WindowStyle::Windowed);
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
                .stages          = {{ShaderStage::Vertex, vertexBlob}, {ShaderStage::Fragment, fragBlob}},
                .layout          = outLayout,
                .polygonMode     = PolygonMode::Fill,
                .cullMode        = CullMode::None,
                .frontFace       = FrontFace::CCW,
                .topology        = Topology::TriangleList,
                .blendAttachment = {.componentFlags = ColorComponentFlags::RGBA},
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
            _stream        = _renderer->CreateCommandStream(10, QueueType::Graphics);
            _copyStream    = _renderer->CreateCommandStream(10, QueueType::Transfer);
            _copySemaphore = _renderer->CreateUserSemaphore();
        }

        //*******************  VERTEX BUFFER CREATION
        {
            // Define a vertex buffer.
            std::vector<Vertex> vertexBuffer =
                {
                    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f}},
                    {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}},
                    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
                    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
                };

            std::vector<uint32> indexBuffer      = {0, 1, 3, 1, 2, 3};
            uint32_t            vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(Vertex);

            ResourceDesc bufferDesc = ResourceDesc{
                .size          = vertexBufferSize,
                .typeHintFlags = TH_VertexBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "VertexBuffer",
            };

            // We create 2 buffers, one CPU visible & mapped, one GPU visible for transfer operations.
            _vertexBufferStaging = _renderer->CreateResource(bufferDesc);
            bufferDesc.heapType  = ResourceHeap::GPUOnly;
            _vertexBufferGPU     = _renderer->CreateResource(bufferDesc);

            // Same for index buffers.
            bufferDesc.typeHintFlags = TH_IndexBuffer;
            bufferDesc.heapType      = ResourceHeap::StagingHeap;
            _indexBufferStaging      = _renderer->CreateResource(bufferDesc);
            bufferDesc.heapType      = ResourceHeap::GPUOnly;
            _indexBufferGPU          = _renderer->CreateResource(bufferDesc);

            // Map & fill data into the staging buffer.
            uint8* vtxData = nullptr;
            _renderer->MapResource(_vertexBufferStaging, vtxData);
            std::memcpy(vtxData, vertexBuffer.data(), sizeof(Vertex) * vertexBuffer.size());
            _renderer->UnmapResource(_vertexBufferStaging);

            uint8* indexData = nullptr;
            _renderer->MapResource(_indexBufferStaging, indexData);
            std::memcpy(indexData, indexBuffer.data(), sizeof(uint32) * indexBuffer.size());
            _renderer->UnmapResource(_indexBufferStaging);
        }

        // Load image.
        TextureLoadData loadedTextureData = {};
        LinaGX::LoadImage("Resources/Textures/LinaGX.png", loadedTextureData, ImageChannelMask::Rgba);

        //******************* TEXTURE
        {
            // Create gpu resource
            Texture2DDesc desc = {
                .usage     = Texture2DUsage::ColorTexture,
                .width     = loadedTextureData.width,
                .height    = loadedTextureData.height,
                .mipLevels = 1,
                .format    = Format::R8G8B8A8_UNORM,
                .debugName = "Lina Logo",
            };
            _textureGPU = _renderer->CreateTexture2D(desc);

            // Sampler
            SamplerDesc samplerDesc = {
                .minFilter  = Filter::Anisotropic,
                .magFilter  = Filter::Anisotropic,
                .mode       = SamplerAddressMode::ClampToBorder,
                .anisotropy = 0,
            };

            _sampler = _renderer->CreateSampler(samplerDesc);
        }

        //******************* TRANSFER
        {
            TextureBuffer txtBuffer = {
                .pixels        = loadedTextureData.pixels,
                .width         = loadedTextureData.width,
                .height        = loadedTextureData.height,
                .bytesPerPixel = 4,
            };

            // Copy texture
            CMDCopyBufferToTexture2D* copyTxt = _copyStream->AddCommand<CMDCopyBufferToTexture2D>();
            copyTxt->destTexture              = _textureGPU;
            copyTxt->mipLevels                = 1;
            copyTxt->buffers                  = &txtBuffer;

            // Record copy command.
            CMDCopyResource* copyVtxBuf = _copyStream->AddCommand<CMDCopyResource>();
            copyVtxBuf->source          = _vertexBufferStaging;
            copyVtxBuf->destination     = _vertexBufferGPU;

            CMDCopyResource* copyIndexBuf = _copyStream->AddCommand<CMDCopyResource>();
            copyIndexBuf->source          = _indexBufferStaging;
            copyIndexBuf->destination     = _indexBufferGPU;

            _renderer->CloseCommandStreams(&_copyStream, 1);

            // Execute copy command on the transfer queue, signal a semaphore when it's done and wait for it on the CPU side.
            _renderer->SubmitCommandStreams({.queue = QueueType::Transfer, .streams = &_copyStream, .streamCount = 1, .useSignal = true, .signalSemaphore = _copySemaphore, .signalValue = ++_copySemaphoreValue});
            _renderer->WaitForUserSemaphore(_copySemaphore, _copySemaphoreValue);

            // Not needed anymore.
            _renderer->DestroyResource(_vertexBufferStaging);
            _renderer->DestroyResource(_indexBufferStaging);

            // Done with pixels.
            LinaGX::FreeImage(loadedTextureData.pixels);
        }

        // Create descriptor set.
        {
            DescriptorBinding binding = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::CombinedImageSampler,
                .stages          = {ShaderStage::Fragment},
            };

            DescriptorSetDesc desc = {
                .bindings      = &binding,
                .bindingsCount = 1,
            };

            _descriptorSet0 = _renderer->CreateDescriptorSet(desc);

            DescriptorUpdateImageDesc imgUpdate = {
                .binding         = 0,
                .descriptorCount = 1,
                .textures        = &_textureGPU,
                .samplers        = &_sampler,
                .descriptorType  = DescriptorType::CombinedImageSampler,
            };

            _renderer->DescriptorUpdateImage(imgUpdate);
        }
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _renderer->DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _renderer->Join();

        // Get rid of resources
        _renderer->DestroyDescriptorSet(_descriptorSet0);
        _renderer->DestroyUserSemaphore(_copySemaphore);
        _renderer->DestroyTexture2D(_textureGPU);
        _renderer->DestroySampler(_sampler);
        _renderer->DestroyResource(_vertexBufferGPU);
        _renderer->DestroyResource(_indexBufferGPU);
        _renderer->DestroySwapchain(_swapchain);
        _renderer->DestroyShader(_shaderProgram);
        _renderer->DestroyCommandStream(_stream);
        _renderer->DestroyCommandStream(_copyStream);

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

        // Bind buffers
        {
            CMDBindVertexBuffers* bind = _stream->AddCommand<CMDBindVertexBuffers>();
            bind->slot                 = 0;
            bind->resource             = _vertexBufferGPU;
            bind->vertexSize           = sizeof(Vertex);
            bind->offset               = 0;

            CMDBindIndexBuffers* bindIndex = _stream->AddCommand<CMDBindIndexBuffers>();
            bindIndex->resource            = _indexBufferGPU;
            bindIndex->indexFormat         = IndexType::Uint32;
            bindIndex->offset              = 0;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = _stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgram;
        }

        // Bind texture descriptor
        {
            CMDBindDescriptorSets* bindTxt = _stream->AddCommand<CMDBindDescriptorSets>();
            bindTxt->firstSet              = 0;
            bindTxt->setCount              = 1;
            bindTxt->descriptorSetHandles  = &_descriptorSet0;
        }

        // Draw the triangle
        {
            CMDDrawIndexedInstanced* drawIndexed = _stream->AddCommand<CMDDrawIndexedInstanced>();
            drawIndexed->baseVertexLocation      = 0;
            drawIndexed->indexCountPerInstance   = 6;
            drawIndexed->instanceCount           = 1;
            drawIndexed->startIndexLocation      = 0;
            drawIndexed->startInstanceLocation   = 0;
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
