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

#include "GLTFSkinning.hpp"
#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
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
    uint32            _depth     = 0;
    Window*           _window    = nullptr;

    struct Vertex
    {
        LGXVector3 position      = {};
        LGXVector2 uv            = {};
        LGXVector4 inBoneIndices = {};
        LGXVector4 inBoneWeights = {};
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
        glm::mat4 bones[31] = {};
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
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX GLTF Skinning", 0, 0, 800, 800, WindowStyle::WindowedApplication);
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

            LinaGX::ShaderColorAttachment colorAttachment = {
                .format = Format::B8G8R8A8_UNORM,

            };

            LinaGX::ShaderDepthStencilDesc depthStencil;
            depthStencil.depthStencilAttachmentFormat = LinaGX::Format::D32_SFLOAT;
            depthStencil.depthTest = depthStencil.depthWrite = true;
            depthStencil.depthCompare                        = LinaGX::CompareOp::Less;

            // Create shader program with vertex & fragment stages.
            ShaderDesc shaderDesc = {
                .stages           = {{ShaderStage::Vertex, outCompiledBlobs[ShaderStage::Vertex]}, {ShaderStage::Fragment, outCompiledBlobs[ShaderStage::Fragment]}},
                .colorAttachments = {colorAttachment},
                .depthStencilDesc = depthStencil,
                .layout           = outLayout,
                .polygonMode      = PolygonMode::Fill,
                .cullMode         = CullMode::Back,
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

            LinaGX::TextureDesc depthDesc = {
                .format    = LinaGX::Format::D32_SFLOAT,
                .flags     = TextureFlags::TF_DepthTexture,
                .width     = _window->GetSize().x,
                .height    = _window->GetSize().y,
                .debugName = "Depth Texture",
            };

            _depth = _lgx->CreateTexture(depthDesc);

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

                _lgx->DestroyTexture(_depth);
                LinaGX::TextureDesc depthDesc = {
                    .format    = LinaGX::Format::D32_SFLOAT,
                    .flags     = TextureFlags::TF_DepthTexture,
                    .width     = _window->GetSize().x,
                    .height    = _window->GetSize().y,
                    .debugName = "Depth Texture",
                };

                _depth = _lgx->CreateTexture(depthDesc);
            });

            // Create command stream to record draw calls.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].stream        = _lgx->CreateCommandStream({CommandType::Graphics, 10, 512, 4096, 64});
                _pfd[i].copyStream    = _lgx->CreateCommandStream({CommandType::Transfer, 10, 512, 4096, 64});
                _pfd[i].copySemaphore = _lgx->CreateUserSemaphore();
            }
        }

        //*******************  MODEL (Vertex Data & Object Array)
        {
            LinaGX::LoadGLTFBinary("Resources/Models/Fox.glb", _modelData);

            const auto ns = static_cast<uint32>(_modelData.allNodes.size());
            for (uint32 i = 0; i < ns; i++)
            {
                ModelNode* node = _modelData.allNodes[i];

                if (node->mesh != nullptr)
                {
                    _objects.push_back({});
                    Object& obj = _objects[_objects.size() - 1];
                    obj.index   = static_cast<uint32>(_objects.size() - 1);

                    const auto ps = static_cast<uint32>(node->mesh->primitives.size());
                    for (uint32 j = 0; j < ps; j++)
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

            _ubo = _lgx->CreateResource(desc);
            _lgx->MapResource(_ubo, _uboMapping);
        }

        //*******************  DESCRIPTOR SET
        {
            DescriptorBinding set0Binding0 = {
                .descriptorCount = 1,
                .type            = DescriptorType::UBO,
                .stages          = {ShaderStage::Vertex},
            };

            DescriptorBinding set0Binding1 = {
                .descriptorCount = 1,
                .type            = DescriptorType::CombinedImageSampler,
                .stages          = {ShaderStage::Fragment},
            };

            DescriptorSetDesc set0Desc = {
                .bindings = {set0Binding0, set0Binding1},
            };

            _descriptorSet0 = _lgx->CreateDescriptorSet(set0Desc);

            DescriptorBinding set1Binding0 = {
                .descriptorCount = 1,
                .type            = DescriptorType::SSBO,
                .stages          = {ShaderStage::Vertex},
            };

            DescriptorSetDesc set1Desc = {
                .bindings = {set1Binding0},
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

            DescriptorUpdateBufferDesc uboUpdate = {
                .setHandle = _descriptorSet0,
                .binding   = 0,
                .buffers   = {_ubo},
            };

            DescriptorUpdateImageDesc imgUpdate = {
                .setHandle = _descriptorSet0,
                .binding   = 1,
                .textures  = {_baseColorGPU},
                .samplers  = {_sampler},
            };

            _lgx->DescriptorUpdateBuffer(uboUpdate);
            _lgx->DescriptorUpdateImage(imgUpdate);
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
        }

        _lgx->DestroyResource(_ubo);
        _lgx->DestroyDescriptorSet(_descriptorSet0);
        _lgx->DestroyTexture(_baseColorGPU);
        _lgx->DestroyTexture(_depth);
        _lgx->DestroySampler(_sampler);
        _lgx->DestroySwapchain(_swapchain);
        _lgx->DestroyShader(_shaderProgram);

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

        // Let LinaGX know we are starting a new frame.
        _lgx->StartFrame();

        auto& currentFrame = _pfd[_lgx->GetCurrentFrameIndex()];

        // Sample animation
        {
            auto         targetAnimation = _modelData.allAnims[0] + 0;
            static float time            = 0.0f;
            time += m_deltaSeconds;
            if (time > targetAnimation->duration)
                time = 0.0f;
            SampleAnimation(_modelData.allSkins[0], targetAnimation, time);
        }

        // Copy SSBO data on copy queue
        {
            LINAGX_VEC<GPUObjectData> objectData;
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
            std::memcpy(_uboMapping, &sceneData, sizeof(GPUSceneData));
        }

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

        // Barrier to Depth Write
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
            Viewport                  viewport        = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y, .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect              sc              = {.x = 0, .y = 0, .width = _window->GetSize().x, .height = _window->GetSize().y};
            CMDBeginRenderPass*       beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();
            RenderPassColorAttachment colorAttachment;
            colorAttachment.clearColor                       = {0.8f, 0.8f, 0.8f, 1.0f};
            colorAttachment.texture                          = static_cast<uint32>(_swapchain);
            colorAttachment.isSwapchain                      = true;
            colorAttachment.loadOp                           = LoadOp::Clear;
            colorAttachment.storeOp                          = StoreOp::Store;
            beginRenderPass->colorAttachmentCount            = 1;
            beginRenderPass->colorAttachments                = currentFrame.stream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->viewport                        = viewport;
            beginRenderPass->scissors                        = sc;
            beginRenderPass->depthStencilAttachment.useDepth = true;
            beginRenderPass->depthStencilAttachment.texture  = _depth;
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
        _lgx->SubmitCommandStreams({.targetQueue = _lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &currentFrame.stream, .streamCount = 1, .useWait = true, .waitCount = 1, .waitSemaphores = &currentFrame.copySemaphore, .waitValues = &currentFrame.copySemaphoreValue});

        // Present main swapchain.
        _lgx->Present({.swapchains = &_swapchain, .swapchainCount = 1});

        // Let LinaGX know we are finalizing this frame.
        _lgx->EndFrame();
    }

} // namespace LinaGX::Examples
