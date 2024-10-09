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

Example: FoxLounge

A complete example demonstrating a simple renderer workflow with:

- Everything from previous examples.
- Deferred PBR rendering.
- Point lights.
- Shadows.
- Realtime generated IBL cubemap with Irradiance, prefilter and BRDF steps.
- SSAO.
- Simple post-processing.
- Using Input API for camera.
- And possibly many more tiny little stuff.

This example is the most comprehensive one, and normally it's not sensible to do such a rendering pipeline with just couple of classes :)
But for the sake of keeping everythint together, here it is. It's main purpose is to show that LinaGX's abstraction is capable enough to pull a scene like this
under couple thousand LOC, which could very well be put down to hundreds with a proper architecture.

*/

#include "App.hpp"
#include "FoxLounge.hpp"
#include "ThreadPool.hpp"
#include "LinaGX/Common/CommonData.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include <iostream>
#include <cstdarg>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <chrono>
#include <random>

namespace LinaGX::Examples
{

#define MAX_OBJECTS          10
#define MAX_LIGHTS           10
#define MAX_MATS             10
#define REFLECTION_PASS_RES  1024
#define ENVIRONMENT_MAP_RES  1024
#define IRRADIANCE_MAP_RES   32
#define PREFILTER_MAP_RES    128
#define PREFILTER_MIP_LEVELS 5
#define BRDF_MAP_RES         512
#define SHADOW_MAP_RES       512

    void Example::ConfigureInitializeLinaGX()
    {
        BackendAPI api = BackendAPI::DX12;

#ifdef LINAGX_PLATFORM_APPLE
        api = BackendAPI::Metal;
#endif

        LinaGX::Config.api                                    = api;
        LinaGX::Config.gpu                                    = PreferredGPUType::Discrete;
        LinaGX::Config.framesInFlight                         = FRAMES_IN_FLIGHT;
        LinaGX::Config.backbufferCount                        = BACKBUFFER_COUNT;
        LinaGX::Config.gpuLimits                              = {};
        LinaGX::Config.logLevel                               = LogLevel::Verbose;
        LinaGX::Config.errorCallback                          = LogError;
        LinaGX::Config.infoCallback                           = LogInfo;
        LinaGX::Config.dx12Config.serializeShaderDebugSymbols = false;
        LinaGX::Config.dx12Config.enableDebugLayers           = true;
        LinaGX::Config.vulkanConfig.enableVulkanFeatures      = VulkanFeatureFlags::VKF_SamplerAnisotropy;

        m_lgx = new LinaGX::Instance();

        // You can check supported features from the supported flag.
        uint32 supported = m_lgx->VKQueryFeatureSupport(PreferredGPUType::Discrete);

        const bool success = m_lgx->Initialize();

        if (!success)
        {
            LOGA(false, "Current GPU does not support one of the features required by this example!");
        }

        std::vector<LinaGX::Format> formatSupport = {
            Format::R32G32B32A32_SFLOAT,
            Format::R16G16B16A16_SFLOAT,
            Format::R16G16B16A16_UNORM,
            Format::R8G8B8A8_SRGB,
            Format::R8G8B8A8_UNORM,
            Format::B8G8R8A8_UNORM,
        };

        for (auto fmt : formatSupport)
        {
            const LinaGX::FormatSupportInfo fsi = m_lgx->GetFormatSupport(fmt);

            if (fsi.format == LinaGX::Format::UNDEFINED)
                LOGE("Current GPU does not support the formats required by this example!");
        }
    }

    void Example::CreateMainWindow()
    {
        m_window = m_lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX FoxLounge", 0, 0, 800, 800, WindowStyle::WindowedApplication);
        App::RegisterWindowCallbacks(m_window);
        m_windowX = m_window->GetSize().x;
        m_windowY = m_window->GetSize().y;

        m_swapchain = m_lgx->CreateSwapchain({
            .format       = Format::B8G8R8A8_SRGB,
            .x            = 0,
            .y            = 0,
            .width        = m_windowX,
            .height       = m_windowY,
            .window       = m_window->GetWindowHandle(),
            .osHandle     = m_window->GetOSHandle(),
            .isFullscreen = false,
            .vsyncStyle   = {VKVsync::None, DXVsync::None},
        });
    }

    void Example::OnWindowResized(uint32 w, uint32 h)
    {
        if (w == 0 || h == 0)
            return;

        LGXVector2ui monitor = m_window->GetMonitorSize();

        SwapchainRecreateDesc resizeDesc = {
            .swapchain    = m_swapchain,
            .width        = w,
            .height       = h,
            .isFullscreen = w == monitor.x && h == monitor.y,
        };

        m_lgx->RecreateSwapchain(resizeDesc);

        m_windowX = w;
        m_windowY = h;

        DestroyPasses(false);
        CreatePasses(false, 0, 0);
    }

    void Example::SetupStreams()
    {
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd                  = m_pfd[i];
            pfd.graphicsStream         = m_lgx->CreateCommandStream({CommandType::Graphics, 1500, 24000, 10000, 128, "Graphics Stream"});
            pfd.transferStream         = m_lgx->CreateCommandStream({CommandType::Transfer, 100, 12000, 10000, 0, "Transfer Stream"});
            pfd.transferSemaphoreValue = 0;
            pfd.transferSemaphore      = m_lgx->CreateUserSemaphore();
        }
    }

    void Example::LoadTexture(const char* path, uint32 id)
    {
        LinaGX::TextureBuffer outData;
        LinaGX::LoadImageFromFile(path, outData, 4);
        auto& txt = m_textures.at(id);
        txt.allLevels.push_back(outData);
        txt.path            = path;
        const uint32 levels = LinaGX::CalculateMipLevels(outData.width, outData.height);
        LinaGX::GenerateMipmaps(outData, txt.allLevels, MipmapFilter::Default, 4, false);
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
                mat.gpuMat.emissiveFac = glm::vec4(1, 1, 1, 1) * 34.0f;
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

                LinaGX::CMDCopyResource* copy = m_pfd[0].transferStream->AddCommand<LinaGX::CMDCopyResource>();
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
        LinaGX::LoadGLTFBinary("Resources/Models/SkyCube/SkyCube.glb", skyDomeData);
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
        };

        auto createVertexBuffer = [&](std::vector<Vertex>& vertices, GPUBuffer& buf) {
            LinaGX::ResourceDesc vtxBufDesc = {
                .size          = sizeof(Vertex) * vertices.size(),
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
            std::memcpy(mapped, vertices.data(), vertices.size() * sizeof(Vertex));
        };

        uint32 matIndex     = 0;
        auto   parseObjects = [&](const LinaGX::ModelData& modelData, bool isSkyCube) {
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
                wo.isSkyCube    = isSkyCube;

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

                    std::vector<Vertex> vertices;
                    std::vector<uint32> indices;
                    primitive.materialIndex = p->material ? (matIndex + p->material->index) : 0;

                    for (uint32 k = 0; k < p->vertexCount; k++)
                    {
                        vertices.push_back({});
                        Vertex& vtx  = vertices.back();
                        vtx.position = p->positions[k];
                        vtx.uv       = p->texCoords[k];
                        vtx.normal   = p->normals[k];

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
                    copy->destination             = primitive.indexBuffer.gpu;
                    copy->source                  = primitive.indexBuffer.staging;

                    LinaGX::CMDCopyResource* copyVtx = pfd.transferStream->AddCommand<LinaGX::CMDCopyResource>();
                    copyVtx->destination             = primitive.vtxBuffer.gpu;
                    copyVtx->source                  = primitive.vtxBuffer.staging;
                }
            }

            matIndex += static_cast<uint32>(modelData.allMaterials.size());
        };

        parseObjects(skyDomeData, true);
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

                LinaGX::TextureDesc desc = {
                    .format    = format,
                    .views     = {{0, 1, 0, 0, false}},
                    .flags     = LinaGX::TextureFlags::TF_CopyDest | LinaGX::TextureFlags::TF_Sampled,
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
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = static_cast<uint32>(m_textures.size());
                textureBarrier->textureBarriers      = pfd.transferStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * m_textures.size());
                textureBarrier->srcStageFlags        = LinaGX::PSF_TopOfPipe;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;

                uint32 index = 0;
                for (const auto& txt : m_textures)
                {
                    auto& barrier          = textureBarrier->textureBarriers[index];
                    barrier.texture        = txt.gpuHandle;
                    barrier.toState        = LinaGX::TextureBarrierState::TransferDestination;
                    barrier.isSwapchain    = false;
                    barrier.srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
                    barrier.dstAccessFlags = LinaGX::AF_TransferWrite;
                    index++;
                }
            }

            // Perform copy.
            for (auto& txt : m_textures)
            {
                LinaGX::CMDCopyBufferToTexture2D* copyTxt = pfd.transferStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
                copyTxt->destTexture                      = txt.gpuHandle;
                copyTxt->mipLevels                        = static_cast<uint32>(txt.allLevels.size()),
                copyTxt->buffers                          = pfd.transferStream->EmplaceAuxMemory<LinaGX::TextureBuffer>(txt.allLevels.data(), sizeof(LinaGX::TextureBuffer) * txt.allLevels.size());
                copyTxt->destinationSlice                 = 0;
            }

            // Transition all to shader read;
            {
                LinaGX::CMDBarrier* textureBarrier   = pfd.transferStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = static_cast<uint32>(m_textures.size());
                textureBarrier->textureBarriers      = pfd.transferStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * m_textures.size());
                textureBarrier->srcStageFlags        = LinaGX::PSF_Transfer;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;

                uint32 index = 0;
                for (const auto& txt : m_textures)
                {
                    auto& barrier          = textureBarrier->textureBarriers[index];
                    barrier.texture        = txt.gpuHandle;
                    barrier.toState        = LinaGX::TextureBarrierState::ShaderRead;
                    barrier.srcAccessFlags = LinaGX::AF_TransferWrite;
                    barrier.dstAccessFlags = LinaGX::AF_TransferRead;
                    barrier.isSwapchain    = false;
                    index++;
                }
            }
        }

        m_sceneCubemap = m_lgx->CreateTexture(Utility::GetTxtDescCube("Environment Map", ENVIRONMENT_MAP_RES, ENVIRONMENT_MAP_RES));
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

        LinaGX::SamplerDesc depthSampler = {
            .minFilter  = Filter::Nearest,
            .magFilter  = Filter::Nearest,
            .mode       = SamplerAddressMode::Repeat,
            .mipmapMode = MipmapMode::Nearest,
            .anisotropy = 0,
            .minLod     = 0.0f,
            .maxLod     = 1.0f,
            //.mipLodBias = 6.0f,
        };

        LinaGX::SamplerDesc clampSampler = {
            .minFilter  = Filter::Linear,
            .magFilter  = Filter::Linear,
            .mode       = SamplerAddressMode::ClampToEdge,
            .mipmapMode = MipmapMode::Linear,
            .anisotropy = 0,
            .minLod     = 0.0f,
            .maxLod     = 1.0f,
            //.mipLodBias = 6.0f,
        };

        m_samplers.push_back(m_lgx->CreateSampler(defaultSampler));
        m_samplers.push_back(m_lgx->CreateSampler(depthSampler));
        m_samplers.push_back(m_lgx->CreateSampler(clampSampler));

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
                    .samplers  = m_samplers,
                };

                m_lgx->DescriptorUpdateImage(smpUpdate);
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

        LinaGX::PipelineLayoutDesc pipelineLayoutDefaultObjects = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionObjectPass(), Utility::GetSetDescriptionObjectMaterial()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_Objects] = m_lgx->CreatePipelineLayout(pipelineLayoutDefaultObjects);

        LinaGX::PipelineLayoutDesc pipelineLayoutSSAOGeometry = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionObjectPass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_SSAOGeometry] = m_lgx->CreatePipelineLayout(pipelineLayoutSSAOGeometry);

        LinaGX::PipelineLayoutDesc pipelineLayoutSimple = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionObjectPass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_Simple] = m_lgx->CreatePipelineLayout(pipelineLayoutSimple);

        LinaGX::PipelineLayoutDesc pipelineLayoutLightingPassAdv = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionLightingPass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };

        m_pipelineLayouts[PipelineLayoutType::PL_Lighting] = m_lgx->CreatePipelineLayout(pipelineLayoutLightingPassAdv);

        LinaGX::PipelineLayoutDesc pipelineLayoutSimpleQuadPass = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionSimpleQuadPass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_SimpleQuad] = m_lgx->CreatePipelineLayout(pipelineLayoutSimpleQuadPass);

        LinaGX::PipelineLayoutDesc pipelineLayoutFinalQuadPass = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionFinalPass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_FinalQuad] = m_lgx->CreatePipelineLayout(pipelineLayoutFinalQuadPass);

        LinaGX::PipelineLayoutDesc pipelineLayoutIrradiancePass = {
            .descriptorSetDescriptions = {Utility::GetSetDescriptionGlobal(), Utility::GetSetDescriptionIrradiancePass()},
            .constantRanges            = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };
        m_pipelineLayouts[PipelineLayoutType::PL_Irradiance] = m_lgx->CreatePipelineLayout(pipelineLayoutIrradiancePass);
    }

    void Example::SetupShaders()
    {
        LOGT("Compiling shaders...");

        m_shaders.resize(Shader::SH_Max);
        m_shaders[Shader::SH_Lighting]     = Utility::CreateShader(m_lgx, "Resources/Shaders/lightpass_vert.glsl", "Resources/Shaders/lightpass_frag.glsl", LinaGX::CullMode::None, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, false, 2, true, m_pipelineLayouts[PL_Lighting], "Deferred Lighpass Shader");
        m_shaders[Shader::SH_Default]      = Utility::CreateShader(m_lgx, "Resources/Shaders/default_pbr_vert.glsl", "Resources/Shaders/default_pbr_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, true, 4, true, m_pipelineLayouts[PL_Objects], "PBR Default Shader");
        m_shaders[Shader::SH_SSAOGeometry] = Utility::CreateShader(m_lgx, "Resources/Shaders/ssao_geometry_vert.glsl", "Resources/Shaders/ssao_geometry_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, true, 2, true, m_pipelineLayouts[PL_SSAOGeometry], "SSAO Geometry");
        m_shaders[Shader::SH_Shadows]      = Utility::CreateShader(m_lgx, "Resources/Shaders/default_pbr_vert.glsl", "Resources/Shaders/shadowpass_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, true, 0, true, m_pipelineLayouts[PL_Simple], "Unlit Shader");
        m_shaders[Shader::SH_Quad]         = Utility::CreateShader(m_lgx, "Resources/Shaders/screenquad_vert.glsl", "Resources/Shaders/screenquad_frag.glsl", LinaGX::CullMode::None, LinaGX::Format::B8G8R8A8_SRGB, CompareOp::Less, LinaGX::FrontFace::CCW, false, 1, true, m_pipelineLayouts[PL_FinalQuad], "Final Quad Shader");
        m_shaders[Shader::SH_Bloom]        = Utility::CreateShader(m_lgx, "Resources/Shaders/screenquad_vert.glsl", "Resources/Shaders/bloom_frag.glsl", LinaGX::CullMode::None, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, false, 1, true, m_pipelineLayouts[PL_SimpleQuad], "Bloom Shader");
        m_shaders[Shader::SH_Skybox]       = Utility::CreateShader(m_lgx, "Resources/Shaders/skybox_vert.glsl", "Resources/Shaders/skybox_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::LEqual, LinaGX::FrontFace::CCW, true, 4, true, m_pipelineLayouts[PL_Objects], "Skybox Shader");
        m_shaders[Shader::SH_Irradiance]   = Utility::CreateShader(m_lgx, "Resources/Shaders/skybox_vert.glsl", "Resources/Shaders/irradiance_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::LEqual, LinaGX::FrontFace::CCW, false, 1, true, m_pipelineLayouts[PL_Irradiance], "Irradiance Shader");
        m_shaders[Shader::SH_Prefilter]    = Utility::CreateShader(m_lgx, "Resources/Shaders/skybox_vert.glsl", "Resources/Shaders/prefilter_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::LEqual, LinaGX::FrontFace::CCW, false, 1, true, m_pipelineLayouts[PL_Irradiance], "Prefilter Shader");
        m_shaders[Shader::SH_BRDF]         = Utility::CreateShader(m_lgx, "Resources/Shaders/screenquad_vert.glsl", "Resources/Shaders/brdf_frag.glsl", LinaGX::CullMode::None, LinaGX::Format::R16G16B16A16_SFLOAT, CompareOp::Less, LinaGX::FrontFace::CCW, false, 1, true, m_pipelineLayouts[PL_FinalQuad], "BRDF Shader");
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

            const uint32 camDataPadded = Utility::GetPaddedItemSize(sizeof(GPUCameraData), static_cast<uint32>(GPUInfo.minConstantBufferOffsetAlignment));

            // 0 - scene camera
            // 1-6 - reflection/scene capture views
            // 7-12 - light views.
            LinaGX::ResourceDesc cameraDataResource = {
                .size          = camDataPadded * 13,
                .typeHintFlags = LinaGX::TH_ConstantBuffer,
                .heapType      = LinaGX::ResourceHeap::StagingHeap,
                .debugName     = "Camera Data CPU",
            };

            pfd.rscCameraData = m_lgx->CreateResource(cameraDataResource);
            m_lgx->MapResource(pfd.rscCameraData, pfd.rscCameraDataMapping);

            cameraDataResource.debugName = "Camera Data GPU";
            cameraDataResource.heapType  = LinaGX::ResourceHeap::GPUOnly;
            pfd.rscCameraDataGPU         = m_lgx->CreateResource(cameraDataResource);

            // For environment mapping.
            for (uint32 i = 0; i < 6; i++)
            {
                // const glm::vec3 pos   = glm::vec3(-3.0f, -2.2f, -0.33f);
                const glm::vec3 pos   = glm::vec3(0.0f, 0.0f, 0.0f);
                glm::vec3       euler = glm::vec3(0, 0, 0);

                if (i == 0)
                    euler.y = DEG2RAD(-90.0f);
                else if (i == 1)
                    euler.y = DEG2RAD(90.0f);
                else if (i == 4)
                    euler.y = DEG2RAD(0.0f);
                else if (i == 5)
                    euler.y = DEG2RAD(-180.0f);
                else if (i == 2)
                    euler = glm::vec3(DEG2RAD(90.0f), 0.0f, 0.0f);
                else if (i == 3)
                    euler = glm::vec3(DEG2RAD(-90.0f), 0.0f, 0.0f);

                glm::mat4 rotationMatrix    = glm::mat4_cast(glm::inverse(glm::quat(euler)));
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pos);

                GPUCameraData cd = {
                    .view        = rotationMatrix * translationMatrix,
                    .proj        = glm::perspective(DEG2RAD(FOV), 1.0f, NEAR_PLANE, FAR_PLANE),
                    .camPosition = glm::vec4(pos.x, pos.y, pos.z, 0),
                };
                std::memcpy(pfd.rscCameraDataMapping + camDataPadded + (camDataPadded * i), &cd, sizeof(GPUCameraData));
            }

            LinaGX::CMDCopyResource* camCopy = m_pfd[0].transferStream->AddCommand<LinaGX::CMDCopyResource>();
            camCopy->destination             = pfd.rscCameraDataGPU;
            camCopy->source                  = pfd.rscCameraData;
        }

        // SSAO Kernels & Noise
        {
            auto& pfd = m_pfd[0];

            LinaGX::ResourceDesc ssaoData = {
                .size          = sizeof(GPUSSAOData),
                .typeHintFlags = LinaGX::TH_ConstantBuffer,
                .heapType      = LinaGX::ResourceHeap::StagingHeap,
                .debugName     = "SSAO Data CPU",
            };

            m_ssaoDataCPU      = m_lgx->CreateResource(ssaoData);
            ssaoData.heapType  = LinaGX::ResourceHeap::GPUOnly;
            ssaoData.debugName = "SSAO Data GPU";
            m_ssaoDataGPU      = m_lgx->CreateResource(ssaoData);

            uint8* mapped = nullptr;
            m_lgx->MapResource(m_ssaoDataCPU, mapped);

            GPUSSAOData gpuSSAOData = {};

            std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
            std::default_random_engine            generator;
            std::vector<glm::vec3>                ssaoKernel;
            for (unsigned int i = 0; i < 64; ++i)
            {
                glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
                sample = glm::normalize(sample);
                sample *= randomFloats(generator);
                float scale = float(i) / 64.0f;

                // scale samples s.t. they're more aligned to center of kernel
                scale = LinaGX::Lerp(0.1f, 1.0f, scale * scale);
                sample *= scale;
                gpuSSAOData.samples[i] = glm::vec4(sample.x, sample.y, sample.z, 0.0f);
            }

            std::memcpy(mapped, &gpuSSAOData, sizeof(GPUSSAOData));
            LinaGX::CMDCopyResource* copy = pfd.transferStream->AddCommand<LinaGX::CMDCopyResource>();
            copy->destination             = m_ssaoDataGPU;
            copy->source                  = m_ssaoDataCPU;

            std::vector<glm::vec4> ssaoNoise;
            for (unsigned int i = 0; i < 16; i++)
            {
                glm::vec4 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f, 0.0f); // rotate around z-axis (in tangent space)
                ssaoNoise.push_back(noise);
            }

            LinaGX::TextureDesc noise = {
                .type        = TextureType::Texture2D,
                .format      = Format::R32G32B32A32_SFLOAT,
                .flags       = LinaGX::TF_Sampled | LinaGX::TF_CopyDest | LinaGX::TF_LinearTiling,
                .width       = 4,
                .height      = 4,
                .mipLevels   = 1,
                .arrayLength = 1,
                .debugName   = "SSAO Noise Texture",
            };

            m_ssaoNoiseTexture = m_lgx->CreateTexture(noise);
            m_ssaoNoiseBuffer  = {
                 .width         = 4,
                 .height        = 4,
                 .bytesPerPixel = 16,
            };

            m_ssaoNoiseBuffer.pixels = new uint8[4 * 4 * 16];
            std::memcpy(m_ssaoNoiseBuffer.pixels, ssaoNoise.data(), 4 * 4 * 16);

            // To copy dest
            {
                LinaGX::CMDBarrier* textureBarrier   = pfd.transferStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = 1;
                textureBarrier->textureBarriers      = pfd.transferStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
                textureBarrier->srcStageFlags        = LinaGX::PSF_TopOfPipe;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;

                auto& barrier          = textureBarrier->textureBarriers[0];
                barrier.texture        = m_ssaoNoiseTexture;
                barrier.toState        = LinaGX::TextureBarrierState::TransferDestination;
                barrier.isSwapchain    = false;
                barrier.srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
                barrier.dstAccessFlags = LinaGX::AF_TransferWrite;
            }

            LinaGX::CMDCopyBufferToTexture2D* copyTxt = pfd.transferStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
            copyTxt->destinationSlice                 = 0;
            copyTxt->destTexture                      = m_ssaoNoiseTexture;
            copyTxt->mipLevels                        = 1;
            copyTxt->buffers                          = pfd.transferStream->EmplaceAuxMemory<LinaGX::TextureBuffer>(m_ssaoNoiseBuffer);

            // Transition all to shader read;
            {
                LinaGX::CMDBarrier* textureBarrier   = pfd.transferStream->AddCommand<LinaGX::CMDBarrier>();
                textureBarrier->resourceBarrierCount = 0;
                textureBarrier->textureBarrierCount  = 1;
                textureBarrier->textureBarriers      = pfd.transferStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
                textureBarrier->srcStageFlags        = LinaGX::PSF_Transfer;
                textureBarrier->dstStageFlags        = LinaGX::PSF_Transfer;

                auto& barrier          = textureBarrier->textureBarriers[0];
                barrier.texture        = m_ssaoNoiseTexture;
                barrier.toState        = LinaGX::TextureBarrierState::ShaderRead;
                barrier.srcAccessFlags = LinaGX::AF_TransferWrite;
                barrier.dstAccessFlags = LinaGX::AF_TransferRead;
                barrier.isSwapchain    = false;
            }
        }
    } // namespace LinaGX::Examples

    void Example::SetupPass(PassType passType, const std::vector<LinaGX::TextureDesc>& renderTargetDescriptions, bool hasDepth, const LinaGX::TextureDesc& depthDescription, const LinaGX::DescriptorSetDesc& descriptorDesc, bool isSwapchain)
    {
        auto&                     pass = m_passes[passType];
        RenderPassColorAttachment att  = {};
        att.clearColor                 = {0.1f, 0.1f, 0.1f, 1.0f};
        att.loadOp                     = LinaGX::LoadOp::Clear;
        att.storeOp                    = LinaGX::StoreOp::Store;

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            pass.renderTargets[i].colorAttachments.clear();
            pass.renderTargets[i].depthStencilAttachment.clearDepth = 1.0f;
            pass.descriptorSets[i]                                  = m_lgx->CreateDescriptorSet(descriptorDesc);

            if (isSwapchain)
            {
                att.isSwapchain = true;
                att.texture     = static_cast<uint32>(m_swapchain);
                pass.renderTargets[i].colorAttachments.push_back(att);
                continue;
            }

            for (const auto& desc : renderTargetDescriptions)
            {
                att.texture = m_lgx->CreateTexture(desc);
                pass.renderTargets[i].colorAttachments.push_back(att);
            }

            if (hasDepth)
            {
                pass.renderTargets[i].depthStencilAttachment.useDepth     = true;
                pass.renderTargets[i].depthStencilAttachment.depthLoadOp  = LinaGX::LoadOp::Clear;
                pass.renderTargets[i].depthStencilAttachment.depthStoreOp = LinaGX::StoreOp::Store;
                pass.renderTargets[i].depthStencilAttachment.texture      = m_lgx->CreateTexture(depthDescription);
            }
            else
                pass.renderTargets[i].depthStencilAttachment.useDepth = false;
        }
    }

    void Example::CreatePasses(bool isFirst, uint32 customWidth, uint32 customHeight)
    {
        LOGT("Setting up passes...");

        if (customWidth == 0)
            customWidth = m_windowX;

        if (customHeight == 0)
            customHeight = m_windowY;

        // Object default & lighting default passes.
        {
            const auto& rtDescObjects1 = Utility::GetRTDesc("GBuf AlbedoRoughness", customWidth, customHeight);
            const auto& rtDescObjects2 = Utility::GetRTDesc("GBuf NormalMetallic", customWidth, customHeight);
            const auto& rtDescObjects3 = Utility::GetRTDesc("GBuf Position", customWidth, customHeight);
            const auto& rtDescObjects4 = Utility::GetRTDesc("GBuf Emission", customWidth, customHeight);
            const auto& depthDesc      = Utility::GetDepthDesc("GBuf Depth", customWidth, customHeight);
            SetupPass(PassType::PS_Objects, {rtDescObjects1, rtDescObjects2, rtDescObjects3, rtDescObjects4}, true, depthDesc, Utility::GetSetDescriptionObjectPass());

            const auto& rtDescSSAOGeo    = Utility::GetRTDesc("SSAO GEO Position", customWidth, customHeight);
            const auto& rtDescSSAOGeo2   = Utility::GetRTDesc("SSAO GEO Normal", customWidth, customHeight);
            const auto& depthDescSSAOGeo = Utility::GetDepthDesc("SSAO GEO Depth", customWidth, customHeight);
            SetupPass(PassType::PS_SSAOGeometry, {rtDescSSAOGeo, rtDescSSAOGeo2}, true, depthDescSSAOGeo, Utility::GetSetDescriptionObjectPass());

            const auto& rtDescLighting = Utility::GetRTDesc("Lighting", customWidth, customHeight);
            const auto& rtDescBloom    = Utility::GetRTDesc("Brightness", customWidth, customHeight);
            SetupPass(PassType::PS_Lighting, {rtDescLighting, rtDescBloom}, false, {}, Utility::GetSetDescriptionLightingPass());
        }

        if (isFirst)
        {
            // Irradiance
            {
                const auto& irr = Utility::GetRTDescCube("Irradiance", IRRADIANCE_MAP_RES, IRRADIANCE_MAP_RES);
                SetupPass(PassType::PS_Irradiance, {irr}, false, {}, Utility::GetSetDescriptionIrradiancePass());
            }

            // Shadows
            {
                const auto& sh = Utility::GetRTDescCube("Shadow Map", SHADOW_MAP_RES, SHADOW_MAP_RES);
                const auto& d  = Utility::GetRTDescCubeDepth("Shadow Depth Map", SHADOW_MAP_RES, SHADOW_MAP_RES);
                SetupPass(PassType::PS_Shadows, {}, true, {d}, Utility::GetSetDescriptionObjectPass());
            }

            // Prefilter
            {
                const auto& pref = Utility::GetRTDescPrefilter("Prefilter", PREFILTER_MAP_RES, PREFILTER_MAP_RES, PREFILTER_MIP_LEVELS);
                SetupPass(PassType::PS_Prefilter, {pref}, false, {}, Utility::GetSetDescriptionIrradiancePass());
            }

            // BRDF
            {
                const auto& brdf = Utility::GetRTDesc("BRDF", BRDF_MAP_RES, BRDF_MAP_RES);
                SetupPass(PassType::PS_BRDF, {brdf}, false, {}, Utility::GetSetDescriptionSimpleQuadPass());
            }
        }

        // Bloom
        {
            const auto& rtDesc = Utility::GetRTDesc("Bloom", customWidth, customHeight);
            SetupPass(PassType::PS_BloomHori, {rtDesc}, false, {}, Utility::GetSetDescriptionSimpleQuadPass(), false);
            SetupPass(PassType::PS_BloomVert, {rtDesc}, false, {}, Utility::GetSetDescriptionSimpleQuadPass(), false);

            // Requires additional descriptor set.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
                m_passes[PassType::PS_BloomHori].additionalDescriptorSets[i] = m_lgx->CreateDescriptorSet(Utility::GetSetDescriptionSimpleQuadPass());
        }

        // Final Quad
        {
            SetupPass(PassType::PS_FinalQuad, {}, false, {}, Utility::GetSetDescriptionFinalPass(), true);
        }

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            // Objects Default
            {
                LinaGX::DescriptorUpdateBufferDesc bufferUpdate = {
                    .setHandle = m_passes[PS_Objects].descriptorSets[i],
                    .binding   = 0,
                    .buffers   = {m_pfd[i].rscCameraDataGPU},
                    .ranges    = {sizeof(GPUCameraData)},
                };

                m_lgx->DescriptorUpdateBuffer(bufferUpdate);
            }

            // SSAO Geometry
            {
                LinaGX::DescriptorUpdateBufferDesc bufferUpdate = {
                    .setHandle = m_passes[PS_SSAOGeometry].descriptorSets[i],
                    .binding   = 0,
                    .buffers   = {m_pfd[i].rscCameraDataGPU},
                    .ranges    = {sizeof(GPUCameraData)},
                };

                m_lgx->DescriptorUpdateBuffer(bufferUpdate);
            }

            // Lighting Default
            {
                LinaGX::DescriptorUpdateBufferDesc bufferUpdate = {
                    .setHandle = m_passes[PS_Lighting].descriptorSets[i],
                    .binding   = 0,
                    .buffers   = {m_pfd[i].rscCameraDataGPU},
                    .ranges    = {sizeof(GPUCameraData)},
                };
                m_lgx->DescriptorUpdateBuffer(bufferUpdate);

                LinaGX::DescriptorUpdateImageDesc updateGBuf = {
                    .setHandle = m_passes[PS_Lighting].descriptorSets[i],
                    .binding   = 1,
                    .textures  = {
                        m_passes[PassType::PS_Objects].renderTargets[i].colorAttachments[0].texture,
                        m_passes[PassType::PS_Objects].renderTargets[i].colorAttachments[1].texture,
                        m_passes[PassType::PS_Objects].renderTargets[i].colorAttachments[2].texture,
                        m_passes[PassType::PS_Objects].renderTargets[i].colorAttachments[3].texture,
                        m_passes[PassType::PS_Objects].renderTargets[i].depthStencilAttachment.texture,
                    },
                };

                m_lgx->DescriptorUpdateImage(updateGBuf);

                LinaGX::DescriptorUpdateImageDesc updateIrradiance = {
                    .setHandle          = m_passes[PS_Lighting].descriptorSets[i],
                    .binding            = 2,
                    .textures           = {m_passes[PassType::PS_Irradiance].renderTargets[i].colorAttachments[0].texture},
                    .textureViewIndices = {6}, // last is cubeview
                };

                m_lgx->DescriptorUpdateImage(updateIrradiance);

                LinaGX::DescriptorUpdateImageDesc updatePrefilter = {
                    .setHandle          = m_passes[PS_Lighting].descriptorSets[i],
                    .binding            = 3,
                    .textures           = {m_passes[PassType::PS_Prefilter].renderTargets[i].colorAttachments[0].texture},
                    .textureViewIndices = {30}, // last is cubeview
                };

                m_lgx->DescriptorUpdateImage(updatePrefilter);

                LinaGX::DescriptorUpdateImageDesc updateBRDF = {
                    .setHandle = m_passes[PS_Lighting].descriptorSets[i],
                    .binding   = 4,
                    .textures  = {m_passes[PassType::PS_BRDF].renderTargets[i].colorAttachments[0].texture},
                };

                m_lgx->DescriptorUpdateImage(updateBRDF);

                LinaGX::DescriptorUpdateImageDesc updateShadows = {
                    .setHandle          = m_passes[PS_Lighting].descriptorSets[i],
                    .binding            = 5,
                    .textures           = {m_passes[PassType::PS_Shadows].renderTargets[i].depthStencilAttachment.texture},
                    .textureViewIndices = {6}, // last is cubeview
                };

                m_lgx->DescriptorUpdateImage(updateShadows);

                LinaGX::DescriptorUpdateImageDesc updateSSAOTexts = {
                    .setHandle = m_passes[PS_Lighting].descriptorSets[i],
                    .binding   = 6,
                    .textures  = {m_passes[PassType::PS_SSAOGeometry].renderTargets[i].colorAttachments[0].texture, m_passes[PassType::PS_SSAOGeometry].renderTargets[i].colorAttachments[1].texture, m_ssaoNoiseTexture},
                };

                m_lgx->DescriptorUpdateImage(updateSSAOTexts);

                LinaGX::DescriptorUpdateBufferDesc updateSSAOKernels = {
                    .setHandle = m_passes[PS_Lighting].descriptorSets[i],
                    .binding   = 7,
                    .buffers   = {m_ssaoDataGPU},
                };

                m_lgx->DescriptorUpdateBuffer(updateSSAOKernels);
            }

            if (isFirst)
            {
                // Irradiance
                {
                    LinaGX::DescriptorUpdateBufferDesc bufferUpdate = {
                        .setHandle = m_passes[PS_Irradiance].descriptorSets[i],
                        .binding   = 0,
                        .buffers   = {m_pfd[i].rscCameraDataGPU},
                        .ranges    = {sizeof(GPUCameraData)},
                    };

                    m_lgx->DescriptorUpdateBuffer(bufferUpdate);

                    LinaGX::DescriptorUpdateImageDesc update = {
                        .setHandle = m_passes[PS_Irradiance].descriptorSets[i],
                        .binding   = 1,
                        .textures  = {m_sceneCubemap},
                    };

                    m_lgx->DescriptorUpdateImage(update);
                }

                // Shadows
                {
                    LinaGX::DescriptorUpdateBufferDesc bufferUpdate = {
                        .setHandle = m_passes[PS_Shadows].descriptorSets[i],
                        .binding   = 0,
                        .buffers   = {m_pfd[i].rscCameraDataGPU},
                        .ranges    = {sizeof(GPUCameraData)},
                    };

                    m_lgx->DescriptorUpdateBuffer(bufferUpdate);
                }

                // Prefilter
                {
                    LinaGX::DescriptorUpdateBufferDesc bufferUpdate = {
                        .setHandle = m_passes[PS_Prefilter].descriptorSets[i],
                        .binding   = 0,
                        .buffers   = {m_pfd[i].rscCameraDataGPU},
                        .ranges    = {sizeof(GPUCameraData)},
                    };

                    m_lgx->DescriptorUpdateBuffer(bufferUpdate);

                    LinaGX::DescriptorUpdateImageDesc update = {
                        .setHandle = m_passes[PS_Prefilter].descriptorSets[i],
                        .binding   = 1,
                        .textures  = {m_sceneCubemap},
                    };
                    m_lgx->DescriptorUpdateImage(update);
                }
            }

            // Bloom Horizontal
            {
                LinaGX::DescriptorUpdateImageDesc update = {
                    .setHandle = m_passes[PS_BloomHori].descriptorSets[i],
                    .binding   = 0,
                    .textures  = {m_passes[PassType::PS_BloomVert].renderTargets[i].colorAttachments[0].texture},
                };

                m_lgx->DescriptorUpdateImage(update);

                LinaGX::DescriptorUpdateImageDesc update2 = {
                    .setHandle = m_passes[PS_BloomHori].additionalDescriptorSets[i],
                    .binding   = 0,
                    .textures  = {m_passes[PassType::PS_Lighting].renderTargets[i].colorAttachments[1].texture},
                };
                m_lgx->DescriptorUpdateImage(update2);
            }

            // Bloom Vertical
            {
                LinaGX::DescriptorUpdateImageDesc update = {
                    .setHandle = m_passes[PS_BloomVert].descriptorSets[i],
                    .binding   = 0,
                    .textures  = {m_passes[PassType::PS_BloomHori].renderTargets[i].colorAttachments[0].texture},
                };
                m_lgx->DescriptorUpdateImage(update);
            }

            // Final quad pass.
            {
                LinaGX::DescriptorUpdateImageDesc update = {
                    .setHandle = m_passes[PS_FinalQuad].descriptorSets[i],
                    .binding   = 0,
                    .textures  = {m_passes[PassType::PS_Lighting].renderTargets[i].colorAttachments[0].texture},
                };

                m_lgx->DescriptorUpdateImage(update);

                LinaGX::DescriptorUpdateImageDesc update2 = {
                    .setHandle = m_passes[PS_FinalQuad].descriptorSets[i],
                    .binding   = 1,
                    .textures  = {m_passes[PassType::PS_BloomVert].renderTargets[i].colorAttachments[0].texture},
                };

                m_lgx->DescriptorUpdateImage(update2);
            }
        }
    }

    void Example::DestroyPasses(bool isShutdown)
    {
        LOGT("Destroying dynamic passes...");

        for (uint32 passType = 0; passType < PassType::PS_Max; passType++)
        {
            if (!isShutdown && (passType == PassType::PS_Irradiance || passType == PassType::PS_Prefilter || passType == PassType::PS_BRDF || passType == PassType::PS_Shadows))
                continue;

            auto& pass = m_passes[passType];

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                const auto& rt = pass.renderTargets[i];

                for (auto& att : rt.colorAttachments)
                {
                    if (!att.isSwapchain)
                        m_lgx->DestroyTexture(att.texture);
                }

                if (rt.depthStencilAttachment.useDepth)
                    m_lgx->DestroyTexture(rt.depthStencilAttachment.texture);

                m_lgx->DestroyDescriptorSet(pass.descriptorSets[i]);

                if (passType == PassType::PS_BloomHori)
                    m_lgx->DestroyDescriptorSet(pass.additionalDescriptorSets[i]);
            }
        }
    }

    void Example::BeginPass(uint32 frameIndex, PassType pass, uint32 width, uint32 height, uint32 viewIndex, uint32 depthViewIndex)
    {
        if (width == 0)
            width = m_windowX;

        if (height == 0)
            height = m_windowY;

        const auto&        currentFrame = m_pfd[frameIndex];
        const Viewport     viewport     = {.x = 0, .y = 0, .width = width, .height = height, .minDepth = 0.0f, .maxDepth = 1.0f};
        const ScissorsRect sc           = {.x = 0, .y = 0, .width = width, .height = height};
        auto&              passData     = m_passes[pass];

        // Pass Begin
        {
            auto& rt = passData.renderTargets[frameIndex];

            LINAGX_STRING               lbl   = Utility::PassToString(pass);
            LinaGX::CMDDebugBeginLabel* label = currentFrame.graphicsStream->AddCommand<LinaGX::CMDDebugBeginLabel>();
            label->label                      = currentFrame.graphicsStream->EmplaceAuxMemory<const char>(lbl.data(), lbl.size());

            LinaGX::CMDBeginRenderPass* beginRenderPass = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBeginRenderPass>();
            beginRenderPass->colorAttachmentCount       = static_cast<uint32>(rt.colorAttachments.size());
            beginRenderPass->colorAttachments           = rt.colorAttachments.empty() ? nullptr : currentFrame.graphicsStream->EmplaceAuxMemory<LinaGX::RenderPassColorAttachment>(rt.colorAttachments.data(), sizeof(LinaGX::RenderPassColorAttachment) * rt.colorAttachments.size());
            beginRenderPass->depthStencilAttachment     = rt.depthStencilAttachment;
            beginRenderPass->viewport                   = viewport;
            beginRenderPass->scissors                   = sc;

            for (uint32 i = 0; i < beginRenderPass->colorAttachmentCount; i++)
                beginRenderPass->colorAttachments[i].viewIndex = viewIndex;

            beginRenderPass->depthStencilAttachment.viewIndex = depthViewIndex;
        }
    }

    void Example::EndPass(uint32 frameIndex)
    {
        auto&                     currentFrame = m_pfd[frameIndex];
        LinaGX::CMDEndRenderPass* end          = currentFrame.graphicsStream->AddCommand<LinaGX::CMDEndRenderPass>();

        currentFrame.graphicsStream->AddCommand<LinaGX::CMDDebugEndLabel>();
    }

    void Example::Initialize()
    {
        App::Initialize();

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

            // We will first capture environment cubemap, create passes with that resolution first.
            m_passes.resize(PassType::PS_Max);
            CreatePasses(true, ENVIRONMENT_MAP_RES, ENVIRONMENT_MAP_RES);
        }

        // Above operations will record bunch of transfer commands.
        // Start the work on the gpu while we continue on our init code.
        {
            auto& pfd = m_pfd[0];
            m_lgx->CloseCommandStreams(&pfd.transferStream, 1);
            pfd.transferSemaphoreValue++;
            m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &pfd.transferStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &pfd.transferSemaphore, .signalValues = &pfd.transferSemaphoreValue});
        }

        LOGT("Waiting for init transfers...");

        // Make sure we wait for all gpu ops before proceeding on the CPU.
        // Then remove some resources we are done with & continue the biz.
        {
            auto& pfd = m_pfd[0];
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

            m_lgx->DestroyResource(m_ssaoDataCPU);

            delete[] m_ssaoNoiseBuffer.pixels;
        }

        m_camera.Initialize(m_lgx);
        m_camera.Tick(0.016f);

        // Generate environment cubemap, irradiance map, convolution prefilter map and BRDF LUT texture.
        IrradiancePrefilterBRDF();
        m_lgx->Join();

        // Re-create passes with window resolution.
        DestroyPasses(false);
        CreatePasses(false, 0, 0);

        LOGT("All init OK! Starting frames...");
    }

    void Example::Shutdown()
    {
        m_lgx->Join();

        // First get rid of the window.
        m_lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        for (auto s : m_shaders)
            m_lgx->DestroyShader(s);

        m_lgx->DestroyTexture(m_sceneCubemap);
        m_lgx->DestroySwapchain(m_swapchain);
        m_lgx->DestroyResource(m_ssaoDataGPU);
        m_lgx->DestroyTexture(m_ssaoNoiseTexture);

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

        DestroyPasses(true);

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

            m_lgx->DestroyResource(pfd.rscObjDataCPU);
            m_lgx->DestroyResource(pfd.rscObjDataGPU);
            m_lgx->DestroyResource(pfd.rscCameraData);
            m_lgx->DestroyResource(pfd.rscCameraDataGPU);

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

    void Example::DrawObjects(uint32 frameIndex, uint16 flags, Shader shader, bool bindMaterials)
    {

        auto& currentFrame = m_pfd[frameIndex];

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
                if (obj.isSkyCube)
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
                if (bindMaterials)
                {
                    LinaGX::CMDBindDescriptorSets* bind = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
                    bind->isCompute                     = false;
                    bind->firstSet                      = 2;
                    bind->setCount                      = 1;
                    bind->dynamicOffsetCount            = 0;
                    bind->descriptorSetHandles          = currentFrame.graphicsStream->EmplaceAuxMemory<uint16>(mat.descriptorSets[frameIndex]);
                    bind->layoutSource                  = DescriptorSetsLayoutSource::LastBoundShader;
                }

                for (const auto& primData : prims)
                {
                    BindConstants(frameIndex, primData.objIndex, 0);

                    // Global index buffer.
                    {
                        LinaGX::CMDBindIndexBuffers* index = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindIndexBuffers>();
                        index->indexType                   = LinaGX::IndexType::Uint32;
                        index->offset                      = 0;
                        index->resource                    = primData.prim->indexBuffer.gpu;
                    }

                    // Global vertex buffer.
                    {
                        LinaGX::CMDBindVertexBuffers* vtx = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindVertexBuffers>();
                        vtx->offset                       = 0;
                        vtx->slot                         = 0;
                        vtx->vertexSize                   = sizeof(Vertex);
                        vtx->resource                     = primData.prim->vtxBuffer.gpu;
                    }

                    LinaGX::CMDDrawIndexedInstanced* draw = currentFrame.graphicsStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
                    draw->instanceCount                   = 1;
                    draw->startInstanceLocation           = 0;
                    draw->startIndexLocation              = 0;
                    draw->indexCountPerInstance           = primData.prim->indexCount;
                    draw->baseVertexLocation              = 0;
                }
            }
        };

        if (flags & DrawObjectFlags::DrawDefault)
        {
            BindShader(frameIndex, shader);
            draw();
        }

        if (flags & DrawObjectFlags::DrawSkybox)
        {
            BindShader(frameIndex, Shader::SH_Skybox);
            DrawCube(frameIndex);
        }
    }

    void Example::DrawCube(uint32 frameIndex)
    {
        auto& currentFrame = m_pfd[frameIndex];

        for (auto& obj : m_worldObjects)
        {
            if (!obj.isSkyCube)
                continue;

            for (auto& prim : obj.primitives)
            {
                // index buffer.
                {
                    LinaGX::CMDBindIndexBuffers* index = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindIndexBuffers>();
                    index->indexType                   = LinaGX::IndexType::Uint32;
                    index->offset                      = 0;
                    index->resource                    = prim.indexBuffer.gpu;
                }

                // Global vertex buffer.
                {
                    LinaGX::CMDBindVertexBuffers* vtx = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindVertexBuffers>();
                    vtx->offset                       = 0;
                    vtx->slot                         = 0;
                    vtx->vertexSize                   = sizeof(Vertex);
                    vtx->resource                     = prim.vtxBuffer.gpu;
                }

                LinaGX::CMDDrawIndexedInstanced* draw = currentFrame.graphicsStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
                draw->baseVertexLocation              = 0;
                draw->startIndexLocation              = 0;
                draw->startInstanceLocation           = 0;
                draw->instanceCount                   = 1;
                draw->indexCountPerInstance           = prim.indexCount;
            }
        }
    }

    void Example::DrawFullscreenQuad(uint32 frameIndex)
    {
        auto& pfd = m_pfd[frameIndex];

        CMDDrawInstanced* draw       = pfd.graphicsStream->AddCommand<CMDDrawInstanced>();
        draw->instanceCount          = 1;
        draw->startInstanceLocation  = 0;
        draw->startVertexLocation    = 0;
        draw->vertexCountPerInstance = 6;
    }

    void Example::BindShader(uint32 frameIndex, uint32 target)
    {
        auto& pfd = m_pfd[frameIndex];

        if (static_cast<uint32>(pfd.boundShader) == target)
            return;

        LinaGX::CMDBindPipeline* pipeline = pfd.graphicsStream->AddCommand<LinaGX::CMDBindPipeline>();
        pipeline->shader                  = m_shaders[target];

        pfd.boundShader = static_cast<int32>(target);
    }

    void Example::BindPassSet(uint32 frameIndex, PipelineLayoutType layout, uint16 set, uint32 offset, bool useDynamicOffset)
    {
        auto&                          pfd  = m_pfd[frameIndex];
        LinaGX::CMDBindDescriptorSets* bind = pfd.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
        bind->isCompute                     = false;
        bind->firstSet                      = 1;
        bind->setCount                      = 1;
        bind->dynamicOffsetCount            = useDynamicOffset ? 1 : 0;
        bind->dynamicOffsets                = pfd.graphicsStream->EmplaceAuxMemory<uint32>(offset);
        bind->descriptorSetHandles          = pfd.graphicsStream->EmplaceAuxMemory<uint16>(set);
        bind->layoutSource                  = DescriptorSetsLayoutSource::CustomLayout;
        bind->customLayout                  = m_pipelineLayouts[layout];
    }

    void Example::ReflectionPass(uint32 frameIndex)
    {
        //  auto&        pfd               = m_pfd[currentFrameIndex];
        //
        //  for (uint32 i = 0; i < 6; i++)
        //  {
        //      const uint32 padded = Utility::GetPaddedItemSize(sizeof(GPUCameraData), GPUInfo.minConstantBufferOffsetAlignment);
        //      BindPassSet(frameIndex, PipelineLayoutType::PL_Objects, m_passes[PassType::PS_Objects].descriptorSets[currentFrameIndex], padded + padded * i, true);
        //      BeginPass(frameIndex, PassType::PS_ObjectsReflections, REFLECTION_PASS_RES, REFLECTION_PASS_RES);
        //      DrawObjects(frameIndex, Shader::SH_Default);
        //      EndPass(frameIndex);
        //
        //      CollectPassBarrier(frameIndex, PS_ObjectsReflections, LinaGX::TextureBarrierState::ShaderRead);
        //      ExecPassBarriers();
        //
        //      BindPassSet(frameIndex, PipelineLayoutType::PL_LightingPassSimple, m_passes[PassType::PS_LightingReflections].descriptorSets[currentFrameIndex], padded + padded * i, true);
        //      BeginPass(frameIndex, PassType::PS_LightingReflections, REFLECTION_PASS_RES, REFLECTION_PASS_RES, i);
        //      BindShader(frameIndex, Shader::SH_LightingSimple);
        //      DrawFullscreenQuad(frameIndex);
        //      EndPass(frameIndex);
        //
        //      CollectPassBarrier(frameIndex, PS_ObjectsReflections, LinaGX::TextureBarrierState::ColorAttachment);
        //      ExecPassBarriers();
        //  }
        //
        //  CollectPassBarrier(frameIndex, PS_LightingReflections, LinaGX::TextureBarrierState::ShaderRead);
        //  CollectPassBarrier(frameIndex, PS_ObjectsReflections, LinaGX::TextureBarrierState::ShaderRead);
        //  ExecPassBarriers();
    }

    void Example::CollectPassBarrier(uint32 frameIndex, PassType pass, LinaGX::TextureBarrierState target, uint32 srcAccess, uint32 dstAccess, bool collectDepth)
    {
        auto& pfd = m_pfd[frameIndex];

        if (collectDepth)
        {
            const auto& dsAtt = m_passes[pass].renderTargets[frameIndex].depthStencilAttachment;
            if (dsAtt.useDepth)
            {
                LinaGX::TextureBarrier barrier = {};
                barrier.toState                = target;
                barrier.texture                = dsAtt.texture;
                barrier.isSwapchain            = false;
                barrier.srcAccessFlags         = srcAccess;
                barrier.dstAccessFlags         = dstAccess;
                m_passBarriers.push_back(barrier);
            }
            return;
        }

        for (const auto& att : m_passes[pass].renderTargets[frameIndex].colorAttachments)
        {
            LinaGX::TextureBarrier barrier = {};
            barrier.toState                = target;
            barrier.texture                = att.texture;
            barrier.isSwapchain            = pass == PassType::PS_FinalQuad;
            barrier.srcAccessFlags         = srcAccess;
            barrier.dstAccessFlags         = dstAccess;
            m_passBarriers.push_back(barrier);
        }
    }

    void Example::ExecPassBarriers(LinaGX::CommandStream* stream, uint32 srcStage, uint32 dstStage)
    {
        LinaGX::CMDBarrier* barrier   = stream->AddCommand<LinaGX::CMDBarrier>();
        barrier->resourceBarrierCount = 0;
        barrier->textureBarrierCount  = static_cast<uint32>(m_passBarriers.size());
        barrier->textureBarriers      = stream->EmplaceAuxMemory<LinaGX::TextureBarrier>(m_passBarriers.data(), sizeof(LinaGX::TextureBarrier) * m_passBarriers.size());
        barrier->srcStageFlags        = srcStage;
        barrier->dstStageFlags        = dstStage;
        m_passBarriers.clear();
    }

    void Example::TransferGlobalData(uint32 frameIndex)
    {
        auto& currentFrame = m_pfd[frameIndex];

        m_lightPos = glm::vec4(1.5f, 2.5f, 0.0f, 0.0f);

        // Scene data.
        {
            GPUSceneData sceneData = {
                .skyColor1  = glm::vec4(0.002f, 0.137f, 0.004f, 1.0f),
                .skyColor2  = glm::vec4(0.0f, 0.156f, 0.278f, 1.0f),
                .lightPos   = m_lightPos,
                .lightColor = glm::vec4(1.0f, 0.684244f, 0.240f, 0.0f) * 40.0f,
                .resolution = glm::vec2(m_windowX, m_windowY),
                .nearPlane  = NEAR_PLANE,
                .farPlane   = FAR_PLANE,
            };

            std::memcpy(currentFrame.rscSceneDataMapping, &sceneData, sizeof(GPUSceneData));
        }

        // Camera data.
        {
            const uint32 camDataPadded = Utility::GetPaddedItemSize(sizeof(GPUCameraData), static_cast<uint32>(GPUInfo.minConstantBufferOffsetAlignment));

            // Scene camera.
            GPUCameraData camData = {
                .view        = m_camera.GetView(),
                .proj        = m_camera.GetProj(),
                .camPosition = m_camera.GetPosition(),
            };

            std::memcpy(currentFrame.rscCameraDataMapping, &camData, sizeof(GPUCameraData));

            // Light cameras.
            for (uint32 i = 0; i < 6; i++)
            {
                const glm::vec3 pos   = m_lightPos;
                glm::vec3       euler = glm::vec3(0, 0, 0);

                if (i == 0)
                    euler.y = DEG2RAD(-90.0f);
                else if (i == 1)
                    euler.y = DEG2RAD(90.0f);
                else if (i == 4)
                    euler.y = DEG2RAD(0.0f);
                else if (i == 5)
                    euler.y = DEG2RAD(-180.0f);
                else if (i == 2)
                    euler = glm::vec3(DEG2RAD(90.0f), 0.0f, 0.0f);
                else if (i == 3)
                    euler = glm::vec3(DEG2RAD(-90.0f), 0.0f, 0.0f);
                glm::mat4 rotationMatrix    = glm::mat4_cast(glm::inverse(glm::quat(euler)));
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -pos);

                GPUCameraData cd = {
                    .view        = rotationMatrix * translationMatrix,
                    .proj        = glm::perspective(DEG2RAD(FOV), 1.0f, NEAR_PLANE, FAR_PLANE),
                    .camPosition = glm::vec4(pos.x, pos.y, pos.z, 0),
                };

                std::memcpy(currentFrame.rscCameraDataMapping + camDataPadded * 7 + (camDataPadded * i), &cd, sizeof(GPUCameraData));
            }

            LinaGX::CMDCopyResource* camCopy = currentFrame.transferStream->AddCommand<LinaGX::CMDCopyResource>();
            camCopy->destination             = currentFrame.rscCameraDataGPU;
            camCopy->source                  = currentFrame.rscCameraData;
        }

        // Obj data.
        {
            std::vector<GPUObjectData> objData;
            for (const auto& obj : m_worldObjects)
            {
                if (obj.isSkyCube)
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

            m_lgx->CloseCommandStreams(&currentFrame.transferStream, 1);
            m_lgx->SubmitCommandStreams(transferSubmit);
        }
    }

    void Example::BindGlobalSet(uint32 frameIndex)
    {
        auto&                          currentFrame = m_pfd[frameIndex];
        LinaGX::CMDBindDescriptorSets* bind         = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
        bind->isCompute                             = false;
        bind->firstSet                              = 0;
        bind->setCount                              = 1;
        bind->dynamicOffsetCount                    = 0;
        bind->descriptorSetHandles                  = currentFrame.graphicsStream->EmplaceAuxMemory<uint16>(currentFrame.globalSet);
        bind->layoutSource                          = DescriptorSetsLayoutSource::CustomLayout;
        bind->customLayout                          = m_pipelineLayouts[PipelineLayoutType::PL_GlobalSet];
    }

    void Example::BindConstants(uint32 frameIndex, uint32 c1, uint32 c2)
    {
        auto&        currentFrame    = m_pfd[frameIndex];
        GPUConstants constants       = {};
        constants.int1               = c1;
        constants.int2               = c2;
        LinaGX::CMDBindConstants* ct = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindConstants>();
        ct->offset                   = 0;
        ct->size                     = sizeof(GPUConstants);
        ct->stages                   = currentFrame.graphicsStream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment);
        ct->stagesSize               = 2;
        ct->data                     = currentFrame.graphicsStream->EmplaceAuxMemory<GPUConstants>(constants);
    }

    void Example::DeferredRenderScene(uint32 frameIndex, uint16 drawObjFlags, uint32 cameraDataIndex, uint32 width, uint32 height)
    {
        const uint32 camDataPadded = Utility::GetPaddedItemSize(sizeof(GPUCameraData), static_cast<uint32>(GPUInfo.minConstantBufferOffsetAlignment));
        auto&        currentFrame  = m_pfd[frameIndex];

        SSAOGeometry(frameIndex, width, height);

        CollectPassBarrier(frameIndex, PS_Objects, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
        CollectPassBarrier(frameIndex, PS_Objects, LinaGX::TextureBarrierState::DepthStencilAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_DepthStencilAttachmentRead, true);
        CollectPassBarrier(frameIndex, PS_Lighting, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
        ExecPassBarriers(currentFrame.graphicsStream, LinaGX::PSF_TopOfPipe, LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment);

        BindPassSet(frameIndex, PipelineLayoutType::PL_Objects, m_passes[PS_Objects].descriptorSets[frameIndex], cameraDataIndex * camDataPadded, true);
        BeginPass(frameIndex, PassType::PS_Objects, width, height);
        DrawObjects(frameIndex, drawObjFlags, Shader::SH_Default);
        EndPass(frameIndex);

        CollectPassBarrier(frameIndex, PS_Objects, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
        CollectPassBarrier(frameIndex, PS_Objects, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_DepthStencilAttachmentRead, LinaGX::AF_ShaderRead, true);
        ExecPassBarriers(currentFrame.graphicsStream, LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment, LinaGX::PSF_FragmentShader);

        BindPassSet(frameIndex, PipelineLayoutType::PL_Lighting, m_passes[PS_Lighting].descriptorSets[frameIndex], cameraDataIndex * camDataPadded, true);
        BeginPass(frameIndex, PassType::PS_Lighting, width, height);
        BindShader(frameIndex, Shader::SH_Lighting);
        DrawFullscreenQuad(frameIndex);
        EndPass(frameIndex);

        CollectPassBarrier(frameIndex, PS_Lighting, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
        ExecPassBarriers(currentFrame.graphicsStream, LinaGX::PSF_ColorAttachment, LinaGX::PSF_FragmentShader);
    }

    void Example::IrradiancePrefilterBRDF()
    {
        for (uint32 frame = 0; frame < FRAMES_IN_FLIGHT; frame++)
        {
            auto& pfd = m_pfd[frame];

            TransferGlobalData(frame);
            BindGlobalSet(frame);
            CollectPassBarrier(frame, PS_Irradiance, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
            CollectPassBarrier(frame, PS_Prefilter, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
            CollectPassBarrier(frame, PS_BRDF, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
            CollectPassBarrier(frame, PS_Shadows, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_DepthStencilAttachmentRead, LinaGX::AF_ShaderRead, true);
            ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment, LinaGX::PSF_FragmentShader);

            for (uint32 i = 0; i < 6; i++)
            {
                DeferredRenderScene(frame, DrawObjectFlags::DrawSkybox, i + 1, ENVIRONMENT_MAP_RES, ENVIRONMENT_MAP_RES);

                {
                    CollectPassBarrier(frame, PassType::PS_Lighting, LinaGX::TextureBarrierState::TransferSource, LinaGX::AF_MemoryRead, LinaGX::AF_TransferRead);

                    LinaGX::TextureBarrier barrier = {};
                    barrier.toState                = LinaGX::TextureBarrierState::TransferDestination;
                    barrier.texture                = m_sceneCubemap;
                    barrier.isSwapchain            = false;
                    barrier.srcAccessFlags         = AF_MemoryRead | AF_MemoryWrite;
                    barrier.dstAccessFlags         = AF_TransferWrite;
                    m_passBarriers.push_back(barrier);
                    ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_TopOfPipe, LinaGX::PSF_Transfer);
                }

                LinaGX::CMDCopyTexture* copyTexture = pfd.graphicsStream->AddCommand<LinaGX::CMDCopyTexture>();
                copyTexture->srcTexture             = m_passes[PassType::PS_Lighting].renderTargets[frame].colorAttachments[0].texture;
                copyTexture->dstTexture             = m_sceneCubemap;
                copyTexture->dstLayer               = i;
                copyTexture->srcLayer               = 0;
                copyTexture->dstMip                 = 0;
                copyTexture->srcMip                 = 0;

                {
                    CollectPassBarrier(frame, PassType::PS_Lighting, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_TransferRead, LinaGX::AF_MemoryRead);
                    LinaGX::TextureBarrier barrier = {};
                    barrier.toState                = LinaGX::TextureBarrierState::ShaderRead;
                    barrier.texture                = m_sceneCubemap;
                    barrier.isSwapchain            = false;
                    barrier.srcAccessFlags         = LinaGX::AF_TransferWrite;
                    barrier.dstAccessFlags         = 0;
                    m_passBarriers.push_back(barrier);
                    ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_Transfer, LinaGX::PSF_TopOfPipe);
                }
            }

            CollectPassBarrier(frame, PassType::PS_Irradiance, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
            CollectPassBarrier(frame, PassType::PS_Prefilter, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
            CollectPassBarrier(frame, PassType::PS_BRDF, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
            ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_TopOfPipe, LinaGX::PSF_ColorAttachment);

            for (uint32 i = 0; i < 6; i++)
            {
                const uint32 camDataPadded = Utility::GetPaddedItemSize(sizeof(GPUCameraData), static_cast<uint32>(GPUInfo.minConstantBufferOffsetAlignment));
                BindPassSet(frame, PipelineLayoutType::PL_Irradiance, m_passes[PS_Irradiance].descriptorSets[frame], camDataPadded + camDataPadded * i, true);
                BeginPass(frame, PassType::PS_Irradiance, IRRADIANCE_MAP_RES, IRRADIANCE_MAP_RES, i);
                BindShader(frame, Shader::SH_Irradiance);
                DrawCube(frame);
                EndPass(frame);
            }

            BindShader(frame, Shader::SH_Prefilter);

            for (uint32 mip = 0; mip < PREFILTER_MIP_LEVELS; mip++)
            {
                unsigned int mipWidth  = static_cast<unsigned int>(PREFILTER_MAP_RES * std::pow(0.5, mip));
                unsigned int mipHeight = static_cast<unsigned int>(PREFILTER_MAP_RES * std::pow(0.5, mip));

                BindConstants(frame, static_cast<int>(static_cast<float>(mip) / static_cast<float>(PREFILTER_MIP_LEVELS - 1) * 100.0f), 0);

                for (uint32 i = 0; i < 6; i++)
                {
                    const uint32 camDataPadded = Utility::GetPaddedItemSize(sizeof(GPUCameraData), static_cast<uint32>(GPUInfo.minConstantBufferOffsetAlignment));
                    BindPassSet(frame, PipelineLayoutType::PL_Irradiance, m_passes[PS_Prefilter].descriptorSets[frame], camDataPadded + camDataPadded * i, true);
                    BeginPass(frame, PassType::PS_Prefilter, mipWidth, mipHeight, i * PREFILTER_MIP_LEVELS + mip);
                    DrawCube(frame);
                    EndPass(frame);
                }
            }

            BeginPass(frame, PassType::PS_BRDF, BRDF_MAP_RES, BRDF_MAP_RES);
            BindShader(frame, Shader::SH_BRDF);
            DrawFullscreenQuad(frame);
            EndPass(frame);

            CollectPassBarrier(frame, PS_Irradiance, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
            CollectPassBarrier(frame, PS_Prefilter, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
            CollectPassBarrier(frame, PS_BRDF, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
            ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_ColorAttachment, LinaGX::PSF_FragmentShader);

            m_lgx->CloseCommandStreams(&pfd.graphicsStream, 1);
            m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &pfd.graphicsStream, .streamCount = 1, .useWait = true, .waitCount = 1, .waitSemaphores = &pfd.transferSemaphore, .waitValues = &pfd.transferSemaphoreValue});
        }
    }

    void Example::DrawShadowmap(uint32 frame)
    {
        auto& pfd = m_pfd[frame];

        CollectPassBarrier(frame, PS_Shadows, LinaGX::TextureBarrierState::DepthStencilAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_DepthStencilAttachmentRead, true);
        ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_TopOfPipe, LinaGX::PSF_EarlyFragment);

        for (uint32 i = 0; i < 6; i++)
        {
            const uint32 cameraDataIndex = i + 7;
            const uint32 camDataPadded   = Utility::GetPaddedItemSize(sizeof(GPUCameraData), static_cast<uint32>(GPUInfo.minConstantBufferOffsetAlignment));
            BindPassSet(frame, PipelineLayoutType::PL_Simple, m_passes[PS_Shadows].descriptorSets[frame], cameraDataIndex * camDataPadded, true);
            BeginPass(frame, PassType::PS_Shadows, SHADOW_MAP_RES, SHADOW_MAP_RES, i, i);
            DrawObjects(frame, DrawObjectFlags::DrawDefault, Shader::SH_Shadows, false);
            EndPass(frame);
        }
        CollectPassBarrier(frame, PS_Shadows, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_DepthStencilAttachmentRead, LinaGX::AF_ShaderRead, true);
        ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_EarlyFragment, LinaGX::PSF_FragmentShader);
    }

    void Example::Bloom(uint32 frameIndex)
    {
        auto&  pfd        = m_pfd[frameIndex];
        bool   horizontal = true;
        uint32 amount     = 8;

        for (uint32 i = 0; i < amount; i++)
        {
            if (horizontal)
                CollectPassBarrier(frameIndex, PS_BloomHori, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
            else
                CollectPassBarrier(frameIndex, PS_BloomVert, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);

            ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_TopOfPipe, LinaGX::PSF_ColorAttachment);

            if (i == 0)
                BindPassSet(frameIndex, PipelineLayoutType::PL_SimpleQuad, m_passes[PS_BloomHori].additionalDescriptorSets[frameIndex], 0, false);
            else
            {
                if (horizontal)
                    BindPassSet(frameIndex, PipelineLayoutType::PL_SimpleQuad, m_passes[PS_BloomHori].descriptorSets[frameIndex], 0, false);
                else
                    BindPassSet(frameIndex, PipelineLayoutType::PL_SimpleQuad, m_passes[PS_BloomVert].descriptorSets[frameIndex], 0, false);
            }

            BindConstants(frameIndex, horizontal ? 1 : 0, 0);
            BeginPass(frameIndex, horizontal ? PassType::PS_BloomHori : PassType::PS_BloomVert);
            BindShader(frameIndex, Shader::SH_Bloom);
            DrawFullscreenQuad(frameIndex);
            EndPass(frameIndex);

            if (horizontal)
                CollectPassBarrier(frameIndex, PS_BloomHori, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
            else
                CollectPassBarrier(frameIndex, PS_BloomVert, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);

            ExecPassBarriers(pfd.graphicsStream, LinaGX::PSF_ColorAttachment, LinaGX::PSF_FragmentShader);
            horizontal = !horizontal;
        }
    }

    void Example::SSAOGeometry(uint32 frameIndex, uint32 width, uint32 height)
    {
        auto& currentFrame = m_pfd[frameIndex];

        CollectPassBarrier(frameIndex, PS_SSAOGeometry, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
        CollectPassBarrier(frameIndex, PS_SSAOGeometry, LinaGX::TextureBarrierState::DepthStencilAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_DepthStencilAttachmentRead, true);
        ExecPassBarriers(currentFrame.graphicsStream, LinaGX::PSF_TopOfPipe, LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment);

        BindPassSet(frameIndex, PipelineLayoutType::PL_SSAOGeometry, m_passes[PS_SSAOGeometry].descriptorSets[frameIndex], 0, true);
        BeginPass(frameIndex, PassType::PS_SSAOGeometry, width, height);
        DrawObjects(frameIndex, DrawObjectFlags::DrawDefault, Shader::SH_SSAOGeometry, false);
        EndPass(frameIndex);

        CollectPassBarrier(frameIndex, PS_SSAOGeometry, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_ColorAttachmentRead, LinaGX::AF_ShaderRead);
        CollectPassBarrier(frameIndex, PS_SSAOGeometry, LinaGX::TextureBarrierState::ShaderRead, LinaGX::AF_DepthStencilAttachmentRead, LinaGX::AF_ShaderRead, true);
        ExecPassBarriers(currentFrame.graphicsStream, LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment, LinaGX::PSF_FragmentShader);
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        m_lgx->TickWindowSystem();
        m_camera.Tick(m_deltaSeconds);
    }

    void Example::OnRender()
    {
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
            dbgUpdateTxt[0] = m_passes[PassType::PS_Objects].renderTargets[0].colorAttachments[0].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_Objects].renderTargets[1].colorAttachments[0].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_3))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_Objects].renderTargets[0].colorAttachments[1].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_Objects].renderTargets[1].colorAttachments[1].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_4))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_Objects].renderTargets[0].colorAttachments[2].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_Objects].renderTargets[1].colorAttachments[2].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_5))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_Objects].renderTargets[0].colorAttachments[3].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_Objects].renderTargets[1].colorAttachments[3].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_6))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_Objects].renderTargets[0].depthStencilAttachment.texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_Objects].renderTargets[1].depthStencilAttachment.texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_7))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_BRDF].renderTargets[0].colorAttachments[0].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_BRDF].renderTargets[1].colorAttachments[0].texture;
            dbgUpdate       = true;
        }

        if (m_lgx->GetInput().GetKeyDown(LINAGX_KEY_8))
        {
            dbgUpdateTxt[0] = m_passes[PassType::PS_Lighting].renderTargets[0].colorAttachments[1].texture;
            dbgUpdateTxt[1] = m_passes[PassType::PS_Lighting].renderTargets[1].colorAttachments[1].texture;
            dbgUpdate       = true;
        }

        if (dbgUpdate)
        {
            m_lgx->Join();

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                LinaGX::DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle = m_passes[PassType::PS_FinalQuad].descriptorSets[i],
                    .binding   = 0,
                    .textures  = {dbgUpdateTxt[i]},
                };

                m_lgx->DescriptorUpdateImage(imgUpdate);
            }
        }

        if (m_windowX == 0 || m_windowY == 0)
            return;

        // Let LinaGX know we are starting a new frame.
        m_lgx->StartFrame();

        const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&        currentFrame      = m_pfd[currentFrameIndex];
        currentFrame.boundShader       = -1;

        TransferGlobalData(currentFrameIndex);
        BindGlobalSet(currentFrameIndex);

        DrawShadowmap(currentFrameIndex);

        CollectPassBarrier(currentFrameIndex, PS_FinalQuad, LinaGX::TextureBarrierState::ColorAttachment, LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite, LinaGX::AF_ColorAttachmentRead);
        DeferredRenderScene(currentFrameIndex, DrawObjectFlags::DrawDefault | DrawObjectFlags::DrawSkybox, 0, 0, 0);

        Bloom(currentFrameIndex);

        BindPassSet(currentFrameIndex, PipelineLayoutType::PL_FinalQuad, m_passes[PS_FinalQuad].descriptorSets[currentFrameIndex], 0, false);
        BeginPass(currentFrameIndex, PassType::PS_FinalQuad);
        BindShader(currentFrameIndex, Shader::SH_Quad);
        DrawFullscreenQuad(currentFrameIndex);
        EndPass(currentFrameIndex);

        CollectPassBarrier(currentFrameIndex, PS_FinalQuad, LinaGX::TextureBarrierState::Present, LinaGX::AF_ColorAttachmentWrite, 0);
        ExecPassBarriers(currentFrame.graphicsStream, LinaGX::PSF_ColorAttachment, LinaGX::PSF_BottomOfPipe);

        // This does the actual *recording* of every single command stream alive.
        m_lgx->CloseCommandStreams(&currentFrame.graphicsStream, 1);

        // Submit work on gpu.
        m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &currentFrame.graphicsStream, .streamCount = 1, .useWait = true, .waitCount = 1, .waitSemaphores = &currentFrame.transferSemaphore, .waitValues = &currentFrame.transferSemaphoreValue});

        // Present main swapchain.
        m_lgx->Present({.swapchains = &m_swapchain, .swapchainCount = 1});

        // Let LinaGX know we are finalizing this frame.
        m_lgx->EndFrame();
    }

} // namespace LinaGX::Examples
