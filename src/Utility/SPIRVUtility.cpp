/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2023-] [Inan Evin]

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

#include "Utility/SPIRVUtility.hpp"
#include "spirv_cross/spirv_glsl.hpp"
#include "spirv_cross/spirv_msl.hpp"
#include "spirv_cross/spirv_hlsl.hpp"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/SPIRV/spirv.hpp"
#include "glslang/Public/ShaderLang.h"
#include <sstream>

namespace LinaGX
{
    void SPIRVUtility::Initialize()
    {
        glslang::InitializeProcess();
    }

    void SPIRVUtility::Shutdown()
    {
        glslang::FinalizeProcess();
    }

    void SPIRVUtility::InitResources(TBuiltInResource& resources)
    {
        resources.maxLights                                   = 32;
        resources.maxClipPlanes                               = 6;
        resources.maxTextureUnits                             = 32;
        resources.maxTextureCoords                            = 32;
        resources.maxVertexAttribs                            = 64;
        resources.maxVertexUniformComponents                  = 4096;
        resources.maxVaryingFloats                            = 64;
        resources.maxVertexTextureImageUnits                  = 32;
        resources.maxCombinedTextureImageUnits                = 80;
        resources.maxTextureImageUnits                        = 32;
        resources.maxFragmentUniformComponents                = 4096;
        resources.maxDrawBuffers                              = 32;
        resources.maxVertexUniformVectors                     = 128;
        resources.maxVaryingVectors                           = 8;
        resources.maxFragmentUniformVectors                   = 16;
        resources.maxVertexOutputVectors                      = 16;
        resources.maxFragmentInputVectors                     = 15;
        resources.minProgramTexelOffset                       = -8;
        resources.maxProgramTexelOffset                       = 7;
        resources.maxClipDistances                            = 8;
        resources.maxComputeWorkGroupCountX                   = 65535;
        resources.maxComputeWorkGroupCountY                   = 65535;
        resources.maxComputeWorkGroupCountZ                   = 65535;
        resources.maxComputeWorkGroupSizeX                    = 1024;
        resources.maxComputeWorkGroupSizeY                    = 1024;
        resources.maxComputeWorkGroupSizeZ                    = 64;
        resources.maxComputeUniformComponents                 = 1024;
        resources.maxComputeTextureImageUnits                 = 16;
        resources.maxComputeImageUniforms                     = 8;
        resources.maxComputeAtomicCounters                    = 8;
        resources.maxComputeAtomicCounterBuffers              = 1;
        resources.maxVaryingComponents                        = 60;
        resources.maxVertexOutputComponents                   = 64;
        resources.maxGeometryInputComponents                  = 64;
        resources.maxGeometryOutputComponents                 = 128;
        resources.maxFragmentInputComponents                  = 128;
        resources.maxImageUnits                               = 8;
        resources.maxCombinedImageUnitsAndFragmentOutputs     = 8;
        resources.maxCombinedShaderOutputResources            = 8;
        resources.maxImageSamples                             = 0;
        resources.maxVertexImageUniforms                      = 0;
        resources.maxTessControlImageUniforms                 = 0;
        resources.maxTessEvaluationImageUniforms              = 0;
        resources.maxGeometryImageUniforms                    = 0;
        resources.maxFragmentImageUniforms                    = 8;
        resources.maxCombinedImageUniforms                    = 8;
        resources.maxGeometryTextureImageUnits                = 16;
        resources.maxGeometryOutputVertices                   = 256;
        resources.maxGeometryTotalOutputComponents            = 1024;
        resources.maxGeometryUniformComponents                = 1024;
        resources.maxGeometryVaryingComponents                = 64;
        resources.maxTessControlInputComponents               = 128;
        resources.maxTessControlOutputComponents              = 128;
        resources.maxTessControlTextureImageUnits             = 16;
        resources.maxTessControlUniformComponents             = 1024;
        resources.maxTessControlTotalOutputComponents         = 4096;
        resources.maxTessEvaluationInputComponents            = 128;
        resources.maxTessEvaluationOutputComponents           = 128;
        resources.maxTessEvaluationTextureImageUnits          = 16;
        resources.maxTessEvaluationUniformComponents          = 1024;
        resources.maxTessPatchComponents                      = 120;
        resources.maxPatchVertices                            = 32;
        resources.maxTessGenLevel                             = 64;
        resources.maxViewports                                = 16;
        resources.maxVertexAtomicCounters                     = 0;
        resources.maxTessControlAtomicCounters                = 0;
        resources.maxTessEvaluationAtomicCounters             = 0;
        resources.maxGeometryAtomicCounters                   = 0;
        resources.maxFragmentAtomicCounters                   = 8;
        resources.maxCombinedAtomicCounters                   = 8;
        resources.maxAtomicCounterBindings                    = 1;
        resources.maxVertexAtomicCounterBuffers               = 0;
        resources.maxTessControlAtomicCounterBuffers          = 0;
        resources.maxTessEvaluationAtomicCounterBuffers       = 0;
        resources.maxGeometryAtomicCounterBuffers             = 0;
        resources.maxFragmentAtomicCounterBuffers             = 1;
        resources.maxCombinedAtomicCounterBuffers             = 1;
        resources.maxAtomicCounterBufferSize                  = 16384;
        resources.maxTransformFeedbackBuffers                 = 4;
        resources.maxTransformFeedbackInterleavedComponents   = 64;
        resources.maxCullDistances                            = 8;
        resources.maxCombinedClipAndCullDistances             = 8;
        resources.maxSamples                                  = 4;
        resources.maxMeshOutputVerticesNV                     = 256;
        resources.maxMeshOutputPrimitivesNV                   = 512;
        resources.maxMeshWorkGroupSizeX_NV                    = 32;
        resources.maxMeshWorkGroupSizeY_NV                    = 1;
        resources.maxMeshWorkGroupSizeZ_NV                    = 1;
        resources.maxTaskWorkGroupSizeX_NV                    = 32;
        resources.maxTaskWorkGroupSizeY_NV                    = 1;
        resources.maxTaskWorkGroupSizeZ_NV                    = 1;
        resources.maxMeshViewCountNV                          = 4;
        resources.limits.nonInductiveForLoops                 = 1;
        resources.limits.whileLoops                           = 1;
        resources.limits.doWhileLoops                         = 1;
        resources.limits.generalUniformIndexing               = 1;
        resources.limits.generalAttributeMatrixVectorIndexing = 1;
        resources.limits.generalVaryingIndexing               = 1;
        resources.limits.generalSamplerIndexing               = 1;
        resources.limits.generalVariableIndexing              = 1;
        resources.limits.generalConstantMatrixVectorIndexing  = 1;
    }

    void SPIRVUtility::GetShaderTextWithIncludes(LINAGX_STRING& outStr, const char* shader, const char* includePath)
    {
        std::istringstream f(shader);
        LINAGX_STRING      line = "";
        outStr.clear();
        bool isCommentBlock = false;

        const LINAGX_STRING includeText = "#include";

        while (std::getline(f, line))
        {
            if (!line.empty() && *line.rbegin() == '\r')
                line.erase(line.end() - 1);

            if (isCommentBlock)
            {
                if (line.find("*/") != std::string::npos)
                    isCommentBlock = false;

                continue;
            }

            const size_t include = line.find(includeText);

            if (include != std::string::npos)
            {

                std::size_t firstQuote = line.find('\"');
                std::size_t lastQuote  = line.rfind('\"');

                if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote != lastQuote)
                {
                    std::string         filename = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                    const LINAGX_STRING str      = LINAGX_STRING(includePath) + "/" + filename;

                    LINAGX_STRING contents      = Internal::ReadFileContentsAsString(str.c_str());
                    const size_t  endOfComments = contents.find("*/");

                    // Remove comments if exists.
                    if (endOfComments != std::string::npos)
                        contents = contents.substr(endOfComments + 2, contents.size() - endOfComments - 2);

                    outStr += contents.c_str();
                    continue;
                }
            }

            outStr += line + "\n";
        }
    }

    EShLanguage FindLanguage(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::Vertex:
            return EShLangVertex;
        case ShaderStage::Geometry:
            return EShLangGeometry;
        case ShaderStage::Pixel:
            return EShLangFragment;
        case ShaderStage::Compute:
            return EShLangCompute;
        default:
            return EShLangVertex;
        }
    }

    glslang_stage_t GetStage(ShaderStage stg)
    {
        switch (stg)
        {
        case ShaderStage::Vertex:
            return glslang_stage_t::GLSLANG_STAGE_VERTEX;
        case ShaderStage::Geometry:
            return glslang_stage_t::GLSLANG_STAGE_GEOMETRY;
        case ShaderStage::Pixel:
            return glslang_stage_t::GLSLANG_STAGE_FRAGMENT;
        case ShaderStage::Compute:
            return glslang_stage_t::GLSLANG_STAGE_COMPUTE;
        default:
            return glslang_stage_t::GLSLANG_STAGE_VERTEX;
        }
    }

    const glslang_resource_t DefaultTBuiltInResource = {
        /* .MaxLights = */ 32,
        /* .MaxClipPlanes = */ 6,
        /* .MaxTextureUnits = */ 32,
        /* .MaxTextureCoords = */ 32,
        /* .MaxVertexAttribs = */ 64,
        /* .MaxVertexUniformComponents = */ 4096,
        /* .MaxVaryingFloats = */ 64,
        /* .MaxVertexTextureImageUnits = */ 32,
        /* .MaxCombinedTextureImageUnits = */ 80,
        /* .MaxTextureImageUnits = */ 32,
        /* .MaxFragmentUniformComponents = */ 4096,
        /* .MaxDrawBuffers = */ 32,
        /* .MaxVertexUniformVectors = */ 128,
        /* .MaxVaryingVectors = */ 8,
        /* .MaxFragmentUniformVectors = */ 16,
        /* .MaxVertexOutputVectors = */ 16,
        /* .MaxFragmentInputVectors = */ 15,
        /* .MinProgramTexelOffset = */ -8,
        /* .MaxProgramTexelOffset = */ 7,
        /* .MaxClipDistances = */ 8,
        /* .MaxComputeWorkGroupCountX = */ 65535,
        /* .MaxComputeWorkGroupCountY = */ 65535,
        /* .MaxComputeWorkGroupCountZ = */ 65535,
        /* .MaxComputeWorkGroupSizeX = */ 1024,
        /* .MaxComputeWorkGroupSizeY = */ 1024,
        /* .MaxComputeWorkGroupSizeZ = */ 64,
        /* .MaxComputeUniformComponents = */ 1024,
        /* .MaxComputeTextureImageUnits = */ 16,
        /* .MaxComputeImageUniforms = */ 8,
        /* .MaxComputeAtomicCounters = */ 8,
        /* .MaxComputeAtomicCounterBuffers = */ 1,
        /* .MaxVaryingComponents = */ 60,
        /* .MaxVertexOutputComponents = */ 64,
        /* .MaxGeometryInputComponents = */ 64,
        /* .MaxGeometryOutputComponents = */ 128,
        /* .MaxFragmentInputComponents = */ 128,
        /* .MaxImageUnits = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
        /* .MaxCombinedShaderOutputResources = */ 8,
        /* .MaxImageSamples = */ 0,
        /* .MaxVertexImageUniforms = */ 0,
        /* .MaxTessControlImageUniforms = */ 0,
        /* .MaxTessEvaluationImageUniforms = */ 0,
        /* .MaxGeometryImageUniforms = */ 0,
        /* .MaxFragmentImageUniforms = */ 8,
        /* .MaxCombinedImageUniforms = */ 8,
        /* .MaxGeometryTextureImageUnits = */ 16,
        /* .MaxGeometryOutputVertices = */ 256,
        /* .MaxGeometryTotalOutputComponents = */ 1024,
        /* .MaxGeometryUniformComponents = */ 1024,
        /* .MaxGeometryVaryingComponents = */ 64,
        /* .MaxTessControlInputComponents = */ 128,
        /* .MaxTessControlOutputComponents = */ 128,
        /* .MaxTessControlTextureImageUnits = */ 16,
        /* .MaxTessControlUniformComponents = */ 1024,
        /* .MaxTessControlTotalOutputComponents = */ 4096,
        /* .MaxTessEvaluationInputComponents = */ 128,
        /* .MaxTessEvaluationOutputComponents = */ 128,
        /* .MaxTessEvaluationTextureImageUnits = */ 16,
        /* .MaxTessEvaluationUniformComponents = */ 1024,
        /* .MaxTessPatchComponents = */ 120,
        /* .MaxPatchVertices = */ 32,
        /* .MaxTessGenLevel = */ 64,
        /* .MaxViewports = */ 16,
        /* .MaxVertexAtomicCounters = */ 0,
        /* .MaxTessControlAtomicCounters = */ 0,
        /* .MaxTessEvaluationAtomicCounters = */ 0,
        /* .MaxGeometryAtomicCounters = */ 0,
        /* .MaxFragmentAtomicCounters = */ 8,
        /* .MaxCombinedAtomicCounters = */ 8,
        /* .MaxAtomicCounterBindings = */ 1,
        /* .MaxVertexAtomicCounterBuffers = */ 0,
        /* .MaxTessControlAtomicCounterBuffers = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
        /* .MaxGeometryAtomicCounterBuffers = */ 0,
        /* .MaxFragmentAtomicCounterBuffers = */ 1,
        /* .MaxCombinedAtomicCounterBuffers = */ 1,
        /* .MaxAtomicCounterBufferSize = */ 16384,
        /* .MaxTransformFeedbackBuffers = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances = */ 8,
        /* .MaxCombinedClipAndCullDistances = */ 8,
        /* .MaxSamples = */ 4,
        /* .maxMeshOutputVerticesNV = */ 256,
        /* .maxMeshOutputPrimitivesNV = */ 512,
        /* .maxMeshWorkGroupSizeX_NV = */ 32,
        /* .maxMeshWorkGroupSizeY_NV = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV = */ 1,
        /* .maxTaskWorkGroupSizeX_NV = */ 32,
        /* .maxTaskWorkGroupSizeY_NV = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV = */ 1,
        /* .maxMeshViewCountNV = */ 4,
        /* .maxMeshOutputVerticesEXT = */ 256,
        /* .maxMeshOutputPrimitivesEXT = */ 256,
        /* .maxMeshWorkGroupSizeX_EXT = */ 128,
        /* .maxMeshWorkGroupSizeY_EXT = */ 128,
        /* .maxMeshWorkGroupSizeZ_EXT = */ 128,
        /* .maxTaskWorkGroupSizeX_EXT = */ 128,
        /* .maxTaskWorkGroupSizeY_EXT = */ 128,
        /* .maxTaskWorkGroupSizeZ_EXT = */ 128,
        /* .maxMeshViewCountEXT = */ 4,
        /* .maxDualSourceDrawBuffersEXT = */ 1,

        /* .limits = */ {
            /* .nonInductiveForLoops = */ 1,
            /* .whileLoops = */ 1,
            /* .doWhileLoops = */ 1,
            /* .generalUniformIndexing = */ 1,
            /* .generalAttributeMatrixVectorIndexing = */ 1,
            /* .generalVaryingIndexing = */ 1,
            /* .generalSamplerIndexing = */ 1,
            /* .generalVariableIndexing = */ 1,
            /* .generalConstantMatrixVectorIndexing = */ 1,
        }};

    ShaderMemberType GetMemberType(spirv_cross::SPIRType type)
    {
        ShaderMemberType outType = ShaderMemberType::Float;
        const bool       isFloat = type.basetype == spirv_cross::SPIRType::BaseType::Float;

        if (type.columns == 1)
        {
            if (type.vecsize == 1)
                outType = isFloat ? ShaderMemberType::Float : ShaderMemberType::Int;
            else if (type.vecsize == 2)
                outType = isFloat ? ShaderMemberType::Float2 : ShaderMemberType::Int2;
            else if (type.vecsize == 3)
                outType = isFloat ? ShaderMemberType::Float3 : ShaderMemberType::Int3;
            else
                outType = isFloat ? ShaderMemberType::Float4 : ShaderMemberType::Int4;
        }
        else if (type.columns == 2 && type.vecsize == 2)
        {
            outType = ShaderMemberType::Mat2x2;
        }
        else if (type.columns == 3 && type.vecsize == 3)
            outType = ShaderMemberType::Mat3x3;
        else if (type.columns == 4 && type.vecsize == 4)
            outType = ShaderMemberType::Mat4x4;
        else
            outType = ShaderMemberType::MatUnknown;

        return outType;
    }

    void FillStructMembers(spirv_cross::CompilerGLSL& compiler, spirv_cross::SPIRType type, LINAGX_VEC<UBOMember>& members)
    {
        unsigned int memberCount = static_cast<unsigned int>(type.member_types.size());
        for (unsigned int i = 0; i < memberCount; i++)
        {
            UBOMember member;

            auto& member_type = compiler.get_type(type.member_types[i]);
            member.size       = compiler.get_declared_struct_member_size(type, i);
            member.type       = GetMemberType(member_type);
            member.offset     = compiler.type_struct_member_offset(type, i);

            if (!member_type.array.empty())
            {
                // Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
                member.arrayStride = compiler.type_struct_member_array_stride(type, i);
                member.isArray     = true;
                member.arraySize   = member_type.array[0];
            }

            if (member_type.columns > 1)
            {
                // Get bytes stride between columns (if column major), for float4x4 -> 16 bytes.
                member.matrixStride = compiler.type_struct_member_matrix_stride(type, i);
            }

            member.name = compiler.get_member_name(type.self, i);
            members.push_back(member);
        }
    };

    bool SPIRVUtility::GLSL2SPV(ShaderStage stg, const char* pShader, const char* includePath, CompiledShaderBlob& compiledBlob, ShaderLayout& outLayout)
    {
        glslang_initialize_process();

        glslang_stage_t stage = GetStage(stg);

        LINAGX_STRING fullShaderStr = "";
        GetShaderTextWithIncludes(fullShaderStr, pShader, includePath);

        glslang_input_t input                   = {};
        input.language                          = GLSLANG_SOURCE_GLSL;
        input.stage                             = stage;
        input.client                            = GLSLANG_CLIENT_VULKAN;
        input.target_language                   = GLSLANG_TARGET_SPV;
        input.target_language_version           = GLSLANG_TARGET_SPV_1_0;
        input.code                              = fullShaderStr.c_str();
        input.default_version                   = 100;
        input.default_profile                   = GLSLANG_NO_PROFILE;
        input.force_default_version_and_profile = false;
        input.forward_compatible                = false;
        input.messages                          = GLSLANG_MSG_DEFAULT_BIT;
        input.resource                          = &DefaultTBuiltInResource;

        glslang_shader_t* shader = glslang_shader_create(&input);

        if (!glslang_shader_preprocess(shader, &input))
        {
            LOGE("GLSL2SPV -> Preprocess failed %s, %s", glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
            return false;
        }

        if (!glslang_shader_parse(shader, &input))
        {
            LOGE("GLSL2SPV -> Parsing failed %s, %s", glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
            return false;
        }

        glslang_program_t* program = glslang_program_create();
        glslang_program_add_shader(program, shader);

        if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT))
        {
            LOGE("GLSL2SPV -> Linking failed %s, %s", glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
            return false;
        }

        glslang_program_SPIRV_generate(program, stage);

        std::vector<uint32> spirvBinary;
        const size_t        elemCount = glslang_program_SPIRV_get_size(program);
        spirvBinary.resize(elemCount);
        glslang_program_SPIRV_get(program, spirvBinary.data());

        // Copy to blob
        {
            const size_t programSize = elemCount * sizeof(uint32);
            compiledBlob.ptr         = new uint8[programSize];
            compiledBlob.size        = programSize;
            LINAGX_MEMCPY(compiledBlob.ptr, spirvBinary.data(), programSize);
        }

        const char* spirv_messages =
            glslang_program_SPIRV_get_messages(program);

        if (spirv_messages)
            fprintf(stderr, "%s", spirv_messages);

        glslang_program_delete(program);
        glslang_shader_delete(shader);

        glslang_finalize_process();

        spirv_cross::CompilerGLSL    compiler(std::move(spirvBinary));
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Stage Inputs
        {
            if (stg == ShaderStage::Vertex)
            {
                for (const auto& resource : resources.stage_inputs)
                {
                    // Get the SPIR-V ID of the stage input
                    auto id = resource.id;

                    // Get the location of this input
                    uint32_t location = compiler.get_decoration(id, spv::DecorationLocation);

                    // Get type information about the input
                    const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);

                    const bool isFloat = type.basetype == spirv_cross::SPIRType::Float;
                    StageInput input   = {};
                    input.location     = location;
                    input.elements     = type.vecsize;
                    input.size         = type.vecsize * type.columns * (isFloat ? sizeof(float) : sizeof(int));
                    if (type.vecsize == 1)
                        input.format = isFloat ? Format::R32_SFLOAT : Format::R32_SINT;
                    else if (type.vecsize == 2)
                        input.format = isFloat ? Format::R32G32_SFLOAT : Format::R32G32_SINT;
                    else if (type.vecsize == 3)
                        input.format = isFloat ? Format::R32G32B32_SFLOAT : Format::R32G32B32_SINT;
                    else
                        input.format = isFloat ? Format::R32G32B32A32_SFLOAT : Format::R32G32B32A32_SINT;

                    outLayout.vertexInputs.push_back(input);
                }

                // Sort & set the offsets of the inputs.
                std::sort(outLayout.vertexInputs.begin(), outLayout.vertexInputs.end(), [](const StageInput& i1, const StageInput& i2) { return i1.location < i2.location; });
                size_t totalOffset = 0;
                for (auto& input : outLayout.vertexInputs)
                {
                    input.offset = totalOffset;
                    totalOffset += input.size;
                }
            }
        }

        // Pipeline layout & signature
        {
            for (const auto& resource : resources.uniform_buffers)
            {
                // Get the SPIR-V ID of the uniform buffer
                auto id = resource.id;

                UBO ubo     = {};
                ubo.set     = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                ubo.binding = compiler.get_decoration(id, spv::DecorationBinding);

                auto it = std::find_if(outLayout.ubos.begin(), outLayout.ubos.end(), [ubo](const UBO& existing) { return ubo.set == existing.set && ubo.binding == existing.binding; });
                if (it != outLayout.ubos.end())
                {
                    it->stages.push_back(stg);
                    continue;
                }
                ubo.stages.push_back(stg);

                const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
                ubo.size                          = compiler.get_declared_struct_size(type);
                ubo.name                          = compiler.get_name(resource.id);

                FillStructMembers(compiler, type, ubo.members);
                outLayout.ubos.push_back(ubo);
            }

            for (auto& resource : resources.push_constant_buffers)
            {
                // Get the SPIR-V ID of the uniform buffer
                auto id = resource.id;

                ConstantBlock block = {};

                auto it = std::find_if(outLayout.constantBuffers.begin(), outLayout.constantBuffers.end(), [block](const ConstantBlock& existing) { return existing.name.compare(block.name) == 0; });
                if (it != outLayout.constantBuffers.end())
                {
                    it->stages.push_back(stg);
                    continue;
                }
                block.stages.push_back(stg);

                const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
                block.size                        = compiler.get_declared_struct_size(type);
                block.name                        = compiler.get_name(resource.id);
                FillStructMembers(compiler, type, block.members);
                outLayout.constantBuffers.push_back(block);
            }

            for (const auto& resource : resources.sampled_images)
            {
                auto         id  = resource.id;
                SRVTexture2D txt = {};

                // Get the set and binding number for this uniform buffer
                txt.set     = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                txt.binding = compiler.get_decoration(id, spv::DecorationBinding);

                auto it = std::find_if(outLayout.texture2ds.begin(), outLayout.texture2ds.end(), [txt](const SRVTexture2D& existing) { return txt.set == existing.set && txt.binding == existing.binding; });
                if (it != outLayout.texture2ds.end())
                {
                    it->stages.push_back(stg);
                    continue;
                }
                txt.stages.push_back(stg);

                // Get type information about the uniform buffer
                const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
                txt.name                          = compiler.get_name(resource.id);
                outLayout.texture2ds.push_back(txt);
            }

            for (const auto& resource : resources.separate_samplers)
            {
                auto    id      = resource.id;
                Sampler sampler = {};

                // Get the set and binding number for this uniform buffer
                sampler.set     = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                sampler.binding = compiler.get_decoration(id, spv::DecorationBinding);

                auto it = std::find_if(outLayout.samplers.begin(), outLayout.samplers.end(), [sampler](const Sampler& existing) { return sampler.set == existing.set && sampler.binding == existing.binding; });
                if (it != outLayout.samplers.end())
                {
                    it->stages.push_back(stg);
                    continue;
                }
                sampler.stages.push_back(stg);

                // Get type information about the uniform buffer
                const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
                sampler.name                      = compiler.get_name(resource.id);
                outLayout.samplers.push_back(sampler);
            }

            for (const auto& resource : resources.storage_buffers)
            {
                auto id   = resource.id;
                SSBO ssbo = {};

                // Get the set and binding number for this uniform buffer
                ssbo.set     = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                ssbo.binding = compiler.get_decoration(id, spv::DecorationBinding);

                auto it = std::find_if(outLayout.ssbos.begin(), outLayout.ssbos.end(), [ssbo](const SSBO& existing) { return ssbo.set == existing.set && ssbo.binding == existing.binding; });
                if (it != outLayout.ssbos.end())
                {
                    it->stages.push_back(stg);
                    continue;
                }
                ssbo.stages.push_back(stg);

                // Get type information about the uniform buffer
                const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
                ssbo.name                         = compiler.get_name(resource.id);
                outLayout.ssbos.push_back(ssbo);
            }
        }
        return true;
    }

    bool SPIRVUtility::SPV2HLSL(ShaderStage stg, const CompiledShaderBlob& spv, LINAGX_STRING& out)
    {
        try
        {
            spirv_cross::CompilerHLSL compiler(reinterpret_cast<uint32*>(spv.ptr), spv.size / sizeof(uint32));

            spirv_cross::CompilerHLSL::Options options;
            options.shader_model = 60; // SM6_0
            compiler.set_hlsl_options(options);

            // Perform the conversion
            out = compiler.compile();
        }
        catch (spirv_cross::CompilerError& e)
        {
            const LINAGX_STRING err = e.what();
            const LINAGX_STRING log = "SPV2HLSL -> " + err;
            LOGE(log.c_str());
            return false;
        }

        return true;
    }

    bool SPIRVUtility::SPV2MSL(ShaderStage stg, const CompiledShaderBlob& spv, LINAGX_STRING& out)
    {
        try
        {
            spirv_cross::CompilerMSL compiler(reinterpret_cast<uint32*>(spv.ptr), spv.size / sizeof(uint32));

            spirv_cross::CompilerMSL::Options options;

            // Perform the conversion
            out = compiler.compile();
        }
        catch (spirv_cross::CompilerError& e)
        {
            const LINAGX_STRING err = e.what();
            const LINAGX_STRING log = "SPV2MSL -> " + err;
            LOGE(log.c_str());
            return false;
        }

        return true;
    }

} // namespace LinaGX