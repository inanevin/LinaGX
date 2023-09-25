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
#include "FoxLounge.hpp"
#include "ThreadPool.hpp"

#include <iostream>
#include <cstdarg>
#include <filesystem>
#include "LinaGX/Common/CommonData.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <easy/profiler.h>
#include <chrono>

namespace LinaGX::Examples
{

#define MAX_OBJECTS         10
#define MAX_LIGHTS          10
#define MAX_MATS            10
#define REFLECTION_PASS_RES 512

    void Example::ConfigureInitializeLinaGX()
    {
        LinaGX::Config.logLevel                               = LogLevel::Verbose;
        LinaGX::Config.errorCallback                          = LogError;
        LinaGX::Config.infoCallback                           = LogInfo;
        LinaGX::Config.dx12Config.serializeShaderDebugSymbols = true;
        LinaGX::Config.dx12Config.enableDebugLayers           = true;

        BackendAPI api = BackendAPI::DX12;

#ifdef LINAGX_PLATFORM_APPLE
        api = BackendAPI::Metal;
#endif
        LinaGX::GPUFeatures features = {
            .enableBindless = true,
        };

        LinaGX::GPULimits limits = {};

        LinaGX::InitInfo initInfo = InitInfo{
            .api                   = api,
            .gpu                   = PreferredGPUType::Discrete,
            .framesInFlight        = FRAMES_IN_FLIGHT,
            .backbufferCount       = BACKBUFFER_COUNT,
            .gpuLimits             = limits,
            .gpuFeatures           = features,
            .checkForFormatSupport = {Format::B8G8R8A8_UNORM, Format::D32_SFLOAT},
        };

        m_lgx = new LinaGX::Instance();
        m_lgx->Initialize(initInfo);
    }

    void Example::CreateMainWindow()
    {
        m_window = m_lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX SSBO", 0, 0, 800, 800, WindowStyle::WindowedApplication);
        m_window->SetCallbackClose([this]() { Quit(); });

        m_window->SetCallbackSizeChanged([&](const LGXVector2ui& newSize) {
            LGXVector2ui monitor = m_window->GetMonitorSize();

            SwapchainRecreateDesc resizeDesc = {
                .swapchain    = m_swapchain,
                .width        = newSize.x,
                .height       = newSize.y,
                .isFullscreen = newSize.x == monitor.x && newSize.y == monitor.y,
            };

            m_lgx->RecreateSwapchain(resizeDesc);

            DestroyDynamicTextures();
            CreateDynamicTextures();
        });

        m_swapchain = m_lgx->CreateSwapchain({
            .format       = Format::B8G8R8A8_SRGB,
            .x            = 0,
            .y            = 0,
            .width        = m_window->GetSize().x,
            .height       = m_window->GetSize().y,
            .window       = m_window->GetWindowHandle(),
            .osHandle     = m_window->GetOSHandle(),
            .isFullscreen = false,
            .vsyncMode    = VsyncMode::None,
        });
    }

    void Example::SetupStreams()
    {
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd                  = m_pfd[i];
            pfd.graphicsStream         = m_lgx->CreateCommandStream(500, CommandType::Graphics, 10000);
            pfd.transferStream         = m_lgx->CreateCommandStream(100, CommandType::Transfer, 10000);
            pfd.transferSemaphoreValue = 0;
            pfd.transferSemaphore      = m_lgx->CreateUserSemaphore();
        }
    }

    void Example::LoadTexture(const char* path, uint32 id)
    {
        LinaGX::TextureBuffer outData;
        LinaGX::LoadImageFromFile(path, outData, ImageChannelMask::RGBA);
        auto& txt = m_textures.at(id);
        txt.allLevels.push_back(outData);
        txt.path            = path;
        const uint32 levels = LinaGX::CalculateMipLevels(outData.width, outData.height);
        LinaGX::GenerateMipmaps(outData, txt.allLevels, MipmapFilter::Default, ImageChannelMask::RGBA, false);
    }

    void Example::SetupMaterials()
    {
        LOGT("Setting up materials...");

        // works bc. we have unique names.
        // not fast :).
        auto getTextureIndex = [&](const char* path) -> int32 {
            int32 i = 0;

            for (const auto& txt : m_textures)
            {
                if (txt.path.compare(path) == 0)
                    return i;

                i++;
            }

            return -1;
        };

        const auto& materialTextureMapping = Utility::GetMaterialTextureMapping();

        // Normally you'd do such a custom setup on an editor
        // but well this is an example app :).
        for (auto& mat : m_materials)
        {
            auto        sid     = LinaGX::LGX_ToSID(mat.name.c_str());
            const auto& mapping = materialTextureMapping.at(sid);

            for (const auto& [type, path] : mapping)
                mat.textureIndices[type] = getTextureIndex(path);

            if (sid == "Terrain"_hs)
            {
                mat.gpuMat.specialTexture = getTextureIndex("Resources/Textures/noiseTexture.png");
                mat.gpuMat.baseColorFac   = glm::vec4(1);
            }

            mat.gpuMat.baseColor         = mat.textureIndices.at(LinaGX::GLTFTextureType::BaseColor);
            mat.gpuMat.normal            = mat.textureIndices.at(LinaGX::GLTFTextureType::Normal);
            mat.gpuMat.metallicRoughness = mat.textureIndices.at(LinaGX::GLTFTextureType::MetallicRoughness);
            auto it                      = mat.textureIndices.find(LinaGX::GLTFTextureType::Emissive);

            if (it != mat.textureIndices.end())
            {
                mat.gpuMat.emissive    = it->second;
                mat.gpuMat.emissiveFac = glm::vec4(1, 1, 1, 1) * 2.0f;
            }
            else
            {
                mat.gpuMat.emissive    = -1;
                mat.gpuMat.emissiveFac = glm::vec4(0, 0, 0, 0);
            }

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                LinaGX::ResourceDesc matDataResource = {
                    .size          = sizeof(GPUMaterialData),
                    .typeHintFlags = LinaGX::TH_ConstantBuffer,
                    .heapType      = LinaGX::ResourceHeap::StagingHeap,
                    .debugName     = "Material Data Staging",
                };

                mat.stagingResources[i]   = m_lgx->CreateResource(matDataResource);
                matDataResource.heapType  = LinaGX::ResourceHeap::GPUOnly;
                matDataResource.debugName = "Material Data GPU";
                mat.gpuResources[i]       = m_lgx->CreateResource(matDataResource);

                uint8* mapping = nullptr;
                m_lgx->MapResource(mat.stagingResources[i], mapping);
                std::memcpy(mapping, &mat.gpuMat, sizeof(GPUMaterialData));
                m_lgx->UnmapResource(mat.stagingResources[i]);

                LinaGX::CMDCopyResource* copy = m_pfd[0].transferStream->AddCommand<LinaGX::CMDCopyResource>();
                copy->extension               = nullptr;
                copy->source                  = mat.stagingResources[i];
                copy->destination             = mat.gpuResources[i];

                mat.descriptorSets[i]                     = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionObjectMaterial());
                LinaGX::DescriptorUpdateBufferDesc update = {
                    .setHandle = mat.descriptorSets[i],
                    .binding   = 0,
                    .buffers   = {mat.gpuResources[i]},
                };

                m_lgx->DescriptorUpdateBuffer(update);

                LinaGX::DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle = mat.descriptorSets[i],
                    .binding   = 1,
                    .textures  = {m_textures[mat.gpuMat.baseColor].gpuHandle, m_textures[mat.gpuMat.normal].gpuHandle, m_textures[mat.gpuMat.metallicRoughness].gpuHandle},
                };

                if (mat.gpuMat.emissive != -1)
                {
                    imgUpdate.textures.push_back(m_textures[mat.gpuMat.emissive].gpuHandle);
                }
                else
                {
                    if (mat.gpuMat.specialTexture == -1)
                        imgUpdate.textures.push_back(m_textures[getTextureIndex("Resources/Textures/black32.png")].gpuHandle);
                    else
                        imgUpdate.textures.push_back(m_textures[mat.gpuMat.specialTexture].gpuHandle);
                }

                m_lgx->DescriptorUpdateImage(imgUpdate);
            }
        }
    }

    void Example::LoadAndParseModels()
    {
        LOGT("Parsing models...");

        // Load em.
        LinaGX::ModelData skyDomeData, foxLoungeData, foxData = {};
        LinaGX::LoadGLTFBinary("Resources/Models/SkyDome/SkyCube.glb", skyDomeData);
        LinaGX::LoadGLTFBinary("Resources/Models/FoxLounge/FoxLounge.glb", foxLoungeData);
        LinaGX::LoadGLTFBinary("Resources/Models/Fox/Fox.glb", foxData);
        /*
            We will hold skinned & non-skinned vertices seperately.
            So there will be 2 global vertex buffers in total, holding all data for all objects.
            Same for indices.
        */

        auto& pfd = m_pfd[0];

        auto createIndexBuffer = [&](std::vector<uint32>& indices, GPUBuffer& buf) {
            LinaGX::ResourceDesc indexBufferDesc = {
                .size          = sizeof(uint32) * indices.size(),
                .typeHintFlags = LinaGX::TH_IndexBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "IB Staging",
            };

            buf.staging               = m_lgx->CreateResource(indexBufferDesc);
            indexBufferDesc.heapType  = ResourceHeap::GPUOnly;
            indexBufferDesc.debugName = "IB GPU";
            buf.gpu                   = m_lgx->CreateResource(indexBufferDesc);

            // Map to CPU & copy.
            uint8* mapped = nullptr;
            m_lgx->MapResource(buf.staging, mapped);
            std::memcpy(mapped, indices.data(), indices.size() * sizeof(uint32));
            m_lgx->UnmapResource(buf.staging);
        };

        auto createVertexBuffer = [&](std::vector<VertexSkinned>& vertices, GPUBuffer& buf) {
            LinaGX::ResourceDesc vtxBufDesc = {
                .size          = sizeof(VertexSkinned) * vertices.size(),
                .typeHintFlags = LinaGX::TH_VertexBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "VB Staging",
            };

            buf.staging          = m_lgx->CreateResource(vtxBufDesc);
            vtxBufDesc.heapType  = ResourceHeap::GPUOnly;
            vtxBufDesc.debugName = "VB GPU";
            buf.gpu              = m_lgx->CreateResource(vtxBufDesc);

            // Map to CPU & copy.
            uint8* mapped = nullptr;
            m_lgx->MapResource(buf.staging, mapped);
            std::memcpy(mapped, vertices.data(), vertices.size() * sizeof(VertexSkinned));
            m_lgx->UnmapResource(buf.staging);
        };

        uint32 matIndex     = 0;
        auto   parseObjects = [&](const LinaGX::ModelData& modelData, bool manualDraw) {
            for (auto* modMat : modelData.allMaterials)
            {
                m_materials.push_back({});
                auto& mat               = m_materials.back();
                mat.name                = modMat->name;
                mat.gpuMat.alphaCutoff  = modMat->alphaCutoff;
                mat.gpuMat.metallic     = modMat->metallicFactor;
                mat.gpuMat.roughness    = modMat->roughnessFactor;
                mat.gpuMat.baseColorFac = glm::vec4(modMat->baseColor.x, modMat->baseColor.y, modMat->baseColor.z, modMat->baseColor.w);
                mat.gpuMat.emissiveFac  = glm::vec4(modMat->emissiveFactor.x, modMat->emissiveFactor.y, modMat->emissiveFactor.z, 0.0f);
            }

            for (auto* node : modelData.allNodes)
            {
                if (node->mesh == nullptr)
                    continue;

                m_worldObjects.push_back({});
                auto& wo        = m_worldObjects.back();
                wo.name         = node->name;
                wo.globalMatrix = Utility::CalculateGlobalMatrix(node);
                wo.isSkinned    = node->skin != nullptr;
                wo.hasMesh      = node->mesh != nullptr;
                wo.manualDraw   = manualDraw;

                if (wo.name.compare("fox") == 0)
                    wo.globalMatrix = glm::scale(wo.globalMatrix, glm::vec3(50.0f));

                if (!node->inverseBindMatrix.empty())
                    wo.invBindMatrix = glm::make_mat4(node->inverseBindMatrix.data());

                for (auto* p : node->mesh->primitives)
                {
                    // if (p->material != nullptr && p->material->name.compare("Terrain") == 0)
                    //     continue;

                    wo.primitives.push_back({});
                    MeshPrimitive& primitive = wo.primitives.back();

                    std::vector<VertexSkinned> vertices;
                    std::vector<uint32>        indices;
                    primitive.materialIndex = p->material ? (matIndex + p->material->index) : 0;

                    for (uint32 k = 0; k < p->vertexCount; k++)
                    {
                        vertices.push_back({});
                        VertexSkinned& vtx = vertices.back();
                        vtx.position       = p->positions[k];
                        vtx.uv             = p->texCoords[k];
                        vtx.normal         = p->normals[k];

                        // if (!p->weights.empty())
                        //     vtx.inBoneWeights = p->weights[k];
                        //
                        // if (!p->jointsui16.empty())
                        // {
                        //     vtx.inBoneIndices.x = static_cast<float>(p->jointsui16[k].x);
                        //     vtx.inBoneIndices.y = static_cast<float>(p->jointsui16[k].y);
                        //     vtx.inBoneIndices.z = static_cast<float>(p->jointsui16[k].z);
                        //     vtx.inBoneIndices.w = static_cast<float>(p->jointsui16[k].w);
                        // }
                        // else if (!p->jointsui8.empty())
                        // {
                        //     vtx.inBoneIndices.x = static_cast<float>(p->jointsui8[k].x);
                        //     vtx.inBoneIndices.y = static_cast<float>(p->jointsui8[k].y);
                        //     vtx.inBoneIndices.z = static_cast<float>(p->jointsui8[k].z);
                        //     vtx.inBoneIndices.w = static_cast<float>(p->jointsui8[k].w);
                        // }
                    }

                    if (p->indexType == LinaGX::IndexType::Uint16)
                    {
                        const uint32 sz = static_cast<uint32>(p->indices.size()) / sizeof(uint16);
                        uint16_t     temp;
                        for (size_t i = 0; i < p->indices.size(); i += 2)
                        {
                            std::memcpy(&temp, &p->indices[i], sizeof(uint16_t));
                            indices.push_back(static_cast<uint32>(temp));
                        }
                        primitive.indexCount += sz;
                    }
                    else
                    {
                        const uint32 sz       = static_cast<uint32>(p->indices.size()) / sizeof(uint32);
                        auto         origSize = indices.size();
                        indices.resize(origSize + sz);
                        std::memcpy(indices.data() + origSize, p->indices.data(), p->indices.size());
                        primitive.indexCount += sz;
                    }

                    createVertexBuffer(vertices, primitive.vtxBuffer);
                    createIndexBuffer(indices, primitive.indexBuffer);

                    LinaGX::CMDCopyResource* copy = pfd.transferStream->AddCommand<LinaGX::CMDCopyResource>();
                    copy->extension               = nullptr;
                    copy->destination             = primitive.indexBuffer.gpu;
                    copy->source                  = primitive.indexBuffer.staging;

                    LinaGX::CMDCopyResource* copySkinned = pfd.transferStream->AddCommand<LinaGX::CMDCopyResource>();
                    copySkinned->extension               = nullptr;
                    copySkinned->destination             = primitive.vtxBuffer.gpu;
                    copySkinned->source                  = primitive.vtxBuffer.staging;
                }
            }

            matIndex += static_cast<uint32>(modelData.allMaterials.size());
        };

        parseObjects(skyDomeData, true);
        // m_skyboxIndexCount = static_cast<uint32>(indices.size());
        parseObjects(foxLoungeData, false);
        parseObjects(foxData, false);
    }

    void Example::SetupTextures()
    {
        LOGT("Loading textures...");

        std::vector<std::string> texturePaths;

        // Fetch all texture paths from io.
        {
            const std::string basePathLounge = "Resources/Textures/";
            try
            {
                for (const auto& entry : std::filesystem::directory_iterator(basePathLounge))
                {
                    if (entry.is_regular_file())
                        texturePaths.push_back(entry.path().string());
                }
            }
            catch (std::filesystem::filesystem_error& e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        // Load textures in parallel.
        {
            m_textures.resize(texturePaths.size());
            ThreadPool pool;
            uint32     id = 0;
            for (const auto& path : texturePaths)
            {
                pool.AddTask([path, this, id]() { LoadTexture(path.c_str(), id); });
                id++;
            }
            pool.Run();
            pool.Join();
        }

        // Create gpu handles & record transfer commands.
        {
            const auto& linearTextures = Utility::GetLinearTextures();

            auto isLinear = [&](const char* path) -> bool {
                for (auto& linear : linearTextures)
                {
                    auto sid = LinaGX::LGX_ToSID(path);

                    if (sid == linear)
                        return true;
                }

                return false;
            };

            auto& pfd = m_pfd[0];

            for (auto& txt : m_textures)
            {
                LinaGX::Format format = LinaGX::Format::R8G8B8A8_SRGB;
                if (txt.allLevels[0].bytesPerPixel == 8)
                    format = LinaGX::Format::R16G16B16A16_UNORM;
                else if (isLinear(txt.path.c_str()))
                    format = LinaGX::Format::R8G8B8A8_UNORM;

                TextureDesc desc = {
                    .usage     = TextureUsage::ColorTexture,
                    .format    = format,
                    .sampled   = true,
                    .width     = txt.allLevels[0].width,
                    .height    = txt.allLevels[0].height,
                    .mipLevels = static_cast<uint32>(txt.allLevels.size()),
                    .debugName = txt.path.c_str(),
                };

                txt.gpuHandle = m_lgx->CreateTexture(desc);
            }

            // Transition all to transfer destination.
            {
                LinaGX::CMDBarrier* textureBarrier   = pfd.transferStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->extension            = nullptr;
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = static_cast<uint32>(m_textures.size());
                textureBarrier->textureBarriers      = pfd.transferStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * m_textures.size());

                uint32 index = 0;
                for (const auto& txt : m_textures)
                {
                    auto& barrier       = textureBarrier->textureBarriers[index];
                    barrier.texture     = txt.gpuHandle;
                    barrier.toState     = LinaGX::TextureBarrierState::TransferDestination;
                    barrier.isSwapchain = false;
                    index++;
                }
            }

            // Perform copy.
            for (auto& txt : m_textures)
            {
                LinaGX::CMDCopyBufferToTexture2D* copyTxt = pfd.transferStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
                copyTxt->extension                        = nullptr;
                copyTxt->destTexture                      = txt.gpuHandle;
                copyTxt->mipLevels                        = static_cast<uint32>(txt.allLevels.size()),
                copyTxt->buffers                          = pfd.transferStream->EmplaceAuxMemory<LinaGX::TextureBuffer>(txt.allLevels.data(), sizeof(LinaGX::TextureBuffer) * txt.allLevels.size());
                copyTxt->destinationSlice                 = 0;
            }

            // Transition all to shader read;
            {
                LinaGX::CMDBarrier* textureBarrier   = pfd.transferStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->extension            = nullptr;
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = static_cast<uint32>(m_textures.size());
                textureBarrier->textureBarriers      = pfd.transferStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * m_textures.size());

                uint32 index = 0;
                for (const auto& txt : m_textures)
                {
                    auto& barrier       = textureBarrier->textureBarriers[index];
                    barrier.texture     = txt.gpuHandle;
                    barrier.toState     = LinaGX::TextureBarrierState::ShaderRead;
                    barrier.isSwapchain = false;
                    index++;
                }
            }
        }
    }

    void Example::SetupSamplers()
    {
        LOGT("Setting up samplers...");

        LinaGX::SamplerDesc defaultSampler = {
            .minFilter  = Filter::Linear,
            .magFilter  = Filter::Linear,
            .mode       = SamplerAddressMode::Repeat,
            .mipmapMode = MipmapMode::Linear,
            .anisotropy = 0,
            .minLod     = 0.0f,
            .maxLod     = 12.0f,
            //.mipLodBias = 6.0f,
        };

        m_samplers.push_back(m_lgx->CreateSampler(defaultSampler));
    }

    void Example::SetupGlobalDescriptorSet()
    {
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd = m_pfd[i];

            // Descriptor Set 0 - Globals
            {
                pfd.globalSet = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionGlobal());

                LinaGX::DescriptorUpdateBufferDesc update = {
                    .setHandle = pfd.globalSet,
                    .binding   = 0,
                    .buffers   = {pfd.rscSceneData},
                };

                m_lgx->DescriptorUpdateBuffer(update);

                LinaGX::DescriptorUpdateBufferDesc update2 = {
                    .setHandle = pfd.globalSet,
                    .binding   = 1,
                    .buffers   = {pfd.rscObjDataGPU},
                };

                m_lgx->DescriptorUpdateBuffer(update2);

                LinaGX::DescriptorUpdateImageDesc smpUpdate = {
                    .setHandle = pfd.globalSet,
                    .binding   = 2,
                    .samplers  = {m_samplers[0]}};

                m_lgx->DescriptorUpdateImage(smpUpdate);
            }

            // Descriptor Set 1 - Camera Data.
            {
                pfd.cameraSet0 = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionCameraData());

                LinaGX::DescriptorUpdateBufferDesc update = {
                    .setHandle = pfd.cameraSet0,
                    .binding   = 0,
                    .buffers   = {pfd.rscCameraData0},
                };

                m_lgx->DescriptorUpdateBuffer(update);

                pfd.cameraSetCubemap = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionCameraData());

                LinaGX::DescriptorUpdateBufferDesc update2 = {
                    .setHandle = pfd.cameraSetCubemap,
                    .binding   = 0,
                    .buffers   = {pfd.rscCameraDataCubemap},
                    .ranges    = {sizeof(GPUCameraData)},
                };

                m_lgx->DescriptorUpdateBuffer(update2);
            }

            // Lighting pass set.
            {
                pfd.lightingPassMaterialSet  = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionLightingPass());
                pfd.lightingReflectionPassMaterialSet  = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionLightingPass());
                pfd.finalQuadPassMaterialSet = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionQuadPass());
            }
        }
    }

    void Example::SetupPipelineLayouts()
    {
        m_pipelineLayouts.resize(PipelineLayoutType::PL_Max);

        LinaGX::PipelineLayoutDesc pipelineLayoutSetGlobal = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };

        m_pipelineLayouts[PipelineLayoutType::PL_GlobalSet] = m_lgx->CreatePipelineLayout(pipelineLayoutSetGlobal);

        LinaGX::PipelineLayoutDesc pipelineLayoutGlobalCam = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionCameraData()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_GlobalAndCameraSet] = m_lgx->CreatePipelineLayout(pipelineLayoutGlobalCam);

        LinaGX::PipelineLayoutDesc pipelineLayoutDefaultObjects = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionCameraData(), Utility::GetSetDescriptionObjectMaterial()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_DefaultObjects] = m_lgx->CreatePipelineLayout(pipelineLayoutDefaultObjects);

        LinaGX::PipelineLayoutDesc pipelineLayoutLightingPass = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionCameraData(), Utility::GetSetDescriptionLightingPass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_LightingPass] = m_lgx->CreatePipelineLayout(pipelineLayoutLightingPass);

        LinaGX::PipelineLayoutDesc pipelineLayoutFinalQuadPass = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionCameraData(), Utility::GetSetDescriptionQuadPass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };

        m_pipelineLayouts[PipelineLayoutType::PL_FinalQuadPass] = m_lgx->CreatePipelineLayout(pipelineLayoutFinalQuadPass);
    }

    void Example::SetupShaders()
    {
        LOGT("Compiling shaders...");
        m_shaders.resize(Shader::SH_Max);
        m_shaders[Shader::SH_Default]      = Utility::CreateShader(m_lgx, "Resources/Shaders/default_pbr_vert.glsl", "Resources/Shaders/default_pbr_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_FLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, true, true, true, m_pipelineLayouts[PL_DefaultObjects], "PBR Default");
        m_shaders[Shader::SH_LightingPass] = Utility::CreateShader(m_lgx, "Resources/Shaders/lightpass_vert.glsl", "Resources/Shaders/lightpass_frag.glsl", LinaGX::CullMode::None, LinaGX::Format::R16G16B16A16_FLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, false, false, true, m_pipelineLayouts[PL_LightingPass], "Deferred Lighting");
        m_shaders[Shader::SH_Quad]         = Utility::CreateShader(m_lgx, "Resources/Shaders/screenquad_vert.glsl", "Resources/Shaders/screenquad_frag.glsl", LinaGX::CullMode::None, LinaGX::Format::B8G8R8A8_SRGB, CompareOp::Less, LinaGX::FrontFace::CCW, false, false, true, m_pipelineLayouts[PL_FinalQuadPass], "Final Quad");
        // m_shaders[Shader::Skybox]         = Utility::CreateShader(m_lgx, "Resources/Shaders/skybox_vert.glsl", "Resources/Shaders/skybox_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_FLOAT, CompareOp::LEqual, LinaGX::FrontFace::CCW, true, false, m_pipelineLayout);
    }

    void Example::SetupPasses()
    {
        m_passes.resize(PassType::PS_Max, {});

        for (auto& pass : m_passes)
        {
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
            }
        }
    }

    void Example::SetupGlobalResources()
    {
        LOGT("Setting up global resources...");

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd = m_pfd[i];

            LinaGX::ResourceDesc sceneDataResource = {
                .size          = sizeof(GPUSceneData),
                .typeHintFlags = LinaGX::TH_ConstantBuffer,
                .heapType      = LinaGX::ResourceHeap::StagingHeap,
                .debugName     = "Scene Data Buffer",
            };

            pfd.rscSceneData = m_lgx->CreateResource(sceneDataResource);
            m_lgx->MapResource(pfd.rscSceneData, pfd.rscSceneDataMapping);

            LinaGX::ResourceDesc objDataResource = {
                .size          = sizeof(GPUObjectData) * MAX_OBJECTS,
                .typeHintFlags = LinaGX::TH_StorageBuffer,
                .heapType      = LinaGX::ResourceHeap::StagingHeap,
                .debugName     = "Object Data Buffer CPU",
            };

            pfd.rscObjDataCPU         = m_lgx->CreateResource(objDataResource);
            objDataResource.heapType  = LinaGX::ResourceHeap::GPUOnly;
            objDataResource.debugName = "Object Data Buffer GPU";
            pfd.rscObjDataGPU         = m_lgx->CreateResource(objDataResource);
            m_lgx->MapResource(pfd.rscObjDataCPU, pfd.rscObjDataCPUMapping);

            LinaGX::ResourceDesc cameraDataResource = {
                .size          = sizeof(GPUCameraData),
                .typeHintFlags = LinaGX::TH_ConstantBuffer,
                .heapType      = LinaGX::ResourceHeap::StagingHeap,
                .debugName     = "Camera Data 1",
            };

            pfd.rscCameraData0 = m_lgx->CreateResource(cameraDataResource);
            m_lgx->MapResource(pfd.rscCameraData0, pfd.rscCameraDataMapping0);

            cameraDataResource.size *= 6;
            cameraDataResource.debugName = "Camera Data Cubemap";
            pfd.rscCameraDataCubemap     = m_lgx->CreateResource(cameraDataResource);
            m_lgx->MapResource(pfd.rscCameraDataCubemap, pfd.rscCameraDataMappingCubemap);

            LINAGX_VEC<GPUCameraData> camData;

            for (uint32 i = 0; i < 6; i++)
            {
                const glm::vec3 pos   = glm::vec3(-3.0f, -2.2f, -0.33f);
                glm::vec3       euler = glm::vec3(0, 0, 0);

                if (i == 0)
                    euler.y = DEG2RAD(-90.0f);
                else if (i == 1)
                    euler.y = DEG2RAD(90.0f);
                else if (i == 2)
                    euler.y = DEG2RAD(0.0f);
                else if (i == 3)
                    euler.y = DEG2RAD(-180.0f);
                else if (i == 4)
                    euler = glm::vec3(DEG2RAD(90.0f), 0.0f, 0.0f);
                else if (i == 5)
                    euler = glm::vec3(DEG2RAD(-90.0f), 0.0f, 0.0f);

                glm::mat4 rotationMatrix    = glm::mat4_cast(glm::inverse(glm::quat(euler)));
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pos);

                GPUCameraData cd = {
                    .view        = rotationMatrix * translationMatrix,
                    .proj        = glm::perspective(DEG2RAD(90.0f), static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y), 0.01f, 1000.0f),
                    .camPosition = glm::vec4(pos.x, pos.y, pos.z, 0),
                };

                camData.push_back(cd);
            }

            std::memcpy(pfd.rscCameraDataMappingCubemap, camData.data(), sizeof(GPUCameraData) * 6);
        }
    }

    void Example::CreateDynamicTextures()
    {
        LOGT("Setting up passes...");

        LinaGX::TextureDesc defaultRTDesc = {
            .usage   = LinaGX::TextureUsage::ColorTextureRenderTarget,
            .format  = LinaGX::Format::R16G16B16A16_FLOAT,
            .sampled = true,
            .width   = m_window->GetSize().x,
            .height  = m_window->GetSize().y,
        };
        
        LinaGX::TextureDesc defaultReflectionsRTDesc = {
            .usage   = LinaGX::TextureUsage::ColorTextureRenderTarget,
            .format  = LinaGX::Format::R16G16B16A16_FLOAT,
            .sampled = true,
            .width   = REFLECTION_PASS_RES,
            .height  = REFLECTION_PASS_RES,
        };

        LinaGX::TextureDesc reflectionRTDesc = {
            .usage   = LinaGX::TextureUsage::ColorTextureRenderTarget,
            .format  = LinaGX::Format::R16G16B16A16_FLOAT,
            .sampled = true,
            .width   = 512,
            .height  = 512,
        };

        LinaGX::TextureDesc descDepth = {
            .usage              = LinaGX::TextureUsage::DepthStencilTexture,
            .format             = LinaGX::Format::D32_SFLOAT,
            .depthStencilAspect = LinaGX::DepthStencilAspect::DepthOnly,
            .width              = m_window->GetSize().x,
            .height             = m_window->GetSize().y,
            .debugName          = "RT Depth Texture",
        };
        
        LinaGX::TextureDesc descDepthReflections = {
            .usage              = LinaGX::TextureUsage::DepthStencilTexture,
            .format             = LinaGX::Format::D32_SFLOAT,
            .depthStencilAspect = LinaGX::DepthStencilAspect::DepthOnly,
            .width              = REFLECTION_PASS_RES,
            .height             = REFLECTION_PASS_RES,
            .debugName          = "RT Depth Texture",
        };

        LinaGX::TextureDesc reflectionsCube = {
            .usage       = LinaGX::TextureUsage::ColorTextureRenderTarget,
            .type        = LinaGX::TextureType::Texture2D,
            .format      = LinaGX::Format::R16G16B16A16_FLOAT,
            .sampled     = true,
            .isCubemap   = true,
            .width       = REFLECTION_PASS_RES,
            .height      = REFLECTION_PASS_RES,
            .arrayLength = 6,
            .viewCount   = 6,
        };

        LinaGX::RenderPassColorAttachment colorAttachment;
        colorAttachment.clearColor = {0.3f, 0.1f, 0.1f, 1.0f};

        LinaGX::RenderPassDepthStencilAttachment depthStencilAttachment;
        depthStencilAttachment.depthLoadOp  = LinaGX::LoadOp::Clear;
        depthStencilAttachment.depthStoreOp = LinaGX::StoreOp::Store;
        depthStencilAttachment.clearDepth   = 1.0f;
        depthStencilAttachment.useStencil   = false;

        LinaGX::DescriptorUpdateImageDesc updateImage = {};
        updateImage.binding                           = 1;

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            colorAttachment.loadOp  = LinaGX::LoadOp::Clear;
            colorAttachment.storeOp = LinaGX::StoreOp::Store;
            colorAttachment.isSwapchain = false;

            // Objs default
            {
                auto& objsDefault = m_passes[PassType::PS_ObjectsDefault];
                objsDefault.renderTargets[i].colorAttachments.clear();

                // GBuf Albedo Roughness
                defaultRTDesc.debugName     = "GBuf Albedo";
                colorAttachment.texture     = m_lgx->CreateTexture(defaultRTDesc);
                objsDefault.renderTargets[i].colorAttachments.push_back(colorAttachment);

                // GBuf NormalMetallic
                defaultRTDesc.debugName = "GBuf Normal";
                colorAttachment.texture = m_lgx->CreateTexture(defaultRTDesc);
                objsDefault.renderTargets[i].colorAttachments.push_back(colorAttachment);

                // GBuf PositionAO
                defaultRTDesc.debugName = "GBuf Position";
                colorAttachment.texture = m_lgx->CreateTexture(defaultRTDesc);
                objsDefault.renderTargets[i].colorAttachments.push_back(colorAttachment);

                // GBuf depth stencil.
                defaultRTDesc.debugName                             = "Depth";
                depthStencilAttachment.useDepth                     = true;
                depthStencilAttachment.texture                      = m_lgx->CreateTexture(descDepth);
                objsDefault.renderTargets[i].depthStencilAttachment = depthStencilAttachment;

                // Lighting pass descriptor set will use above 3 images.
                LinaGX::DescriptorUpdateImageDesc imgUpdateLightingPass = {
                    .setHandle = m_pfd[i].lightingPassMaterialSet,
                    .binding   = 0,
                    .textures  = {objsDefault.renderTargets[i].colorAttachments[0].texture, objsDefault.renderTargets[i].colorAttachments[1].texture, objsDefault.renderTargets[i].colorAttachments[2].texture},
                };
                
                m_lgx->DescriptorUpdateImage(imgUpdateLightingPass);
            }
            
            // Objs reflection
            {
                auto& objsReflection = m_passes[PassType::PS_ObjectsReflection];
                objsReflection.renderTargets[i].colorAttachments.clear();

                // GBuf Albedo Roughness
                defaultRTDesc.debugName     = "GBuf Albedo";
                colorAttachment.texture     = m_lgx->CreateTexture(defaultReflectionsRTDesc);
                objsReflection.renderTargets[i].colorAttachments.push_back(colorAttachment);

                // GBuf NormalMetallic
                defaultRTDesc.debugName = "GBuf Normal";
                colorAttachment.texture = m_lgx->CreateTexture(defaultReflectionsRTDesc);
                objsReflection.renderTargets[i].colorAttachments.push_back(colorAttachment);

                // GBuf PositionAO
                defaultRTDesc.debugName = "GBuf Position";
                colorAttachment.texture = m_lgx->CreateTexture(defaultReflectionsRTDesc);
                objsReflection.renderTargets[i].colorAttachments.push_back(colorAttachment);

                // GBuf depth stencil.
                defaultRTDesc.debugName                             = "Depth";
                depthStencilAttachment.useDepth                     = true;
                depthStencilAttachment.texture                      = m_lgx->CreateTexture(descDepthReflections);
                objsReflection.renderTargets[i].depthStencilAttachment = depthStencilAttachment;

                // Lighting pass descriptor set will use above 3 images.
                LinaGX::DescriptorUpdateImageDesc imgUpdateLightingPass = {
                    .setHandle = m_pfd[i].lightingReflectionPassMaterialSet,
                    .binding   = 0,
                    .textures  = {objsReflection.renderTargets[i].colorAttachments[0].texture, objsReflection.renderTargets[i].colorAttachments[1].texture, objsReflection.renderTargets[i].colorAttachments[2].texture},
                };
                
                m_lgx->DescriptorUpdateImage(imgUpdateLightingPass);
            }
            

            // Lighting pass
            {
                auto& lightingDefault = m_passes[PassType::PS_Lighting];
                lightingDefault.renderTargets[i].colorAttachments.clear();

                defaultRTDesc.debugName     = "Lighting Pass Output";
                colorAttachment.texture     = m_lgx->CreateTexture(defaultRTDesc);
                lightingDefault.renderTargets[i].colorAttachments.push_back(colorAttachment);

                depthStencilAttachment.useDepth                         = false;
                lightingDefault.renderTargets[i].depthStencilAttachment = depthStencilAttachment;
                
                LinaGX::DescriptorUpdateImageDesc imgUpdateFinalQuadPass = {
                    .setHandle = m_pfd[i].finalQuadPassMaterialSet,
                    .binding   = 0,
                    .textures  = {lightingDefault.renderTargets[i].colorAttachments[0].texture},
                };

                m_lgx->DescriptorUpdateImage(imgUpdateFinalQuadPass);
            }

            // Lighting reflections pass
            {
                auto& reflections = m_passes[PassType::PS_LightingReflections];
                reflections.renderTargets[i].colorAttachments.clear();

                reflectionsCube.debugName   = "Reflection Cube";
                colorAttachment.texture     = m_lgx->CreateTexture(reflectionsCube);
                // colorAttachment.loadOp      = LoadOp::Load;
                reflections.renderTargets[i].colorAttachments.push_back(colorAttachment);
                depthStencilAttachment.useDepth                     = false;
                reflections.renderTargets[i].depthStencilAttachment = depthStencilAttachment;
            }

            auto& finalQuad = m_passes[PassType::PS_FinalQuad];
            finalQuad.renderTargets[i].colorAttachments.clear();

            colorAttachment.isSwapchain = true;
            colorAttachment.texture     = static_cast<uint32>(m_swapchain);
            finalQuad.renderTargets[i].colorAttachments.push_back(colorAttachment);

            depthStencilAttachment.useDepth                   = false;
            finalQuad.renderTargets[i].depthStencilAttachment = depthStencilAttachment;

        }
    }

    void Example::DestroyDynamicTextures()
    {
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            for (auto& pass : m_passes)
            {
                const auto& rt = pass.renderTargets[i];

                for (auto& att : rt.colorAttachments)
                {
                    if (!att.isSwapchain)
                        m_lgx->DestroyTexture(att.texture);
                }

                if (rt.depthStencilAttachment.useDepth)
                    m_lgx->DestroyTexture(rt.depthStencilAttachment.texture);
            }
        }
    }

    void Example::BeginPass(PassType pass, uint32 width, uint32 height, uint32 layer)
    {
        if (width == 0)
            width = m_window->GetSize().x;

        if (height == 0)
            height = m_window->GetSize().y;

        const uint32       currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        const auto&        currentFrame      = m_pfd[currentFrameIndex];
        const Viewport     viewport          = {.x = 0, .y = 0, .width = width, .height = height, .minDepth = 0.0f, .maxDepth = 1.0f};
        const ScissorsRect sc                = {.x = 0, .y = 0, .width = width, .height = height};
        auto&              passData          = m_passes[pass];

        // Pass Begin
        {
            auto& rt = passData.renderTargets[currentFrameIndex];

            LinaGX::CMDBeginRenderPass* beginRenderPass = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBeginRenderPass>();
            beginRenderPass->extension                  = nullptr;
            beginRenderPass->colorAttachmentCount       = static_cast<uint32>(rt.colorAttachments.size());
            beginRenderPass->colorAttachments           = currentFrame.graphicsStream->EmplaceAuxMemory<LinaGX::RenderPassColorAttachment>(rt.colorAttachments.data(), sizeof(LinaGX::RenderPassColorAttachment) * rt.colorAttachments.size());
            beginRenderPass->depthStencilAttachment     = rt.depthStencilAttachment;
            beginRenderPass->viewport                   = viewport;
            beginRenderPass->scissors                   = sc;

            for (uint32 i = 0; i < beginRenderPass->colorAttachmentCount; i++)
            {
                beginRenderPass->colorAttachments[i].layer = layer;
                beginRenderPass->depthStencilAttachment.layer = 0;
            }
        }
    }

    void Example::EndPass()
    {
        auto&                     currentFrame = m_pfd[m_lgx->GetCurrentFrameIndex()];
        LinaGX::CMDEndRenderPass* end          = currentFrame.graphicsStream->AddCommand<LinaGX::CMDEndRenderPass>();
    }

    void Example::Initialize()
    {
        App::Initialize();

        // Per frame data, we'll use 0 for our init operations.
        auto& pfd = m_pfd[0];

        // Config init.
        {
            ConfigureInitializeLinaGX();
            CreateMainWindow();
        }

        // Resource init.
        {
            SetupStreams();
            LoadAndParseModels();
            SetupTextures();
            SetupMaterials();
            SetupSamplers();
            SetupGlobalResources();
            SetupGlobalDescriptorSet();
            SetupPipelineLayouts();
            SetupShaders();
            SetupPasses();
            CreateDynamicTextures();
        }

        // Above operations will record bunch of transfer commands.
        // Start the work on the gpu while we continue on our init code.
        {
            m_lgx->CloseCommandStreams(&pfd.transferStream, 1);
            pfd.transferSemaphoreValue++;
            m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &pfd.transferStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &pfd.transferSemaphore, .signalValues = &pfd.transferSemaphoreValue});
        }

        LOGT("Waiting for init transfers...");

        // Make sure we wait for all gpu ops before proceeding on the CPU.
        // Then remove some resources we are done with & continue the biz.
        {
            m_lgx->WaitForUserSemaphore(pfd.transferSemaphore, pfd.transferSemaphoreValue);

            for (auto& txt : m_textures)
            {
                for (auto& level : txt.allLevels)
                    LinaGX::FreeImage(level.pixels);
            }

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                for (auto& mat : m_materials)
                {
                    m_lgx->DestroyResource(mat.stagingResources[i]);
                }
            }

            // Also these are safe to release.
            for (const auto& obj : m_worldObjects)
            {
                for (const auto& p : obj.primitives)
                {
                    m_lgx->DestroyResource(p.indexBuffer.staging);
                    m_lgx->DestroyResource(p.vtxBuffer.staging);
                }
            }
        }
        m_camera.Initialize(m_lgx);
        EASY_PROFILER_ENABLE;
        EASY_MAIN_THREAD;

        LOGT("All init OK! Starting frames...");
    }

    void Example::Shutdown()
    {
        profiler::dumpBlocksToFile("test_profile.prof");
        m_lgx->Join();

        // First get rid of the window.
        m_lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        for (auto s : m_shaders)
            m_lgx->DestroyShader(s);

        m_lgx->DestroySwapchain(m_swapchain);

        for (const auto& obj : m_worldObjects)
        {
            for (const auto& p : obj.primitives)
            {
                m_lgx->DestroyResource(p.indexBuffer.gpu);
                m_lgx->DestroyResource(p.vtxBuffer.gpu);
            }
        }

        for (auto smp : m_samplers)
            m_lgx->DestroySampler(smp);

        for (const auto& t2d : m_textures)
            m_lgx->DestroyTexture(t2d.gpuHandle);

        DestroyDynamicTextures();

        for (auto lyt : m_pipelineLayouts)
            m_lgx->DestroyPipelineLayout(lyt);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd = m_pfd[i];
            m_lgx->DestroyCommandStream(pfd.graphicsStream);
            m_lgx->DestroyCommandStream(pfd.transferStream);
            m_lgx->DestroyUserSemaphore(pfd.transferSemaphore);
            m_lgx->DestroyResource(pfd.rscSceneData);
            m_lgx->DestroyDescriptorSet(pfd.globalSet);
            m_lgx->DestroyDescriptorSet(pfd.cameraSet0);
            m_lgx->DestroyDescriptorSet(pfd.cameraSetCubemap);
            m_lgx->DestroyDescriptorSet(pfd.lightingPassMaterialSet);
            m_lgx->DestroyDescriptorSet(pfd.lightingReflectionPassMaterialSet);
            m_lgx->DestroyDescriptorSet(pfd.finalQuadPassMaterialSet);
            m_lgx->DestroyResource(pfd.rscObjDataCPU);
            m_lgx->DestroyResource(pfd.rscObjDataGPU);
            m_lgx->DestroyResource(pfd.rscCameraData0);
            m_lgx->DestroyResource(pfd.rscCameraDataCubemap);

            for (auto& mat : m_materials)
            {
                m_lgx->DestroyResource(mat.gpuResources[i]);
                m_lgx->DestroyDescriptorSet(mat.descriptorSets[i]);
            }

            for (const auto& pass : m_passes)
            {
            }
        }

        // Terminate renderer & shutdown app.
        delete m_lgx;
        App::Shutdown();
    }

    void Example::CaptureCubemap()
    {
    }

    void Example::DrawObjects(Shader shader)
    {
        BindShader(shader);

        const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&        currentFrame      = m_pfd[currentFrameIndex];

        struct PrimData
        {
            MeshPrimitive* prim     = nullptr;
            uint32         objIndex = 0;
        };

        auto draw = [&]() {
            std::unordered_map<uint32, std::vector<PrimData>> materialPrimitiveBatch;

            uint32 objIndex = 0;
            for (auto& obj : m_worldObjects)
            {
                if (obj.manualDraw)
                    continue;

                for (auto& prim : obj.primitives)
                {
                    auto& map = materialPrimitiveBatch[prim.materialIndex];
                    map.push_back({&prim, objIndex});
                }

                objIndex++;
            }

            for (const auto& [matIndex, prims] : materialPrimitiveBatch)
            {
                const auto& mat = m_materials[matIndex];

                // Bind material descriptor.
                {
                    LinaGX::CMDBindDescriptorSets* bind = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
                    bind->extension                     = nullptr;
                    bind->isCompute                     = false;
                    bind->firstSet                      = 2;
                    bind->setCount                      = 1;
                    bind->dynamicOffsetCount            = 0;
                    bind->descriptorSetHandles          = currentFrame.graphicsStream->EmplaceAuxMemory<uint16>(mat.descriptorSets[currentFrameIndex]);
                    bind->layoutSource                  = DescriptorSetsLayoutSource::LastBoundShader;
                }

                for (const auto& primData : prims)
                {
                    GPUConstants constants = {};
                    constants.int1         = primData.objIndex;
                    constants.int2         = 0;

                    LinaGX::CMDBindConstants* ct = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindConstants>();
                    ct->extension                = nullptr;
                    ct->offset                   = 0;
                    ct->size                     = sizeof(GPUConstants);
                    ct->stages                   = currentFrame.graphicsStream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment);
                    ct->stagesSize               = 2;
                    ct->data                     = currentFrame.graphicsStream->EmplaceAuxMemory<GPUConstants>(constants);

                    // Global index buffer.
                    {
                        LinaGX::CMDBindIndexBuffers* index = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindIndexBuffers>();
                        index->extension                   = nullptr;
                        index->indexType                   = LinaGX::IndexType::Uint32;
                        index->offset                      = 0;
                        index->resource                    = primData.prim->indexBuffer.gpu;
                    }

                    // Global vertex buffer.
                    {
                        LinaGX::CMDBindVertexBuffers* vtx = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindVertexBuffers>();
                        vtx->extension                    = nullptr;
                        vtx->offset                       = 0;
                        vtx->slot                         = 0;
                        vtx->vertexSize                   = sizeof(VertexSkinned);
                        vtx->resource                     = primData.prim->vtxBuffer.gpu;
                    }

                    LinaGX::CMDDrawIndexedInstanced* draw = currentFrame.graphicsStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
                    draw->extension                       = nullptr;
                    draw->instanceCount                   = 1;
                    draw->startInstanceLocation           = 0;
                    draw->startIndexLocation              = 0;
                    draw->indexCountPerInstance           = primData.prim->indexCount;
                    draw->baseVertexLocation              = 0;
                }
            }
        };

        draw();

        // DrawSkybox();
    }

    void Example::DrawSkybox()
    {
        // auto& pfd = m_pfd[m_lgx->GetCurrentFrameIndex()];
        //
        // BindShader(Shader::SH_Skybox);
        //
        // LinaGX::CMDDrawIndexedInstanced* draw = pfd.graphicsStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
        // draw->extension                       = nullptr;
        // draw->baseVertexLocation              = 0;
        // draw->startIndexLocation              = 0;
        // draw->startInstanceLocation           = 0;
        // draw->instanceCount                   = 1;
        // draw->indexCountPerInstance           = m_skyboxIndexCount;
    }

    void Example::BindMaterialSet(uint16 materialSet)
    {
        auto& pfd = m_pfd[m_lgx->GetCurrentFrameIndex()];

        LinaGX::CMDBindDescriptorSets* bind = pfd.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
        bind->extension                     = nullptr;
        bind->isCompute                     = false;
        bind->firstSet                      = 2;
        bind->setCount                      = 1;
        bind->dynamicOffsetCount            = 0;
        bind->descriptorSetHandles          = pfd.graphicsStream->EmplaceAuxMemory<uint16>(materialSet);
        bind->layoutSource                  = DescriptorSetsLayoutSource::LastBoundShader;
    }

    void Example::DrawFullscreenQuad()
    {
        auto& pfd = m_pfd[m_lgx->GetCurrentFrameIndex()];

        CMDDrawInstanced* draw       = pfd.graphicsStream->AddCommand<CMDDrawInstanced>();
        draw->instanceCount          = 1;
        draw->startInstanceLocation  = 0;
        draw->startVertexLocation    = 0;
        draw->vertexCountPerInstance = 6;
    }

    void Example::BindShader(uint32 target)
    {
        if (static_cast<uint32>(m_boundShader) == target)
            return;

        auto&                    pfd      = m_pfd[m_lgx->GetCurrentFrameIndex()];
        LinaGX::CMDBindPipeline* pipeline = pfd.graphicsStream->AddCommand<LinaGX::CMDBindPipeline>();
        pipeline->extension               = nullptr;
        pipeline->shader                  = m_shaders[target];

        m_boundShader = static_cast<int32>(target);
    }

    void Example::BindCameraDescriptor(uint16 set, uint32 offset)
    {
        const uint32                   currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&                          pfd               = m_pfd[currentFrameIndex];
        LinaGX::CMDBindDescriptorSets* bind              = pfd.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
        bind->extension                                  = nullptr;
        bind->isCompute                                  = false;
        bind->firstSet                                   = 1;
        bind->setCount                                   = 1;
        bind->dynamicOffsetCount                         = 1;
        bind->dynamicOffsets                             = pfd.graphicsStream->EmplaceAuxMemory<uint32>(offset);
        bind->descriptorSetHandles                       = pfd.graphicsStream->EmplaceAuxMemory<uint16>(set);
        bind->layoutSource                               = DescriptorSetsLayoutSource::CustomLayout;
        bind->customLayout                               = m_pipelineLayouts[PipelineLayoutType::PL_GlobalAndCameraSet];
    }

    void Example::ReflectionPass()
    {
        const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&        pfd               = m_pfd[currentFrameIndex];

        for (uint32 i = 0; i < 6; i++)
        {
            BindCameraDescriptor(pfd.cameraSetCubemap, sizeof(GPUCameraData) * i);

            BeginPass(PassType::PS_ObjectsReflection, REFLECTION_PASS_RES, REFLECTION_PASS_RES);
            DrawObjects(Shader::SH_Default);
            EndPass();

            CollectPassBarrier(PS_ObjectsReflection, LinaGX::TextureBarrierState::ShaderRead);
            ExecPassBarriers();

            BeginPass(PassType::PS_LightingReflections, REFLECTION_PASS_RES, REFLECTION_PASS_RES, i);
            BindShader(Shader::SH_LightingPass);
            BindMaterialSet(pfd.lightingReflectionPassMaterialSet);
            DrawFullscreenQuad();
            EndPass();

            CollectPassBarrier(PS_ObjectsReflection, LinaGX::TextureBarrierState::ColorAttachment);
            ExecPassBarriers();
        }

        CollectPassBarrier(PS_LightingReflections, LinaGX::TextureBarrierState::ShaderRead);
        ExecPassBarriers();
    }

    void Example::CollectPassBarrier(PassType pass, LinaGX::TextureBarrierState target)
    {
        const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&        pfd               = m_pfd[currentFrameIndex];

        for (const auto& att : m_passes[pass].renderTargets[currentFrameIndex].colorAttachments)
        {
            LinaGX::TextureBarrier barrier = {};
            barrier.toState                = target;
            barrier.texture                = att.texture;
            barrier.isSwapchain            = pass == PassType::PS_FinalQuad;
            m_passBarriers.push_back(barrier);
        }
    }

    void Example::ExecPassBarriers()
    {
        const uint32        currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&               pfd               = m_pfd[currentFrameIndex];
        LinaGX::CMDBarrier* barrier           = pfd.graphicsStream->AddCommand<LinaGX::CMDBarrier>();
        barrier->extension                    = nullptr;
        barrier->resourceBarrierCount         = 0;
        barrier->textureBarrierCount          = static_cast<uint32>(m_passBarriers.size());
        barrier->textureBarriers              = pfd.graphicsStream->EmplaceAuxMemory<LinaGX::TextureBarrier>(m_passBarriers.data(), sizeof(LinaGX::TextureBarrier) * m_passBarriers.size());
        m_passBarriers.clear();
    }

    void Example::BuildCommands(uint32 frameIndex)
    {
        auto& currentFrame = m_pfd[frameIndex];

        // Global descriptor binding.
        {
            LinaGX::CMDBindDescriptorSets* bind = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
            bind->extension                     = nullptr;
            bind->isCompute                     = false;
            bind->firstSet                      = 0;
            bind->setCount                      = 1;
            bind->dynamicOffsetCount            = 0;
            bind->descriptorSetHandles          = currentFrame.graphicsStream->EmplaceAuxMemory<uint16>(currentFrame.globalSet);
            bind->layoutSource                  = DescriptorSetsLayoutSource::CustomLayout;
            bind->customLayout                  = m_pipelineLayouts[PipelineLayoutType::PL_GlobalSet];
        }

        CollectPassBarrier(PS_Lighting, LinaGX::TextureBarrierState::ColorAttachment);
        CollectPassBarrier(PS_ObjectsDefault, LinaGX::TextureBarrierState::ColorAttachment);
        CollectPassBarrier(PS_ObjectsReflection, LinaGX::TextureBarrierState::ColorAttachment);
        CollectPassBarrier(PS_FinalQuad, LinaGX::TextureBarrierState::ColorAttachment);
        CollectPassBarrier(PS_LightingReflections, LinaGX::TextureBarrierState::ColorAttachment);
        ExecPassBarriers();

        ReflectionPass();

        BindCameraDescriptor(currentFrame.cameraSet0, 0);

        BeginPass(PassType::PS_ObjectsDefault);
        DrawObjects(Shader::SH_Default);
        EndPass();

        CollectPassBarrier(PS_ObjectsDefault, LinaGX::TextureBarrierState::ShaderRead);
        ExecPassBarriers();

        BeginPass(PassType::PS_Lighting);
        BindShader(Shader::SH_LightingPass);
        BindMaterialSet(currentFrame.lightingPassMaterialSet);
        DrawFullscreenQuad();
        EndPass();

        CollectPassBarrier(PS_Lighting, LinaGX::TextureBarrierState::ShaderRead);
        ExecPassBarriers();

        BeginPass(PassType::PS_FinalQuad);
        BindShader(Shader::SH_Quad);
        BindMaterialSet(currentFrame.finalQuadPassMaterialSet);
        DrawFullscreenQuad();
        EndPass();

        CollectPassBarrier(PS_FinalQuad, LinaGX::TextureBarrierState::Present);
        ExecPassBarriers();

        EASY_END_BLOCK;

        EASY_BLOCK("LinaGX Submit Graphics");

        // This does the actual *recording* of every single command stream alive.
        m_lgx->CloseCommandStreams(&currentFrame.graphicsStream, 1);
    }

    void Example::OnTick()
    {
        auto now = std::chrono::high_resolution_clock::now();
        EASY_FUNCTION(profiler::colors::Red);

        m_boundShader = -1;
        m_camera.Tick(m_deltaSeconds);

        // Check for window inputs.
        EASY_BLOCK("Polling Input");
        m_lgx->PollWindowsAndInput();

        bool dbgUpdate = false;

        uint32 dbgUpdateTxt[2] = {0};
        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_1))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_Lighting].renderTargets[0].colorAttachments[0].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_Lighting].renderTargets[1].colorAttachments[0].texture;
            dbgUpdate       = true;
        }
        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_2))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_ObjectsDefault].renderTargets[0].colorAttachments[0].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_ObjectsDefault].renderTargets[1].colorAttachments[0].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_3))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_ObjectsDefault].renderTargets[0].colorAttachments[1].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_ObjectsDefault].renderTargets[1].colorAttachments[1].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_4))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_ObjectsDefault].renderTargets[0].colorAttachments[2].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_ObjectsDefault].renderTargets[1].colorAttachments[2].texture;
            dbgUpdate       = true;
        }
        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_5))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_ObjectsDefault].renderTargets[0].colorAttachments[3].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_ObjectsDefault].renderTargets[1].colorAttachments[3].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_6))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_ObjectsDefault].renderTargets[0].depthStencilAttachment.texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_ObjectsDefault].renderTargets[1].depthStencilAttachment.texture;
            dbgUpdate       = true;
        }

        if (dbgUpdate)
        {
            m_lgx->Join();

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                LinaGX::DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle = m_pfd[i].finalQuadPassMaterialSet,
                    .binding   = 0,
                    .textures  = {dbgUpdateTxt[i]},
                };

                m_lgx->DescriptorUpdateImage(imgUpdate);
            }
        }

        EASY_END_BLOCK;

        // Let LinaGX know we are starting a new frame.
        EASY_BLOCK("LinaGX Start Frame");
        m_lgx->StartFrame();
        EASY_END_BLOCK;

        EASY_BLOCK("Record Rest of the Frame");

        const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&        currentFrame      = m_pfd[currentFrameIndex];

        // Scene data.
        {
            GPUSceneData sceneData = {
                .skyColor1  = glm::vec4(0.002f, 0.137f, 0.004f, 1.0f),
                .skyColor2  = glm::vec4(0.0f, 0.156f, 0.278f, 1.0f),
                .lightPos   = glm::vec4(1.5f, 2.0f, 0.0f, 0.0f),
                .lightColor = glm::vec4(1.0f, 0.684244f, 0.240f, 0.0f) * 20.0f,
            };

            std::memcpy(currentFrame.rscSceneDataMapping, &sceneData, sizeof(GPUSceneData));
        }

        // Camera data.
        {
            const auto& camPos = m_camera.GetPosition();

            GPUCameraData camData = {
                .view        = m_camera.GetView(),
                .proj        = m_camera.GetProj(),
                .camPosition = m_camera.GetPosition(),
            };

            std::memcpy(currentFrame.rscCameraDataMapping0, &camData, sizeof(GPUCameraData));
        }

        // Obj data.
        {
            std::vector<GPUObjectData> objData;
            for (const auto& obj : m_worldObjects)
            {
                if (obj.manualDraw)
                    continue;

                GPUObjectData data = {};
                glm::mat4     mat  = glm::mat4(1.0f);
                glm::quat     q    = glm::quat(glm::vec3(0.0f, DEG2RAD(0.0f), 0.0f));
                mat                = Utility::TranslateRotateScale({0.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {10.0f, 10.0f, 10.0f});
                data.modelMatrix   = mat;
                data.modelMatrix   = obj.globalMatrix;
                data.normalMatrix  = glm::transpose(glm::inverse(glm::mat3(data.modelMatrix)));
                data.hasSkin       = obj.isSkinned;
                objData.push_back(data);
            }

            std::memcpy(currentFrame.rscObjDataCPUMapping, objData.data(), sizeof(GPUObjectData) * objData.size());

            LinaGX::CMDCopyResource* copy = currentFrame.transferStream->AddCommand<LinaGX::CMDCopyResource>();
            copy->destination             = currentFrame.rscObjDataGPU;
            copy->source                  = currentFrame.rscObjDataCPU;
            copy->extension               = nullptr;
        }

        // Send transfers.
        {
            currentFrame.transferSemaphoreValue++;
            LinaGX::SubmitDesc transferSubmit = {
                .targetQueue      = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
                .streams          = &currentFrame.transferStream,
                .streamCount      = 1,
                .useWait          = false,
                .useSignal        = true,
                .signalCount      = 1,
                .signalSemaphores = &currentFrame.transferSemaphore,
                .signalValues     = &currentFrame.transferSemaphoreValue,
            };

            EASY_BLOCK("LinaGX Submit Frame Transfers");
            m_lgx->CloseCommandStreams(&currentFrame.transferStream, 1);
            m_lgx->SubmitCommandStreams(transferSubmit);
            EASY_END_BLOCK;
        }

        BuildCommands(currentFrameIndex);

        // Submit work on gpu.
        m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &currentFrame.graphicsStream, .streamCount = 1, .useWait = true, .waitCount = 1, .waitSemaphores = &currentFrame.transferSemaphore, .waitValues = &currentFrame.transferSemaphoreValue});

        EASY_END_BLOCK;

        EASY_BLOCK("LinaGX Present");

        // Present main swapchain.
        m_lgx->Present({.swapchains = &m_swapchain, .swapchainCount = 1});

        EASY_END_BLOCK;

        EASY_BLOCK("LinaGX End Frame");

        // Let LinaGX know we are finalizing this frame.
        m_lgx->EndFrame();

        EASY_END_BLOCK;
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - now);
    }

} // namespace LinaGX::Examples
