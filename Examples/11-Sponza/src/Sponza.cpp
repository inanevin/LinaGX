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

#include "Sponza.hpp"
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

    LinaGX::Renderer* _renderer  = nullptr;
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
        uint32 materialIndex = 0;

        LINAGX_VEC<Vertex>        vertices;
        LINAGX_VEC<unsigned char> indices;
        IndexType                 indexType = IndexType::Uint16;

        uint32 vertexBufferStaging = 0;
        uint32 vertexBufferGPU     = 0;

        uint32 indexBufferStaging = 0;
        uint32 indexBufferGPU     = 0;
    };

    struct Material
    {
        uint16 descriptorSet = 0;
        uint32 albedoTexture = 0;
        uint32 ubo           = 0;
        uint8* uboMapping    = nullptr;
    };

    struct ConstantsData
    {
        uint32 index   = 0;
        uint32 hasSkin = 0;
    };

    struct Object
    {
        LinaGX::ModelData    model;
        ConstantsData        constants;
        glm::mat4            modelMatrix;
        LINAGX_VEC<Mesh>     meshes;
        LINAGX_VEC<uint32>   texturesGPU;
        LINAGX_VEC<Material> materials;
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

    struct GPUMaterialData
    {
        glm::vec4 baseColorTint;
        float     alphaCutoff;
    };

    struct CameraData
    {
        glm::vec3 position;
        glm::quat rotation    = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 euler       = {};
        glm::vec3 targetEuler = {};

        float speed         = 125.0f;
        float rotationSpeed = 5.0f;

        bool moveForward   = false;
        bool moveBackwards = false;
        bool moveRight     = false;
        bool moveLeft      = false;

        uint32 prevMouseX = 0;
        uint32 prevMouseY = 0;

        bool isPressed = false;
    };

    // Shaders.
    uint16 _shaderProgramFinalPass = 0;

    // Objects
    LINAGX_VEC<Object> _objects;
    CameraData         _camera;

    // Resources
    uint32 _sampler = 0;

    struct PerFrameData
    {
        uint32                 ssboStaging             = 0;
        uint32                 ssboGPU                 = 0;
        uint8*                 ssboMapping             = nullptr;
        uint16                 ssboSet                 = 0;
        LinaGX::CommandStream* stream                  = nullptr;
        LinaGX::CommandStream* copyStream              = nullptr;
        uint16                 copySemaphore           = 0;
        uint64                 copySemaphoreValue      = 0;
        uint16                 descriptorSetSceneData0 = 0;
        uint32                 ubo0                    = 0;
        uint8*                 uboMapping0             = 0;
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

            BackendAPI api = BackendAPI::DX12;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::InitInfo initInfo = InitInfo{
                .api                   = api,
                .gpu                   = PreferredGPUType::Integrated,
                .framesInFlight        = FRAMES_IN_FLIGHT,
                .backbufferCount       = 2,
                .gpuLimits             = limits,
                .checkForFormatSupport = {Format::B8G8R8A8_UNORM, Format::D32_SFLOAT, Format::R8G8B8A8_SRGB},
            };

            _renderer = new LinaGX::Renderer();
            _renderer->Initialize(initInfo);
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _renderer->CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Sponza", 0, 0, 800, 800, WindowStyle::Windowed);
            _window->SetCallbackClose([this]() { m_isRunning = false; });

            _window->SetCallbackKey([](uint32 key, uint32 scanCode, InputAction action) {
                if (key == LINAGX_KEY_W)
                    _camera.moveForward = action != InputAction::Released;
                else if (key == LINAGX_KEY_S)
                    _camera.moveBackwards = action != InputAction::Released;
                else if (key == LINAGX_KEY_D)
                    _camera.moveRight = action != InputAction::Released;
                else if (key == LINAGX_KEY_A)
                    _camera.moveLeft = action != InputAction::Released;
            });

            _window->SetCallbackMouse([this](uint32 button, InputAction action) {
                if (button == LINAGX_MOUSE_0)
                    _camera.isPressed = action != InputAction::Released;
            });

            _window->SetCallbackMouseMove([this](int32 x, int32 y) {
                if (_camera.isPressed)
                {
                    int32 deltaX = static_cast<int32>(x) - static_cast<int32>(_camera.prevMouseX);
                    int32 deltaY = static_cast<int32>(y) - static_cast<int32>(_camera.prevMouseY);

                    const float sensitivity = _camera.rotationSpeed * m_deltaSeconds; // Assuming this is already normalized

                    // Convert mouse movement to angles
                    float yaw   = sensitivity * static_cast<float>(deltaX);
                    float pitch = sensitivity * static_cast<float>(deltaY);

                    _camera.targetEuler.y -= yaw;
                    _camera.targetEuler.x -= pitch;
                }

                // Store the current mouse position for the next frame
                _camera.prevMouseX = x;
                _camera.prevMouseY = y;
            });

            _window->SetCallbackMouseWheel([this](uint32 delta) {

            });
        }

        //******************* DEFAULT SHADER CREATION
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

            _shaderProgramFinalPass = _renderer->CreateShader(shaderDesc);

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
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].stream        = _renderer->CreateCommandStream(1000, QueueType::Graphics);
                _pfd[i].copyStream    = _renderer->CreateCommandStream(1000, QueueType::Transfer);
                _pfd[i].copySemaphore = _renderer->CreateUserSemaphore();
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

            _sampler = _renderer->CreateSampler(samplerDesc);
        }

        //*******************  MODEL (Vertex Data & Object Array)
        {
            _objects.push_back({});
            _objects.push_back({});

            glm::mat4 mat = glm::mat4(1.0f);
            glm::quat q   = glm::quat(glm::vec3(0.0f, DEG2RAD(45.0f), 0.0f));
            mat           = TranslateRotateScale({0.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {0.5f, 0.5f, 0.5f});

            auto& fox             = _objects[0];
            fox.modelMatrix       = mat;
            fox.constants.hasSkin = true;
            fox.constants.index   = 0;

            auto& sponza             = _objects[1];
            sponza.modelMatrix       = TranslateRotateScale({0.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {0.25f, 0.25f, 0.25f});
            sponza.constants.hasSkin = false;
            sponza.constants.index   = 1;

            LinaGX::LoadGLTFBinary("Resources/Models/Fox.glb", fox.model);
            LinaGX::LoadGLTFASCII("Resources/Models/Sponza/Sponza.gltf", sponza.model);

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
                        .format    = Format::R8G8B8A8_UNORM,
                        .debugName = "Material Texture",
                    };

                    obj.texturesGPU[i] = _renderer->CreateTexture2D(desc);
                }

                obj.materials.resize(obj.model.allMaterialsCount);

                for (uint32 i = 0; i < obj.model.allMaterialsCount; i++)
                {
                    ModelMaterial* modMat   = obj.model.allMaterials + i;
                    auto&          material = obj.materials[i];

                    material.albedoTexture = modMat->textureIndices[GLTFTextureType::BaseColor];

                    ResourceDesc uboDesc = {
                        .size          = sizeof(GPUMaterialData),
                        .typeHintFlags = ResourceTypeHint::TH_ConstantBuffer,
                        .heapType      = ResourceHeap::StagingHeap,
                        .debugName     = "Material UBO",
                    };

                    material.ubo = _renderer->CreateResource(uboDesc);
                    _renderer->MapResource(material.ubo, material.uboMapping);

                    GPUMaterialData gpuMatData;
                    gpuMatData.alphaCutoff   = modMat->alphaCutoff;
                    gpuMatData.baseColorTint = glm::vec4(modMat->baseColor.x, modMat->baseColor.y, modMat->baseColor.z, modMat->baseColor.w);
                    std::memcpy(material.uboMapping, &gpuMatData, sizeof(GPUMaterialData));
                    _renderer->UnmapResource(material.ubo);

                    DescriptorBinding bindings[2];

                    bindings[0] = {
                        .binding         = 0,
                        .descriptorCount = 1,
                        .type            = DescriptorType::UBO,
                        .stages          = {ShaderStage::Fragment},
                    };

                    bindings[1] = {
                        .binding         = 1,
                        .descriptorCount = 1,
                        .type            = DescriptorType::CombinedImageSampler,
                        .stages          = {ShaderStage::Fragment},
                    };

                    DescriptorSetDesc desc = {
                        .bindings      = bindings,
                        .bindingsCount = 2,
                    };

                    material.descriptorSet = _renderer->CreateDescriptorSet(desc);

                    DescriptorUpdateBufferDesc bufferDesc = {
                        .setHandle       = material.descriptorSet,
                        .binding         = 0,
                        .descriptorCount = 1,
                        .resources       = &material.ubo,
                        .descriptorType  = DescriptorType::UBO,
                    };

                    _renderer->DescriptorUpdateBuffer(bufferDesc);

                    DescriptorUpdateImageDesc imgUpdate = {
                        .setHandle       = material.descriptorSet,
                        .binding         = 1,
                        .descriptorCount = 1,
                        .textures        = &obj.texturesGPU[material.albedoTexture],
                        .samplers        = &_sampler,
                        .descriptorType  = DescriptorType::CombinedImageSampler,
                    };

                    _renderer->DescriptorUpdateImage(imgUpdate);
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
                            mesh.materialIndex       = prim->material->index;

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

            for (auto& obj : _objects)
            {
                for (auto& mesh : obj.meshes)
                {
                    const uint32 vertexBufferSize = static_cast<uint32>(sizeof(Vertex) * mesh.vertices.size());

                    ResourceDesc vbDesc = ResourceDesc{
                        .size          = vertexBufferSize,
                        .typeHintFlags = TH_VertexBuffer,
                        .heapType      = ResourceHeap::StagingHeap,
                        .debugName     = "VertexBuffer",
                    };

                    mesh.vertexBufferStaging = _renderer->CreateResource(vbDesc);
                    vbDesc.heapType          = ResourceHeap::GPUOnly;
                    mesh.vertexBufferGPU     = _renderer->CreateResource(vbDesc);

                    uint8* vtxData = nullptr;
                    _renderer->MapResource(mesh.vertexBufferStaging, vtxData);
                    std::memcpy(vtxData, mesh.vertices.data(), vertexBufferSize);
                    _renderer->UnmapResource(mesh.vertexBufferStaging);

                    if (!mesh.indices.empty())
                    {
                        const uint32 indexBufferSize = static_cast<uint32>(mesh.indices.size());

                        ResourceDesc ibDesc = ResourceDesc{
                            .size          = indexBufferSize,
                            .typeHintFlags = TH_IndexBuffer,
                            .heapType      = ResourceHeap::StagingHeap,
                            .debugName     = "IndexBuffer",
                        };

                        mesh.indexBufferStaging = _renderer->CreateResource(ibDesc);
                        ibDesc.heapType         = ResourceHeap::GPUOnly;
                        mesh.indexBufferGPU     = _renderer->CreateResource(ibDesc);

                        uint8* ibData = nullptr;
                        _renderer->MapResource(mesh.indexBufferStaging, ibData);
                        std::memcpy(ibData, mesh.indices.data(), indexBufferSize);
                        _renderer->UnmapResource(mesh.indexBufferStaging);
                    }
                }
            }
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

            for (const auto& obj : _objects)
            {
                for (const auto& mesh : obj.meshes)
                {
                    // Record copy command.
                    CMDCopyResource* copyVtxBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
                    copyVtxBuf->source          = mesh.vertexBufferStaging;
                    copyVtxBuf->destination     = mesh.vertexBufferGPU;

                    if (!mesh.indices.empty())
                    {
                        CMDCopyResource* copyIndexBuf = _pfd[0].copyStream->AddCommand<CMDCopyResource>();
                        copyIndexBuf->source          = mesh.indexBufferStaging;
                        copyIndexBuf->destination     = mesh.indexBufferGPU;
                    }
                }
            }

            _renderer->CloseCommandStreams(&_pfd[0].copyStream, 1);

            // Execute copy command on the transfer queue, signal a semaphore when it's done and wait for it on the CPU side.
            _renderer->SubmitCommandStreams({.queue = QueueType::Transfer, .streams = &_pfd[0].copyStream, .streamCount = 1, .useSignal = true, .signalSemaphore = _pfd[0].copySemaphore, .signalValue = ++_pfd[0].copySemaphoreValue});
            _renderer->WaitForUserSemaphore(_pfd[0].copySemaphore, _pfd[0].copySemaphoreValue);

            // Not needed anymore.
            for (auto& obj : _objects)
            {
                for (auto& mesh : obj.meshes)
                {
                    _renderer->DestroyResource(mesh.vertexBufferStaging);

                    if (!mesh.indices.empty())
                        _renderer->DestroyResource(mesh.indexBufferStaging);
                }
            }
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

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].ubo0 = _renderer->CreateResource(desc);
                _renderer->MapResource(_pfd[i].ubo0, _pfd[i].uboMapping0);
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
                _pfd[i].descriptorSetSceneData0 = _renderer->CreateDescriptorSet(set0Desc);

                DescriptorUpdateBufferDesc uboUpdate = {
                    .setHandle       = _pfd[i].descriptorSetSceneData0,
                    .binding         = 0,
                    .descriptorCount = 1,
                    .resources       = &_pfd[i].ubo0,
                    .descriptorType  = DescriptorType::UBO,
                };

                _renderer->DescriptorUpdateBuffer(uboUpdate);
            }

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
        }
    } // namespace LinaGX::Examples

    void Example::Shutdown()
    {
        // First get rid of the window.
        _renderer->DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _renderer->Join();

        // Get rid of resources
        for (auto& obj : _objects)
        {
            for (auto& mat : obj.materials)
            {
                _renderer->DestroyDescriptorSet(mat.descriptorSet);
                _renderer->DestroyResource(mat.ubo);
            }

            for (auto& mesh : obj.meshes)
            {
                _renderer->DestroyResource(mesh.vertexBufferGPU);

                if (!mesh.indices.empty())
                    _renderer->DestroyResource(mesh.indexBufferGPU);
            }

            for (auto& txt : obj.texturesGPU)
                _renderer->DestroyTexture2D(txt);
        }

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            _renderer->DestroyResource(_pfd[i].ssboStaging);
            _renderer->DestroyResource(_pfd[i].ssboGPU);
            _renderer->DestroyDescriptorSet(_pfd[i].ssboSet);
            _renderer->DestroyUserSemaphore(_pfd[i].copySemaphore);
            _renderer->DestroyCommandStream(_pfd[i].stream);
            _renderer->DestroyCommandStream(_pfd[i].copyStream);
            _renderer->DestroyResource(_pfd[i].ubo0);
            _renderer->DestroyDescriptorSet(_pfd[i].descriptorSetSceneData0);
        }

        _renderer->DestroySampler(_sampler);
        _renderer->DestroySwapchain(_swapchain);
        _renderer->DestroyShader(_shaderProgramFinalPass);

        // Terminate renderer & shutdown app.
        delete _renderer;
        App::Shutdown();
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

        return LGXVector4::Lerp(value1, value2, alpha);
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

    void DrawObjects(const PerFrameData& currentFrame)
    {
        for (auto& obj : _objects)
        {
            CMDBindConstants* constants = currentFrame.stream->AddCommand<CMDBindConstants>();
            constants->size             = sizeof(ConstantsData);
            constants->stages           = currentFrame.stream->EmplaceAuxMemory<ShaderStage>(ShaderStage::Vertex);
            constants->stagesSize       = 1;
            constants->offset           = 0;
            constants->data             = currentFrame.stream->EmplaceAuxMemory<ConstantsData>(obj.constants);

            for (const auto& mesh : obj.meshes)
            {
                const auto& material = obj.materials[mesh.materialIndex];

                // Bind the descriptors
                {
                    CMDBindDescriptorSets* bindSets = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
                    bindSets->firstSet              = 0;
                    bindSets->setCount              = 3;
                    bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(currentFrame.descriptorSetSceneData0, currentFrame.ssboSet, material.descriptorSet);
                }

                CMDBindVertexBuffers* vtx = currentFrame.stream->AddCommand<CMDBindVertexBuffers>();
                vtx->slot                 = 0;
                vtx->resource             = mesh.vertexBufferGPU;
                vtx->vertexSize           = sizeof(Vertex);
                vtx->offset               = 0;

                if (mesh.indices.empty())
                {
                    CMDDrawInstanced* draw       = currentFrame.stream->AddCommand<CMDDrawInstanced>();
                    draw->instanceCount          = 1;
                    draw->startInstanceLocation  = 0;
                    draw->startVertexLocation    = 0;
                    draw->vertexCountPerInstance = static_cast<uint32>(mesh.vertices.size());
                }
                else
                {
                    CMDBindIndexBuffers* indx = currentFrame.stream->AddCommand<CMDBindIndexBuffers>();
                    indx->indexFormat         = mesh.indexType;
                    indx->offset              = 0;
                    indx->resource            = mesh.indexBufferGPU;

                    CMDDrawIndexedInstanced* draw = currentFrame.stream->AddCommand<CMDDrawIndexedInstanced>();
                    uint32                   ic   = static_cast<uint32>(mesh.indices.size()) / (mesh.indexType == IndexType::Uint16 ? sizeof(uint16) : sizeof(uint32));
                    draw->baseVertexLocation      = 0;
                    draw->indexCountPerInstance   = ic;
                    draw->instanceCount           = 1;
                    draw->startIndexLocation      = 0;
                    draw->startInstanceLocation   = 0;
                }
            }
        }
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        _renderer->PollWindow();

        // Let LinaGX know we are starting a new frame.
        _renderer->StartFrame();

        auto& currentFrame = _pfd[_renderer->GetCurrentFrameIndex()];

        // Sample animation
        for (const auto& obj : _objects)
        {
            for (uint32 i = 0; i < obj.model.allSkinsCount; i++)
            {
                ModelSkin* skin = obj.model.allSkins + i;

                auto         targetAnimation = obj.model.allAnimations + 0;
                static float time            = 0.0f;
                time += m_deltaSeconds;
                if (time > targetAnimation->duration)
                    time = 0.0f;
                SampleAnimation(obj.model.allSkins, targetAnimation, time);
            }
        }

        // Copy SSBO data on copy queue
        {
            LINAGX_VEC<GPUObjectData> objectData;
            objectData.resize(_objects.size());

            // Inverse root-global matrix.
            auto            root       = _objects[0].model.allSkins->rootJoint;
            const glm::mat4 rootGlobal = CalculateGlobalMatrix(root);
            const glm::mat4 inv        = glm::inverse(rootGlobal);

            for (size_t i = 0; i < _objects.size(); i++)
            {
                const auto& obj = _objects[i];

                // Model.
                objectData[i].modelMatrix = obj.modelMatrix;

                if (obj.constants.hasSkin)
                {
                    // Assign all skinning matrices.
                    // This is a very basic implementation of skinned animations
                    uint32 k = 0;
                    for (auto joint : obj.model.allSkins->joints)
                    {
                        const glm::mat4 jointGlobal = CalculateGlobalMatrix(joint);
                        const glm::mat4 inverseBind = glm::make_mat4(joint->inverseBindMatrix.data());
                        objectData[i].bones[k]      = inv * jointGlobal * inverseBind;
                        k++;
                    }
                }
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
            _camera.euler.x  = LinaGX::Lerp(_camera.euler.x, _camera.targetEuler.x, m_deltaSeconds * 50.0f);
            _camera.euler.y  = LinaGX::Lerp(_camera.euler.y, _camera.targetEuler.y, m_deltaSeconds * 50.0f);
            _camera.rotation = glm::quat(_camera.euler);

            glm::vec3 forwardVector = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 rightVector   = glm::vec3(1.0f, 0.0f, 0.0f);
            forwardVector           = _camera.rotation * forwardVector;
            forwardVector           = glm::normalize(forwardVector) * _camera.speed * m_deltaSeconds;
            rightVector             = _camera.rotation * rightVector;
            rightVector             = glm::normalize(rightVector) * _camera.speed * m_deltaSeconds;

            if (_camera.moveForward)
                _camera.position += forwardVector;
            else if (_camera.moveBackwards)
                _camera.position -= forwardVector;
            if (_camera.moveRight)
                _camera.position += rightVector;
            else if (_camera.moveLeft)
                _camera.position -= rightVector;

            glm::mat4 rotationMatrix    = glm::mat4_cast(glm::inverse(_camera.rotation));
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -_camera.position);
            glm::mat4 viewMatrix        = rotationMatrix * translationMatrix;

            const glm::mat4 projection = glm::perspective(DEG2RAD(90.0f), static_cast<float>(_window->GetWidth()) / static_cast<float>(_window->GetHeight()), 0.1f, 1200.0f);
            GPUSceneData    sceneData  = {};
            sceneData.viewProj         = projection * viewMatrix;
            std::memcpy(currentFrame.uboMapping0, &sceneData, sizeof(GPUSceneData));
        }

        Viewport     viewport = {.x = 0, .y = 0, .width = _window->GetWidth(), .height = _window->GetHeight(), .minDepth = 0.0f, .maxDepth = 1.0f};
        ScissorsRect sc       = {.x = 0, .y = 0, .width = _window->GetWidth(), .height = _window->GetHeight()};

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
            bindPipeline->shader          = _shaderProgramFinalPass;
        }

        // Per object draw
        {
            DrawObjects(currentFrame);
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
