/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2023-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*

Example: TexturesAndBinding

Simple example drawing a quad in the middle of the screen rendering a texture from the resources.
The texture is sent to the shader as Sampler2DArray and the second element is rendered.

Demonstrates:
- Everything from previous examples.
- Transfer streams for copying data to GPU using copy queues.
- User semaphores, custom submission signalling and waiting.
- Vertex & index buffers.
- Descriptor sets.
- Texture loading utilities and everything related to texture transferring, barriers.

*/

#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include "TexturesAndBinding.hpp"
#include <iostream>
#include <cstdarg>
#include <LinaGX/Utility/PlatformUtility.hpp>
#include <LinaGX/Utility/ImageUtility.hpp>

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID   0
#define FRAMES_IN_FLIGHT 2
#define BACK_BUFFER      2

    LinaGX::Instance* _lgx       = nullptr;
    uint8             _swapchain = 0;
    Window*           _window    = nullptr;
    uint32            _windowX   = 0;
    uint32            _windowY   = 0;

    // Shaders.
    uint16 _shaderProgram = 0;

    // Streams.
    LinaGX::CommandStream* _copyStream = nullptr;

    // Resources
    uint32 _vertexBufferStaging = 0;
    uint32 _vertexBufferGPU     = 0;
    uint32 _indexBufferStaging  = 0;
    uint32 _indexBufferGPU      = 0;
    uint32 _textureGPU          = 0;
    uint32 _sampler             = 0;
    uint16 _descriptorSet0      = 0;

    struct PerFrameData
    {
        LinaGX::CommandStream* stream = nullptr;
    };

    PerFrameData _pfd[FRAMES_IN_FLIGHT];

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
            BackendAPI api = BackendAPI::Vulkan;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::Config.api             = api;
            LinaGX::Config.gpu             = PreferredGPUType::Discrete;
            LinaGX::Config.framesInFlight  = FRAMES_IN_FLIGHT;
            LinaGX::Config.backbufferCount = BACK_BUFFER;
            LinaGX::Config.gpuLimits       = {};
            LinaGX::Config.logLevel        = LogLevel::Verbose;
            LinaGX::Config.errorCallback   = LogError;
            LinaGX::Config.infoCallback    = LogInfo;

            _lgx = new LinaGX::Instance();
            _lgx->Initialize();

            std::vector<LinaGX::Format> formatSupportCheck = {LinaGX::Format::B8G8R8A8_UNORM, LinaGX::Format::R8G8B8A8_UNORM};
            for (auto fmt : formatSupportCheck)
            {
                const LinaGX::FormatSupportInfo fsi = _lgx->GetFormatSupport(fmt);

                if (fsi.format == LinaGX::Format::UNDEFINED)
                    LOGE("Current GPU does not support the formats required by this example!");
            }
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX TexturesAndBinding", 200, 200, 800, 800, WindowStyle::WindowedApplication);
            App::RegisterWindowCallbacks(_window);
            _windowX = _window->GetSize().x;
            _windowY = _window->GetSize().y;
        }

        //******************* SHADER CREATION
        {
            // Compile shaders.
            const std::string vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/vert.glsl");
            const std::string fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/frag.glsl");
            ShaderLayout      outLayout  = {};

            LINAGX_VEC<ShaderCompileData> compileData;
            compileData.push_back({
                .stage       = ShaderStage::Vertex,
                .text        = vtxShader,
                .includePath = "Resources/Shaders/Include",
            });

            compileData.push_back({
                .stage       = ShaderStage::Fragment,
                .text        = fragShader,
                .includePath = "Resources/Shaders/Include",
            });

            _lgx->CompileShader(compileData, outLayout);

            // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.

            // Create shader program with vertex & fragment stages.
            ShaderColorAttachment colorAttachment = ShaderColorAttachment{
                .format          = Format::B8G8R8A8_UNORM,
                .blendAttachment = {},
            };

            LinaGX::ShaderDepthStencilDesc depthStencil;
            depthStencil.depthStencilAttachmentFormat = LinaGX::Format::UNDEFINED;

            ShaderDesc shaderDesc = {
                .stages           = compileData,
                .colorAttachments = {colorAttachment},
                .depthStencilDesc = depthStencil,
                .layout           = outLayout,
                .polygonMode      = PolygonMode::Fill,
                .cullMode         = CullMode::None,
                .frontFace        = FrontFace::CCW,
                .topology         = Topology::TriangleList,
            };
            _shaderProgram = _lgx->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            for (auto& data : compileData)
                free(data.outBlob.ptr);
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

            // Create command stream to record draw calls.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
                _pfd[i].stream = _lgx->CreateCommandStream({CommandType::Graphics, 10, 512, 512, 64});

            _copyStream    = _lgx->CreateCommandStream({CommandType::Transfer, 10, 512, 4096, 64});
            _copySemaphore = _lgx->CreateUserSemaphore();
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
            _vertexBufferStaging = _lgx->CreateResource(bufferDesc);
            bufferDesc.heapType  = ResourceHeap::GPUOnly;
            _vertexBufferGPU     = _lgx->CreateResource(bufferDesc);

            // Same for index buffers.
            bufferDesc.typeHintFlags = TH_IndexBuffer;
            bufferDesc.heapType      = ResourceHeap::StagingHeap;
            _indexBufferStaging      = _lgx->CreateResource(bufferDesc);
            bufferDesc.heapType      = ResourceHeap::GPUOnly;
            _indexBufferGPU          = _lgx->CreateResource(bufferDesc);

            // Map & fill data into the staging buffer.
            uint8* vtxData = nullptr;
            _lgx->MapResource(_vertexBufferStaging, vtxData);
            std::memcpy(vtxData, vertexBuffer.data(), sizeof(Vertex) * vertexBuffer.size());
            _lgx->UnmapResource(_vertexBufferStaging);

            uint8* indexData = nullptr;
            _lgx->MapResource(_indexBufferStaging, indexData);
            std::memcpy(indexData, indexBuffer.data(), sizeof(uint32) * indexBuffer.size());
            _lgx->UnmapResource(_indexBufferStaging);
        }

        // Load image.
        TextureBuffer loadedTextureData  = {};
        TextureBuffer loadedTextureData2 = {};
        LinaGX::LoadImageFromFile("Resources/Textures/LinaGX.png", loadedTextureData, 4);
        LinaGX::LoadImageFromFile("Resources/Textures/LinaGX2.png", loadedTextureData2, 4);

        std::vector<TextureBuffer> txtData1 = {loadedTextureData}, txtData2 = {loadedTextureData2};

        // Generate mipmaps
        LinaGX::GenerateMipmaps(loadedTextureData, txtData1, MipmapFilter::Default, 4, false);
        LinaGX::GenerateMipmaps(loadedTextureData2, txtData2, MipmapFilter::Default, 4, false);

        // Need big enough staging resource, calculate size.
        uint32 totalStagingResourceSize = 0;
        for (const auto& md : txtData1)
            totalStagingResourceSize += md.width * md.height * 4;

        //******************* TEXTURE
        {
            // Create gpu resource
            TextureDesc desc = {
                .format      = Format::R8G8B8A8_UNORM,
                .flags       = LinaGX::TextureFlags::TF_CopyDest | LinaGX::TextureFlags::TF_Sampled,
                .width       = loadedTextureData.width,
                .height      = loadedTextureData.height,
                .mipLevels   = static_cast<uint32>(txtData1.size()),
                .arrayLength = 2,
                .debugName   = "Lina Logo",
            };
            _textureGPU = _lgx->CreateTexture(desc);

            // Sampler
            SamplerDesc samplerDesc = {
                .minFilter  = Filter::Anisotropic,
                .magFilter  = Filter::Anisotropic,
                .mode       = SamplerAddressMode::ClampToBorder,
                .anisotropy = 0,
            };

            _sampler = _lgx->CreateSampler(samplerDesc);
        }

        //******************* TRANSFER
        {
            TextureBuffer txtBuffer = {
                .pixels        = loadedTextureData.pixels,
                .width         = loadedTextureData.width,
                .height        = loadedTextureData.height,
                .bytesPerPixel = 4,
            };

            TextureBuffer txtBuffer2 = {
                .pixels        = loadedTextureData2.pixels,
                .width         = loadedTextureData2.width,
                .height        = loadedTextureData2.height,
                .bytesPerPixel = 4,
            };

            // Barrier to transfer destination.
            {
                LinaGX::CMDBarrier* textureBarrier   = _copyStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = 1;
                textureBarrier->textureBarriers      = _copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
                textureBarrier->srcStageFlags        = LinaGX::PSF_TopOfPipe;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;
                auto& barrier                        = textureBarrier->textureBarriers[0];
                barrier.texture                      = _textureGPU;
                barrier.toState                      = LinaGX::TextureState::TransferDestination;
                barrier.isSwapchain                  = false;
                barrier.srcAccessFlags               = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
                barrier.dstAccessFlags               = LinaGX::AF_TransferWrite;
            }

            // Copy texture
            CMDCopyBufferToTexture2D* copyTxt = _copyStream->AddCommand<CMDCopyBufferToTexture2D>();
            copyTxt->destTexture              = _textureGPU;
            copyTxt->mipLevels                = static_cast<uint32>(txtData1.size());
            copyTxt->buffers                  = _copyStream->EmplaceAuxMemory<TextureBuffer>(txtData1.data(), sizeof(TextureBuffer) * txtData1.size());
            copyTxt->destinationSlice         = 0;

            CMDCopyBufferToTexture2D* copyTxt2 = _copyStream->AddCommand<CMDCopyBufferToTexture2D>();
            copyTxt2->destTexture              = _textureGPU;
            copyTxt2->mipLevels                = static_cast<uint32>(txtData2.size());
            copyTxt2->buffers                  = _copyStream->EmplaceAuxMemory<TextureBuffer>(txtData2.data(), sizeof(TextureBuffer) * txtData2.size());
            copyTxt2->destinationSlice         = 1;

            // Barrier back to sampling.
            {
                LinaGX::CMDBarrier* textureBarrier   = _copyStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = 1;
                textureBarrier->textureBarriers      = _copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
                textureBarrier->srcStageFlags        = LinaGX::PSF_Transfer;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;
                auto& barrier                        = textureBarrier->textureBarriers[0];
                barrier.texture                      = _textureGPU;
                barrier.toState                      = LinaGX::TextureState::ShaderRead;
                barrier.srcAccessFlags               = LinaGX::AF_TransferWrite;
                barrier.dstAccessFlags               = LinaGX::AF_TransferRead;
                barrier.isSwapchain                  = false;
            }

            // Record copy command.
            CMDCopyResource* copyVtxBuf = _copyStream->AddCommand<CMDCopyResource>();
            copyVtxBuf->source          = _vertexBufferStaging;
            copyVtxBuf->destination     = _vertexBufferGPU;

            CMDCopyResource* copyIndexBuf = _copyStream->AddCommand<CMDCopyResource>();
            copyIndexBuf->source          = _indexBufferStaging;
            copyIndexBuf->destination     = _indexBufferGPU;

            _lgx->CloseCommandStreams(&_copyStream, 1);

            // Execute copy command on the transfer queue, signal a semaphore when it's done and wait for it on the CPU side.
            _copySemaphoreValue++;
            _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &_copyStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &_copySemaphore, .signalValues = &_copySemaphoreValue});
            _lgx->WaitForUserSemaphore(_copySemaphore, _copySemaphoreValue);

            // Not needed anymore.
            _lgx->DestroyResource(_vertexBufferStaging);
            _lgx->DestroyResource(_indexBufferStaging);

            for (const auto& td : txtData1)
                LinaGX::FreeImage(td.pixels);

            for (const auto& td : txtData2)
                LinaGX::FreeImage(td.pixels);
        }

        //******************* DESCRIPTOR SET
        {
            DescriptorBinding binding = {
                .descriptorCount  = 1,
                .type             = DescriptorType::CombinedImageSampler,
                .useDynamicOffset = false,
                .stages           = {ShaderStage::Fragment},
            };

            DescriptorSetDesc desc = {
                .bindings = {binding},
            };

            _descriptorSet0 = _lgx->CreateDescriptorSet(desc);

            DescriptorUpdateImageDesc imgUpdate = {
                .binding  = 0,
                .textures = {_textureGPU},
                .samplers = {_sampler},
            };

            _lgx->DescriptorUpdateImage(imgUpdate);
        }
    }

    void Example::OnWindowResized(uint32 w, uint32 h)
    {
        LGXVector2ui          monitor    = _window->GetMonitorSize();
        SwapchainRecreateDesc resizeDesc = {
            .swapchain    = _swapchain,
            .width        = w,
            .height       = h,
            .isFullscreen = w == monitor.x && h == monitor.y,
        };
        _lgx->RecreateSwapchain(resizeDesc);
        _windowX = w;
        _windowY = h;
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _lgx->Join();

        // Get rid of resources
        _lgx->DestroyDescriptorSet(_descriptorSet0);
        _lgx->DestroyUserSemaphore(_copySemaphore);
        _lgx->DestroyTexture(_textureGPU);
        _lgx->DestroySampler(_sampler);
        _lgx->DestroyResource(_vertexBufferGPU);
        _lgx->DestroyResource(_indexBufferGPU);
        _lgx->DestroySwapchain(_swapchain);
        _lgx->DestroyShader(_shaderProgram);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            _lgx->DestroyCommandStream(_pfd[i].stream);

        _lgx->DestroyCommandStream(_copyStream);

        // Terminate renderer & shutdown app.
        delete _lgx;
        App::Shutdown();
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        _lgx->TickWindowSystem();
    }

    void Example::OnRender()
    {

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
            barrier->textureBarriers->toState        = LinaGX::TextureState::ColorAttachment;
        }

        // Render pass begin
        {
            Viewport                  viewport        = {.x = 0, .y = 0, .width = _windowX, .height = _windowY, .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect              sc              = {.x = 0, .y = 0, .width = _windowX, .height = _windowY};
            CMDBeginRenderPass*       beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();
            RenderPassColorAttachment colorAttachment;
            colorAttachment.clearColor                       = {32.0f / 255.0f, 17.0f / 255.0f, 39.0f / 255.0f, 1.0f};
            colorAttachment.texture                          = static_cast<uint32>(_swapchain);
            colorAttachment.isSwapchain                      = true;
            colorAttachment.loadOp                           = LoadOp::Clear;
            colorAttachment.storeOp                          = StoreOp::Store;
            beginRenderPass->colorAttachmentCount            = 1;
            beginRenderPass->colorAttachments                = currentFrame.stream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->viewport                        = viewport;
            beginRenderPass->scissors                        = sc;
            beginRenderPass->depthStencilAttachment.useDepth = beginRenderPass->depthStencilAttachment.useStencil = false;
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
            bindIndex->indexType           = IndexType::Uint32;
            bindIndex->offset              = 0;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = currentFrame.stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgram;
        }

        // Bind texture descriptor
        {
            CMDBindDescriptorSets* bindTxt = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindTxt->firstSet              = 0;
            bindTxt->setCount              = 1;
            bindTxt->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSet0);
            bindTxt->layoutSource          = LinaGX::DescriptorSetsLayoutSource::LastBoundShader;
        }

        // Draw the quad
        {
            CMDDrawIndexedInstanced* drawIndexed = currentFrame.stream->AddCommand<CMDDrawIndexedInstanced>();
            drawIndexed->baseVertexLocation      = 0;
            drawIndexed->indexCountPerInstance   = 6;
            drawIndexed->instanceCount           = 1;
            drawIndexed->startIndexLocation      = 0;
            drawIndexed->startInstanceLocation   = 0;
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
            barrier->textureBarriers->toState        = LinaGX::TextureState::Present;
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
