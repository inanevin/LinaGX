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

#include "RenderTarget.hpp"
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

    LinaGX::Renderer* _renderer  = nullptr;
    uint8             _swapchain = 0;
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
    uint16 _shaderProgramColorPass = 0;
    uint16 _shaderProgramFinalPass = 0;
    uint16 _quadShaderProgram      = 0;

    // Objects
    LinaGX::ModelData  _modelData   = {};
    ModelTexture*      _baseTexture = nullptr;
    LINAGX_VEC<Object> _objects;

    // Resources
    uint32 _sampler                  = 0;
    uint16 _descriptorSetSceneData0  = 0;
    uint16 _descriptorSetSceneData1  = 0;
    uint16 _descriptorSetTexture     = 0;
    uint16 _descriptorSetQuadTexture = 0;
    uint32 _ubo0                     = 0;
    uint32 _ubo1                     = 0;
    uint8* _uboMapping0              = 0;
    uint8* _uboMapping1              = 0;
    uint32 _baseColorGPU             = 0;
    uint32 _renderTargetColor        = 0;
    uint32 _renderTargetDepth        = 0;

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

            BackendAPI api = BackendAPI::Vulkan;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::InitInfo initInfo = InitInfo{
                .api                   = api,
                .gpu                   = PreferredGPUType::Integrated,
                .framesInFlight        = FRAMES_IN_FLIGHT,
                .backbufferCount       = 2,
                .checkForFormatSupport = {Format::B8G8R8A8_UNORM, Format::D32_SFLOAT, Format::R8G8B8A8_SRGB},
            };

            _renderer = new LinaGX::Renderer();
            _renderer->Initialize(initInfo);
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _renderer->CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX GLTF Render Target", 0, 0, 800, 800, WindowStyle::Windowed);
            _window->SetCallbackClose([this]() { m_isRunning = false; });
        }

        //******************* DEFAULT SHADER CREATION
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
                .stages                = {{ShaderStage::Vertex, vertexBlob}, {ShaderStage::Fragment, fragBlob}},
                .colorAttachmentFormat = Format::R8G8B8A8_SRGB,
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
            _shaderProgramColorPass = _renderer->CreateShader(shaderDesc);

            shaderDesc.colorAttachmentFormat = Format::B8G8R8A8_UNORM;
            _shaderProgramFinalPass          = _renderer->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            free(vertexBlob.ptr);
            free(fragBlob.ptr);
        }

        //******************* QUAD SHADER CREATION
        {
            // Compile shaders.
            const std::string vtxShader  = LinaGX::ReadFileContentsAsString("Resources/Shaders/quad_vert.glsl");
            const std::string fragShader = LinaGX::ReadFileContentsAsString("Resources/Shaders/quad_frag.glsl");
            ShaderLayout      outLayout  = {};
            DataBlob          vertexBlob = {};
            DataBlob          fragBlob   = {};
            _renderer->CompileShader(ShaderStage::Vertex, vtxShader.c_str(), "Resources/Shaders/Include", vertexBlob, outLayout);
            _renderer->CompileShader(ShaderStage::Fragment, fragShader.c_str(), "Resources/Shaders/Include", fragBlob, outLayout);

            // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.

            // Create shader program with vertex & fragment stages.
            ShaderDesc shaderDesc = {
                .stages                = {{ShaderStage::Vertex, vertexBlob}, {ShaderStage::Fragment, fragBlob}},
                .colorAttachmentFormat = Format::B8G8R8A8_UNORM,
                .depthAttachmentFormat = Format::D32_SFLOAT,
                .layout                = outLayout,
                .polygonMode           = PolygonMode::Fill,
                .cullMode              = CullMode::None,
                .frontFace             = FrontFace::CCW,
                .depthTest             = true,
                .depthWrite            = true,
                .depthCompare          = CompareOp::Less,
                .topology              = Topology::TriangleList,
                .blendAttachment       = {.componentFlags = ColorComponentFlags::RGBA},
            };

            _quadShaderProgram = _renderer->CreateShader(shaderDesc);

            // Compiled binaries are not needed anymore.
            free(vertexBlob.ptr);
            free(fragBlob.ptr);
        }

        auto createRenderTargets = [&]() {
            Texture2DDesc desc = {
                .usage     = Texture2DUsage::ColorTextureRenderTarget,
                .width     = _window->GetWidth(),
                .height    = _window->GetHeight(),
                .mipLevels = 1,
                .format    = Format::R8G8B8A8_SRGB,
                .debugName = "LinaGXRTTexture",
            };

            _renderTargetColor = _renderer->CreateTexture2D(desc);

            desc.format             = Format::D32_SFLOAT;
            desc.usage              = Texture2DUsage::DepthStencilTexture;
            desc.depthStencilAspect = DepthStencilAspect::DepthOnly;
            desc.debugName          = "LinaGXRTDepthTexture";

            _renderTargetDepth = _renderer->CreateTexture2D(desc);
        };

        //******************* RENDER TARGET
        {
            createRenderTargets();
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

                // re-create render targets.
                _renderer->DestroyTexture2D(_renderTargetColor);
                _renderer->DestroyTexture2D(_renderTargetDepth);
                createRenderTargets();

                DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle       = _descriptorSetQuadTexture,
                    .binding         = 0,
                    .descriptorCount = 1,
                    .textures        = &_renderTargetColor,
                    .samplers        = &_sampler,
                    .descriptorType  = DescriptorType::CombinedImageSampler,
                };
                _renderer->DescriptorUpdateImage(imgUpdate);
            });

            // Create command stream to record draw calls.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                _pfd[i].stream        = _renderer->CreateCommandStream(20, QueueType::Graphics);
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
                            Vertex vtx          = {};
                            vtx.position        = prim->positions[k];
                            vtx.uv              = prim->texCoords[k];
                            vtx.inBoneWeights   = prim->weights[k];
                            vtx.inBoneIndices.x = static_cast<float>(prim->joints[k].x);
                            vtx.inBoneIndices.y = static_cast<float>(prim->joints[k].y);
                            vtx.inBoneIndices.z = static_cast<float>(prim->joints[k].z);
                            vtx.inBoneIndices.w = static_cast<float>(prim->joints[k].w);
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

            _ubo0 = _renderer->CreateResource(desc);
            _ubo1 = _renderer->CreateResource(desc);
            _renderer->MapResource(_ubo0, _uboMapping0);
            _renderer->MapResource(_ubo1, _uboMapping1);
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

            DescriptorSetDesc set0Desc = {
                .bindings      = set0Bindings,
                .bindingsCount = 1,
            };

            _descriptorSetTexture     = _renderer->CreateDescriptorSet(set0Desc);
            _descriptorSetQuadTexture = _renderer->CreateDescriptorSet(set0Desc);

            DescriptorUpdateImageDesc imgUpdate = {
                .setHandle       = _descriptorSetTexture,
                .binding         = 0,
                .descriptorCount = 1,
                .textures        = &_baseColorGPU,
                .samplers        = &_sampler,
                .descriptorType  = DescriptorType::CombinedImageSampler,
            };
            _renderer->DescriptorUpdateImage(imgUpdate);

            imgUpdate.setHandle = _descriptorSetQuadTexture;
            imgUpdate.textures  = &_renderTargetColor;
            _renderer->DescriptorUpdateImage(imgUpdate);

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

            DescriptorBinding set2Bindings[1];

            set2Bindings[0] = {
                .binding         = 0,
                .descriptorCount = 1,
                .type            = DescriptorType::UBO,
                .stages          = {ShaderStage::Vertex},
            };

            DescriptorSetDesc set2Desc = {
                .bindings      = set2Bindings,
                .bindingsCount = 1,
            };

            _descriptorSetSceneData0 = _renderer->CreateDescriptorSet(set2Desc);
            _descriptorSetSceneData1 = _renderer->CreateDescriptorSet(set2Desc);

            DescriptorUpdateBufferDesc uboUpdate = {
                .setHandle       = _descriptorSetSceneData0,
                .binding         = 0,
                .descriptorCount = 1,
                .resources       = &_ubo0,
                .descriptorType  = DescriptorType::UBO,
            };

            _renderer->DescriptorUpdateBuffer(uboUpdate);

            uboUpdate.setHandle = _descriptorSetSceneData1;
            uboUpdate.resources = &_ubo1;

            _renderer->DescriptorUpdateBuffer(uboUpdate);
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

        _renderer->DestroyTexture2D(_renderTargetColor);
        _renderer->DestroyTexture2D(_renderTargetDepth);
        _renderer->DestroyResource(_ubo0);
        _renderer->DestroyResource(_ubo1);
        _renderer->DestroyDescriptorSet(_descriptorSetTexture);
        _renderer->DestroyDescriptorSet(_descriptorSetQuadTexture);
        _renderer->DestroyDescriptorSet(_descriptorSetSceneData0);
        _renderer->DestroyDescriptorSet(_descriptorSetSceneData1);
        _renderer->DestroyTexture2D(_baseColorGPU);
        _renderer->DestroySampler(_sampler);
        _renderer->DestroySwapchain(_swapchain);
        _renderer->DestroyShader(_shaderProgramColorPass);
        _renderer->DestroyShader(_shaderProgramFinalPass);
        _renderer->DestroyShader(_quadShaderProgram);

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
        _renderer->PollWindow();

        // Let LinaGX know we are starting a new frame.
        _renderer->StartFrame();

        auto& currentFrame = _pfd[_renderer->GetCurrentFrameIndex()];

        // Sample animation
        {
            auto         targetAnimation = _modelData.allAnimations + 0;
            static float time            = 0.0f;
            time += m_deltaSeconds;
            if (time > targetAnimation->duration)
                time = 0.0f;
            SampleAnimation(_modelData.allSkins, targetAnimation, time);
        }

        // Copy SSBO data on copy queue
        {
            LINAGX_VEC<GPUObjectData> objectData;
            objectData.resize(_objects.size());

            // Inverse root-global matrix.
            auto            root       = _modelData.allSkins->rootJoint;
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
                for (auto joint : _modelData.allSkins->joints)
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
            const glm::mat4 projection = glm::perspective(DEG2RAD(90.0f), static_cast<float>(_window->GetWidth()) / static_cast<float>(_window->GetHeight()), 0.01f, 1000.0f);
            GPUSceneData    sceneData  = {};
            sceneData.viewProj         = projection * eye;
            std::memcpy(_uboMapping0, &sceneData, sizeof(GPUSceneData));
        }

        Viewport     viewport = {.x = 0, .y = 0, .width = _window->GetWidth(), .height = _window->GetHeight(), .minDepth = 0.0f, .maxDepth = 1.0f};
        ScissorsRect sc       = {.x = 0, .y = 0, .width = _window->GetWidth(), .height = _window->GetHeight()};

        // Render pass 1.
        {
            CMDBeginRenderPass* beginRenderPass = currentFrame.stream->AddCommand<CMDBeginRenderPass>();
            beginRenderPass->isSwapchain        = false;
            beginRenderPass->colorTexture       = _renderTargetColor;
            beginRenderPass->depthTexture       = _renderTargetDepth;
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
            bindPipeline->shader          = _shaderProgramColorPass;
        }

        // Bind the descriptors
        {
            CMDBindDescriptorSets* bindSets = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindSets->firstSet              = 0;
            bindSets->setCount              = 3;
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSetTexture, currentFrame.ssboSet, _descriptorSetSceneData0);
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
            end->isSwapchain      = false;
            end->texture          = _renderTargetColor;
        }

        // Update scene data
        {
            const glm::mat4 eye        = glm::lookAt(glm::vec3(0.0f, 0.0f, -200.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            const glm::mat4 projection = glm::perspective(DEG2RAD(90.0f), static_cast<float>(_window->GetWidth()) / static_cast<float>(_window->GetHeight()), 0.01f, 1000.0f);
            GPUSceneData    sceneData  = {};
            sceneData.viewProj         = projection * eye;
            std::memcpy(_uboMapping1, &sceneData, sizeof(GPUSceneData));
        }

        // Render pass swapchain.
        {
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
            bindPipeline->shader          = _shaderProgramFinalPass;
        }

        // Bind the descriptors
        {
            CMDBindDescriptorSets* bindSets = currentFrame.stream->AddCommand<CMDBindDescriptorSets>();
            bindSets->firstSet              = 0;
            bindSets->setCount              = 3;
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSetTexture, currentFrame.ssboSet, _descriptorSetSceneData1);
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
            bindSets->descriptorSetHandles  = currentFrame.stream->EmplaceAuxMemory<uint16>(_descriptorSetQuadTexture);
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
