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

#include "IndirectRendering.hpp"
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

    LinaGX::Instance* _lgx  = nullptr;
    uint8             _swapchain = 0;
    Window*           _window    = nullptr;

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
        std::vector<Vertex>        vertices;
        std::vector<unsigned char> indices;
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
        std::vector<Mesh>   meshes;
        std::vector<uint32> texturesGPU;
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
    uint16 _shaderProgram = 0;

    // Objects
    std::vector<Object> _objects;

    struct DefaultTexture
    {
        TextureLoadData loadData;
        uint32          gpuResource = 0;
        const char*     path        = "";
    };

    // Resources
    uint32             _sampler = 0;
    std::vector<uint16> _allMaterialDescriptors;
    uint16             _descriptorSetMaterial;
    GPUMaterialData    _material1;
    GPUMaterialData    _material2;

    uint32 _mergedVertexBufferStaging = 0;
    uint32 _mergedVertexBufferGPU     = 0;
    uint32 _mergedIndexBufferStaging  = 0;
    uint32 _mergedIndexBufferGPU      = 0;

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
        uint16                 copySemaphore           = 0;
        uint64                 copySemaphoreValue      = 0;
        uint16                 descriptorSetSceneData0 = 0;
        uint32                 ubo0                    = 0;
        uint8*                 uboMapping0             = 0;
        uint32                 indirectBufferGPU       = 0;
        uint32                 indirectBufferStaging   = 0;
        uint8*                 indirectBufferMapping   = nullptr;
        uint32                 indirectArgsGPU         = 0;
        uint32                 indirectArgsStaging     = 0;
        uint8*                 indirectArgsMapping     = nullptr;
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
            LinaGX::Config.logLevel      = LogLevel::Verbose;
            LinaGX::Config.errorCallback = LogError;
            LinaGX::Config.infoCallback  = LogInfo;

            BackendAPI api = BackendAPI::Vulkan;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif
            LinaGX::GPULimits   limits;
            LinaGX::GPUFeatures features;
            features.enableBindless = true;

            LinaGX::InitInfo initInfo = InitInfo{
                .api                   = api,
                .gpu                   = PreferredGPUType::Integrated,
                .framesInFlight        = FRAMES_IN_FLIGHT,
                .backbufferCount       = 2,
                .gpuLimits             = limits,
                .gpuFeatures           = features,
                .checkForFormatSupport = {Format::B8G8R8A8_UNORM, Format::D32_SFLOAT},
            };

            _lgx = new LinaGX::Instance();
            _lgx->Initialize(initInfo);
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Sponza", 0, 0, 800, 800, WindowStyle::Windowed);
            _window->SetCallbackClose([this]() { m_isRunning = false; });
        }

        //******************* DEFAULT SHADER CREATION
        {
            // Compile shaders.
            const std::string                 vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/vert.glsl");
            const std::string                 fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/frag.glsl");
            ShaderLayout                      outLayout  = {};
            ShaderCompileData                 dataVertex = {vtxShader.c_str(), "Resources/Shaders/Include"};
            ShaderCompileData                 dataFrag   = {fragShader.c_str(), "Resources/Shaders/Include"};
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
                .componentFlags      = ColorComponentFlags::RGBA,
            };

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
                .blendAttachment       = blend,
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
                _lgx->RecreateSwapchain(resizeDesc);
            });

            // Create command stream to record draw calls.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].stream        = _lgx->CreateCommandStream(100, QueueType::Graphics);
                _pfd[i].copyStream    = _lgx->CreateCommandStream(100, QueueType::Transfer);
                _pfd[i].copySemaphore = _lgx->CreateUserSemaphore();
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

            glm::mat4 mat = glm::mat4(1.0f);
            glm::quat q   = glm::quat(glm::vec3(0.0f, DEG2RAD(180.0f), 0.0f));
            mat           = TranslateRotateScale({-150.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {3000, 3000, 3000});

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
                obj.texturesGPU.resize(obj.model.allTexturesCount);

                for (uint32 i = 0; i < obj.model.allTexturesCount; i++)
                {
                    auto* txt = obj.model.allTextures + i;

                    Texture2DDesc desc = {
                        .usage     = Texture2DUsage::ColorTexture,
                        .width     = txt->buffer.width,
                        .height    = txt->buffer.height,
                        .mipLevels = 1,
                        .format    = Format::R8G8B8A8_SRGB,
                        .debugName = "Material Texture",
                    };

                    obj.texturesGPU[i] = _lgx->CreateTexture2D(desc);
                }

                for (uint32 i = 0; i < obj.model.allNodesCount; i++)
                {
                    ModelNode* node = obj.model.allNodes + i;

                    if (node->mesh != nullptr)
                    {
                        for (uint32 j = 0; j < node->mesh->primitiveCount; j++)
                        {
                            obj.meshes.push_back({});
                            auto& mesh = obj.meshes[obj.meshes.size() - 1];

                            ModelMeshPrimitive* prim = node->mesh->primitives + j;
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

                                if (!prim->joints.empty())
                                {
                                    vtx.inBoneIndices.x = static_cast<float>(prim->joints[k].x);
                                    vtx.inBoneIndices.y = static_cast<float>(prim->joints[k].y);
                                    vtx.inBoneIndices.z = static_cast<float>(prim->joints[k].z);
                                    vtx.inBoneIndices.w = static_cast<float>(prim->joints[k].w);
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

            std::vector<Vertex>        combinedVertices;
            std::vector<unsigned char> combinedIndices;

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
                obj.texturesGPU.resize(obj.model.allTexturesCount);

                for (uint32 i = 0; i < obj.model.allTexturesCount; i++)
                {
                    auto* txt = obj.model.allTextures + i;

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
            _lgx->SubmitCommandStreams({.queue = QueueType::Transfer, .streams = &_pfd[0].copyStream, .streamCount = 1, .useSignal = true, .signalSemaphore = _pfd[0].copySemaphore, .signalValue = ++_pfd[0].copySemaphoreValue});
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
                descArgs.heapType             = ResourceHeap::StagingHeap;
                _pfd[i].indirectBufferStaging = _lgx->CreateResource(desc);
                _pfd[i].indirectArgsStaging   = _lgx->CreateResource(descArgs);
                _lgx->MapResource(_pfd[i].indirectBufferStaging, _pfd[i].indirectBufferMapping);
                _lgx->MapResource(_pfd[i].indirectArgsStaging, _pfd[i].indirectArgsMapping);

                desc.heapType             = ResourceHeap::GPUOnly;
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
            DescriptorBinding set0Bindings[1];

            set0Bindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::UBO,
                .stages          = {ShaderStage::Vertex},
            };

            DescriptorSetDesc set0Desc = {
                .bindings      = set0Bindings,
                .bindingsCount = 1,
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].descriptorSetSceneData0 = _lgx->CreateDescriptorSet(set0Desc);

                DescriptorUpdateBufferDesc uboUpdate = {
                    .setHandle       = _pfd[i].descriptorSetSceneData0,
                    .binding         = 0,
                    .descriptorCount = 1,
                    .resources       = &_pfd[i].ubo0,
                    .descriptorType  = DescriptorType::UBO,
                };

                _lgx->DescriptorUpdateBuffer(uboUpdate);
            }

            DescriptorBinding set1Bindings[2];

            set1Bindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex},
            };

            set1Bindings[1] = {
                .binding         = 1,
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex, ShaderStage::Fragment},
            };

            DescriptorSetDesc set1Desc = {
                .bindings      = set1Bindings,
                .bindingsCount = 2,
            };

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ssboSet = _lgx->CreateDescriptorSet(set1Desc);

                DescriptorUpdateBufferDesc bufferDesc = {
                    .setHandle       = _pfd[i].ssboSet,
                    .binding         = 0,
                    .descriptorCount = 1,
                    .resources       = &_pfd[i].ssboGPU,
                    .descriptorType  = DescriptorType::SSBO,
                };

                DescriptorUpdateBufferDesc bufferDesc2 = {
                    .setHandle       = _pfd[i].ssboSet,
                    .binding         = 1,
                    .descriptorCount = 1,
                    .resources       = &_pfd[i].indirectArgsGPU,
                    .descriptorType  = DescriptorType::SSBO,
                };

                _lgx->DescriptorUpdateBuffer(bufferDesc);
                _lgx->DescriptorUpdateBuffer(bufferDesc2);
            }

            DescriptorBinding set2Bindings[2];
            set2Bindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Fragment},
            };

            // 10 as max number
            set2Bindings[1] = {
                .binding         = 1,
                .descriptorCount = 10,
                .type            = DescriptorType::CombinedImageSampler,
                .stages          = {ShaderStage::Fragment},
                .bindless        = true,
            };

            DescriptorSetDesc set2Desc = {
                .bindings      = set2Bindings,
                .bindingsCount = 2,
            };

            std::vector<uint32> textures;

            for (const auto& obj : _objects)
            {
                for (const auto txt : obj.texturesGPU)
                    textures.push_back(txt);
            }

            std::vector<uint32> samplers;
            samplers.resize(textures.size());

            for (size_t i = 0; i < samplers.size(); i++)
                samplers[i] = _sampler;

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ssboMaterialsSet = _lgx->CreateDescriptorSet(set2Desc);

                DescriptorUpdateBufferDesc bufferDesc = {
                    .setHandle       = _pfd[i].ssboMaterialsSet,
                    .binding         = 0,
                    .descriptorCount = 1,
                    .resources       = &_pfd[i].ssboMaterialsGPU,
                    .descriptorType  = DescriptorType::SSBO,
                };

                _lgx->DescriptorUpdateBuffer(bufferDesc);

                DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle       = _pfd[i].ssboMaterialsSet,
                    .binding         = 1,
                    .descriptorCount = static_cast<uint32>(textures.size()),
                    .textures        = textures.data(),
                    .samplers        = samplers.data(),
                    .descriptorType  = DescriptorType::CombinedImageSampler,
                };

                _lgx->DescriptorUpdateImage(imgUpdate);
            }
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
        {
            for (auto& txt : obj.texturesGPU)
                _lgx->DestroyTexture2D(txt);
        }

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            _lgx->DestroyResource(_pfd[i].ssboStaging);
            _lgx->DestroyResource(_pfd[i].ssboGPU);
            _lgx->DestroyDescriptorSet(_pfd[i].ssboSet);
            _lgx->DestroyResource(_pfd[i].ssboMaterialsStaging);
            _lgx->DestroyResource(_pfd[i].ssboMaterialsGPU);
            _lgx->DestroyResource(_pfd[i].indirectBufferStaging);
            _lgx->DestroyResource(_pfd[i].indirectBufferGPU);
            _lgx->DestroyResource(_pfd[i].indirectArgsStaging);
            _lgx->DestroyResource(_pfd[i].indirectArgsGPU);
            _lgx->DestroyDescriptorSet(_pfd[i].ssboMaterialsSet);
            _lgx->DestroyUserSemaphore(_pfd[i].copySemaphore);
            _lgx->DestroyCommandStream(_pfd[i].stream);
            _lgx->DestroyCommandStream(_pfd[i].copyStream);
            _lgx->DestroyResource(_pfd[i].ubo0);
            _lgx->DestroyDescriptorSet(_pfd[i].descriptorSetSceneData0);
        }
        _lgx->DestroyResource(_mergedVertexBufferGPU);
        _lgx->DestroyResource(_mergedIndexBufferGPU);
        _lgx->DestroySampler(_sampler);
        _lgx->DestroySwapchain(_swapchain);
        _lgx->DestroyShader(_shaderProgram);

        // Terminate renderer & shutdown app.
        delete _lgx;
        App::Shutdown();
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        _lgx->PollWindow();

        // Let LinaGX know we are starting a new frame.
        _lgx->StartFrame();

        auto& currentFrame = _pfd[_lgx->GetCurrentFrameIndex()];

        std::vector<IndexedIndirectCommand> indirectCommands;
        std::vector<ConstantsData>          indirectArguments;

        // Copy indirect buffer.
        {
            for (const auto& obj : _objects)
            {
                for (const auto& mesh : obj.meshes)
                {
                    uint32                 ic  = static_cast<uint32>(mesh.indices.size()) / (mesh.indexType == IndexType::Uint16 ? sizeof(uint16) : sizeof(uint32));
                    IndexedIndirectCommand cmd = IndexedIndirectCommand{
                        .LGX_DrawID            = static_cast<uint32>(indirectCommands.size()),
                        .indexCountPerInstance = ic,
                        .instanceCount         = 1,
                        .startIndexLocation    = mesh.indexOffset,
                        .baseVertexLocation    = mesh.vertexOffset,
                        .startInstanceLocation = 0,
                    };

                    indirectCommands.push_back(cmd);

                    ConstantsData data = {.index = obj.constants.index, .materialByteIndex = obj.constants.materialByteIndex};
                    indirectArguments.push_back(data);
                }
            }

            std::memcpy(currentFrame.indirectBufferMapping, indirectCommands.data(), sizeof(IndexedIndirectCommand) * indirectCommands.size());
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
            std::vector<GPUObjectData> objectData;
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

            SubmitDesc submit = {
                .queue           = QueueType::Transfer,
                .streams         = &currentFrame.copyStream,
                .streamCount     = 1,
                .useWait         = false,
                .useSignal       = true,
                .signalSemaphore = currentFrame.copySemaphore,
                .signalValue     = ++currentFrame.copySemaphoreValue,
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

        Viewport     viewport = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y, .minDepth = 0.0f, .maxDepth = 1.0f};
        ScissorsRect sc       = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y};

        // Render pass 1.
        {
            CMDBeginRenderPass* beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();
            beginRenderPass->isSwapchain        = true;
            beginRenderPass->swapchain          = _swapchain;
            beginRenderPass->clearColor[0]      = 0.2f;
            beginRenderPass->clearColor[1]      = 0.7f;
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
            indx->indexFormat         = IndexType::Uint16;
            indx->offset              = 0;
            indx->resource            = _mergedIndexBufferGPU;
        }

        // Indirect draw call.
        {
            CMDDrawIndexedIndirect* indirect = currentFrame.stream->AddCommand<CMDDrawIndexedIndirect>();
            indirect->count                  = static_cast<uint32>(indirectCommands.size());
            indirect->indirectBuffer         = currentFrame.indirectBufferGPU;
            indirect->stride                 = sizeof(IndexedIndirectCommand);
        }

        // End render pass
        {
            CMDEndRenderPass* end = currentFrame.stream->AddCommand<CMDEndRenderPass>();
            end->isSwapchain      = true;
            end->swapchain        = _swapchain;
        }

        // This does the actual *recording* of every single command stream alive.
        _lgx->CloseCommandStreams(&currentFrame.stream, 1);

        // Submit work on gpu.
        _lgx->SubmitCommandStreams({.streams = &currentFrame.stream, .streamCount = 1, .useWait = true, .waitSemaphore = currentFrame.copySemaphore, .waitValue = currentFrame.copySemaphoreValue});

        // Present main swapchain.
        _lgx->Present({.swapchain = _swapchain});

        // Let LinaGX know we are finalizing this frame.
        _lgx->EndFrame();
    }

} // namespace LinaGX::Examples
