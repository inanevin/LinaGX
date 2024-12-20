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

#include "Utility.hpp"

namespace LinaGX::Examples
{

    std::unordered_map<PassType, std::string> g_passToString = {
        {PS_Objects, "Object Pass"},
        {PS_Lighting, "Lighting Pass"},
        {PS_FinalQuad, "Final Quad Pass"},
        {PS_Irradiance, "Irradiance Pass"},
        {PS_Prefilter, "Prefilter Pass"},
        {PS_BRDF, "BRDF Pass"},
        {PS_Shadows, "Shadow Pass"},
        {PS_BloomHori, "BloomHorizontal Pass"},
        {PS_BloomVert, "BloomVertical Pass"},
        {PS_SSAOGeometry, "SSAOPass Geometry"},
    };

    /*
        Import normals and roughness maps as linear formats.
    */
    std::vector<LINAGX_STRINGID> g_linearTextures = {
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
        "Resources/Textures/noiseTexture.png"_hs,
    };

    /*
           Normally you'd do this in an editor.
           But hey this is a sample app :)
           And we want to have custom shaders/materials controlled from code setup.
           So we are not gonna be using GLTF texture mappings.
    */
    std::unordered_map<LINAGX_STRINGID, TextureMapping> g_materialTextureMapping =
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
                               {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/park_dirt_diff_2k.jpg"},
                               {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/park_dirt_rough_2k.png"},
                               {LinaGX::GLTFTextureType::Normal, "Resources/Textures/park_dirt_nor_gl_2k.png"},
                           }},
            {"Tree"_hs, {
                            {LinaGX::GLTFTextureType::BaseColor, "Resources/Textures/island_tree_02_diff_2k.jpg"},
                            {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/island_tree_02_rough.png"},
                            {LinaGX::GLTFTextureType::Normal, "Resources/Textures/island_tree_02_nor_gl.png"},
                        }},

    };

    std::unordered_map<LINAGX_STRINGID, TextureMapping> Utility::GetMaterialTextureMapping()
    {
        return g_materialTextureMapping;
    }

    std::vector<LINAGX_STRINGID> Utility::GetLinearTextures()
    {
        return g_linearTextures;
    }

    uint32 Utility::GetPaddedItemSize(uint32 itemSize, uint32 alignment)
    {
        if (alignment == 0)
            return itemSize;
        return itemSize + (alignment - itemSize % alignment) % alignment;
    }

    std::string Utility::PassToString(PassType pt)
    {
        return g_passToString[pt];
    }

    glm::mat4 Utility::TranslateRotateScale(const LinaGX::LGXVector3& pos, const LinaGX::LGXVector4& rot, const LinaGX::LGXVector3& scale)
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

    glm::mat4 Utility::CalculateGlobalMatrix(LinaGX::ModelNode* node)
    {
        const glm::mat4 nodeLocal = node->localMatrix.empty() ? TranslateRotateScale(node->position, node->quatRot, node->scale) : glm::make_mat4(node->localMatrix.data());
        if (node->parent != nullptr)
            return CalculateGlobalMatrix(node->parent) * nodeLocal;
        return nodeLocal;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionGlobal()
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

        LinaGX::DescriptorBinding binding2 = {
            .descriptorCount = 3,
            .type            = LinaGX::DescriptorType::SeparateSampler,
            .stages          = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {
            .bindings = {binding0, binding1, binding2},
        };

        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionLightingPass()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount  = 1,
            .type             = LinaGX::DescriptorType::UBO,
            .useDynamicOffset = true,
            .stages           = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding1 = {
            .descriptorCount = 5,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding2 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding3 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding4 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding5 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding6 = {
            .descriptorCount = 3,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding7 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::UBO,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0, binding1, binding2, binding3, binding4, binding5, binding6, binding7}};

        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionSimpleQuadPass()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0}};

        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionFinalPass()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding1 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},

        };
        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0, binding1}};

        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionObjectPass()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount  = 1,
            .type             = LinaGX::DescriptorType::UBO,
            .useDynamicOffset = true,
            .stages           = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0}};

        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionObjectMaterial()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::UBO,
            .stages          = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding1 = {
            .descriptorCount = 4,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0, binding1}};
        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionIrradiancePass()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount  = 1,
            .type             = LinaGX::DescriptorType::UBO,
            .useDynamicOffset = true,
            .stages           = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding1 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0, binding1}};

        return desc;
    }

    LinaGX::TextureDesc Utility::GetRTDesc(const char* debugName, uint32 width, uint32 height)
    {
        LinaGX::TextureDesc desc = {
            .format    = LinaGX::Format::R16G16B16A16_SFLOAT,
            .views     = {{0, 0, 0, 0, false}},
            .flags     = TextureFlags::TF_ColorAttachment | TextureFlags::TF_Sampled | TextureFlags::TF_CopySource,
            .width     = width,
            .height    = height,
            .debugName = debugName,
        };

        return desc;
    }

    LinaGX::TextureDesc Utility::GetTxtDescCube(const char* debugName, uint32 width, uint32 height)
    {
        std::vector<LinaGX::ViewDesc> views;
        views.push_back({0, 0, 0, 0, true});

        LinaGX::TextureDesc desc{
            .type        = LinaGX::TextureType::Texture2D,
            .format      = LinaGX::Format::R16G16B16A16_SFLOAT,
            .views       = views,
            .flags       = TextureFlags::TF_ColorAttachment | TextureFlags::TF_Sampled | TextureFlags::TF_CopyDest | TextureFlags::TF_Cubemap,
            .width       = width,
            .height      = height,
            .arrayLength = 6,
            .debugName   = debugName,
        };

        return desc;
    }

    LinaGX::TextureDesc Utility::GetRTDescCube(const char* debugName, uint32 width, uint32 height)
    {
        std::vector<LinaGX::ViewDesc> views;

        for (uint32 i = 0; i < 6; i++)
            views.push_back({i, 0, 0, 0, false});

        views.push_back({0, 0, 0, 0, true});

        LinaGX::TextureDesc desc{
            .type        = LinaGX::TextureType::Texture2D,
            .format      = LinaGX::Format::R16G16B16A16_SFLOAT,
            .views       = views,
            .flags       = TextureFlags::TF_ColorAttachment | TextureFlags::TF_Sampled | TextureFlags::TF_CopyDest | TextureFlags::TF_Cubemap,
            .width       = width,
            .height      = height,
            .arrayLength = 6,
            .debugName   = debugName,
        };

        return desc;
    }

    LinaGX::TextureDesc Utility::GetRTDescCubeDepth(const char* debugName, uint32 width, uint32 height)
    {
        std::vector<LinaGX::ViewDesc> views;

        for (uint32 i = 0; i < 6; i++)
            views.push_back({i, 0, 0, 0, false});

        views.push_back({0, 0, 0, 0, true});

        LinaGX::TextureDesc desc{
            .type        = LinaGX::TextureType::Texture2D,
            .format      = LinaGX::Format::D32_SFLOAT,
            .views       = views,
            .flags       = TextureFlags::TF_DepthTexture | TextureFlags::TF_Sampled | TextureFlags::TF_Cubemap,
            .width       = width,
            .height      = height,
            .arrayLength = 6,
            .debugName   = debugName,
        };

        return desc;
    }

    LinaGX::TextureDesc Utility::GetRTDescPrefilter(const char* debugName, uint32 width, uint32 height, uint32 mipLevels)
    {
        std::vector<LinaGX::ViewDesc> views;

        for (uint32 arrLevel = 0; arrLevel < 6; arrLevel++)
        {
            for (uint32 mip = 0; mip < mipLevels; mip++)
                views.push_back({arrLevel, 0, mip, 0, false});
        }

        views.push_back({0, 0, 0, 0, true});

        LinaGX::TextureDesc desc{
            .type        = LinaGX::TextureType::Texture2D,
            .format      = LinaGX::Format::R16G16B16A16_SFLOAT,
            .views       = views,
            .flags       = TextureFlags::TF_ColorAttachment | TextureFlags::TF_Sampled | TextureFlags::TF_CopyDest | TextureFlags::TF_Cubemap,
            .width       = width,
            .height      = height,
            .mipLevels   = mipLevels,
            .arrayLength = 6,
            .debugName   = debugName,
        };

        return desc;
    }

    LinaGX::TextureDesc Utility::GetDepthDesc(const char* debugName, uint32 width, uint32 height)
    {
        LinaGX::TextureDesc desc = {
            .format    = LinaGX::Format::D32_SFLOAT,
            .views     = {{0, 0, 0, 0, false}},
            .flags     = TextureFlags::TF_Sampled | TextureFlags::TF_DepthTexture,
            .width     = width,
            .height    = height,
            .debugName = debugName,
        };

        return desc;
    }

    uint32 Utility::CreateShader(LinaGX::Instance* lgx, const char* vertex, const char* fragment, LinaGX::CullMode cullMode, LinaGX::Format passFormat, LinaGX::CompareOp depthCompare, LinaGX::FrontFace front, bool depthWrite, uint32 attachmentCount, bool useCustomLayout, uint16 layout, const char* debugName)
    {
        // Compile shaders.
        const std::string vtxShader  = LinaGX::ReadFileContentsAsString(vertex);
        const std::string fragShader = LinaGX::ReadFileContentsAsString(fragment);
        ShaderLayout      outLayout  = {};

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

        lgx->CompileShader(compileData, outLayout);

        // At this stage you could serialize the blobs to disk and read it next time, instead of compiling each time.
        ColorBlendAttachment blendAtt = {
            .blendEnabled        = false,
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

        std::vector<ShaderColorAttachment> colorAttachments;
        colorAttachments.resize(attachmentCount, att);

        ShaderDepthStencilDesc depthStencilDesc = {
            .depthStencilAttachmentFormat = depthWrite ? Format::D32_SFLOAT : Format::UNDEFINED,
            .depthWrite                   = depthWrite,
            .depthTest                    = true,
            .depthCompare                 = depthCompare,
            .stencilEnabled               = false,
        };

        // Create shader program with vertex & fragment stages.
        ShaderDesc shaderDesc = {
            .stages                  = compileData,
            .colorAttachments        = colorAttachments,
            .depthStencilDesc        = depthStencilDesc,
            .layout                  = outLayout,
            .polygonMode             = PolygonMode::Fill,
            .cullMode                = cullMode,
            .frontFace               = front,
            .topology                = Topology::TriangleList,
            .useCustomPipelineLayout = useCustomLayout,
            .customPipelineLayout    = layout,
            .debugName               = debugName,
        };

        uint16 shader = lgx->CreateShader(shaderDesc);

        // Compiled binaries are not needed anymore.
        for (auto& data : compileData)
            free(data.outBlob.ptr);

        return shader;
    }
} // namespace LinaGX::Examples
