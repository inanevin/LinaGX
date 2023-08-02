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

#include "GLTF.hpp"
#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include <iostream>
#include <cstdarg>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID   0
#define FRAMES_IN_FLIGHT 2

    LinaGX::Renderer* _renderer  = nullptr;
    uint8             _swapchain = 0;
    Window*           _window    = nullptr;

    struct Vertex
    {
        LGXVector3 position = {};
        LGXVector2 uv       = {};
    };

    struct Object
    {
        LINAGX_VEC<Vertex> vertices;
        uint32             index = 0;

        uint32 vertexBufferStaging = 0;
        uint32 vertexBufferGPU     = 0;
    };

    struct GPUSceneData
    {
        glm::mat4 viewProj;
    };

    struct GPUObjectData
    {
        glm::mat4 modelMatrix;
    };

    // Shaders.
    uint16 _shaderProgram = 0;

    // Objects
    LinaGX::ModelData  _modelData   = {};
    ModelTexture*      _baseTexture = nullptr;
    LINAGX_VEC<Object> _objects;

    // Resources
    uint32 _sampler        = 0;
    uint16 _descriptorSet0 = 0;
    uint32 _ubo            = 0;
    uint8* _uboMapping     = 0;
    uint32 _baseColorGPU   = 0;

    struct PerFrameData
    {
        uint32                 ssboStaging        = 0;
        uint32                 ssboGPU            = 0;
        uint8*                 ssboMapping        = nullptr;
        uint16                 ssboSet            = 0;
        LinaGX::CommandStream* stream             = nullptr;
        LinaGX::CommandStream* copyStream         = nullptr;
        uint16                 copySemaphore      = 0;
        uint64                 copySemaphoreValue = 0;
    };

    PerFrameData _pfd[FRAMES_IN_FLIGHT];

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
            _window = _renderer->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX GLTF", 0, 0, 800, 800, WindowStyle::Windowed);
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
                .depthAttachmentFormat = Format::D32_SFLOAT,
                .layout                = outLayout,
                .polygonMode           = PolygonMode::Fill,
                .cullMode              = CullMode::Back,
                .frontFace             = FrontFace::CCW,
                .depthTest             = true,
                .depthWrite            = true,
                .depthCompare          = CompareOp::Less,
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
                .depthFormat  = Format::D32_SFLOAT,
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

        //*******************  MODEL (Vertex Data & Object Array)
        {
            LinaGX::LoadGLTFBinary("Resources/Models/Fox.glb", _modelData);

            for (uint32 i = 0; i < _modelData.allNodesCount; i++)
            {
                ModelNode* node = _modelData.allNodes + i;

                if (node->mesh != nullptr)
                {
                    _objects.push_back({});
                    Object& obj = _objects[_objects.size() - 1];
                    obj.index   = static_cast<uint32>(_objects.size() - 1);

                    for (uint32 j = 0; j < node->mesh->primitiveCount; j++)
                    {
                        ModelMeshPrimitive* prim = node->mesh->primitives + j;

                        for (uint32 k = 0; k < prim->vertexCount; k++)
                        {
                            Vertex vtx   = {};
                            vtx.position = prim->positions[k];
                            vtx.uv       = prim->texCoords[k];
                            obj.vertices.push_back(vtx);
                        }
                    }
                }
            }

            for (auto& obj : _objects)
            {
                const uint32 vertexBufferSize = static_cast<uint32>(sizeof(Vertex) * obj.vertices.size());

                ResourceDesc vbDesc = ResourceDesc{
                    .size          = vertexBufferSize,
                    .typeHintFlags = TH_VertexBuffer,
                    .heapType      = ResourceHeap::StagingHeap,
                    .debugName     = "VertexBuffer",
                };

                obj.vertexBufferStaging = _renderer->CreateResource(vbDesc);
                vbDesc.heapType         = ResourceHeap::GPUOnly;
                obj.vertexBufferGPU     = _renderer->CreateResource(vbDesc);

                uint8* vtxData = nullptr;
                _renderer->MapResource(obj.vertexBufferStaging, vtxData);
                std::memcpy(vtxData, obj.vertices.data(), vertexBufferSize);
                _renderer->UnmapResource(obj.vertexBufferStaging);
            }
        }

        //******************* TEXTURE AND SAMPLER
        {

            // Sampler
            SamplerDesc samplerDesc = {
                .minFilter  = Filter::Anisotropic,
                .magFilter  = Filter::Anisotropic,
                .mode       = SamplerAddressMode::ClampToBorder,
                .anisotropy = 0,
            };

            _sampler = _renderer->CreateSampler(samplerDesc);

            // Will be base color, skipping the whole material shenanigans from gltf for now.
            _baseTexture = _modelData.allTextures;

            // Create gpu resource
            Texture2DDesc desc = {
                .usage     = Texture2DUsage::ColorTexture,
                .width     = _baseTexture->buffer.width,
                .height    = _baseTexture->buffer.height,
                .mipLevels = 1,
                .format    = Format::R8G8B8A8_UNORM,
                .debugName = "Lina Logo",
            };
            _baseColorGPU = _renderer->CreateTexture2D(desc);
        }

        //******************* TRANSFER
        {
            TextureBuffer txtBuffer = {
                .pixels        = _baseTexture->buffer.pixels,
                .width         = _baseTexture->buffer.width,
                .height        = _baseTexture->buffer.height,
                .bytesPerPixel = 4,
            };

            // Copy texture
            CMDCopyBufferToTexture2D* copyTxt = _pfd[0].copyStream->AddCommand<CMDCopyBufferToTexture2D>();
            copyTxt->destTexture              = _baseColorGPU;
            copyTxt->mipLevels                = 1;
            copyTxt->buffers                  = _pfd[0].copyStream->EmplaceAuxMemory<TextureBuffer>(txtBuffer);

            for (auto& obj : _objects)
            {
                // Record copy command.
                CMDCopyResource* copyVtxBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
                copyVtxBuf->source          = obj.vertexBufferStaging;
                copyVtxBuf->destination     = obj.vertexBufferGPU;
            }

            _renderer->CloseCommandStreams(&_pfd[0].copyStream, 1);

            // Execute copy command on the transfer queue, signal a semaphore when it's done and wait for it on the CPU side.
            _renderer->SubmitCommandStreams({.queue = QueueType::Transfer, .streams = &_pfd[0].copyStream, .streamCount = 1, .useSignal = true, .signalSemaphore = _pfd[0].copySemaphore, .signalValue = ++_pfd[0].copySemaphoreValue});
            _renderer->WaitForUserSemaphore(_pfd[0].copySemaphore, _pfd[0].copySemaphoreValue);

            // Not needed anymore.
            for (auto& obj : _objects)
                _renderer->DestroyResource(obj.vertexBufferStaging);
        }

        //*******************  SSBO
        {
            const uint32 objectCount = static_cast<uint32>(_objects.size());

            ResourceDesc desc = {
                .size          = sizeof(GPUObjectData) * objectCount,
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

        //*******************  UBO
        {
            ResourceDesc desc = {
                .size          = sizeof(GPUSceneData),
                .typeHintFlags = ResourceTypeHint::TH_ConstantBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "UBO",
            };

            _ubo = _renderer->CreateResource(desc);
            _renderer->MapResource(_ubo, _uboMapping);
        }

        //*******************  DESCRIPTOR SET
        {

            DescriptorBinding set0Bindings[2];

            set0Bindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::UBO,
                .stages          = {ShaderStage::Vertex},
            };

            set0Bindings[1] = {
                .binding         = 1,
                .descriptorCount = 1,
                .type            = DescriptorType::CombinedImageSampler,
                .stages          = {ShaderStage::Fragment},
            };

            DescriptorSetDesc set0Desc = {
                .bindings      = set0Bindings,
                .bindingsCount = 2,
            };

            _descriptorSet0 = _renderer->CreateDescriptorSet(set0Desc);

            DescriptorBinding set1Bindings[1];

            set1Bindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex},
            };

            DescriptorSetDesc set1Desc = {
                .bindings      = set1Bindings,
                .bindingsCount = 1,
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ssboSet = _renderer->CreateDescriptorSet(set1Desc);

                DescriptorUpdateBufferDesc bufferDesc = {
                    .setHandle       = _pfd[i].ssboSet,
                    .binding         = 0,
                    .descriptorCount = 1,
                    .resources       = &_pfd[i].ssboGPU,
                    .descriptorType  = DescriptorType::SSBO,
                };

                _renderer->DescriptorUpdateBuffer(bufferDesc);
            }

            DescriptorUpdateBufferDesc uboUpdate = {
                .setHandle       = _descriptorSet0,
                .binding         = 0,
                .descriptorCount = 1,
                .resources       = &_ubo,
                .descriptorType  = DescriptorType::UBO,
            };

            DescriptorUpdateImageDesc imgUpdate = {
                .setHandle       = _descriptorSet0,
                .binding         = 1,
                .descriptorCount = 1,
                .textures        = &_baseColorGPU,
                .samplers        = &_sampler,
                .descriptorType  = DescriptorType::CombinedImageSampler,
            };

            _renderer->DescriptorUpdateBuffer(uboUpdate);
            _renderer->DescriptorUpdateImage(imgUpdate);
        }
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _renderer->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _renderer->Join();

        // Get rid of resources
        for (auto& obj : _objects)
            _renderer->DestroyResource(obj.vertexBufferGPU);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            _renderer->DestroyResource(_pfd[i].ssboStaging);
            _renderer->DestroyResource(_pfd[i].ssboGPU);
            _renderer->DestroyDescriptorSet(_pfd[i].ssboSet);
            _renderer->DestroyUserSemaphore(_pfd[i].copySemaphore);
            _renderer->DestroyCommandStream(_pfd[i].stream);
            _renderer->DestroyCommandStream(_pfd[i].copyStream);
        }

        _renderer->DestroyResource(_ubo);
        _renderer->DestroyDescriptorSet(_descriptorSet0);
        _renderer->DestroyTexture2D(_baseColorGPU);
        _renderer->DestroySampler(_sampler);
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

        auto& currentFrame = _pfd[_renderer->GetCurrentFrameIndex()];

        // Copy SSBO data on copy queue
        {
            LINAGX_VEC<GPUObjectData> objectData;
            objectData.resize(_objects.size());

            for (size_t i = 0; i < _objects.size(); i++)
            {
                glm::mat4 mat             = glm::mat4(1.0f);
                mat                       = glm::rotate(mat, m_elapsedSeconds, glm::vec3(0.0f, 1.0f, 0.0f));
                objectData[i].modelMatrix = mat;
            }

            auto sz = sizeof(GPUObjectData) * _objects.size();
            std::memcpy(currentFrame.ssboMapping, objectData.data(), sz);

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

        // Update scene data
        {
            const glm::mat4 eye        = glm::lookAt(glm::vec3(0.0f, 0.0f, 200.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            const glm::mat4 projection = glm::perspective(DEG2RAD(90.0f), static_cast<float>(_window->GetSize().x) / static_cast<float>(_window->GetSize().y), 0.01f, 1000.0f);
            GPUSceneData    sceneData  = {};
            sceneData.viewProj         = projection * eye;
            std::memcpy(_uboMapping, &sceneData, sizeof(GPUSceneData));
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

        // Set shader
        {
            CMDBindPipeline* bindPipeline = currentFrame.stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgram;
        }

        // Bind the descriptors
        {
            CMDBindDescriptorSets* bindSets = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindSets->firstSet              = 0;
            bindSets->setCount              = 2;
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSet0, currentFrame.ssboSet);
            bindSets->isCompute             = false;
        }

        // Per object, bind vertex buffers, push constants and draw.
        {
            for (auto& obj : _objects)
            {
                CMDBindVertexBuffers* vtx = currentFrame.stream->AddCommand<CMDBindVertexBuffers>();
                vtx->slot                 = 0;
                vtx->resource             = obj.vertexBufferGPU;
                vtx->vertexSize           = sizeof(Vertex);
                vtx->offset               = 0;

                CMDBindConstants* constants = currentFrame.stream->AddCommand<CMDBindConstants>();
                constants->size             = sizeof(int);
                constants->stages           = currentFrame.stream->EmplaceAuxMemory<ShaderStage>(ShaderStage::Vertex);
                constants->stagesSize       = 1;
                constants->offset           = 0;
                constants->data             = currentFrame.stream->EmplaceAuxMemory<uint32>(obj.index);

                CMDDrawInstanced* draw       = currentFrame.stream->AddCommand<CMDDrawInstanced>();
                draw->instanceCount          = 1;
                draw->startInstanceLocation  = 0;
                draw->startVertexLocation    = 0;
                draw->vertexCountPerInstance = static_cast<uint32>(obj.vertices.size());
            }
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
