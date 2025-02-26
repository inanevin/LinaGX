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

Example: RenderTargetsGLTF

Example rendering a GLTF fox model with animations from 2 different angles into 2 different render targets
and displaying them both on a final quad.

Demonstrates:

- Everything from previous examples.
- GLTF loading and parsing utilities.
- Multiple render passes/targets.

*/

#include "RenderTargetsGLTF.hpp"
#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include <iostream>
#include <cstdarg>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <LinaGX/Utility/PlatformUtility.hpp>
#include <LinaGX/Utility/ImageUtility.hpp>
#include <LinaGX/Utility/ModelUtility.hpp>

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

    struct Vertex
    {
        LGXVector3 position      = {};
        LGXVector2 uv            = {};
        LGXVector4 inBoneIndices = {};
        LGXVector4 inBoneWeights = {};
    };

    struct Object
    {
        std::vector<Vertex> vertices;
        uint32              index = 0;

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
        glm::mat4 bones[31] = {};
    };

    // Shaders.
    uint16 _shaderProgramColorPass = 0;
    uint16 _shaderProgramFinalPass = 0;
    uint16 _quadShaderProgram      = 0;

    // Objects
    LinaGX::ModelData   _modelData   = {};
    ModelTexture*       _baseTexture = nullptr;
    std::vector<Object> _objects;

    // Resources
    uint32 _sampler                 = 0;
    uint16 _descriptorSetSceneData0 = 0;
    uint16 _descriptorSetSceneData1 = 0;
    uint16 _descriptorSetTexture    = 0;
    uint32 _ubo0                    = 0;
    uint32 _ubo1                    = 0;
    uint8* _uboMapping0             = 0;
    uint8* _uboMapping1             = 0;
    uint32 _baseColorGPU            = 0;

    struct PerFrameData
    {
        uint32                 ssboStaging              = 0;
        uint32                 ssboGPU                  = 0;
        uint8*                 ssboMapping              = nullptr;
        uint16                 ssboSet                  = 0;
        LinaGX::CommandStream* stream                   = nullptr;
        LinaGX::CommandStream* copyStream               = nullptr;
        uint16                 copySemaphore            = 0;
        uint64                 copySemaphoreValue       = 0;
        uint32                 renderTargetColor        = 0;
        uint32                 renderTargetDepth        = 0;
        uint16                 descriptorSetQuadTexture = 0;
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
            LinaGX::Config.gpu             = PreferredGPUType::Discrete;
            LinaGX::Config.framesInFlight  = FRAMES_IN_FLIGHT;
            LinaGX::Config.backbufferCount = BACK_BUFFER;
            LinaGX::Config.gpuLimits       = {};
            LinaGX::Config.logLevel        = LogLevel::Verbose;
            LinaGX::Config.errorCallback   = LogError;
            LinaGX::Config.infoCallback    = LogInfo;

            _lgx = new LinaGX::Instance();
            _lgx->Initialize();

            std::vector<LinaGX::Format> formatSupportCheck = {LinaGX::Format::B8G8R8A8_UNORM, LinaGX::Format::R8G8B8A8_UNORM, LinaGX::Format::R8G8B8A8_SRGB, LinaGX::Format::D32_SFLOAT};
            for (auto fmt : formatSupportCheck)
            {
                const LinaGX::FormatSupportInfo fsi = _lgx->GetFormatSupport(fmt);

                if (fsi.format == LinaGX::Format::UNDEFINED)
                    LOGE("Current GPU does not support the formats required by this example!");
            }
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX RenderTargetsGLTF", 0, 0, 800, 800, WindowStyle::WindowedApplication);
            App::RegisterWindowCallbacks(_window);
            _windowX = _window->GetSize().x;
            _windowY = _window->GetSize().y;
        }

        //******************* DEFAULT SHADER CREATION
        {
            // Compile shaders.
            const std::string             vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/vert.glsl");
            const std::string             fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/frag.glsl");
            ShaderLayout                  outLayout  = {};
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

            ShaderColorAttachment att = ShaderColorAttachment{
                .format          = Format::R8G8B8A8_SRGB,
                .blendAttachment = {},
            };

            ShaderDepthStencilDesc depthStencilDesc = {
                .depthStencilAttachmentFormat = Format::D32_SFLOAT,
                .depthWrite                   = true,
                .depthTest                    = true,
                .depthCompare                 = CompareOp::Less,
                .stencilEnabled               = false,
            };

            // Create shader program with vertex & fragment stages.
            ShaderDesc shaderDesc = {
                .stages           = compileData,
                .colorAttachments = {att},
                .depthStencilDesc = depthStencilDesc,
                .layout           = outLayout,
                .polygonMode      = PolygonMode::Fill,
                .cullMode         = CullMode::Back,
                .frontFace        = FrontFace::CCW,
                .topology         = Topology::TriangleList,
            };

            _shaderProgramColorPass = _lgx->CreateShader(shaderDesc);

            shaderDesc.colorAttachments[0].format                    = Format::B8G8R8A8_UNORM;
            shaderDesc.depthStencilDesc.depthWrite                   = false;
            shaderDesc.depthStencilDesc.depthStencilAttachmentFormat = Format::UNDEFINED;
            _shaderProgramFinalPass                                  = _lgx->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            for (auto& data : compileData)
                free(data.outBlob.ptr);
        }

        //******************* QUAD SHADER CREATION
        {
            // Compile shaders.
            const std::string             vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/quad_vert.glsl");
            const std::string             fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/quad_frag.glsl");
            ShaderLayout                  outLayout  = {};
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

            ShaderColorAttachment att = ShaderColorAttachment{
                .format          = Format::B8G8R8A8_UNORM,
                .blendAttachment = {},
            };

            ShaderDepthStencilDesc depthStencilDesc = {
                .depthStencilAttachmentFormat = Format::UNDEFINED,
            };

            // Create shader program with vertex & fragment stages.
            ShaderDesc shaderDesc = {
                .stages           = compileData,
                .colorAttachments = {att},
                .depthStencilDesc = depthStencilDesc,
                .layout           = outLayout,
                .polygonMode      = PolygonMode::Fill,
                .cullMode         = CullMode::None,
                .frontFace        = FrontFace::CCW,
                .topology         = Topology::TriangleList,
            };

            _quadShaderProgram = _lgx->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            for (auto& data : compileData)
                free(data.outBlob.ptr);
        }

        //******************* RENDER TARGET
        {
            CreateRenderTargets();
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
            {
                _pfd[i].stream        = _lgx->CreateCommandStream({CommandType::Graphics, 100, 4096, 4096, 64});
                _pfd[i].copyStream    = _lgx->CreateCommandStream({CommandType::Transfer, 100, 4096, 4096, 64});
                _pfd[i].copySemaphore = _lgx->CreateUserSemaphore();
            }
        }

        //*******************  MODEL (Vertex Data & Object Array)
        {
            LinaGX::LoadGLTFBinary("Resources/Models/Fox.glb", _modelData);

            for (uint32 i = 0; i < _modelData.allNodes.size(); i++)
            {
                ModelNode* node = _modelData.allNodes[i];

                if (node->mesh != nullptr)
                {
                    _objects.push_back({});
                    Object& obj = _objects[_objects.size() - 1];
                    obj.index   = static_cast<uint32>(_objects.size() - 1);

                    for (uint32 j = 0; j < node->mesh->primitives.size(); j++)
                    {
                        ModelMeshPrimitive* prim = node->mesh->primitives[j];

                        for (uint32 k = 0; k < prim->vertexCount; k++)
                        {
                            Vertex vtx          = {};
                            vtx.position        = prim->positions[k];
                            vtx.uv              = prim->texCoords[k];
                            vtx.inBoneWeights   = prim->weights[k];
                            vtx.inBoneIndices.x = static_cast<float>(prim->jointsui16[k].x);
                            vtx.inBoneIndices.y = static_cast<float>(prim->jointsui16[k].y);
                            vtx.inBoneIndices.z = static_cast<float>(prim->jointsui16[k].z);
                            vtx.inBoneIndices.w = static_cast<float>(prim->jointsui16[k].w);
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

                obj.vertexBufferStaging = _lgx->CreateResource(vbDesc);
                vbDesc.heapType         = ResourceHeap::GPUOnly;
                obj.vertexBufferGPU     = _lgx->CreateResource(vbDesc);

                uint8* vtxData = nullptr;
                _lgx->MapResource(obj.vertexBufferStaging, vtxData);
                std::memcpy(vtxData, obj.vertices.data(), vertexBufferSize);
                _lgx->UnmapResource(obj.vertexBufferStaging);
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

            _sampler = _lgx->CreateSampler(samplerDesc);

            // Will be base color, skipping the whole material shenanigans from gltf for now.
            _baseTexture = _modelData.allTextures[0];

            // Create gpu resource
            TextureDesc desc = {
                .format    = Format::R8G8B8A8_UNORM,
                .flags     = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
                .width     = _baseTexture->buffer.width,
                .height    = _baseTexture->buffer.height,
                .mipLevels = 1,
                .debugName = "Lina Logo",
            };
            _baseColorGPU = _lgx->CreateTexture(desc);
        }

        //******************* TRANSFER
        {
            TextureBuffer txtBuffer = {
                .pixels        = _baseTexture->buffer.pixels,
                .width         = _baseTexture->buffer.width,
                .height        = _baseTexture->buffer.height,
                .bytesPerPixel = 4,
            };

            // Barrier to transfer destination.
            {
                LinaGX::CMDBarrier* textureBarrier   = _pfd[0].copyStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = 1;
                textureBarrier->textureBarriers      = _pfd[0].copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
                textureBarrier->srcStageFlags        = LinaGX::PSF_TopOfPipe;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;
                auto& barrier                        = textureBarrier->textureBarriers[0];
                barrier.texture                      = _baseColorGPU;
                barrier.toState                      = LinaGX::TextureState::TransferDestination;
                barrier.isSwapchain                  = false;
                barrier.srcAccessFlags               = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
                barrier.dstAccessFlags               = LinaGX::AF_TransferWrite;
            }

            // Copy texture
            CMDCopyBufferToTexture2D* copyTxt = _pfd[0].copyStream->AddCommand<CMDCopyBufferToTexture2D>();
            copyTxt->destTexture              = _baseColorGPU;
            copyTxt->mipLevels                = 1;
            copyTxt->buffers                  = _pfd[0].copyStream->EmplaceAuxMemory<TextureBuffer>(txtBuffer);
            copyTxt->destinationSlice         = 0;

            // Barrier back to sampling.
            {
                LinaGX::CMDBarrier* textureBarrier   = _pfd[0].copyStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = 1;
                textureBarrier->textureBarriers      = _pfd[0].copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
                textureBarrier->srcStageFlags        = LinaGX::PSF_Transfer;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;
                auto& barrier                        = textureBarrier->textureBarriers[0];
                barrier.texture                      = _baseColorGPU;
                barrier.toState                      = LinaGX::TextureState::ShaderRead;
                barrier.srcAccessFlags               = LinaGX::AF_TransferWrite;
                barrier.dstAccessFlags               = LinaGX::AF_TransferRead;
                barrier.isSwapchain                  = false;
            }

            for (auto& obj : _objects)
            {
                // Record copy command.
                CMDCopyResource* copyVtxBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
                copyVtxBuf->source          = obj.vertexBufferStaging;
                copyVtxBuf->destination     = obj.vertexBufferGPU;
            }

            _lgx->CloseCommandStreams(&_pfd[0].copyStream, 1);

            // Execute copy command on the transfer queue, signal a semaphore when it's done and wait for it on the CPU side.
            _pfd[0].copySemaphoreValue++;
            _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &_pfd[0].copyStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &_pfd[0].copySemaphore, .signalValues = &_pfd[0].copySemaphoreValue});
            _lgx->WaitForUserSemaphore(_pfd[0].copySemaphore, _pfd[0].copySemaphoreValue);

            // Not needed anymore.
            for (auto& obj : _objects)
                _lgx->DestroyResource(obj.vertexBufferStaging);
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
                _pfd[i].ssboStaging = _lgx->CreateResource(desc);
                _lgx->MapResource(_pfd[i].ssboStaging, _pfd[i].ssboMapping);
                desc.heapType   = ResourceHeap::GPUOnly;
                _pfd[i].ssboGPU = _lgx->CreateResource(desc);
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

            _ubo0 = _lgx->CreateResource(desc);
            _ubo1 = _lgx->CreateResource(desc);
            _lgx->MapResource(_ubo0, _uboMapping0);
            _lgx->MapResource(_ubo1, _uboMapping1);
        }

        //*******************  DESCRIPTOR SET
        {
            DescriptorBinding set0Binding = {
                .descriptorCount = 1,
                .type            = DescriptorType::CombinedImageSampler,
                .stages          = {ShaderStage::Fragment},
            };

            DescriptorSetDesc set0Desc = {
                .bindings = {set0Binding},
            };

            _descriptorSetTexture = _lgx->CreateDescriptorSet(set0Desc);

            DescriptorUpdateImageDesc imgUpdate = {
                .setHandle = _descriptorSetTexture,
                .binding   = 0,
                .textures  = {_baseColorGPU},
                .samplers  = {_sampler},
            };
            _lgx->DescriptorUpdateImage(imgUpdate);

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].descriptorSetQuadTexture = _lgx->CreateDescriptorSet(set0Desc);

                imgUpdate.setHandle = _pfd[i].descriptorSetQuadTexture;
                imgUpdate.textures  = {_pfd[i].renderTargetColor};
                _lgx->DescriptorUpdateImage(imgUpdate);
            }

            DescriptorBinding set1Binding = {
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex},
            };

            DescriptorSetDesc set1Desc = {
                .bindings = {set1Binding},
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ssboSet = _lgx->CreateDescriptorSet(set1Desc);

                DescriptorUpdateBufferDesc bufferDesc = {
                    .setHandle = _pfd[i].ssboSet,
                    .binding   = 0,
                    .buffers   = {_pfd[i].ssboGPU},
                };

                _lgx->DescriptorUpdateBuffer(bufferDesc);
            }

            DescriptorBinding set2Binding = {
                .descriptorCount = 1,
                .type            = DescriptorType::UBO,
                .stages          = {ShaderStage::Vertex},
            };

            DescriptorSetDesc set2Desc = {
                .bindings = {set2Binding},
            };

            _descriptorSetSceneData0 = _lgx->CreateDescriptorSet(set2Desc);
            _descriptorSetSceneData1 = _lgx->CreateDescriptorSet(set2Desc);

            DescriptorUpdateBufferDesc uboUpdate = {
                .setHandle = _descriptorSetSceneData0,
                .binding   = 0,
                .buffers   = {_ubo0},
            };

            _lgx->DescriptorUpdateBuffer(uboUpdate);

            uboUpdate.setHandle = _descriptorSetSceneData1;
            uboUpdate.buffers   = {_ubo1};

            _lgx->DescriptorUpdateBuffer(uboUpdate);
        }

    } // namespace LinaGX::Examples

    void Example::OnWindowResized(uint32 w, uint32 h)
    {
        LGXVector2ui monitor = _window->GetMonitorSize();

        SwapchainRecreateDesc resizeDesc = {
            .swapchain    = _swapchain,
            .width        = w,
            .height       = h,
            .isFullscreen = w == monitor.x && h == monitor.y,
        };

        _lgx->RecreateSwapchain(resizeDesc);

        // re-create render targets.
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            _lgx->DestroyTexture(_pfd[i].renderTargetColor);
            _lgx->DestroyTexture(_pfd[i].renderTargetDepth);
        }

        CreateRenderTargets();

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            DescriptorUpdateImageDesc imgUpdate = {
                .setHandle = _pfd[i].descriptorSetQuadTexture,
                .binding   = 0,
                .textures  = {_pfd[i].renderTargetColor},
                .samplers  = {_sampler},
            };
            _lgx->DescriptorUpdateImage(imgUpdate);
        }
        _windowX = w;
        _windowY = h;
    }

    void Example::CreateRenderTargets()
    {
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            TextureDesc desc = {
                .format    = Format::R8G8B8A8_SRGB,
                .flags     = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest | LinaGX::TextureFlags::TF_ColorAttachment,
                .width     = _window->GetSize().x,
                .height    = _window->GetSize().y,
                .mipLevels = 1,
                .debugName = "LinaGXRTTexture",
            };

            _pfd[i].renderTargetColor = _lgx->CreateTexture(desc);

            desc.format    = Format::D32_SFLOAT;
            desc.flags     = LinaGX::TextureFlags::TF_DepthTexture | LinaGX::TextureFlags::TF_Sampled;
            desc.debugName = "LinaGXRTDepthTexture";

            _pfd[i].renderTargetDepth = _lgx->CreateTexture(desc);
        }
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _lgx->Join();

        // Get rid of resources
        for (auto& obj : _objects)
            _lgx->DestroyResource(obj.vertexBufferGPU);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            _lgx->DestroyResource(_pfd[i].ssboStaging);
            _lgx->DestroyResource(_pfd[i].ssboGPU);
            _lgx->DestroyDescriptorSet(_pfd[i].ssboSet);
            _lgx->DestroyUserSemaphore(_pfd[i].copySemaphore);
            _lgx->DestroyCommandStream(_pfd[i].stream);
            _lgx->DestroyCommandStream(_pfd[i].copyStream);
            _lgx->DestroyTexture(_pfd[i].renderTargetColor);
            _lgx->DestroyTexture(_pfd[i].renderTargetDepth);
            _lgx->DestroyDescriptorSet(_pfd[i].descriptorSetQuadTexture);
        }

        _lgx->DestroyResource(_ubo0);
        _lgx->DestroyResource(_ubo1);
        _lgx->DestroyDescriptorSet(_descriptorSetTexture);
        _lgx->DestroyDescriptorSet(_descriptorSetSceneData0);
        _lgx->DestroyDescriptorSet(_descriptorSetSceneData1);
        _lgx->DestroyTexture(_baseColorGPU);
        _lgx->DestroySampler(_sampler);
        _lgx->DestroySwapchain(_swapchain);
        _lgx->DestroyShader(_shaderProgramColorPass);
        _lgx->DestroyShader(_shaderProgramFinalPass);
        _lgx->DestroyShader(_quadShaderProgram);

        // Terminate renderer & shutdown app.
        delete _lgx;
        App::Shutdown();
    }

    LGXVector4 Lerp(const LGXVector4 start, const LGXVector4 end, float t)
    {
        LGXVector4 result;
        result.x = start.x + (end.x - start.x) * t;
        result.y = start.y + (end.y - start.y) * t;
        result.z = start.z + (end.z - start.z) * t;
        result.w = start.w + (end.w - start.w) * t;
        return result;
    }

    LGXVector4 InterpolateKeyframes(const ModelAnimationChannel& channel, float time)
    {
        // Find the two keyframes we need to interpolate between.
        int index1 = -1;
        int index2 = -1;
        for (int i = 0; i < channel.keyframeTimes.size(); ++i)
        {
            if (time < channel.keyframeTimes[i])
            {
                index2 = i;
                if (i > 0)
                {
                    index1 = i - 1;
                }
                break;
            }
        }

        // If time is after the last keyframe, use the last keyframe value.
        if (index1 == -1)
        {
            index1 = index2 = static_cast<int>(channel.keyframeTimes.size() - 1);
        }

        // Extract the keyframe values.
        LGXVector4 value1, value2;

        if (channel.targetProperty == GLTFAnimationProperty::Rotation)
        {
            value1.x = channel.values[index1 * 4 + 0];
            value1.y = channel.values[index1 * 4 + 1];
            value1.z = channel.values[index1 * 4 + 2];
            value1.w = channel.values[index1 * 4 + 3];

            value2.x = channel.values[index2 * 4 + 0];
            value2.y = channel.values[index2 * 4 + 1];
            value2.z = channel.values[index2 * 4 + 2];
            value2.w = channel.values[index2 * 4 + 3];
        }
        else
        {
            value1.x = channel.values[index1 * 3 + 0];
            value1.y = channel.values[index1 * 3 + 1];
            value1.z = channel.values[index1 * 3 + 2];

            value2.x = channel.values[index2 * 3 + 0];
            value2.y = channel.values[index2 * 3 + 1];
            value2.z = channel.values[index2 * 3 + 2];
        }

        // Perform the interpolation.
        float alpha = 0.0f;
        if (index1 != index2)
        {
            alpha = (time - channel.keyframeTimes[index1]) / (channel.keyframeTimes[index2] - channel.keyframeTimes[index1]);
        }

        return Lerp(value1, value2, alpha);
    }

    void SampleAnimation(LinaGX::ModelSkin* skin, LinaGX::ModelAnimation* anim, float time)
    {
        // Very naive animation sampling
        // Simply linear-interpolate pos/rot/scale.
        for (const auto& channel : anim->channels)
        {
            int jointIndex = -1;
            for (int i = 0; i < skin->joints.size(); ++i)
            {
                if (skin->joints[i] == channel.targetNode)
                {
                    jointIndex = i;
                    break;
                }
            }

            if (jointIndex == -1)
                continue;

            LGXVector4 result = InterpolateKeyframes(channel, time);

            if (channel.targetProperty == GLTFAnimationProperty::Position)
            {
                skin->joints[jointIndex]->position = {result.x, result.y, result.z};
            }
            else if (channel.targetProperty == GLTFAnimationProperty::Rotation)
            {
                skin->joints[jointIndex]->quatRot = {result.x, result.y, result.z, result.w};
            }
            else if (channel.targetProperty == GLTFAnimationProperty::Scale)
            {
                skin->joints[jointIndex]->scale = {result.x, result.y, result.z};
            }
        }
    }

    glm::mat4 TranslateRotateScale(const LGXVector3& pos, const LGXVector4& rot, const LGXVector3& scale)
    {
        glm::vec3 p     = glm::vec3(pos.x, pos.y, pos.z);
        glm::vec3 s     = glm::vec3(scale.x, scale.y, scale.z);
        glm::quat r     = glm::quat(rot.w, rot.x, rot.y, rot.z);
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, p);
        model *= glm::mat4_cast(r);
        model = glm::scale(model, s);
        return model;
    }

    glm::mat4 CalculateGlobalMatrix(LinaGX::ModelNode* node)
    {
        const glm::mat4 nodeLocal = node->localMatrix.empty() ? TranslateRotateScale(node->position, node->quatRot, node->scale) : glm::make_mat4(node->localMatrix.data());
        if (node->parent != nullptr)
            return CalculateGlobalMatrix(node->parent) * nodeLocal;
        return nodeLocal;
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        _lgx->TickWindowSystem();
    }

    void Example::OnRender()
    {
        TextureDesc desc = {
            .format    = Format::R8G8B8A8_SRGB,
            .flags     = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest | LinaGX::TextureFlags::TF_ColorAttachment,
            .width     = _window->GetSize().x,
            .height    = _window->GetSize().y,
            .mipLevels = 1,
            .debugName = "LinaGXRTTexture",
        };

        // Let LinaGX know we are starting a new frame.
        _lgx->StartFrame();

        auto& currentFrame = _pfd[_lgx->GetCurrentFrameIndex()];

        // Sample animation
        {
            auto         targetAnimation = _modelData.allAnims[0];
            static float time            = 0.0f;
            time += m_deltaSeconds;
            if (time > targetAnimation->duration)
                time = 0.0f;
            SampleAnimation(_modelData.allSkins[0], targetAnimation, time);
        }

        // Copy SSBO data on copy queue
        {
            std::vector<GPUObjectData> objectData;
            objectData.resize(_objects.size());
            // Inverse root-global matrix.
            auto            root       = _modelData.allSkins[0]->rootJoint;
            const glm::mat4 rootGlobal = CalculateGlobalMatrix(root);
            const glm::mat4 inv        = glm::inverse(rootGlobal);

            for (size_t i = 0; i < _objects.size(); i++)
            {
                // Model.
                glm::mat4 mat             = glm::mat4(1.0f);
                glm::quat q               = glm::quat(glm::vec3(0.0f, DEG2RAD(45.0f), 0.0f));
                mat                       = TranslateRotateScale({0.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {1.0f, 1.0f, 1.0f});
                objectData[i].modelMatrix = mat;

                // Assign all skinning matrices.
                // This is a very basic implementation of skinned animations
                uint32 k = 0;
                for (auto joint : _modelData.allSkins[0]->joints)
                {
                    const glm::mat4 jointGlobal = CalculateGlobalMatrix(joint);
                    const glm::mat4 inverseBind = glm::make_mat4(joint->inverseBindMatrix.data());
                    objectData[i].bones[k]      = inv * jointGlobal * inverseBind;
                    k++;
                }
            }

            auto sz = sizeof(GPUObjectData) * _objects.size();
            std::memcpy(currentFrame.ssboMapping, objectData.data(), sz);

            CMDCopyResource* copyRes = currentFrame.copyStream->AddCommand<CMDCopyResource>();
            copyRes->source          = currentFrame.ssboStaging;
            copyRes->destination     = currentFrame.ssboGPU;

            _lgx->CloseCommandStreams(&currentFrame.copyStream, 1);

            currentFrame.copySemaphoreValue++;

            SubmitDesc submit = {
                .targetQueue      = _lgx->GetPrimaryQueue(CommandType::Transfer),
                .streams          = &currentFrame.copyStream,
                .streamCount      = 1,
                .useWait          = false,
                .useSignal        = true,
                .signalCount      = 1,
                .signalSemaphores = &currentFrame.copySemaphore,
                .signalValues     = &currentFrame.copySemaphoreValue,
            };

            _lgx->SubmitCommandStreams(submit);
        }

        // Update scene data
        {
            const glm::mat4 eye        = glm::lookAt(glm::vec3(0.0f, 0.0f, 200.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            const glm::mat4 projection = glm::perspective(DEG2RAD(90.0f), static_cast<float>(_window->GetSize().x) / static_cast<float>(_window->GetSize().y), 0.01f, 1000.0f);
            GPUSceneData    sceneData  = {};
            sceneData.viewProj         = projection * eye;
            std::memcpy(_uboMapping0, &sceneData, sizeof(GPUSceneData));
        }

        Viewport     viewport = {.x = 0, .y = 0, .width = _windowX, .height = _windowY, .minDepth = 0.0f, .maxDepth = 1.0f};
        ScissorsRect sc       = {.x = 0, .y = 0, .width = _windowX, .height = _windowY};

        // Barrier to Color Attachment RT and Swapchain
        {
            LinaGX::CMDBarrier* barrier  = currentFrame.stream->AddCommand<LinaGX::CMDBarrier>();
            barrier->srcStageFlags       = LinaGX::PSF_TopOfPipe;
            barrier->dstStageFlags       = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
            barrier->textureBarrierCount = 3;
            barrier->textureBarriers     = currentFrame.stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 3);

            // RT 1 To Color
            barrier->textureBarriers[0].srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
            barrier->textureBarriers[0].dstAccessFlags = LinaGX::AF_ColorAttachmentRead;
            barrier->textureBarriers[0].isSwapchain    = false;
            barrier->textureBarriers[0].texture        = currentFrame.renderTargetColor;
            barrier->textureBarriers[0].toState        = LinaGX::TextureState::ColorAttachment;

            // RT 1 To Depth
            barrier->textureBarriers[1].srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
            barrier->textureBarriers[1].dstAccessFlags = LinaGX::AF_DepthStencilAttachmentRead;
            barrier->textureBarriers[1].isSwapchain    = false;
            barrier->textureBarriers[1].texture        = currentFrame.renderTargetDepth;
            barrier->textureBarriers[1].toState        = LinaGX::TextureState::DepthStencilAttachment;

            // Swp to color
            barrier->textureBarriers[2].srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
            barrier->textureBarriers[2].dstAccessFlags = LinaGX::AF_ColorAttachmentRead;
            barrier->textureBarriers[2].isSwapchain    = true;
            barrier->textureBarriers[2].texture        = static_cast<uint32>(_swapchain);
            barrier->textureBarriers[2].toState        = LinaGX::TextureState::ColorAttachment;
        }

        // Render pass 1.
        {
            CMDBeginRenderPass* beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();

            beginRenderPass->viewport = viewport;
            beginRenderPass->scissors = sc;

            RenderPassColorAttachment colorAttachment;
            colorAttachment.clearColor            = {167.0f / 255.0f, 49.0f / 255.0f, 105.0f / 255.0f, 1.0f};
            colorAttachment.texture               = currentFrame.renderTargetColor;
            colorAttachment.isSwapchain           = false;
            colorAttachment.loadOp                = LoadOp::Clear;
            colorAttachment.storeOp               = StoreOp::Store;
            beginRenderPass->colorAttachments     = currentFrame.stream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->colorAttachmentCount = 1;

            beginRenderPass->depthStencilAttachment.useDepth     = true;
            beginRenderPass->depthStencilAttachment.depthLoadOp  = LoadOp::Clear;
            beginRenderPass->depthStencilAttachment.depthStoreOp = StoreOp::Store;
            beginRenderPass->depthStencilAttachment.clearDepth   = 1.0f;
            beginRenderPass->depthStencilAttachment.texture      = currentFrame.renderTargetDepth;
            beginRenderPass->depthStencilAttachment.useStencil   = false;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = currentFrame.stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgramColorPass;
        }

        // Bind the descriptors
        {
            CMDBindDescriptorSets* bindSets = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindSets->firstSet              = 0;
            bindSets->setCount              = 3;
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSetTexture, currentFrame.ssboSet, _descriptorSetSceneData0);
            bindSets->isCompute             = false;
            bindSets->dynamicOffsetCount    = 0;
            bindSets->layoutSource          = DescriptorSetsLayoutSource::LastBoundShader;
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

                CMDBindConstants* constants2 = currentFrame.stream->AddCommand<CMDBindConstants>();
                constants2->size             = sizeof(float);
                constants2->stages           = currentFrame.stream->EmplaceAuxMemory<ShaderStage>(ShaderStage::Fragment);
                constants2->stagesSize       = 1;
                constants2->offset           = sizeof(float);
                constants2->data             = currentFrame.stream->EmplaceAuxMemory<uint32>(1);

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
        }

        // Barrier to Shader Read
        {
            LinaGX::CMDBarrier* barrier  = currentFrame.stream->AddCommand<LinaGX::CMDBarrier>();
            barrier->srcStageFlags       = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
            barrier->dstStageFlags       = LinaGX::PSF_FragmentShader;
            barrier->textureBarrierCount = 2;
            barrier->textureBarriers     = currentFrame.stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);

            // RT 1 To Color
            barrier->textureBarriers[0].srcAccessFlags = LinaGX::AF_ColorAttachmentRead;
            barrier->textureBarriers[0].dstAccessFlags = LinaGX::AF_ShaderRead;
            barrier->textureBarriers[0].isSwapchain    = false;
            barrier->textureBarriers[0].texture        = currentFrame.renderTargetColor;
            barrier->textureBarriers[0].toState        = LinaGX::TextureState::ShaderRead;

            // RT 1 To Depth
            barrier->textureBarriers[1].srcAccessFlags = AF_DepthStencilAttachmentRead;
            barrier->textureBarriers[1].dstAccessFlags = LinaGX::AF_ShaderRead;
            barrier->textureBarriers[1].isSwapchain    = false;
            barrier->textureBarriers[1].texture        = currentFrame.renderTargetDepth;
            barrier->textureBarriers[1].toState        = LinaGX::TextureState::ShaderRead;
        }

        // Update scene data
        {
            const glm::mat4 eye        = glm::lookAt(glm::vec3(0.0f, 0.0f, -200.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            const glm::mat4 projection = glm::perspective(DEG2RAD(90.0f), static_cast<float>(_window->GetSize().x) / static_cast<float>(_window->GetSize().y), 0.01f, 1000.0f);
            GPUSceneData    sceneData  = {};
            sceneData.viewProj         = projection * eye;
            std::memcpy(_uboMapping1, &sceneData, sizeof(GPUSceneData));
        }

        // Render pass swapchain.
        {
            CMDBeginRenderPass*       beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();
            RenderPassColorAttachment colorAttachment;
            colorAttachment.clearColor                         = {32.0f / 255.0f, 17.0f / 255.0f, 39.0f / 255.0f, 1.0f};
            colorAttachment.texture                            = static_cast<uint32>(_swapchain);
            colorAttachment.isSwapchain                        = true;
            colorAttachment.loadOp                             = LoadOp::Clear;
            colorAttachment.storeOp                            = StoreOp::Store;
            beginRenderPass->colorAttachments                  = currentFrame.stream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->colorAttachmentCount              = 1;
            beginRenderPass->viewport                          = viewport;
            beginRenderPass->scissors                          = sc;
            beginRenderPass->depthStencilAttachment.useDepth   = false;
            beginRenderPass->depthStencilAttachment.useStencil = false;
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = currentFrame.stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgramFinalPass;
        }

        // Bind the descriptors
        {
            CMDBindDescriptorSets* bindSets = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindSets->firstSet              = 0;
            bindSets->setCount              = 3;
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSetTexture, currentFrame.ssboSet, _descriptorSetSceneData1);
            bindSets->isCompute             = false;
            bindSets->dynamicOffsetCount    = 0;
            bindSets->layoutSource          = DescriptorSetsLayoutSource::LastBoundShader;
        }

        // Per object, bind vertex buffers, push constants and draw.
        {
            for (const auto& obj : _objects)
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

                CMDBindConstants* constants2 = currentFrame.stream->AddCommand<CMDBindConstants>();
                constants2->size             = sizeof(float);
                constants2->stages           = currentFrame.stream->EmplaceAuxMemory<ShaderStage>(ShaderStage::Fragment);
                constants2->stagesSize       = 1;
                constants2->offset           = sizeof(float);
                constants2->data             = currentFrame.stream->EmplaceAuxMemory<uint32>(1);

                CMDDrawInstanced* draw       = currentFrame.stream->AddCommand<CMDDrawInstanced>();
                draw->instanceCount          = 1;
                draw->startInstanceLocation  = 0;
                draw->startVertexLocation    = 0;
                draw->vertexCountPerInstance = static_cast<uint32>(obj.vertices.size());
            }
        }

        // Set shader
        {
            CMDBindPipeline* bindPipeline = currentFrame.stream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _quadShaderProgram;
        }

        // Bind the descriptors
        {
            CMDBindDescriptorSets* bindSets = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindSets->firstSet              = 0;
            bindSets->setCount              = 1;
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(currentFrame.descriptorSetQuadTexture);
            bindSets->isCompute             = false;
            bindSets->dynamicOffsetCount    = 0;
            bindSets->layoutSource          = DescriptorSetsLayoutSource::LastBoundShader;
        }

        // Draw quad
        {
            CMDDrawInstanced* draw       = currentFrame.stream->AddCommand<CMDDrawInstanced>();
            draw->instanceCount          = 1;
            draw->startInstanceLocation  = 0;
            draw->startVertexLocation    = 0;
            draw->vertexCountPerInstance = 6;
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
        _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &currentFrame.stream, .streamCount = 1, .useWait = true, .waitCount = 1, .waitSemaphores = &currentFrame.copySemaphore, .waitValues = &currentFrame.copySemaphoreValue});

        // Present main swapchain.
        _lgx->Present({.swapchains = &_swapchain, .swapchainCount = 1});

        // Let LinaGX know we are finalizing this frame.
        _lgx->EndFrame();
    }

} // namespace LinaGX::Examples
