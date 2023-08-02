﻿/*
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
#include "SSBO.hpp"

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID   0
#define FRAMES_IN_FLIGHT 2

    LinaGX::Renderer* _renderer  = nullptr;
    uint8             _swapchain = 0;
    Window*           _window    = nullptr;

    // Shaders.
    uint16 _shaderProgram = 0;

    // Resources
    uint32 _vertexBufferStaging  = 0;
    uint32 _vertexBufferGPU      = 0;
    uint32 _indexBufferStaging   = 0;
    uint32 _indexBufferGPU       = 0;
    uint32 _textureGPU           = 0;
    uint32 _sampler              = 0;
    uint16 _descriptorSetTexture = 0;

    struct PerFrameData
    {
        uint16                 ssboSet            = 0;
        uint32                 ssboStaging        = 0;
        uint32                 ssboGPU            = 0;
        uint8*                 ssboMapping        = nullptr;
        LinaGX::CommandStream* stream             = nullptr;
        LinaGX::CommandStream* copyStream         = nullptr;
        uint16                 copySemaphore      = 0;
        uint64                 copySemaphoreValue = 0;
    };

    PerFrameData _pfd[FRAMES_IN_FLIGHT];

    struct Vertex
    {
        float position[3];
        float uv[2];
    };

    struct TriangleProperties
    {
        float color[4];
        float positionOffset[4];
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
                .framesInFlight        = FRAMES_IN_FLIGHT,
                .backbufferCount       = 2,
                .checkForFormatSupport = {Format::B8G8R8A8_UNORM, Format::D32_SFLOAT},
            };

            _renderer = new LinaGX::Renderer();
            _renderer->Initialize(initInfo);
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _renderer->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX SSBO", 0, 0, 800, 800, WindowStyle::Windowed);
            _window->SetCallbackClose([this]() { m_isRunning = false; });
        }

        //******************* SHADER CREATION
        {
            // Compile shaders.
            const std::string                 vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/vert.glsl");
            const std::string                 fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/frag.glsl");
            ShaderLayout                      outLayout  = {};
            ShaderCompileData                 dataVertex = {vtxShader.c_str(), "Resources/Shaders/Include"};
            ShaderCompileData                 dataFrag   = {fragShader.c_str(), "Resources/Shaders/Include"};
            LINAGX_MAP<ShaderStage, DataBlob> outCompiledBlobs;
            _renderer->CompileShader({{ShaderStage::Vertex, dataVertex}, {ShaderStage::Fragment, dataFrag}}, outCompiledBlobs, outLayout);

            // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.

            // Create shader program with vertex & fragment stages.
            ShaderDesc shaderDesc = {
                .stages                = {{ShaderStage::Vertex, outCompiledBlobs[ShaderStage::Vertex]}, {ShaderStage::Fragment, outCompiledBlobs[ShaderStage::Fragment]}},
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
            for (auto& [stg, blob] : outCompiledBlobs)
                free(blob.ptr);
        }

        //*******************  MISC
        {
            // Create a swapchain for main window.
            _swapchain = _renderer->CreateSwapchain({
                .format       = Format::B8G8R8A8_UNORM,
                .x            = 0,
                .y            = 0,
                .width        = _window->GetSize().x,
                .height       = _window->GetSize().y,
                .window       = _window->GetWindowHandle(),
                .osHandle     = _window->GetOSHandle(),
                .isFullscreen = false,
                .vsyncMode    = VsyncMode::None,
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
                _renderer->RecreateSwapchain(resizeDesc);
            });

            // Create command stream to record draw calls.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].stream        = _renderer->CreateCommandStream(10, QueueType::Graphics);
                _pfd[i].copyStream    = _renderer->CreateCommandStream(10, QueueType::Transfer);
                _pfd[i].copySemaphore = _renderer->CreateUserSemaphore();
            }
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
        LinaGX::LoadImageFromFile("Resources/Textures/LinaGX.png", loadedTextureData, ImageChannelMask::Rgba);

        // Generate mipmaps
        LINAGX_VEC<MipData> outMipmaps;
        LinaGX::GenerateMipmaps(loadedTextureData, outMipmaps, MipmapFilter::Default, ImageChannelMask::Rgba, false);

        // Need big enough staging resource, calculate size.
        uint32 totalStagingResourceSize = loadedTextureData.width * loadedTextureData.height * 4;
        for (const auto& md : outMipmaps)
            totalStagingResourceSize += md.width * md.height * 4;

        //*******************  TEXTURE
        {
            // Create gpu resource
            Texture2DDesc desc = {
                .usage     = Texture2DUsage::ColorTexture,
                .width     = loadedTextureData.width,
                .height    = loadedTextureData.height,
                .mipLevels = loadedTextureData.totalMipLevels,
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

            // Put the base texture data + all mip data together.
            LINAGX_VEC<TextureBuffer> textureDataWithMips;
            textureDataWithMips.push_back(txtBuffer);

            for (const auto& md : outMipmaps)
            {
                TextureBuffer mip = {
                    .pixels        = md.pixels,
                    .width         = md.width,
                    .height        = md.height,
                    .bytesPerPixel = 4,
                };

                textureDataWithMips.push_back(mip);
            }

            // Copy texture
            CMDCopyBufferToTexture2D* copyTxt = _pfd[0].copyStream->AddCommand<CMDCopyBufferToTexture2D>();
            copyTxt->destTexture              = _textureGPU;
            copyTxt->mipLevels                = loadedTextureData.totalMipLevels;
            copyTxt->buffers                  = _pfd[0].copyStream->EmplaceAuxMemory<TextureBuffer>(textureDataWithMips.data(), sizeof(TextureBuffer) * textureDataWithMips.size());

            // Record copy command.
            CMDCopyResource* copyVtxBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
            copyVtxBuf->source          = _vertexBufferStaging;
            copyVtxBuf->destination     = _vertexBufferGPU;

            CMDCopyResource* copyIndexBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
            copyIndexBuf->source          = _indexBufferStaging;
            copyIndexBuf->destination     = _indexBufferGPU;

            _renderer->CloseCommandStreams(&_pfd[0].copyStream, 1);

            // Execute copy command on the transfer queue, signal a semaphore when it's done and wait for it on the CPU side.
            _renderer->SubmitCommandStreams({.queue = QueueType::Transfer, .streams = &_pfd[0].copyStream, .streamCount = 1, .useSignal = true, .signalSemaphore = _pfd[0].copySemaphore, .signalValue = ++_pfd[0].copySemaphoreValue});
            _renderer->WaitForUserSemaphore(_pfd[0].copySemaphore, _pfd[0].copySemaphoreValue);

            // Not needed anymore.
            _renderer->DestroyResource(_vertexBufferStaging);
            _renderer->DestroyResource(_indexBufferStaging);

            // Done with pixels.
            LinaGX::FreeImage(loadedTextureData.pixels);

            for (const auto& md : outMipmaps)
                LinaGX::FreeImage(md.pixels);
        }

        //*******************  SSBO
        {
            ResourceDesc desc = {
                .size          = sizeof(TriangleProperties) * 2,
                .typeHintFlags = ResourceTypeHint::TH_StorageBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "SSBO",
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                desc.heapType       = ResourceHeap::StagingHeap;
                _pfd[i].ssboStaging = _renderer->CreateResource(desc);
                _renderer->MapResource(_pfd[i].ssboStaging, _pfd[i].ssboMapping);

                desc.heapType   = ResourceHeap::GPUOnly;
                _pfd[i].ssboGPU = _renderer->CreateResource(desc);
            }
        }

        //*******************  DESCRIPTOR SET
        {
            DescriptorBinding set0Bindings[1];

            set0Bindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::CombinedImageSampler,
                .stages          = {ShaderStage::Fragment},
            };

            DescriptorSetDesc desc = {
                .bindings      = &set0Bindings[0],
                .bindingsCount = 1,
            };

            _descriptorSetTexture = _renderer->CreateDescriptorSet(desc);

            DescriptorUpdateImageDesc imgUpdate = {
                .setHandle       = _descriptorSetTexture,
                .binding         = 0,
                .descriptorCount = 1,
                .textures        = &_textureGPU,
                .samplers        = &_sampler,
                .descriptorType  = DescriptorType::CombinedImageSampler,
            };

            _renderer->DescriptorUpdateImage(imgUpdate);

            DescriptorBinding ssboBindings[1];

            ssboBindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex, ShaderStage::Fragment},
            };

            DescriptorSetDesc ssboDesc = {
                .bindings      = &ssboBindings[0],
                .bindingsCount = 1,
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ssboSet = _renderer->CreateDescriptorSet(ssboDesc);

                DescriptorUpdateBufferDesc bufferDesc = {
                    .setHandle       = _pfd[i].ssboSet,
                    .binding         = 0,
                    .descriptorCount = 1,
                    .resources       = &_pfd[i].ssboGPU,
                    .descriptorType  = DescriptorType::SSBO,
                };

                _renderer->DescriptorUpdateBuffer(bufferDesc);
            }
        }
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _renderer->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _renderer->Join();

        // Get rid of resources
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            _renderer->DestroyResource(_pfd[i].ssboStaging);
            _renderer->DestroyResource(_pfd[i].ssboGPU);
            _renderer->DestroyDescriptorSet(_pfd[i].ssboSet);
            _renderer->DestroyUserSemaphore(_pfd[i].copySemaphore);
            _renderer->DestroyCommandStream(_pfd[i].stream);
            _renderer->DestroyCommandStream(_pfd[i].copyStream);
        }

        _renderer->DestroyDescriptorSet(_descriptorSetTexture);
        _renderer->DestroyTexture2D(_textureGPU);
        _renderer->DestroySampler(_sampler);
        _renderer->DestroyResource(_vertexBufferGPU);
        _renderer->DestroyResource(_indexBufferGPU);
        _renderer->DestroySwapchain(_swapchain);
        _renderer->DestroyShader(_shaderProgram);

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

        static float time = 0.0f;
        time += (float)(m_deltaMicroseconds * 1e-6);

        auto& currentFrame = _pfd[_renderer->GetCurrentFrameIndex()];

        // Copy SSBO data on copy queue
        {
            TriangleProperties props[2];
            props[0].color[0]          = (std::sin(time) + 2) * 0.5f;
            props[0].color[1]          = (std::cos(time) + 2) * 0.5f;
            props[0].color[2]          = 0.0f;
            props[0].color[3]          = 1.0f;
            props[0].positionOffset[0] = -0.5f;
            props[0].positionOffset[1] = 0.0f;

            props[1].color[0]          = 0.0f;
            props[1].color[1]          = (std::sin(time) + 2) * 0.5f;
            props[1].color[2]          = (std::cos(time) + 2) * 0.5f;
            props[1].color[3]          = 1.0f;
            props[1].positionOffset[0] = 0.5f;
            props[1].positionOffset[1] = 0.0f;

            auto sz = sizeof(TriangleProperties);
            std::memcpy(currentFrame.ssboMapping, props, sz * 2);

            CMDCopyResource* copyRes = currentFrame.copyStream->AddCommand<CMDCopyResource>();
            copyRes->source          = currentFrame.ssboStaging;
            copyRes->destination     = currentFrame.ssboGPU;

            _renderer->CloseCommandStreams(&currentFrame.copyStream, 1);

            SubmitDesc submit = {
                .queue           = QueueType::Transfer,
                .streams         = &currentFrame.copyStream,
                .streamCount     = 1,
                .useWait         = false,
                .useSignal       = true,
                .signalSemaphore = currentFrame.copySemaphore,
                .signalValue     = ++currentFrame.copySemaphoreValue,
            };

            _renderer->SubmitCommandStreams(submit);
        }

        // Render pass begin
        {
            Viewport            viewport        = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y, .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect        sc              = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y};
            CMDBeginRenderPass* beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();
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
            CMDBindVertexBuffers* bind = currentFrame.stream->AddCommand<CMDBindVertexBuffers>();
            bind->slot                 = 0;
            bind->resource             = _vertexBufferGPU;
            bind->vertexSize           = sizeof(Vertex);
            bind->offset               = 0;

            CMDBindIndexBuffers* bindIndex = currentFrame.stream->AddCommand<CMDBindIndexBuffers>();
            bindIndex->resource            = _indexBufferGPU;
            bindIndex->indexFormat         = IndexType::Uint32;
            bindIndex->offset              = 0;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = currentFrame.stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgram;
        }

        // Bind the descriptor
        {
            CMDBindDescriptorSets* bindTxt = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindTxt->firstSet              = 0;
            bindTxt->setCount              = 2;
            bindTxt->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSetTexture, currentFrame.ssboSet);
            bindTxt->isCompute             = false;
        }

        // Bind constants & draw.
        {
            ShaderStage stages[2] = {ShaderStage::Fragment, ShaderStage::Vertex};

            // Constant 1
            uint32            index    = 0;
            CMDBindConstants* constant = currentFrame.stream->AddCommand<CMDBindConstants>();
            constant->data             = currentFrame.stream->EmplaceAuxMemory<uint32>(index);
            constant->offset           = 0;
            constant->size             = sizeof(int);
            constant->stages           = currentFrame.stream->EmplaceAuxMemory<ShaderStage>(ShaderStage::Fragment, ShaderStage::Vertex);
            constant->stagesSize       = 2;

            // Quad 1
            CMDDrawIndexedInstanced* drawIndexed = currentFrame.stream->AddCommand<CMDDrawIndexedInstanced>();
            drawIndexed->baseVertexLocation      = 0;
            drawIndexed->indexCountPerInstance   = 6;
            drawIndexed->instanceCount           = 1;
            drawIndexed->startIndexLocation      = 0;
            drawIndexed->startInstanceLocation   = 0;

            // Constant 2
            index                       = 1;
            CMDBindConstants* constant2 = currentFrame.stream->AddCommand<CMDBindConstants>();
            constant2->data             = currentFrame.stream->EmplaceAuxMemory<uint32>(index);
            constant2->offset           = 0;
            constant2->size             = sizeof(int);
            constant2->stages           = currentFrame.stream->EmplaceAuxMemory<ShaderStage>(ShaderStage::Fragment, ShaderStage::Vertex);
            constant2->stagesSize       = 2;

            // Quad 2
            CMDDrawIndexedInstanced* drawIndexed2 = currentFrame.stream->AddCommand<CMDDrawIndexedInstanced>();
            drawIndexed2->baseVertexLocation      = 0;
            drawIndexed2->indexCountPerInstance   = 6;
            drawIndexed2->instanceCount           = 1;
            drawIndexed2->startIndexLocation      = 0;
            drawIndexed2->startInstanceLocation   = 0;
        }

        // End render pass
        {
            CMDEndRenderPass* end = currentFrame.stream->AddCommand<CMDEndRenderPass>();
            end->isSwapchain      = true;
            end->swapchain        = _swapchain;
        }

        // This does the actual *recording* of every single command stream alive.
        _renderer->CloseCommandStreams(&currentFrame.stream, 1);

        // Submit work on gpu.
        _renderer->SubmitCommandStreams({.streams = &currentFrame.stream, .streamCount = 1, .useWait = true, .waitSemaphore = currentFrame.copySemaphore, .waitValue = currentFrame.copySemaphoreValue});

        // Present main swapchain.
        _renderer->Present({.swapchain = _swapchain});

        // Let LinaGX know we are finalizing this frame.
        _renderer->EndFrame();
    }

} // namespace LinaGX::Examples
