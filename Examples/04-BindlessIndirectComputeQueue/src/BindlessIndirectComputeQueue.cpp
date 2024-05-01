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

Example: BindlessIndirectComputeQueue

Example rendering 2 GLTF models, avacado and a duck. However avacado is culled from the GPU side using a compute shader.
Rendering is done indirectly, compute shader interrupts the indirect command buffer and sets the instance count of the avacado to 0.
Also, texture bindings are done in a bindless manner.

Demonstrates:

- Everything from previous examples.
- Running compute shaders.
- Using explicit compute queue syncronization with transfer & graphics queues.
- Indirect buffers and indirect argument buffers.
- Bindless textures & descriptor sets.

*/

#include "BindlessIndirectComputeQueue.hpp"
#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include <iostream>
#include <cstdarg>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

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
        LGXVector3 normal        = {};
        LGXVector4 tangents      = {};
        LGXVector4 colors        = {};
        LGXVector4 inBoneIndices = {};
        LGXVector4 inBoneWeights = {};
    };

    struct Mesh
    {
        LINAGX_VEC<Vertex>        vertices;
        LINAGX_VEC<unsigned char> indices;
        IndexType                 indexType = IndexType::Uint16;

        uint32 vertexOffset = 0;
        uint32 indexOffset  = 0;
    };

    struct ConstantsData
    {
        uint32 index             = 0;
        uint32 materialByteIndex = 0;
    };

    struct Object
    {
        LinaGX::ModelData  model;
        ConstantsData      constants;
        glm::mat4          modelMatrix;
        LINAGX_VEC<Mesh>   meshes;
        LINAGX_VEC<uint32> texturesGPU;
    };

    struct GPULightData
    {
        glm::vec4 position;
        glm::vec4 color;
    };

    struct GPUSceneData
    {
        glm::mat4 viewProj;
    };

    struct GPUObjectData
    {
        glm::mat4 modelMatrix;
    };

    struct GPUMaterialData
    {
        glm::vec4 baseColor;
        glm::vec4 baseColorTint;
        uint32    albedoIndex = 0;
    };

    // Shaders.
    uint16 _shaderProgram        = 0;
    uint16 _shaderProgramCompute = 0;

    // Objects
    LINAGX_VEC<Object> _objects;

    struct DefaultTexture
    {
        uint32      gpuResource = 0;
        const char* path        = "";
    };

    // Resources
    uint32             _sampler = 0;
    LINAGX_VEC<uint16> _allMaterialDescriptors;
    uint16             _descriptorSetMaterial;
    GPUMaterialData    _material1;
    GPUMaterialData    _material2;

    uint32 _mergedVertexBufferStaging = 0;
    uint32 _mergedVertexBufferGPU     = 0;
    uint32 _mergedIndexBufferStaging  = 0;
    uint32 _mergedIndexBufferGPU      = 0;

    uint32 _depthTexture = 0;

    struct PerFrameData
    {
        uint32                 ssboStaging             = 0;
        uint32                 ssboGPU                 = 0;
        uint8*                 ssboMapping             = nullptr;
        uint16                 ssboSet                 = 0;
        uint32                 ssboMaterialsStaging    = 0;
        uint32                 ssboMaterialsGPU        = 0;
        uint8*                 ssboMaterialsMapping    = nullptr;
        uint16                 ssboMaterialsSet        = 0;
        LinaGX::CommandStream* stream                  = nullptr;
        LinaGX::CommandStream* copyStream              = nullptr;
        LinaGX::CommandStream* computeStream           = nullptr;
        uint16                 copySemaphore           = 0;
        uint16                 computeSemaphore        = 0;
        uint64                 copySemaphoreValue      = 0;
        uint64                 computeSemaphoreValue   = 0;
        uint16                 descriptorSetSceneData0 = 0;
        uint32                 ubo0                    = 0;
        uint8*                 uboMapping0             = 0;
        uint32                 indirectBufferGPU       = 0;
        uint32                 indirectBufferStaging   = 0;
        uint8*                 indirectBufferMapping   = nullptr;
        uint32                 indirectArgsGPU         = 0;
        uint32                 indirectArgsStaging     = 0;
        uint8*                 indirectArgsMapping     = nullptr;
        uint16                 computeSet              = 0;
    };

    PerFrameData _pfd[FRAMES_IN_FLIGHT];

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

    void Example::Initialize()
    {
        App::Initialize();

        //******************* CONFIGURATION & INITIALIZATION
        {
            BackendAPI api = BackendAPI::Vulkan;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::Config.api                               = api;
            LinaGX::Config.gpu                               = PreferredGPUType::Discrete;
            LinaGX::Config.framesInFlight                    = FRAMES_IN_FLIGHT;
            LinaGX::Config.backbufferCount                   = BACK_BUFFER;
            LinaGX::Config.gpuLimits                         = {};
            LinaGX::Config.logLevel                          = LogLevel::Verbose;
            LinaGX::Config.errorCallback                     = LogError;
            LinaGX::Config.infoCallback                      = LogInfo;
            LinaGX::Config.vulkanConfig.enableVulkanFeatures = LinaGX::VulkanFeatureFlags::VKF_Bindless | LinaGX::VulkanFeatureFlags::VKF_UpdateAfterBind | LinaGX::VulkanFeatureFlags::VKF_MultiDrawIndirect;

            _lgx               = new LinaGX::Instance();
            const bool success = _lgx->Initialize();

            if (!success)
            {
                LOGA(false, "Current GPU does not support one of the features required by this example!");
            }

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
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX BindlessIndirectComputeQueue", 0, 0, 800, 800, WindowStyle::WindowedApplication);
            App::RegisterWindowCallbacks(_window);
            _windowX = _window->GetSize().x;
            _windowY = _window->GetSize().y;
        }

        //******************* DEFAULT SHADER CREATION
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

            ColorBlendAttachment blend = {
                .blendEnabled        = true,
                .srcColorBlendFactor = BlendFactor::SrcAlpha,
                .dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha,
                .colorBlendOp        = BlendOp::Add,
                .srcAlphaBlendFactor = BlendFactor::One,
                .dstAlphaBlendFactor = BlendFactor::Zero,
                .alphaBlendOp        = BlendOp::Add,
            };

            ShaderColorAttachment att = ShaderColorAttachment{
                .format          = Format::B8G8R8A8_UNORM,
                .blendAttachment = blend,
            };

            ShaderDepthStencilDesc depthStencilDesc = {
                .depthStencilAttachmentFormat = Format::D32_SFLOAT,
                .depthWrite                   = true,
                .depthTest                    = true,
                .depthCompare                 = CompareOp::Less,
                .stencilEnabled               = false,
            };

            ShaderDesc shaderDesc = {
                .stages              = {{ShaderStage::Vertex, outCompiledBlobs[ShaderStage::Vertex]}, {ShaderStage::Fragment, outCompiledBlobs[ShaderStage::Fragment]}},
                .colorAttachments    = {att},
                .depthStencilDesc    = depthStencilDesc,
                .layout              = outLayout,
                .polygonMode         = PolygonMode::Fill,
                .cullMode            = CullMode::Back,
                .frontFace           = FrontFace::CCW,
                .topology            = Topology::TriangleList,
                .drawIndirectEnabled = true,
            };

            _shaderProgram = _lgx->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            for (auto& [stg, blob] : outCompiledBlobs)
                free(blob.ptr);
        }

        //******************* DEFAULT SHADER CREATION
        {
            // Compile shaders.
            const std::string                         cmpShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/compute.glsl");
            ShaderLayout                              outLayout = {};
            ShaderCompileData                         dataCmp   = {cmpShader.c_str(), "Resources/Shaders/Include"};
            std::unordered_map<ShaderStage, DataBlob> outCompiledBlobs;
            _lgx->CompileShader({{ShaderStage::Compute, dataCmp}}, outCompiledBlobs, outLayout);

            // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.

            ColorBlendAttachment blend = {
                .blendEnabled = false,
            };

            ShaderColorAttachment att = ShaderColorAttachment{
                .format          = Format::B8G8R8A8_UNORM,
                .blendAttachment = blend,
            };

            ShaderDepthStencilDesc depthStencilDesc = {
                .depthStencilAttachmentFormat = Format::D32_SFLOAT,
                .depthWrite                   = true,
                .depthTest                    = true,
                .depthCompare                 = CompareOp::Less,
                .stencilEnabled               = false,
            };

            ShaderDesc shaderDesc = {
                .stages           = {{ShaderStage::Compute, outCompiledBlobs[ShaderStage::Compute]}},
                .colorAttachments = {att},
                .depthStencilDesc = depthStencilDesc,
                .layout           = outLayout,
                .polygonMode      = PolygonMode::Fill,
                .cullMode         = CullMode::Back,
                .frontFace        = FrontFace::CCW,
                .topology         = Topology::TriangleList,
            };

            _shaderProgramCompute = _lgx->CreateShader(shaderDesc);

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

            CreateDepthTexture();

            // Create command stream to record draw calls.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].stream           = _lgx->CreateCommandStream({CommandType::Graphics, 100, 4096, 4096, 64});
                _pfd[i].copyStream       = _lgx->CreateCommandStream({CommandType::Transfer, 100, 4096, 4096, 64});
                _pfd[i].computeStream    = _lgx->CreateCommandStream({CommandType::Compute, 100, 4096, 4096, 64});
                _pfd[i].copySemaphore    = _lgx->CreateUserSemaphore();
                _pfd[i].computeSemaphore = _lgx->CreateUserSemaphore();
            }
        }

        //******************* SAMPLER
        {
            // Sampler
            SamplerDesc samplerDesc = {
                .minFilter  = Filter::Anisotropic,
                .magFilter  = Filter::Anisotropic,
                .mode       = SamplerAddressMode::Repeat,
                .anisotropy = 0,
            };

            _sampler = _lgx->CreateSampler(samplerDesc);
        }

        //*******************  MODEL & TEXTURES
        {
            _objects.push_back({});
            _objects.push_back({});

            glm::mat4 mat                       = glm::mat4(1.0f);
            glm::quat q                         = glm::quat(glm::vec3(0.0f, DEG2RAD(180.0f), 0.0f));
            mat                                 = TranslateRotateScale({-150.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {3000, 3000, 3000});
            auto& avacado                       = _objects[0];
            avacado.modelMatrix                 = mat;
            avacado.constants.index             = 0;
            avacado.constants.materialByteIndex = 0;

            auto& duck                       = _objects[1];
            q                                = glm::quat(glm::vec3(0.0f, DEG2RAD(-90.0f), 0.0f));
            duck.modelMatrix                 = TranslateRotateScale({150.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {1.0f, 1.0f, 1.0f});
            duck.constants.index             = 1;
            duck.constants.materialByteIndex = sizeof(GPUMaterialData) / sizeof(uint32);

            LinaGX::LoadGLTFBinary("Resources/Models/Avocado.glb", avacado.model);
            LinaGX::LoadGLTFBinary("Resources/Models/Duck.glb", duck.model);

            for (auto& obj : _objects)
            {
                obj.texturesGPU.resize(obj.model.allTextures.size());

                const uint32 sz = static_cast<uint32>(obj.model.allTextures.size());
                for (uint32 i = 0; i < sz; i++)
                {
                    auto* txt = obj.model.allTextures[i];

                    TextureDesc desc = {
                        .format    = Format::R8G8B8A8_SRGB,
                        .flags     = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
                        .width     = txt->buffer.width,
                        .height    = txt->buffer.height,
                        .mipLevels = 1,
                        .debugName = "Material Texture",
                    };

                    obj.texturesGPU[i] = _lgx->CreateTexture(desc);
                }

                const uint32 nodesSz = static_cast<uint32>(obj.model.allNodes.size());

                for (uint32 i = 0; i < nodesSz; i++)
                {
                    ModelNode* node = obj.model.allNodes[i];

                    if (node->mesh != nullptr)
                    {
                        const uint32 primSz = static_cast<uint32>(node->mesh->primitives.size());
                        for (uint32 j = 0; j < primSz; j++)
                        {
                            obj.meshes.push_back({});
                            auto& mesh = obj.meshes[obj.meshes.size() - 1];

                            ModelMeshPrimitive* prim = node->mesh->primitives[j];
                            mesh.indexType           = prim->indexType;

                            if (!prim->indices.empty())
                                mesh.indices = prim->indices;

                            for (uint32 k = 0; k < prim->vertexCount; k++)
                            {
                                Vertex vtx   = {};
                                vtx.position = prim->positions[k];
                                vtx.uv       = prim->texCoords[k];

                                if (!prim->weights.empty())
                                    vtx.inBoneWeights = prim->weights[k];

                                if (!prim->jointsui16.empty())
                                {
                                    vtx.inBoneIndices.x = static_cast<float>(prim->jointsui16[k].x);
                                    vtx.inBoneIndices.y = static_cast<float>(prim->jointsui16[k].y);
                                    vtx.inBoneIndices.z = static_cast<float>(prim->jointsui16[k].z);
                                    vtx.inBoneIndices.w = static_cast<float>(prim->jointsui16[k].w);
                                }

                                if (!prim->normals.empty())
                                    vtx.normal = prim->normals[k];

                                if (!prim->tangents.empty())
                                    vtx.tangents = prim->tangents[k];

                                if (!prim->colors.empty())
                                    vtx.colors = prim->colors[k];

                                mesh.vertices.push_back(vtx);
                            }
                        }
                    }
                }
            }

            LINAGX_VEC<Vertex>        combinedVertices;
            LINAGX_VEC<unsigned char> combinedIndices;

            for (auto& obj : _objects)
            {
                for (auto& mesh : obj.meshes)
                {
                    mesh.vertexOffset = static_cast<uint32>(combinedVertices.size());
                    combinedVertices.insert(combinedVertices.end(), mesh.vertices.begin(), mesh.vertices.end());

                    if (!mesh.indices.empty())
                    {
                        mesh.indexOffset = static_cast<uint32>(combinedIndices.size() / (mesh.indexType == IndexType::Uint16 ? sizeof(uint16) : sizeof(uint32)));
                        combinedIndices.insert(combinedIndices.end(), mesh.indices.begin(), mesh.indices.end());
                    }
                }
            }

            ResourceDesc vbDesc = ResourceDesc{
                .size          = sizeof(Vertex) * combinedVertices.size(),
                .typeHintFlags = TH_VertexBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "VertexBuffer",
            };

            ResourceDesc ibDesc = ResourceDesc{
                .size          = combinedIndices.size(),
                .typeHintFlags = TH_IndexBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "IndexBuffer",
            };

            _mergedVertexBufferStaging = _lgx->CreateResource(vbDesc);
            _mergedIndexBufferStaging  = _lgx->CreateResource(ibDesc);

            uint8* vbData = nullptr;
            uint8* ibData = nullptr;
            _lgx->MapResource(_mergedVertexBufferStaging, vbData);
            _lgx->MapResource(_mergedIndexBufferStaging, ibData);
            std::memcpy(vbData, combinedVertices.data(), sizeof(Vertex) * combinedVertices.size());
            std::memcpy(ibData, combinedIndices.data(), combinedIndices.size());

            vbDesc.heapType = ResourceHeap::GPUOnly;
            ibDesc.heapType = ResourceHeap::GPUOnly;

            _mergedVertexBufferGPU = _lgx->CreateResource(vbDesc);
            _mergedIndexBufferGPU  = _lgx->CreateResource(ibDesc);
        }

        //******************* TRANSFER
        {

            for (auto& obj : _objects)
            {
                obj.texturesGPU.resize(obj.model.allTextures.size());
                const uint32 sz = static_cast<uint32>(obj.model.allTextures.size());

                // Transition all to transfer destination.
                {
                    LinaGX::CMDBarrier* textureBarrier   = _pfd[0].copyStream->AddCommand<LinaGX::CMDBarrier>();
                    textureBarrier->resourceBarrierCount = 0;
                    textureBarrier->textureBarrierCount  = sz;
                    textureBarrier->textureBarriers      = _pfd[0].copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * sz);
                    textureBarrier->srcStageFlags        = LinaGX::PSF_TopOfPipe;
                    textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;

                    uint32 index = 0;
                    for (uint32 i = 0; i < sz; i++)
                    {
                        auto& barrier          = textureBarrier->textureBarriers[index];
                        barrier.texture        = obj.texturesGPU[i];
                        barrier.toState        = LinaGX::TextureBarrierState::TransferDestination;
                        barrier.isSwapchain    = false;
                        barrier.srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
                        barrier.dstAccessFlags = LinaGX::AF_TransferWrite;
                        index++;
                    }
                }

                for (uint32 i = 0; i < sz; i++)
                {
                    auto* txt = obj.model.allTextures[i];

                    TextureBuffer txtBuffer = {
                        .pixels        = txt->buffer.pixels,
                        .width         = txt->buffer.width,
                        .height        = txt->buffer.height,
                        .bytesPerPixel = txt->buffer.bytesPerPixel,
                    };

                    // Copy texture
                    CMDCopyBufferToTexture2D* copyTxt = _pfd[0].copyStream->AddCommand<CMDCopyBufferToTexture2D>();
                    copyTxt->destTexture              = obj.texturesGPU[i];
                    copyTxt->mipLevels                = 1;
                    copyTxt->buffers                  = _pfd[0].copyStream->EmplaceAuxMemory<TextureBuffer>(txtBuffer);
                    copyTxt->destinationSlice         = 0;
                }

                // Transition all to shader read
                {
                    LinaGX::CMDBarrier* textureBarrier   = _pfd[0].copyStream->AddCommand<LinaGX::CMDBarrier>();
                    textureBarrier->resourceBarrierCount = 0;
                    textureBarrier->textureBarrierCount  = sz;
                    textureBarrier->textureBarriers      = _pfd[0].copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * sz);
                    textureBarrier->srcStageFlags        = LinaGX::PSF_Transfer;
                    textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;

                    uint32 index = 0;
                    for (uint32 i = 0; i < sz; i++)
                    {
                        auto& barrier          = textureBarrier->textureBarriers[index];
                        barrier.texture        = obj.texturesGPU[i];
                        barrier.toState        = LinaGX::TextureBarrierState::ShaderRead;
                        barrier.srcAccessFlags = LinaGX::AF_TransferWrite;
                        barrier.dstAccessFlags = LinaGX::AF_TransferRead;
                        barrier.isSwapchain    = false;
                        index++;
                    }
                }
            }

            CMDCopyResource* copyVtxBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
            copyVtxBuf->source          = _mergedVertexBufferStaging;
            copyVtxBuf->destination     = _mergedVertexBufferGPU;

            CMDCopyResource* copyIndexBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
            copyIndexBuf->source          = _mergedIndexBufferStaging;
            copyIndexBuf->destination     = _mergedIndexBufferGPU;

            _lgx->CloseCommandStreams(&_pfd[0].copyStream, 1);

            // Execute copy command on the transfer queue, signal a semaphore when it's done and wait for it on the CPU side.
            _pfd[0].copySemaphoreValue++;
            _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &_pfd[0].copyStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &_pfd[0].copySemaphore, .signalValues = &_pfd[0].copySemaphoreValue});
            _lgx->WaitForUserSemaphore(_pfd[0].copySemaphore, _pfd[0].copySemaphoreValue);

            // Not needed anymore.
            _lgx->DestroyResource(_mergedVertexBufferStaging);
            _lgx->DestroyResource(_mergedIndexBufferStaging);
        }

        //*******************  SSBO
        {
            const uint32 objectCount = static_cast<uint32>(_objects.size());

            ResourceDesc descSSBO = {
                .size          = sizeof(GPUObjectData) * objectCount,
                .typeHintFlags = ResourceTypeHint::TH_StorageBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "SSBO",
            };

            ResourceDesc descSSBOMaterials = {
                .size          = sizeof(GPUMaterialData) * 2,
                .typeHintFlags = ResourceTypeHint::TH_StorageBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "SSBO",
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                descSSBO.heapType            = ResourceHeap::StagingHeap;
                descSSBOMaterials.heapType   = ResourceHeap::StagingHeap;
                _pfd[i].ssboStaging          = _lgx->CreateResource(descSSBO);
                _pfd[i].ssboMaterialsStaging = _lgx->CreateResource(descSSBOMaterials);
                _lgx->MapResource(_pfd[i].ssboStaging, _pfd[i].ssboMapping);
                _lgx->MapResource(_pfd[i].ssboMaterialsStaging, _pfd[i].ssboMaterialsMapping);
                descSSBO.heapType          = ResourceHeap::GPUOnly;
                descSSBOMaterials.heapType = ResourceHeap::GPUOnly;
                _pfd[i].ssboGPU            = _lgx->CreateResource(descSSBO);
                _pfd[i].ssboMaterialsGPU   = _lgx->CreateResource(descSSBOMaterials);
            }
        }

        //******************* INDIRECT BUFFER
        {
            ResourceDesc desc = {
                .size          = sizeof(IndexedIndirectCommand) * 10,
                .typeHintFlags = ResourceTypeHint::TH_IndirectBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "IndirectBuffer"};

            ResourceDesc descArgs = {
                .size          = sizeof(ConstantsData) * 10,
                .typeHintFlags = ResourceTypeHint::TH_StorageBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "IndirectBufferArgs"};

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                desc.heapType                 = ResourceHeap::StagingHeap;
                desc.isGPUWritable            = false;
                descArgs.heapType             = ResourceHeap::StagingHeap;
                _pfd[i].indirectBufferStaging = _lgx->CreateResource(desc);
                _pfd[i].indirectArgsStaging   = _lgx->CreateResource(descArgs);
                _lgx->MapResource(_pfd[i].indirectBufferStaging, _pfd[i].indirectBufferMapping);
                _lgx->MapResource(_pfd[i].indirectArgsStaging, _pfd[i].indirectArgsMapping);

                desc.heapType             = ResourceHeap::GPUOnly;
                desc.isGPUWritable        = true;
                descArgs.heapType         = ResourceHeap::GPUOnly;
                _pfd[i].indirectBufferGPU = _lgx->CreateResource(desc);
                _pfd[i].indirectArgsGPU   = _lgx->CreateResource(descArgs);
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

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ubo0 = _lgx->CreateResource(desc);
                _lgx->MapResource(_pfd[i].ubo0, _pfd[i].uboMapping0);
            }
        }

        //*******************  DESCRIPTOR SET
        {
            DescriptorBinding set0Binding = {
                .descriptorCount = 1,
                .type            = DescriptorType::UBO,
                .stages          = {ShaderStage::Vertex, ShaderStage::Fragment},
            };

            DescriptorSetDesc set0Desc = {
                .bindings = {set0Binding},
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].descriptorSetSceneData0 = _lgx->CreateDescriptorSet(set0Desc);

                DescriptorUpdateBufferDesc uboUpdate = {
                    .setHandle = _pfd[i].descriptorSetSceneData0,
                    .binding   = 0,
                    .buffers   = {_pfd[i].ubo0},
                };

                _lgx->DescriptorUpdateBuffer(uboUpdate);
            }

            DescriptorBinding set1Binding0 = {
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex, ShaderStage::Fragment},
            };

            DescriptorBinding set1Binding1 = {
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex, ShaderStage::Fragment},
            };

            DescriptorSetDesc set1Desc = {
                .bindings = {set1Binding0, set1Binding1},
            };

            DescriptorBinding computeBinding = {
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .isWritable      = true,
                .stages          = {ShaderStage::Compute},
            };

            DescriptorSetDesc computeDesc = {
                .bindings = {computeBinding},
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ssboSet = _lgx->CreateDescriptorSet(set1Desc);

                DescriptorUpdateBufferDesc bufferDesc = {
                    .setHandle = _pfd[i].ssboSet,
                    .binding   = 0,
                    .buffers   = {_pfd[i].ssboGPU},
                };

                DescriptorUpdateBufferDesc bufferDesc2 = {
                    .setHandle = _pfd[i].ssboSet,
                    .binding   = 1,
                    .buffers   = {_pfd[i].indirectArgsGPU},
                };

                _lgx->DescriptorUpdateBuffer(bufferDesc);
                _lgx->DescriptorUpdateBuffer(bufferDesc2);

                _pfd[i].computeSet = _lgx->CreateDescriptorSet(computeDesc);

                DescriptorUpdateBufferDesc computeBufferDesc = {
                    .setHandle     = _pfd[i].computeSet,
                    .binding       = 0,
                    .buffers       = {_pfd[i].indirectBufferGPU},
                    .isWriteAccess = true,
                };

                _lgx->DescriptorUpdateBuffer(computeBufferDesc);
            }

            DescriptorBinding set2Binding0 = {
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Fragment},
            };

            // 10 as max number
            DescriptorBinding set2Binding1 = {
                .descriptorCount = 10,
                .type            = DescriptorType::CombinedImageSampler,
                .unbounded       = true,
                .stages          = {ShaderStage::Fragment},
            };

            DescriptorSetDesc set2Desc = {
                .bindings = {set2Binding0, set2Binding1},
            };

            LINAGX_VEC<uint32> textures;

            for (const auto& obj : _objects)
            {
                for (const auto txt : obj.texturesGPU)
                    textures.push_back(txt);
            }

            LINAGX_VEC<uint32> samplers;
            samplers.resize(textures.size());

            for (size_t i = 0; i < samplers.size(); i++)
                samplers[i] = _sampler;

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ssboMaterialsSet = _lgx->CreateDescriptorSet(set2Desc);

                DescriptorUpdateBufferDesc bufferDesc = {
                    .setHandle = _pfd[i].ssboMaterialsSet,
                    .binding   = 0,
                    .buffers   = {_pfd[i].ssboMaterialsGPU},
                };

                _lgx->DescriptorUpdateBuffer(bufferDesc);

                DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle = _pfd[i].ssboMaterialsSet,
                    .binding   = 1,
                    .textures  = textures,
                    .samplers  = samplers,
                };

                _lgx->DescriptorUpdateImage(imgUpdate);
            }
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

        _lgx->DestroyTexture(_depthTexture);
        CreateDepthTexture();
    }

    void Example::CreateDepthTexture()
    {
        LinaGX::TextureDesc depthDesc = {
            .format      = Format::D32_SFLOAT,
            .flags       = LinaGX::TextureFlags::TF_DepthTexture | LinaGX::TextureFlags::TF_Sampled,
            .width       = _windowX,
            .height      = _windowY,
            .mipLevels   = 1,
            .arrayLength = 1,
        };

        _depthTexture = _lgx->CreateTexture(depthDesc);
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _lgx->Join();

        // Get rid of resources
        for (auto& obj : _objects)
        {
            for (auto& txt : obj.texturesGPU)
                _lgx->DestroyTexture(txt);
        }

        _lgx->DestroyTexture(_depthTexture);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            _lgx->DestroyResource(_pfd[i].ssboStaging);
            _lgx->DestroyResource(_pfd[i].ssboGPU);
            _lgx->DestroyDescriptorSet(_pfd[i].ssboSet);
            _lgx->DestroyDescriptorSet(_pfd[i].computeSet);
            _lgx->DestroyResource(_pfd[i].ssboMaterialsStaging);
            _lgx->DestroyResource(_pfd[i].ssboMaterialsGPU);
            _lgx->DestroyResource(_pfd[i].indirectBufferStaging);
            _lgx->DestroyResource(_pfd[i].indirectBufferGPU);
            _lgx->DestroyResource(_pfd[i].indirectArgsStaging);
            _lgx->DestroyResource(_pfd[i].indirectArgsGPU);
            _lgx->DestroyDescriptorSet(_pfd[i].ssboMaterialsSet);
            _lgx->DestroyUserSemaphore(_pfd[i].copySemaphore);
            _lgx->DestroyUserSemaphore(_pfd[i].computeSemaphore);
            _lgx->DestroyCommandStream(_pfd[i].stream);
            _lgx->DestroyCommandStream(_pfd[i].copyStream);
            _lgx->DestroyCommandStream(_pfd[i].computeStream);
            _lgx->DestroyResource(_pfd[i].ubo0);
            _lgx->DestroyDescriptorSet(_pfd[i].descriptorSetSceneData0);
        }

        _lgx->DestroyResource(_mergedVertexBufferGPU);
        _lgx->DestroyResource(_mergedIndexBufferGPU);
        _lgx->DestroySampler(_sampler);
        _lgx->DestroySwapchain(_swapchain);
        _lgx->DestroyShader(_shaderProgram);
        _lgx->DestroyShader(_shaderProgramCompute);

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

        LINAGX_VEC<ConstantsData>          indirectArguments;

        uint32 cmdCount = 0;
        // Copy indirect buffer.
        {
            for (const auto& obj : _objects)
            {
                for (const auto& mesh : obj.meshes)
                {
                    uint32                 ic  = static_cast<uint32>(mesh.indices.size()) / (mesh.indexType == IndexType::Uint16 ? sizeof(uint16) : sizeof(uint32));
                
                    _lgx->BufferIndexedIndirectCommandData(currentFrame.indirectBufferMapping, _lgx->GetIndexedIndirectCommandSize() * cmdCount, cmdCount, ic, 1, mesh.indexOffset, mesh.vertexOffset, 0);
                    cmdCount++;
                    
                    ConstantsData data = {.index = obj.constants.index, .materialByteIndex = obj.constants.materialByteIndex};
                    indirectArguments.push_back(data);
                }
            }
            std::memcpy(currentFrame.indirectArgsMapping, indirectArguments.data(), sizeof(ConstantsData) * indirectArguments.size());

            CMDCopyResource* copyRes = currentFrame.copyStream->AddCommand<CMDCopyResource>();
            copyRes->source          = currentFrame.indirectBufferStaging;
            copyRes->destination     = currentFrame.indirectBufferGPU;

            CMDCopyResource* copyRes2 = currentFrame.copyStream->AddCommand<CMDCopyResource>();
            copyRes2->source          = currentFrame.indirectArgsStaging;
            copyRes2->destination     = currentFrame.indirectArgsGPU;
        }

        // Copy SSBO data on copy queue
        {
            LINAGX_VEC<GPUObjectData> objectData;
            objectData.resize(_objects.size());

            for (size_t i = 0; i < _objects.size(); i++)
            {
                const auto& obj           = _objects[i];
                objectData[i].modelMatrix = obj.modelMatrix;
            }

            auto sz = sizeof(GPUObjectData) * _objects.size();
            std::memcpy(currentFrame.ssboMapping, objectData.data(), sz);

            CMDCopyResource* copyRes = currentFrame.copyStream->AddCommand<CMDCopyResource>();
            copyRes->source          = currentFrame.ssboStaging;
            copyRes->destination     = currentFrame.ssboGPU;

            _material1.baseColor     = glm::vec4(1, 1, 1, 1);
            _material1.baseColorTint = glm::vec4(1, 1, 1, 1);
            _material1.albedoIndex   = 0;
            _material2.baseColor     = glm::vec4(1, 1, 1, 1);
            _material2.baseColorTint = glm::vec4(1, 1, 1, 1);
            _material2.albedoIndex   = 3;

            std::memcpy(currentFrame.ssboMaterialsMapping, &_material1, sizeof(GPUMaterialData));
            std::memcpy(currentFrame.ssboMaterialsMapping + sizeof(GPUMaterialData), &_material2, sizeof(GPUMaterialData));
            CMDCopyResource* copyRes2 = currentFrame.copyStream->AddCommand<CMDCopyResource>();
            copyRes2->source          = currentFrame.ssboMaterialsStaging;
            copyRes2->destination     = currentFrame.ssboMaterialsGPU;

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
            const glm::mat4 eye        = glm::lookAt(glm::vec3(0.0f, 50.0f, 350.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            const glm::mat4 projection = glm::perspective(DEG2RAD(90.0f), static_cast<float>(_window->GetSize().x) / static_cast<float>(_window->GetSize().y), 0.1f, 1200.0f);
            GPUSceneData    sceneData  = {};
            sceneData.viewProj         = projection * eye;
            std::memcpy(currentFrame.uboMapping0, &sceneData, sizeof(GPUSceneData));
        }

        // Bind for compute
        {
            CMDBindPipeline* bindPipeline = currentFrame.computeStream->AddCommand<CMDBindPipeline>();
            bindPipeline->shader          = _shaderProgramCompute;
        }

        // Bind the descriptors for compute
        {
            CMDBindDescriptorSets* bindSets = currentFrame.computeStream->AddCommand<CMDBindDescriptorSets>();
            bindSets->firstSet              = 0;
            bindSets->setCount              = 1;
            bindSets->descriptorSetHandles  = currentFrame.computeStream->EmplaceAuxMemory<uint16>(currentFrame.computeSet);
            bindSets->isCompute             = true;
        }

        // Dispatch compute work.
        {
            CMDDispatch* dispatch = currentFrame.computeStream->AddCommand<CMDDispatch>();
            dispatch->groupSizeX  = (cmdCount / 226) + 1;
            dispatch->groupSizeY  = 1;
            dispatch->groupSizeZ  = 1;
        }

        // Send compute work.
        {
            _lgx->CloseCommandStreams(&currentFrame.computeStream, 1);
            currentFrame.computeSemaphoreValue++;
            _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Compute), .streams = &currentFrame.computeStream, .streamCount = 1, .useWait = true, .waitCount = 1, .waitSemaphores = &currentFrame.copySemaphore, .waitValues = &currentFrame.copySemaphoreValue, .useSignal = true, .signalCount = 1, .signalSemaphores = &currentFrame.computeSemaphore, .signalValues = &currentFrame.computeSemaphoreValue});
        }

        // No need for a compute barrier as using queue sync.
        // {
        //     LinaGX::CMDBarrier* barrier               = currentFrame.stream->AddCommand<LinaGX::CMDBarrier>();
        //     barrier->srcStageFlags                    = LinaGX::PSF_Compute;
        //     barrier->dstStageFlags                    = LinaGX::PSF_DrawIndirect;
        //     barrier->memoryBarrierCount               = 1;
        //     barrier->memoryBarriers                   = currentFrame.stream->EmplaceAuxMemorySizeOnly<LinaGX::MemBarrier>(sizeof(LinaGX::MemBarrier));
        //     barrier->memoryBarriers[0].srcAccessFlags = LinaGX::AF_ShaderWrite;
        //     barrier->memoryBarriers[0].dstAccessFlags = LinaGX::AF_IndirectCommandRead;
        // }

        // Swapchain to Color Attachment, depth texture to Depth Attachment
        {
            LinaGX::CMDBarrier* barrier  = currentFrame.stream->AddCommand<LinaGX::CMDBarrier>();
            barrier->srcStageFlags       = LinaGX::PSF_TopOfPipe;
            barrier->dstStageFlags       = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
            barrier->textureBarrierCount = 2;
            barrier->textureBarriers     = currentFrame.stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);

            // Swp to color
            barrier->textureBarriers[0].srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
            barrier->textureBarriers[0].dstAccessFlags = LinaGX::AF_ColorAttachmentRead;
            barrier->textureBarriers[0].isSwapchain    = true;
            barrier->textureBarriers[0].texture        = static_cast<uint32>(_swapchain);
            barrier->textureBarriers[0].toState        = LinaGX::TextureBarrierState::ColorAttachment;

            barrier->textureBarriers[1].srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
            barrier->textureBarriers[1].dstAccessFlags = LinaGX::AF_DepthStencilAttachmentRead;
            barrier->textureBarriers[1].isSwapchain    = false;
            barrier->textureBarriers[1].texture        = _depthTexture;
            barrier->textureBarriers[1].toState        = LinaGX::TextureBarrierState::DepthStencilAttachment;
        }

        // Render pass 1.
        {
            Viewport     viewport = {.x = 0, .y = 0, .width = _windowX, .height = _windowY, .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect sc       = {.x = 0, .y = 0, .width = _windowX, .height = _windowY};

            CMDBeginRenderPass* beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();

            beginRenderPass->viewport = viewport;
            beginRenderPass->scissors = sc;

            RenderPassColorAttachment colorAttachment;
            colorAttachment.clearColor                           = {32.0f / 255.0f, 17.0f / 255.0f, 39.0f / 255.0f, 1.0f};
            colorAttachment.texture                              = static_cast<uint32>(_swapchain);
            colorAttachment.isSwapchain                          = true;
            colorAttachment.loadOp                               = LoadOp::Clear;
            colorAttachment.storeOp                              = StoreOp::Store;
            beginRenderPass->colorAttachments                    = currentFrame.stream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->colorAttachmentCount                = 1;
            beginRenderPass->depthStencilAttachment.useDepth     = true;
            beginRenderPass->depthStencilAttachment.depthLoadOp  = LoadOp::Clear;
            beginRenderPass->depthStencilAttachment.depthStoreOp = StoreOp::Store;
            beginRenderPass->depthStencilAttachment.clearDepth   = 1.0f;
            beginRenderPass->depthStencilAttachment.texture      = _depthTexture;
            beginRenderPass->depthStencilAttachment.useStencil   = false;
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
            bindSets->setCount              = 3;
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(currentFrame.descriptorSetSceneData0, currentFrame.ssboSet, currentFrame.ssboMaterialsSet);
            bindSets->isCompute             = false;
        }

        // Bind vertex index buffers.
        {
            CMDBindVertexBuffers* vtx = currentFrame.stream->AddCommand<CMDBindVertexBuffers>();
            vtx->slot                 = 0;
            vtx->resource             = _mergedVertexBufferGPU;
            vtx->vertexSize           = sizeof(Vertex);
            vtx->offset               = 0;

            CMDBindIndexBuffers* indx = currentFrame.stream->AddCommand<CMDBindIndexBuffers>();
            indx->indexType           = IndexType::Uint16;
            indx->offset              = 0;
            indx->resource            = _mergedIndexBufferGPU;
        }

        // Indirect draw call.
        {
            CMDDrawIndexedIndirect* indirect = currentFrame.stream->AddCommand<CMDDrawIndexedIndirect>();
            indirect->count                  = cmdCount;
            indirect->indirectBuffer         = currentFrame.indirectBufferGPU;
            indirect->indirectBufferOffset   = 0;
        }

        // End render pass
        {
            CMDEndRenderPass* end = currentFrame.stream->AddCommand<CMDEndRenderPass>();
        }

        // Swapchain to Present, depth to Shader Read.
        {
            LinaGX::CMDBarrier* barrier  = currentFrame.stream->AddCommand<LinaGX::CMDBarrier>();
            barrier->srcStageFlags       = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
            barrier->dstStageFlags       = LinaGX::PSF_FragmentShader;
            barrier->textureBarrierCount = 2;
            barrier->textureBarriers     = currentFrame.stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);

            barrier->textureBarriers[0].srcAccessFlags = LinaGX::AF_ColorAttachmentWrite;
            barrier->textureBarriers[0].dstAccessFlags = 0;
            barrier->textureBarriers[0].isSwapchain    = true;
            barrier->textureBarriers[0].texture        = static_cast<uint32>(_swapchain);
            barrier->textureBarriers[0].toState        = LinaGX::TextureBarrierState::Present;

            barrier->textureBarriers[1].srcAccessFlags = AF_DepthStencilAttachmentRead;
            barrier->textureBarriers[1].dstAccessFlags = LinaGX::AF_ShaderRead;
            barrier->textureBarriers[1].isSwapchain    = false;
            barrier->textureBarriers[1].texture        = _depthTexture;
            barrier->textureBarriers[1].toState        = LinaGX::TextureBarrierState::ShaderRead;
        }

        // This does the actual *recording* of every single command stream alive.
        _lgx->CloseCommandStreams(&currentFrame.stream, 1);

        // Submit work on gpu.
        _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &currentFrame.stream, .streamCount = 1, .useWait = true, .waitCount = 1, .waitSemaphores = &currentFrame.computeSemaphore, .waitValues = &currentFrame.computeSemaphoreValue});

        // Present main swapchain.
        _lgx->Present({.swapchains = &_swapchain, .swapchainCount = 1});

        // Let LinaGX know we are finalizing this frame.
        _lgx->EndFrame();
    }

} // namespace LinaGX::Examples
