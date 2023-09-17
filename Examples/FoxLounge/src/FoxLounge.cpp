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
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include <iostream>
#include <cstdarg>
#include <filesystem>
#include "LinaGX/Common/CommonData.hpp"

namespace LinaGX::Examples
{

#define MAX_OBJECTS 10
#define MAX_MATS    10
    /*
        Normally you'd do this in an editor.
        But hey this is a sample app :)
        And we want to have custom shaders/materials controlled from code setup.
        So we are not gonna be using GLTF texture mappings.
    */
    std::unordered_map<uint32, TextureMapping> g_materialTextureMapping =
        {
            {"Lantern"_hs, {
                               {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/Image-2.png"},
                               {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/Image-3.png"},
                               {LinaGX::GLTFTextureType::Normal, "Resources/Textures/Image-1.png"},
                               {LinaGX::GLTFTextureType::Emissive, "Resources/Textures/Image.png"},
                           }},

            {"Cat"_hs, {
                           {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/concrete_cat_statue_diff_2k.jpg"},
                           {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/concrete_cat_statue_metal-concrete_cat_statue_rough.png"},
                           {LinaGX::GLTFTextureType::Normal, "Resources/Textures/concrete_cat_statue_nor_gl.png"},
                       }},
            {"Fox"_hs, {
                           {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/rock_surface_diff_2k.png"},
                           {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/rock_surface_rough_2k.png"},
                           {LinaGX::GLTFTextureType::Normal, "Resources/Textures/rock_surface_nor_gl_2k.png"},
                       }},
            {"RockBig"_hs, {
                               {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/coast_rocks_05_diff_2k.jpg"},
                               {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/coast_rocks_05_rough_png.png"},
                               {LinaGX::GLTFTextureType::Normal, "Resources/Textures/coast_rocks_05_nor_gl_png.png"},
                           }},
            {"RockSmall"_hs, {
                                 {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/rock_moss_set_01_diff_2k.jpg"},
                                 {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/rock_moss_set_01_rough_2k.png"},
                                 {LinaGX::GLTFTextureType::Normal, "Resources/Textures/rock_moss_set_01_nor_gl.png"},
                             }},
            {"Terrain"_hs, {
                               {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/park_dirt_diff_2k.png"},
                               {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/park_dirt_rough_2k.png"},
                               {LinaGX::GLTFTextureType::Normal, "Resources/Textures/park_dirt_nor_gl_2k.png"},
                           }},
            {"Tree"_hs, {
                            {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/island_tree_02_diff_2k.jpg"},
                            {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/island_tree_02_nor_gl.png"},
                            {LinaGX::GLTFTextureType::Normal, "Resources/Textures/island_tree_02_rough.png"},
                        }},

    };

    /*
        Import normals and roughness maps as linear formats.
    */
    std::vector<uint32> g_linearTextures = {
        "Resources/Textures/Image-3.png"_hs,
        "Resources/Textures/Image-1.png"_hs,
        "Resources/Textures/concrete_cat_statue_metal-concrete_cat_statue_rough.png"_hs,
        "Resources/Textures/concrete_cat_statue_nor_gl.png"_hs,
        "Resources/Textures/rock_surface_rough_2k.png"_hs,
        "Resources/Textures/rock_surface_nor_dx_2k.png"_hs,
        "Resources/Textures/coast_rocks_05_rough_png.png"_hs,
        "Resources/Textures/coast_rocks_05_nor_gl_png.png"
        "Resources/Textures/rock_moss_set_01_rough_2k.png"_hs,
        "Resources/Textures/rock_moss_set_01_nor_gl.png"_hs,
        "Resources/Textures/park_dirt_rough_2k.png"_hs,
        "Resources/Textures/park_dirt_nor_gl_2k.png"_hs,
        "Resources/Textures/island_tree_02_nor_gl.png"_hs,
        "Resources/Textures/island_tree_02_rough.png"_hs,
    };

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

    void Example::ConfigureInitializeLinaGX()
    {
        LinaGX::Config.logLevel      = LogLevel::Verbose;
        LinaGX::Config.errorCallback = LogError;
        LinaGX::Config.infoCallback  = LogInfo;

        BackendAPI api = BackendAPI::DX12;

#ifdef LINAGX_PLATFORM_APPLE
        api = BackendAPI::Metal;
#endif

        LinaGX::GPUFeatures features = {
            .enableBindless = true,
        };

        LinaGX::GPULimits limits   = {};
        LinaGX::InitInfo  initInfo = InitInfo{
             .api                   = api,
             .gpu                   = PreferredGPUType::Integrated,
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

            // re-create render targets.
            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                m_lgx->DestroyTexture2D(m_pfd[i].rtWorldColor);
                m_lgx->DestroyTexture2D(m_pfd[i].rtWorldDepth);
            }

            SetupRenderTargets();

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                auto& pfd = m_pfd[i];

                LinaGX::DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle = pfd.dscSet1,
                    .binding   = 0,
                };

                for (const auto& txt : m_textures)
                    imgUpdate.textures.push_back(txt.gpuHandle);

                pfd.rtWorldColorIndex = static_cast<uint32>(imgUpdate.textures.size());
                imgUpdate.textures.push_back(pfd.rtWorldColor);
                imgUpdate.textures.push_back(pfd.rtWorldDepth);

                m_lgx->DescriptorUpdateImage(imgUpdate);
            }
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

    void Example::SetupPerFrameResources()
    {
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd                  = m_pfd[i];
            pfd.graphicsStream         = m_lgx->CreateCommandStream(100, CommandType::Graphics);
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

        // Normally you'd do such a custom setup on an editor
        // but well this is an example app :).
        for (auto& mat : m_materials)
        {
            auto        sid     = LinaGX::LGX_ToSID(mat.name.c_str());
            const auto& mapping = g_materialTextureMapping[sid];

            for (const auto& [type, path] : mapping)
                mat.textureIndices[type] = getTextureIndex(path);

            if (sid == "Terrain"_hs)
            {
                mat.gpuMat.specialTexture = getTextureIndex("Resources/Textures/noiseTexture.png");
                mat.shader                = Shader::Terrain;
            }
            else
                mat.shader = Shader::Default;
        }
    }

    void Example::LoadAndParseModels()
    {
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
        std::vector<VertexSkinned> vertices;
        std::vector<uint32>        indices;

        uint32 matIndex     = 0;
        auto   parseObjects = [&](const LinaGX::ModelData& modelData, bool manualDraw) {
            for (auto* modMat : modelData.allMaterials)
            {
                m_materials.push_back({});
                auto& mat = m_materials.back();
                mat.name  = modMat->name;

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
                wo.globalMatrix = CalculateGlobalMatrix(node);
                wo.isSkinned    = node->skin != nullptr;
                wo.hasMesh      = node->mesh != nullptr;
                wo.manualDraw   = manualDraw;

                if (!node->inverseBindMatrix.empty())
                    wo.invBindMatrix = glm::make_mat4(node->inverseBindMatrix.data());

                for (auto* p : node->mesh->primitives)
                {
                    wo.primitives.push_back({});
                    MeshPrimitive& primitive = wo.primitives.back();

                    primitive.vertexBufferStart = static_cast<uint32>(vertices.size());
                    primitive.indexBufferStart  = static_cast<uint32>(indices.size());
                    primitive.materialIndex     = p->material ? (matIndex + p->material->index) : 0;

                    for (uint32 k = 0; k < p->vertexCount; k++)
                    {
                        vertices.push_back({});
                        VertexSkinned& vtx = vertices.back();
                        vtx.position       = p->positions[k];
                        vtx.uv             = p->texCoords[k];
                        vtx.normal         = p->normals[k];

                        if (!p->weights.empty())
                            vtx.inBoneWeights = p->weights[k];

                        if (!p->jointsui16.empty())
                        {
                            vtx.inBoneIndices.x = static_cast<float>(p->jointsui16[k].x);
                            vtx.inBoneIndices.y = static_cast<float>(p->jointsui16[k].y);
                            vtx.inBoneIndices.z = static_cast<float>(p->jointsui16[k].z);
                            vtx.inBoneIndices.w = static_cast<float>(p->jointsui16[k].w);
                        }
                        else if (!p->jointsui8.empty())
                        {
                            vtx.inBoneIndices.x = static_cast<float>(p->jointsui8[k].x);
                            vtx.inBoneIndices.y = static_cast<float>(p->jointsui8[k].y);
                            vtx.inBoneIndices.z = static_cast<float>(p->jointsui8[k].z);
                            vtx.inBoneIndices.w = static_cast<float>(p->jointsui8[k].w);
                        }
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
                }
            }

            matIndex += static_cast<uint32>(modelData.allMaterials.size());
        };

        parseObjects(skyDomeData, true);
        m_skyboxIndexCount = static_cast<uint32>(indices.size());
        parseObjects(foxLoungeData, false);
        parseObjects(foxData, false);

        auto& pfd = m_pfd[0];

        // Global index buffer creation
        {
            LinaGX::ResourceDesc indexBufferDesc = {
                .size          = sizeof(uint32) * indices.size(),
                .typeHintFlags = LinaGX::TH_IndexBuffer,
                .heapType      = ResourceHeap::StagingHeap,
                .debugName     = "IB Staging",
            };

            m_indexBufferStaging      = m_lgx->CreateResource(indexBufferDesc);
            indexBufferDesc.heapType  = ResourceHeap::GPUOnly;
            indexBufferDesc.debugName = "IB GPU";
            m_indexBufferGPU          = m_lgx->CreateResource(indexBufferDesc);

            // Map to CPU & copy.
            uint8* mapped = nullptr;
            m_lgx->MapResource(m_indexBufferStaging, mapped);
            std::memcpy(mapped, indices.data(), indices.size() * sizeof(uint32));
            m_lgx->UnmapResource(m_indexBufferStaging);

            // Record the transfers.
            LinaGX::CMDCopyResource* copy = pfd.transferStream->AddCommand<LinaGX::CMDCopyResource>();
            copy->extension               = nullptr;
            copy->destination             = m_indexBufferGPU;
            copy->source                  = m_indexBufferStaging;
        }

        // Global vertex buffers.
        {
            auto createVtx = [&](VertexBuffer& b, size_t sz, const char* dbg, const char* dbg2) {
                LinaGX::ResourceDesc vtxBufDesc = {
                    .size          = sz,
                    .typeHintFlags = LinaGX::TH_VertexBuffer,
                    .heapType      = ResourceHeap::StagingHeap,
                    .debugName     = dbg,
                };

                b.staging            = m_lgx->CreateResource(vtxBufDesc);
                vtxBufDesc.heapType  = ResourceHeap::GPUOnly;
                vtxBufDesc.debugName = dbg2;
                b.gpu                = m_lgx->CreateResource(vtxBufDesc);
            };

            createVtx(m_vtxBuffer, sizeof(VertexSkinned) * vertices.size(), "VB Staging", "VB GPU");

            // Map to CPU & copy.
            uint8* mapped = nullptr;
            m_lgx->MapResource(m_vtxBuffer.staging, mapped);
            std::memcpy(mapped, vertices.data(), vertices.size() * sizeof(VertexSkinned));
            m_lgx->UnmapResource(m_vtxBuffer.staging);

            // Record the transfers.
            LinaGX::CMDCopyResource* copySkinned = pfd.transferStream->AddCommand<LinaGX::CMDCopyResource>();
            copySkinned->extension               = nullptr;
            copySkinned->destination             = m_vtxBuffer.gpu;
            copySkinned->source                  = m_vtxBuffer.staging;
        }
    }

    uint16 Example::CreateShader(const char* vertex, const char* fragment, LinaGX::CullMode cullMode, LinaGX::Format passFormat, bool useCustomLayout, uint16 customLayout, LinaGX::CompareOp depthCompare, LinaGX::FrontFace front, bool blend, bool depthWrite)
    {
        // Compile shaders.
        const std::string                         vtxShader  = LinaGX::ReadFileContentsAsString(vertex);
        const std::string                         fragShader = LinaGX::ReadFileContentsAsString(fragment);
        ShaderLayout                              outLayout  = {};
        ShaderCompileData                         dataVertex = {vtxShader, "Resources/Shaders/Include"};
        ShaderCompileData                         dataFrag   = {fragShader, "Resources/Shaders/Include"};
        std::unordered_map<ShaderStage, DataBlob> outCompiledBlobs;
        m_lgx->CompileShader({{ShaderStage::Vertex, dataVertex}, {ShaderStage::Fragment, dataFrag}}, outCompiledBlobs, outLayout);

        // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.
        ColorBlendAttachment blendAtt = {
            .blendEnabled        = blend,
            .srcColorBlendFactor = BlendFactor::SrcAlpha,
            .dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha,
            .colorBlendOp        = BlendOp::Add,
            .srcAlphaBlendFactor = BlendFactor::One,
            .dstAlphaBlendFactor = BlendFactor::Zero,
            .alphaBlendOp        = BlendOp::Add,
        };

        ShaderColorAttachment att = ShaderColorAttachment{
            .format          = passFormat,
            .blendAttachment = {},
        };

        ShaderDepthStencilDesc depthStencilDesc = {
            .depthStencilAttachmentFormat = Format::D32_SFLOAT,
            .depthWrite                   = depthWrite,
            .depthTest                    = true,
            .depthCompare                 = depthCompare,
            .stencilEnabled               = false,
        };

        // Create shader program with vertex & fragment stages.
        ShaderDesc shaderDesc = {
            .stages                  = {{ShaderStage::Vertex, outCompiledBlobs[ShaderStage::Vertex]}, {ShaderStage::Fragment, outCompiledBlobs[ShaderStage::Fragment]}},
            .colorAttachments        = {att},
            .depthStencilDesc        = depthStencilDesc,
            .layout                  = outLayout,
            .polygonMode             = PolygonMode::Fill,
            .cullMode                = cullMode,
            .frontFace               = front,
            .topology                = Topology::TriangleList,
            .useCustomPipelineLayout = useCustomLayout,
            .customPipelineLayout    = customLayout,
        };
        return m_lgx->CreateShader(shaderDesc);
    }

    void Example::SetupTextures()
    {
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

            auto isLinear = [](const char* path) -> bool {
                for (auto& linear : g_linearTextures)
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

                Texture2DDesc desc = {
                    .usage     = Texture2DUsage::ColorTexture,
                    .sampled   = true,
                    .width     = txt.allLevels[0].width,
                    .height    = txt.allLevels[0].height,
                    .mipLevels = static_cast<uint32>(txt.allLevels.size()),
                    .format    = format,
                    .debugName = txt.path.c_str(),
                };

                txt.gpuHandle = m_lgx->CreateTexture2D(desc);

                LinaGX::CMDCopyBufferToTexture2D* copyTxt = pfd.transferStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
                copyTxt->extension                        = nullptr;
                copyTxt->destTexture                      = txt.gpuHandle;
                copyTxt->mipLevels                        = static_cast<uint32>(txt.allLevels.size()),
                copyTxt->buffers                          = pfd.transferStream->EmplaceAuxMemory<TextureBuffer>(txt.allLevels.data(), sizeof(TextureBuffer) * txt.allLevels.size());
                copyTxt->destinationSlice                 = 0;
            }
        }
    }

    void Example::SetupSamplers()
    {
        LinaGX::SamplerDesc defaultSampler = {
            .mode = LinaGX::SamplerAddressMode::Repeat,
        };
        m_samplers.push_back(m_lgx->CreateSampler(defaultSampler));
    }

    void Example::SetupRenderTargets()
    {
        LinaGX::Texture2DDesc desc = {
            .usage     = LinaGX::Texture2DUsage::ColorTextureRenderTarget,
            .sampled   = true,
            .width     = m_window->GetSize().x,
            .height    = m_window->GetSize().y,
            .format    = LinaGX::Format::R32G32B32A32_SFLOAT,
            .debugName = "RT World Texture",
        };

        LinaGX::Texture2DDesc descDepth = {
            .usage              = LinaGX::Texture2DUsage::DepthStencilTexture,
            .sampled            = true,
            .depthStencilAspect = LinaGX::DepthStencilAspect::DepthOnly,
            .width              = m_window->GetSize().x,
            .height             = m_window->GetSize().y,
            .format             = LinaGX::Format::D32_SFLOAT,
            .debugName          = "RT Depth Texture"};

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd        = m_pfd[i];
            pfd.rtWorldColor = m_lgx->CreateTexture2D(desc);
            pfd.rtWorldDepth = m_lgx->CreateTexture2D(descDepth);
        }
    }

    void Example::SetupDescriptorSets()
    {

        std::vector<GPUMaterialData> materialData;

        for (auto& mat : m_materials)
        {
            mat.gpuMat.baseColor         = mat.textureIndices.at(LinaGX::GLTFTextureType::BaseColor);
            mat.gpuMat.normal            = mat.textureIndices.at(LinaGX::GLTFTextureType::Normal);
            mat.gpuMat.metallicRoughness = mat.textureIndices.at(LinaGX::GLTFTextureType::MetallicRoughness);
            auto it                      = mat.textureIndices.find(LinaGX::GLTFTextureType::Emissive);
            if (it != mat.textureIndices.end())
                mat.gpuMat.emissive = it->second;
            else
                mat.gpuMat.emissive = 0;

            materialData.push_back(mat.gpuMat);
        }

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

            LinaGX::ResourceDesc matDataResource = {
                .size          = sizeof(GPUMaterialData) * MAX_MATS,
                .typeHintFlags = LinaGX::TH_StorageBuffer,
                .heapType      = LinaGX::ResourceHeap::StagingHeap,
                .debugName     = "Material Data Buffer CPU",
            };

            pfd.rscMatDataCPU         = m_lgx->CreateResource(matDataResource);
            matDataResource.heapType  = LinaGX::ResourceHeap::GPUOnly;
            matDataResource.debugName = "Material Data Buffer GPU";
            pfd.rscMatDataGPU         = m_lgx->CreateResource(matDataResource);
            m_lgx->MapResource(pfd.rscMatDataCPU, pfd.rscMatDataCPUMapping);
            std::memcpy(pfd.rscMatDataCPUMapping, materialData.data(), sizeof(GPUMaterialData) * materialData.size());
            m_lgx->UnmapResource(pfd.rscMatDataCPU);

            // Descriptor Set 0 - Globals
            {
                LinaGX::DescriptorBinding binding0 = {
                    .descriptorCount = 1,
                    .type            = LinaGX::DescriptorType::UBO,
                    .stages          = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
                };

                LinaGX::DescriptorBinding binding1 = {
                    .descriptorCount = 1,
                    .type            = LinaGX::DescriptorType::SSBO,
                    .stages          = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
                };

                LinaGX::DescriptorSetDesc desc = {
                    .bindings = {binding0, binding1},
                };

                pfd.dscSet0 = m_lgx->CreateDescriptorSet(desc);

                LinaGX::DescriptorUpdateBufferDesc update = {
                    .setHandle = pfd.dscSet0,
                    .binding   = 0,
                    .buffers   = {pfd.rscSceneData},
                };

                m_lgx->DescriptorUpdateBuffer(update);

                LinaGX::DescriptorUpdateBufferDesc update2 = {
                    .setHandle = pfd.dscSet0,
                    .binding   = 1,
                    .buffers   = {pfd.rscObjDataGPU},
                };

                m_lgx->DescriptorUpdateBuffer(update2);
            }

            // Descriptor Set 1 - Unbounded resources
            {
                LinaGX::DescriptorBinding binding0 = {
                    .descriptorCount = 200, // max amt.
                    .type            = LinaGX::DescriptorType::SeparateImage,
                    .unbounded       = true,
                    .stages          = {LinaGX::ShaderStage::Fragment},
                };

                LinaGX::DescriptorBinding binding1 = {
                    .descriptorCount = 64, // max amt.
                    .type            = LinaGX::DescriptorType::SeparateSampler,
                    .unbounded       = true,
                    .stages          = {LinaGX::ShaderStage::Fragment},
                };

                LinaGX::DescriptorSetDesc desc = {
                    .bindings = {binding0, binding1},
                };

                pfd.dscSet1 = m_lgx->CreateDescriptorSet(desc);

                LinaGX::DescriptorUpdateImageDesc imgUpdate = {
                    .setHandle = pfd.dscSet1,
                    .binding   = 0,
                };

                for (const auto& txt : m_textures)
                    imgUpdate.textures.push_back(txt.gpuHandle);

                pfd.rtWorldColorIndex = static_cast<uint32>(imgUpdate.textures.size());
                imgUpdate.textures.push_back(pfd.rtWorldColor);
                imgUpdate.textures.push_back(pfd.rtWorldDepth);

                m_lgx->DescriptorUpdateImage(imgUpdate);

                LinaGX::DescriptorUpdateImageDesc smpUpdate = {
                    .setHandle = pfd.dscSet1,
                    .binding   = 1,
                    .samplers  = m_samplers,
                };

                m_lgx->DescriptorUpdateImage(smpUpdate);
            }

            // Descriptor Set 2 - Material data
            {
                LinaGX::DescriptorBinding binding0 = {
                    .descriptorCount = 1,
                    .type            = LinaGX::DescriptorType::SSBO,
                    .stages          = {LinaGX::ShaderStage::Fragment},
                };

                LinaGX::DescriptorSetDesc desc = {.bindings = {binding0}};

                pfd.dscSet2 = m_lgx->CreateDescriptorSet(desc);

                LinaGX::DescriptorUpdateBufferDesc update = {
                    .setHandle = pfd.dscSet2,
                    .binding   = 0,
                    .buffers   = {pfd.rscMatDataGPU},
                };

                m_lgx->DescriptorUpdateBuffer(update);
            }
        }

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            LinaGX::CMDCopyResource* copy = m_pfd[0].transferStream->AddCommand<LinaGX::CMDCopyResource>();
            copy->extension               = nullptr;
            copy->source                  = m_pfd[i].rscMatDataCPU;
            copy->destination             = m_pfd[i].rscMatDataGPU;
        }

        PipelineLayoutDesc desc = {
            .descriptorSets = {m_pfd[0].dscSet0, m_pfd[0].dscSet1, m_pfd[0].dscSet2},
            .constantRanges = {{{LinaGX::ShaderStage::Fragment, LinaGX::ShaderStage::Vertex}, sizeof(GPUConstants)}},
        };

        m_pipelineLayout = m_lgx->CreatePipelineLayout(desc);
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
            SetupPerFrameResources();
            LoadAndParseModels();
            SetupTextures();
            SetupMaterials();
            SetupSamplers();
            SetupRenderTargets();
            SetupDescriptorSets();
        }
        {
            m_shaders.resize(Shader::Max);
            m_shaders[Shader::Skybox]  = CreateShader("Resources/Shaders/skybox_vert.glsl", "Resources/Shaders/skybox_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R32G32B32A32_SFLOAT, true, m_pipelineLayout, CompareOp::LEqual, LinaGX::FrontFace::CCW, false, true);
            m_shaders[Shader::SCQuad]  = CreateShader("Resources/Shaders/screenquad_vert.glsl", "Resources/Shaders/screenquad_frag.glsl", LinaGX::CullMode::None, LinaGX::Format::B8G8R8A8_SRGB, true, m_pipelineLayout, CompareOp::Less, LinaGX::FrontFace::CCW, false, false);
            m_shaders[Shader::Terrain] = CreateShader("Resources/Shaders/terrain_pbr_vert.glsl", "Resources/Shaders/terrain_pbr_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R32G32B32A32_SFLOAT, true, m_pipelineLayout, CompareOp::Less, LinaGX::FrontFace::CCW, true, true);
            m_shaders[Shader::Default] = CreateShader("Resources/Shaders/default_pbr_vert.glsl", "Resources/Shaders/default_pbr_frag.glsl", LinaGX::CullMode::Back, LinaGX::Format::R32G32B32A32_SFLOAT, true, m_pipelineLayout, CompareOp::Less, LinaGX::FrontFace::CCW, false, true);
        }

        // Above operations will record bunch of transfer commands.
        // Start the work on the gpu while we continue on our init code.
        {
            m_lgx->CloseCommandStreams(&pfd.transferStream, 1);
            pfd.transferSemaphoreValue++;
            m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &pfd.transferStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &pfd.transferSemaphore, .signalValues = &pfd.transferSemaphoreValue});
        }

        // Make sure we wait for all gpu ops before proceeding on the CPU.
        // Then remove some resources we are done with & continue the biz.
        {
            m_lgx->WaitForUserSemaphore(pfd.transferSemaphore, pfd.transferSemaphoreValue);

            for (auto& txt : m_textures)
            {
                for (auto& level : txt.allLevels)
                    LinaGX::FreeImage(level.pixels);
            }

            // Also these are safe to release.
            m_lgx->DestroyResource(m_indexBufferStaging);
            m_lgx->DestroyResource(m_vtxBuffer.staging);

            for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                auto& pfd = m_pfd[i];
                m_lgx->DestroyResource(pfd.rscMatDataCPU);
            }
        }

        m_camera.Initialize(m_lgx);
    }

    void Example::Shutdown()
    {
        m_lgx->Join();

        // First get rid of the window.
        m_lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        for (auto s : m_shaders)
            m_lgx->DestroyShader(s);

        m_lgx->DestroySwapchain(m_swapchain);
        m_lgx->DestroyResource(m_indexBufferGPU);
        m_lgx->DestroyResource(m_vtxBuffer.gpu);
        m_lgx->DestroyPipelineLayout(m_pipelineLayout);

        for (auto smp : m_samplers)
            m_lgx->DestroySampler(smp);

        for (const auto& t2d : m_textures)
            m_lgx->DestroyTexture2D(t2d.gpuHandle);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd = m_pfd[i];
            m_lgx->DestroyCommandStream(pfd.graphicsStream);
            m_lgx->DestroyCommandStream(pfd.transferStream);
            m_lgx->DestroyUserSemaphore(pfd.transferSemaphore);
            m_lgx->DestroyResource(pfd.rscSceneData);
            m_lgx->DestroyDescriptorSet(pfd.dscSet0);
            m_lgx->DestroyDescriptorSet(pfd.dscSet1);
            m_lgx->DestroyDescriptorSet(pfd.dscSet2);
            m_lgx->DestroyTexture2D(pfd.rtWorldColor);
            m_lgx->DestroyTexture2D(pfd.rtWorldDepth);
            m_lgx->DestroyResource(pfd.rscObjDataCPU);
            m_lgx->DestroyResource(pfd.rscObjDataGPU);
            m_lgx->DestroyResource(pfd.rscMatDataGPU);
        }

        // Terminate renderer & shutdown app.
        delete m_lgx;
        App::Shutdown();
    }

    void Example::DrawObjects()
    {
        auto& currentFrame = m_pfd[m_lgx->GetCurrentFrameIndex()];

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
                BindShader(mat.shader);

                for (const auto& primData : prims)
                {
                    GPUConstants constants = {};
                    constants.int1         = primData.objIndex;
                    constants.int2         = primData.prim->materialIndex;

                    LinaGX::CMDBindConstants* ct = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindConstants>();
                    ct->extension                = nullptr;
                    ct->offset                   = 0;
                    ct->size                     = sizeof(GPUConstants);
                    ct->stages                   = currentFrame.graphicsStream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment);
                    ct->stagesSize               = 2;
                    ct->data                     = currentFrame.graphicsStream->EmplaceAuxMemory<GPUConstants>(constants);

                    LinaGX::CMDDrawIndexedInstanced* draw = currentFrame.graphicsStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
                    draw->extension                       = nullptr;
                    draw->instanceCount                   = 1;
                    draw->startInstanceLocation           = 0;
                    draw->startIndexLocation              = primData.prim->indexBufferStart;
                    draw->indexCountPerInstance           = primData.prim->indexCount;
                    draw->baseVertexLocation              = primData.prim->vertexBufferStart;
                }
            }
        };

        // DrawSkybox();

        draw();
    }

    void Example::DrawSkybox()
    {
        auto& pfd = m_pfd[m_lgx->GetCurrentFrameIndex()];

        BindShader(Shader::Skybox);

        LinaGX::CMDDrawIndexedInstanced* draw = pfd.graphicsStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
        draw->extension                       = nullptr;
        draw->baseVertexLocation              = 0;
        draw->startIndexLocation              = 0;
        draw->startInstanceLocation           = 0;
        draw->instanceCount                   = 1;
        draw->indexCountPerInstance           = m_skyboxIndexCount;
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

    void Example::OnTick()
    {
        // Check for window inputs.
        m_lgx->PollWindowsAndInput();

        // Let LinaGX know we are starting a new frame.
        m_lgx->StartFrame();

        const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
        auto&        currentFrame      = m_pfd[currentFrameIndex];

        // Scene data.
        {
            m_camera.Tick(m_deltaSeconds);
            GPUSceneData sceneData = {
                .view      = m_camera.GetView(),
                .proj      = m_camera.GetProj(),
                .skyColor1 = glm::vec4(0.002f, 0.137f, 0.004f, 1.0f),
                .skyColor2 = glm::vec4(0.0f, 0.156f, 0.278f, 1.0f),
            };
            std::memcpy(currentFrame.rscSceneDataMapping, &sceneData, sizeof(GPUSceneData));
        }

        // Obj data.
        {
            int                        i = 0;
            std::vector<GPUObjectData> objData;
            for (const auto& obj : m_worldObjects)
            {
                if (obj.manualDraw)
                    continue;

                GPUObjectData data = {};
                glm::mat4     mat  = glm::mat4(1.0f);
                glm::quat     q    = glm::quat(glm::vec3(0.0f, DEG2RAD(0.0f), 0.0f));
                mat                = TranslateRotateScale({0.0f, 0.0f, 0.0f}, {q.x, q.y, q.z, q.w}, {10.0f, 10.0f, 10.0f});
                data.modelMatrix   = mat;
                data.modelMatrix   = obj.globalMatrix;
                data.hasSkin       = i;
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
            m_lgx->CloseCommandStreams(&currentFrame.transferStream, 1);
            m_lgx->SubmitCommandStreams(transferSubmit);
        }

        // Global descriptor binding.
        {
            LinaGX::CMDBindDescriptorSets* bind = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
            bind->extension                     = nullptr;
            bind->isCompute                     = false;
            bind->firstSet                      = 0;
            bind->setCount                      = 1;
            bind->dynamicOffsetCount            = 0;
            bind->descriptorSetHandles          = currentFrame.graphicsStream->EmplaceAuxMemory<uint16>(currentFrame.dscSet0);
            bind->layoutSource                  = DescriptorSetsLayoutSource::CustomLayout;
            bind->customLayout                  = m_pipelineLayout;
        }

        // Global descriptor binding.
        {
            LinaGX::CMDBindDescriptorSets* bind = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
            bind->extension                     = nullptr;
            bind->isCompute                     = false;
            bind->firstSet                      = 1;
            bind->setCount                      = 1;
            bind->dynamicOffsetCount            = 0;
            bind->descriptorSetHandles          = currentFrame.graphicsStream->EmplaceAuxMemory<uint16>(currentFrame.dscSet1);
            bind->layoutSource                  = DescriptorSetsLayoutSource::CustomLayout;
            bind->customLayout                  = m_pipelineLayout;
        }

        // Global descriptor binding.
        {
            LinaGX::CMDBindDescriptorSets* bind = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
            bind->extension                     = nullptr;
            bind->isCompute                     = false;
            bind->firstSet                      = 2;
            bind->setCount                      = 1;
            bind->dynamicOffsetCount            = 0;
            bind->descriptorSetHandles          = currentFrame.graphicsStream->EmplaceAuxMemory<uint16>(currentFrame.dscSet2);
            bind->layoutSource                  = DescriptorSetsLayoutSource::CustomLayout;
            bind->customLayout                  = m_pipelineLayout;
        }

        // Global index buffer.
        {
            LinaGX::CMDBindIndexBuffers* index = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindIndexBuffers>();
            index->extension                   = nullptr;
            index->indexType                   = LinaGX::IndexType::Uint32;
            index->offset                      = 0;
            index->resource                    = m_indexBufferGPU;
        }

        LinaGX::CMDBindVertexBuffers* vtxNoSkin = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindVertexBuffers>();
        vtxNoSkin->extension                    = nullptr;
        vtxNoSkin->offset                       = 0;
        vtxNoSkin->slot                         = 0;
        vtxNoSkin->vertexSize                   = sizeof(VertexSkinned);
        vtxNoSkin->resource                     = m_vtxBuffer.gpu;

        Viewport     viewport = {.x = 0, .y = 0, .width = m_window->GetSize().x, .height = m_window->GetSize().y, .minDepth = 0.0f, .maxDepth = 1.0f};
        ScissorsRect sc       = {.x = 0, .y = 0, .width = m_window->GetSize().x, .height = m_window->GetSize().y};

        // Render pass begin
        {
            LinaGX::CMDBeginRenderPass* beginRenderPass = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBeginRenderPass>();
            RenderPassColorAttachment   colorAttachment;
            colorAttachment.clearColor                           = {0.1f, 0.1f, 0.1f, 1.0f};
            colorAttachment.texture                              = currentFrame.rtWorldColor;
            colorAttachment.isSwapchain                          = false;
            colorAttachment.loadOp                               = LoadOp::Clear;
            colorAttachment.storeOp                              = StoreOp::Store;
            beginRenderPass->colorAttachmentCount                = 1;
            beginRenderPass->colorAttachments                    = currentFrame.graphicsStream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->viewport                            = viewport;
            beginRenderPass->scissors                            = sc;
            beginRenderPass->depthStencilAttachment.useDepth     = true;
            beginRenderPass->depthStencilAttachment.depthLoadOp  = LoadOp::Clear;
            beginRenderPass->depthStencilAttachment.depthStoreOp = StoreOp::Store;
            beginRenderPass->depthStencilAttachment.texture      = currentFrame.rtWorldDepth;
            beginRenderPass->depthStencilAttachment.clearDepth   = 1.0f;
            beginRenderPass->depthStencilAttachment.useStencil   = false;
        }

        {
            DrawObjects();
        }

        // End render pass
        {
            LinaGX::CMDEndRenderPass* end = currentFrame.graphicsStream->AddCommand<LinaGX::CMDEndRenderPass>();
        }

        // Screen Quad Pass Begin
        {
            LinaGX::CMDBeginRenderPass* beginRenderPass = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBeginRenderPass>();
            RenderPassColorAttachment   colorAttachment;
            colorAttachment.clearColor                       = {0.6f, 0.6f, 0.6f, 1.0f};
            colorAttachment.texture                          = static_cast<uint32>(m_swapchain);
            colorAttachment.isSwapchain                      = true;
            colorAttachment.loadOp                           = LoadOp::Clear;
            colorAttachment.storeOp                          = StoreOp::Store;
            beginRenderPass->colorAttachmentCount            = 1;
            beginRenderPass->colorAttachments                = currentFrame.graphicsStream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->viewport                        = viewport;
            beginRenderPass->scissors                        = sc;
            beginRenderPass->depthStencilAttachment.useDepth = beginRenderPass->depthStencilAttachment.useStencil = false;
        }

        // Bind quad shader.
        {
            LinaGX::CMDBindPipeline* pipeline = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindPipeline>();
            pipeline->extension               = nullptr;
            pipeline->shader                  = m_shaders[Shader::SCQuad];
        }

        // Constants.
        {
            GPUConstants constants = {};
            constants.int1         = currentFrame.rtWorldColorIndex;
            constants.int2         = 0;

            LinaGX::CMDBindConstants* ct = currentFrame.graphicsStream->AddCommand<LinaGX::CMDBindConstants>();
            ct->extension                = nullptr;
            ct->offset                   = 0;
            ct->size                     = sizeof(GPUConstants);
            ct->stages                   = currentFrame.graphicsStream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment);
            ct->stagesSize               = 2;
            ct->data                     = currentFrame.graphicsStream->EmplaceAuxMemory<GPUConstants>(constants);
        }

        // Draw quad.
        {
            BindShader(Shader::SCQuad);

            CMDDrawInstanced* draw       = currentFrame.graphicsStream->AddCommand<CMDDrawInstanced>();
            draw->instanceCount          = 1;
            draw->startInstanceLocation  = 0;
            draw->startVertexLocation    = 0;
            draw->vertexCountPerInstance = 6;
        }

        // End quad pass.
        {
            LinaGX::CMDEndRenderPass* end = currentFrame.graphicsStream->AddCommand<LinaGX::CMDEndRenderPass>();
        }

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
