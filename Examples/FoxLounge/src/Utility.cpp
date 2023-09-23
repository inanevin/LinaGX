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

#include "Utility.hpp"

namespace LinaGX::Examples
{

    /*
           Normally you'd do this in an editor.
           But hey this is a sample app :)
           And we want to have custom shaders/materials controlled from code setup.
           So we are not gonna be using GLTF texture mappings.
    */
    std::unordered_map<LINAGX_STRINGID, TextureMapping> Utility::GetMaterialTextureMapping()
    {
        std::unordered_map<LINAGX_STRINGID, TextureMapping> materialTextureMapping =
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
                                {LinaGX::GLTFTextureType::MetallicRoughness, "Resources/Textures/island_tree_02_rough.png"},
                                {LinaGX::GLTFTextureType::Normal, "Resources/Textures/island_tree_02_nor_gl.png"},
                            }},

            };

        return materialTextureMapping;
    }

    /*
        Import normals and roughness maps as linear formats.
    */
    std::vector<LINAGX_STRINGID> Utility::GetLinearTextures()
    {
        std::vector<LINAGX_STRINGID> linearTextures = {
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
        return linearTextures;
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
            .descriptorCount = 32, // max amt.
            .type            = LinaGX::DescriptorType::SeparateImage,
            .unbounded       = true,
            .stages          = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorBinding binding3 = {
            .descriptorCount = 1, // max amt.
            .type            = LinaGX::DescriptorType::SeparateSampler,
            .stages          = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {
            .bindings = {binding0, binding1, binding2, binding3},
        };

        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionCameraData()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::UBO,
            .stages          = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0}};
        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionLightingPass()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount = 3,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = {LinaGX::ShaderStage::Fragment},
        };

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0}};

        return desc;
    }

    LinaGX::DescriptorSetDesc Utility::GetSetDescriptionQuadPass()
    {
        LinaGX::DescriptorBinding binding0 = {
            .descriptorCount = 1,
            .type            = LinaGX::DescriptorType::SeparateImage,
            .stages          = { LinaGX::ShaderStage::Fragment},
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

        LinaGX::DescriptorSetDesc desc = {.bindings = {binding0}};

        return desc;
    }

    uint32 Utility::CreateShader(LinaGX::Instance* lgx, const char* vertex, const char* fragment, LinaGX::CullMode cullMode, LinaGX::Format passFormat, LinaGX::CompareOp depthCompare, LinaGX::FrontFace front, bool depthWrite, bool gBuffer, bool useCustomLayout, uint16 layout)
    {
        // Compile shaders.
        const std::string                         vtxShader  = LinaGX::ReadFileContentsAsString(vertex);
        const std::string                         fragShader = LinaGX::ReadFileContentsAsString(fragment);
        ShaderLayout                              outLayout  = {};
        ShaderCompileData                         dataVertex = {vtxShader, "Resources/Shaders/Include"};
        ShaderCompileData                         dataFrag   = {fragShader, "Resources/Shaders/Include"};
        std::unordered_map<ShaderStage, DataBlob> outCompiledBlobs;
        lgx->CompileShader({{ShaderStage::Vertex, dataVertex}, {ShaderStage::Fragment, dataFrag}}, outCompiledBlobs, outLayout);

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

        colorAttachments.push_back(att);

        if (gBuffer)
        {
            colorAttachments.push_back(att);
            colorAttachments.push_back(att);
        }

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
            .colorAttachments        = colorAttachments,
            .depthStencilDesc        = depthStencilDesc,
            .layout                  = outLayout,
            .polygonMode             = PolygonMode::Fill,
            .cullMode                = cullMode,
            .frontFace               = front,
            .topology                = Topology::TriangleList,
            .useCustomPipelineLayout = useCustomLayout,
            .customPipelineLayout    = layout,
        };

        return lgx->CreateShader(shaderDesc);
    }
} // namespace LinaGX::Examples
