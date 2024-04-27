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

#include "LinaGX/Utility/SPIRVUtility.hpp"
#include "LinaGX/Utility/PlatformUtility.hpp"
#include "spirv_glsl.hpp"
#include "spirv_msl.hpp"
#include "spirv_hlsl.hpp"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/ShaderLang.h"
#include "SPIRV/spirv.hpp"
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

    void SPIRVUtility::GetShaderTextWithIncludes(LINAGX_STRING& outStr, const LINAGX_STRING& shader, const LINAGX_STRING& includePath)
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
                    const LINAGX_STRING str      = includePath + "/" + filename;

                    LINAGX_STRING contents      = ReadFileContentsAsString(str.c_str());
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
        case ShaderStage::Fragment:
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
        case ShaderStage::Fragment:
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

    ShaderMemberType GetMemberType(spirv_cross::SPIRType type, size_t& alignment)
    {
        ShaderMemberType outType = ShaderMemberType::Float;
        const bool       isFloat = type.basetype == spirv_cross::SPIRType::BaseType::Float;

        if (type.columns == 1)
        {
            if (type.vecsize == 1)
            {
                outType   = isFloat ? ShaderMemberType::Float : ShaderMemberType::Int;
                alignment = sizeof(float);
            }
            else if (type.vecsize == 2)
            {
                outType   = isFloat ? ShaderMemberType::Float2 : ShaderMemberType::Int2;
                alignment = sizeof(float) * 2;
            }
            else if (type.vecsize == 3)
            {
                outType   = isFloat ? ShaderMemberType::Float3 : ShaderMemberType::Int3;
                alignment = sizeof(float) * 4;
            }
            else
            {
                outType   = isFloat ? ShaderMemberType::Float4 : ShaderMemberType::Int4;
                alignment = sizeof(float) * 4;
            }
        }
        else if (type.columns == 2 && type.vecsize == 2)
        {
            outType   = ShaderMemberType::Mat2x2;
            alignment = sizeof(float) * 4;
        }
        else if (type.columns == 3 && type.vecsize == 3)
        {
            outType   = ShaderMemberType::Mat3x3;
            alignment = sizeof(float) * 16;
        }
        else if (type.columns == 4 && type.vecsize == 4)
        {
            outType   = ShaderMemberType::Mat4x4;
            alignment = sizeof(float) * 16;
        }
        else
            outType = ShaderMemberType::MatUnknown;

        return outType;
    }

    void FillStructMembers(spirv_cross::CompilerGLSL& compiler, spirv_cross::SPIRType type, LINAGX_VEC<ShaderStructMember>& members)
    {
        unsigned int memberCount = static_cast<unsigned int>(type.member_types.size());
        for (unsigned int i = 0; i < memberCount; i++)
        {
            ShaderStructMember member;

            auto& member_type = compiler.get_type(type.member_types[i]);
            member.size       = compiler.get_declared_struct_member_size(type, i);
            member.type       = GetMemberType(member_type, member.alignment);
            member.offset     = compiler.type_struct_member_offset(type, i);

            if (!member_type.array.empty())
            {
                // Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
                member.arrayStride = compiler.type_struct_member_array_stride(type, i);
                member.elementSize = member_type.array[0];
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

    bool SPIRVUtility::GLSL2SPV(ShaderStage stg, const LINAGX_STRING& pShader, const LINAGX_STRING& includePath, DataBlob& compiledBlob, ShaderLayout& outLayout, BackendAPI targetAPI)
    {
        LINAGX_STRING fullShaderStr = "";
        GetShaderTextWithIncludes(fullShaderStr, pShader, includePath);

        // gl_DrawID is not supported in HLSL/MSL, so we will replace it with our custom cbuffer at post processing
        // But we need to have the identifier generated in HLSL from SPV.
        if (targetAPI == BackendAPI::DX12 || targetAPI == BackendAPI::Metal)
        {
            const LINAGX_STRING searchString  = "gl_DrawID";
            const LINAGX_STRING replaceString = "LGX_GET_DRAW_ID()";
            size_t              pos           = 0;
            while ((pos = fullShaderStr.find(searchString, pos)) != std::string::npos)
            {
                fullShaderStr.replace(pos, searchString.length(), replaceString);
                pos += replaceString.length();
                outLayout.hasGLDrawID = true;
            }

            const LINAGX_STRING constDecl  = "\nint LGX_GET_DRAW_ID() { return 0; } \n";
            size_t              versionPos = fullShaderStr.find("#version");

            if (versionPos != std::string::npos)
            {
                // The #version directive ends at the end of the line it is on, so we need to find that.
                size_t endOfVersionLinePos = fullShaderStr.find("\n", versionPos);
                if (endOfVersionLinePos != std::string::npos)
                {
                    fullShaderStr.insert(endOfVersionLinePos + 1, constDecl);
                }
            }
        }

        glslang_stage_t stage = GetStage(stg);

        glslang_input_t input                   = {};
        input.language                          = GLSLANG_SOURCE_GLSL;
        input.stage                             = stage;
        input.client                            = GLSLANG_CLIENT_VULKAN;
        input.target_language                   = GLSLANG_TARGET_SPV;
        input.target_language_version           = GLSLANG_TARGET_SPV_1_0;
        input.code                              = fullShaderStr.c_str();
        input.default_version                   = 460;
        input.default_profile                   = GLSLANG_NO_PROFILE;
        input.force_default_version_and_profile = false;
        input.forward_compatible                = false;
        input.messages                          = GLSLANG_MSG_DEFAULT_BIT;
        input.resource                          = &DefaultTBuiltInResource;
        glslang_shader_t* shader                = glslang_shader_create(&input);

        if (!glslang_shader_preprocess(shader, &input))
        {
            LINAGX_STRING log = glslang_shader_get_info_log(shader);
            log               = log.substr(0, 100);
            LOGE("GLSL2SPV -> Preprocess failed %s", log.c_str());
            LOGA(false,"");
            return false;
        }
        
        if (!glslang_shader_parse(shader, &input))
        {
            LINAGX_STRING log = glslang_shader_get_info_log(shader);
            log               = log.substr(0, 3000);
            LOGE("GLSL2SPV -> Parsing failed %s", log.c_str());
            LOGA(false,"");
            return false;
        }

        glslang_program_t* program = glslang_program_create();
        glslang_program_add_shader(program, shader);
        if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT))
        {
            LINAGX_STRING log = glslang_shader_get_info_log(shader);
            log               = log.substr(0, 100);
            LOGE("GLSL2SPV -> Linking failed %s, %s", log.c_str());
            LOGA(false, "");
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

        const char* spirv_messages = glslang_program_SPIRV_get_messages(program);

        if (spirv_messages)
            fprintf(stderr, "%s", spirv_messages);

        glslang_program_delete(program);
        glslang_shader_delete(shader);

        spirv_cross::CompilerGLSL    compiler(std::move(spirvBinary));
        spirv_cross::ShaderResources resources       = compiler.get_shader_resources();
        auto                         activeResources = compiler.get_active_interface_variables();

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

                    const bool       isFloat = type.basetype == spirv_cross::SPIRType::Float;
                    ShaderStageInput input   = {};
                    input.location           = location;
                    input.elements           = type.vecsize;
                    input.size               = type.vecsize * type.columns * (isFloat ? sizeof(float) : sizeof(int));
                    input.name               = compiler.get_name(resource.id);

                    switch (type.basetype)
                    {
                    case spirv_cross::SPIRType::Boolean: {
                        if (type.vecsize == 1)
                            input.format = Format::R8_UINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R8G8_UINT;
                        else
                            input.format = Format::R8G8B8A8_UINT;

                        break;
                    }
                    case spirv_cross::SPIRType::SByte: {
                        // Signed 8-bit integer
                        if (type.vecsize == 1)
                            input.format = Format::R8_SINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R8G8_SINT;
                        else
                            input.format = Format::R8G8B8A8_SINT;

                        break;
                    }
                    case spirv_cross::SPIRType::UByte: {
                        // Unsigned 8-bit integer
                        if (type.vecsize == 1)
                            input.format = Format::R8_UINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R8G8_UINT;
                        else
                            input.format = Format::R8G8B8A8_UINT;

                        break;
                    }
                    case spirv_cross::SPIRType::Short: {
                        if (type.vecsize == 1)
                            input.format = Format::R16_SINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R16G16_SINT;
                        else if (type.vecsize == 3)
                            input.format = Format::R16G16B16A16_SINT;

                        break;
                    }
                    case spirv_cross::SPIRType::UShort: {
                        // Unsigned 16-bit integer
                        if (type.vecsize == 1)
                            input.format = Format::R16_UINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R16G16_UINT;
                        else
                            input.format = Format::R16G16B16A16_UINT;

                        break;
                    }
                    case spirv_cross::SPIRType::Int: {
                        // Signed 32-bit integer
                        if (type.vecsize == 1)
                            input.format = Format::R32_SINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R32G32_SINT;
                        else if (type.vecsize == 3)
                            input.format = Format::R32G32B32_SINT;
                        else
                            input.format = Format::R32G32B32A32_SINT;

                        break;
                    }
                    case spirv_cross::SPIRType::UInt: {
                        // Unsigned 32-bit integer
                        if (type.vecsize == 1)
                            input.format = Format::R32_UINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R32G32_UINT;
                        else if (type.vecsize == 3)
                            input.format = Format::R32G32B32_UINT;
                        else
                            input.format = Format::R32G32B32A32_UINT;

                        break;
                    }
                    case spirv_cross::SPIRType::Int64: {
                        LOGE("SPIRVUtility -> 64 bit vertex inputs are not supported!");
                        // Unsigned 32-bit integer
                        if (type.vecsize == 1)
                            input.format = Format::R32_UINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R32G32_UINT;
                        else if (type.vecsize == 3)
                            input.format = Format::R32G32B32_UINT;
                        else
                            input.format = Format::R32G32B32A32_UINT;

                        break;
                    }
                    case spirv_cross::SPIRType::UInt64: {
                        LOGE("SPIRVUtility -> 64 bit vertex inputs are not supported!");
                        // Unsigned 32-bit integer
                        if (type.vecsize == 1)
                            input.format = Format::R32_UINT;
                        else if (type.vecsize == 2)
                            input.format = Format::R32G32_UINT;
                        else if (type.vecsize == 3)
                            input.format = Format::R32G32B32_UINT;
                        else
                            input.format = Format::R32G32B32A32_UINT;

                        break;
                    }
                    case spirv_cross::SPIRType::Half: {
                        // 16-bit float
                        if (type.vecsize == 1)
                            input.format = Format::R16_SFLOAT;
                        else if (type.vecsize == 2)
                            input.format = Format::R16G16_SFLOAT;
                        else if (type.vecsize == 3)
                            input.format = Format::R16G16B16A16_SFLOAT;

                        break;
                    }
                    case spirv_cross::SPIRType::Float: {
                        // 32-bit float
                        if (type.vecsize == 1)
                            input.format = Format::R32_SFLOAT;
                        else if (type.vecsize == 2)
                            input.format = Format::R32G32_SFLOAT;
                        else if (type.vecsize == 3)
                            input.format = Format::R32G32B32_SFLOAT;
                        else
                            input.format = Format::R32G32B32A32_SFLOAT;

                        break;
                    }
                    case spirv_cross::SPIRType::Double: {
                        LOGE("SPIRVUtility -> 64 bit vertex inputs are not supported!");
                        // 32-bit float
                        if (type.vecsize == 1)
                            input.format = Format::R32_SFLOAT;
                        else if (type.vecsize == 2)
                            input.format = Format::R32G32_SFLOAT;
                        else if (type.vecsize == 3)
                            input.format = Format::R32G32B32_SFLOAT;
                        else
                            input.format = Format::R32G32B32A32_SFLOAT;
                        break;
                    }
                    default:
                        break;
                    }

                    outLayout.vertexInputs.push_back(input);
                }

                // Sort & set the offsets of the inputs.
                std::sort(outLayout.vertexInputs.begin(), outLayout.vertexInputs.end(), [](const ShaderStageInput& i1, const ShaderStageInput& i2) { return i1.location < i2.location; });
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
            auto resizeSet = [&](uint32 setIndex) {
                if (outLayout.descriptorSetLayouts.size() < setIndex + 1)
                    outLayout.descriptorSetLayouts.resize(setIndex + 1);
            };

            auto resizeBinding = [&](uint32 set, uint32 bindingIndex) {
                if (outLayout.descriptorSetLayouts[set].bindings.size() < bindingIndex + 1)
                    outLayout.descriptorSetLayouts[set].bindings.resize(bindingIndex + 1);
            };

            for (const auto& resource : resources.uniform_buffers)
            {
                const spirv_cross::SPIRType& type         = compiler.get_type(resource.type_id);
                auto                         id           = resource.id;
                const uint32                 set          = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                const uint32                 bindingIndex = compiler.get_decoration(id, spv::DecorationBinding);
                const uint32                 spvID        = id;
                const auto                   name         = compiler.get_name(resource.id);
                const auto                   size         = compiler.get_declared_struct_size(type);

                resizeSet(set);
                resizeBinding(set, bindingIndex);
                auto& targetBinding   = outLayout.descriptorSetLayouts[set].bindings[bindingIndex];
                targetBinding.binding = bindingIndex;
                targetBinding.name    = name;
                targetBinding.spvID   = spvID;
                targetBinding.stages.push_back(stg);
                targetBinding.size          = size;
                targetBinding.type          = DescriptorType::UBO;
                targetBinding.isActive[stg] = activeResources.count(id) > 0;

                if (type.array.size() > 0)
                    targetBinding.descriptorCount = type.array[0];

                outLayout.totalDescriptors += targetBinding.descriptorCount;
                FillStructMembers(compiler, type, targetBinding.structMembers);
            }

            for (const auto& resource : resources.storage_buffers)
            {
                const spirv_cross::SPIRType& type         = compiler.get_type(resource.type_id);
                auto                         id           = resource.id;
                const uint32                 set          = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                const uint32                 bindingIndex = compiler.get_decoration(id, spv::DecorationBinding);
                const uint32                 spvID        = id;
                const auto                   name         = compiler.get_name(resource.id);
                const auto                   size         = compiler.get_declared_struct_size(type);

                resizeSet(set);
                resizeBinding(set, bindingIndex);
                auto& targetBinding   = outLayout.descriptorSetLayouts[set].bindings[bindingIndex];
                targetBinding.binding = bindingIndex;
                targetBinding.name    = name;
                targetBinding.spvID   = spvID;
                targetBinding.stages.push_back(stg);
                targetBinding.size            = size;
                targetBinding.descriptorCount = 1;
                targetBinding.type            = DescriptorType::SSBO;
                targetBinding.isActive[stg]   = activeResources.count(id) > 0;

                spirv_cross::Bitset buffer_flags = compiler.get_buffer_block_flags(resource.id);
                targetBinding.isWritable         = !buffer_flags.get(spv::DecorationNonWritable);

                outLayout.totalDescriptors++;
            }

            for (const auto& resource : resources.separate_samplers)
            {
                const spirv_cross::SPIRType& type         = compiler.get_type(resource.type_id);
                auto                         id           = resource.id;
                const uint32                 set          = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                const uint32                 bindingIndex = compiler.get_decoration(id, spv::DecorationBinding);
                const uint32                 spvID        = id;
                const auto                   name         = compiler.get_name(resource.id);
                const auto                   size         = 0;

                resizeSet(set);
                resizeBinding(set, bindingIndex);
                auto& targetBinding   = outLayout.descriptorSetLayouts[set].bindings[bindingIndex];
                targetBinding.binding = bindingIndex;
                targetBinding.name    = name;
                targetBinding.spvID   = spvID;
                targetBinding.stages.push_back(stg);
                targetBinding.size          = size;
                targetBinding.type          = DescriptorType::SeparateSampler;
                targetBinding.isActive[stg] = activeResources.count(id) > 0;

                if (type.array_size_literal[0])
                {
                    if (type.array[0] != 1)
                        targetBinding.descriptorCount = type.array[0];
                    else
                        targetBinding.descriptorCount = 0;
                }
                else if (type.array.size() > 0)
                    targetBinding.descriptorCount = type.array[0];

                outLayout.totalDescriptors += targetBinding.descriptorCount == 0 ? 1 : targetBinding.descriptorCount;
            }

            for (const auto& resource : resources.separate_images)
            {
                const spirv_cross::SPIRType& type         = compiler.get_type(resource.type_id);
                auto                         id           = resource.id;
                const uint32                 set          = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                const uint32                 bindingIndex = compiler.get_decoration(id, spv::DecorationBinding);
                const uint32                 spvID        = id;
                const auto                   name         = compiler.get_name(resource.id);
                const auto                   size         = 0;

                resizeSet(set);
                resizeBinding(set, bindingIndex);
                auto& targetBinding   = outLayout.descriptorSetLayouts[set].bindings[bindingIndex];
                targetBinding.binding = bindingIndex;
                targetBinding.name    = name;
                targetBinding.spvID   = spvID;
                targetBinding.stages.push_back(stg);
                targetBinding.size          = size;
                targetBinding.type          = DescriptorType::SeparateImage;
                targetBinding.isActive[stg] = activeResources.count(id) > 0;

                if (type.array_size_literal[0])
                {
                    if (type.array[0] != 1)
                        targetBinding.descriptorCount = type.array[0];
                    else
                        targetBinding.descriptorCount = 0;
                }
                else if (type.array.size() > 0)
                    targetBinding.descriptorCount = type.array[0];
                else
                {
                    // sampler2DArray
                    targetBinding.descriptorCount = 1;
                    targetBinding.isArrayType     = true;
                }

                outLayout.totalDescriptors += targetBinding.descriptorCount == 0 ? 1 : targetBinding.descriptorCount;
            }

            for (const auto& resource : resources.sampled_images)
            {
                const spirv_cross::SPIRType& type         = compiler.get_type(resource.type_id);
                auto                         id           = resource.id;
                const uint32                 set          = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                const uint32                 bindingIndex = compiler.get_decoration(id, spv::DecorationBinding);
                const uint32                 spvID        = id;
                const auto                   name         = compiler.get_name(resource.id);
                const auto                   size         = 0;

                resizeSet(set);
                resizeBinding(set, bindingIndex);
                auto& targetBinding   = outLayout.descriptorSetLayouts[set].bindings[bindingIndex];
                targetBinding.binding = bindingIndex;
                targetBinding.name    = name;
                targetBinding.spvID   = spvID;
                targetBinding.stages.push_back(stg);
                targetBinding.size          = size;
                targetBinding.type          = DescriptorType::CombinedImageSampler;
                targetBinding.isActive[stg] = activeResources.count(id) > 0;

                if (type.array_size_literal[0])
                {
                    if (type.array[0] != 1)
                        targetBinding.descriptorCount = type.array[0];
                    else
                        targetBinding.descriptorCount = 0;
                }
                else if (type.array.size() > 0)
                    targetBinding.descriptorCount = type.array[0];
                else
                {
                    // sampler2DArray
                    targetBinding.descriptorCount = 1;
                    targetBinding.isArrayType     = true;
                }

                outLayout.totalDescriptors += targetBinding.descriptorCount == 0 ? 2 : targetBinding.descriptorCount * 2;
            }

            if (!resources.push_constant_buffers.empty())
            {
                const auto& resource = resources.push_constant_buffers[0];
                auto        id       = resource.id;

                ShaderConstantBlock block = {};

                const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
                block.size                        = compiler.get_declared_struct_size(type);
                block.name                        = compiler.get_name(resource.id);
                block.isActive[stg]               = activeResources.count(id) > 0;

                bool found = false;
                for (auto& b : outLayout.constants)
                {
                    if (b.name.compare(block.name) == 0)
                    {
                        found = true;
                        b.stages.push_back(stg);
                        b.isActive[stg] = activeResources.count(id) > 0;
                        break;
                    }
                }

                if (!found)
                {
                    block.stages.push_back(stg);
                    FillStructMembers(compiler, type, block.members);
                    outLayout.constants.push_back(block);
                }
            }
        }

        return true;
    }

    spv::ExecutionModel GetExecStage(ShaderStage stg)
    {
        if (stg == ShaderStage::Fragment)
            return spv::ExecutionModelFragment;
        else if (stg == ShaderStage::TesellationControl)
            return spv::ExecutionModelTessellationControl;
        else if (stg == ShaderStage::TesellationEval)
            return spv::ExecutionModelTessellationEvaluation;
        else if (stg == ShaderStage::Geometry)
            return spv::ExecutionModelGeometry;
        else if (stg == ShaderStage::Compute)
            return spv::ExecutionModelGLCompute;

        return spv::ExecutionModelVertex;
    };

    bool SPIRVUtility::SPV2HLSL(ShaderStage stg, const DataBlob& spv, LINAGX_STRING& out, const ShaderLayout& layoutReflection)
    {
        try
        {
            spirv_cross::CompilerHLSL compiler(reinterpret_cast<uint32*>(spv.ptr), spv.size / sizeof(uint32));
            spv::ExecutionModel       exec = GetExecStage(stg);

            auto resources = compiler.get_shader_resources();

            spirv_cross::CompilerHLSL::Options    options;
            spirv_cross::HLSLVertexAttributeRemap attribs;

            options.shader_model = 60; // SM6_0

            compiler.set_hlsl_options(options);
            compiler.add_vertex_attribute_remap(attribs);

            uint32 setIndex = 0;

            auto hasStage = [stg](const ShaderDescriptorSetBinding& binding) -> bool {
                for (auto bstg : binding.stages)
                {
                    if (bstg == stg)
                        return true;
                }

                return false;
            };

            for (auto& layout : layoutReflection.descriptorSetLayouts)
            {
                uint32 bindingIndex = 0;
                uint32 sampler = 0, cbv = 0, srv = 0, uav = 0;

                for (auto& bindingData : layout.bindings)
                {
                    // If not exists for this stage, or not active skip entirely.
                    if (!hasStage(bindingData))
                    {
                        bindingIndex++;
                        continue;
                    }

                    spirv_cross::HLSLResourceBinding hlslb;
                    hlslb.stage    = exec;
                    hlslb.desc_set = setIndex;
                    hlslb.binding  = bindingIndex;

                    if (bindingData.type == DescriptorType::UBO)
                    {
                        hlslb.cbv.register_space   = setIndex;
                        hlslb.cbv.register_binding = cbv;
                        cbv += bindingData.descriptorCount == 0 ? 1 : bindingData.descriptorCount;
                        compiler.add_hlsl_resource_binding(hlslb);
                    }
                    else if (bindingData.type == DescriptorType::SSBO)
                    {
                        hlslb.srv.register_space   = setIndex;
                        hlslb.srv.register_binding = srv;
                        srv += bindingData.descriptorCount == 0 ? 1 : bindingData.descriptorCount;
                        compiler.add_hlsl_resource_binding(hlslb);
                    }
                    else if (bindingData.type == DescriptorType::SeparateImage)
                    {
                        hlslb.srv.register_space   = setIndex;
                        hlslb.srv.register_binding = srv;

                        if (bindingData.descriptorCount == 0)
                            srv++;
                        else
                            srv += bindingData.descriptorCount;

                        compiler.add_hlsl_resource_binding(hlslb);
                    }
                    else if (bindingData.type == DescriptorType::SeparateSampler)
                    {
                        hlslb.sampler.register_space   = setIndex;
                        hlslb.sampler.register_binding = sampler;

                        if (bindingData.descriptorCount == 0)
                            sampler++;
                        else
                            sampler += bindingData.descriptorCount;

                        compiler.add_hlsl_resource_binding(hlslb);
                    }
                    else if (bindingData.type == DescriptorType::CombinedImageSampler)
                    {
                        hlslb.sampler.register_space   = setIndex;
                        hlslb.sampler.register_binding = sampler;
                        hlslb.srv.register_space       = setIndex;
                        hlslb.srv.register_binding     = srv;

                        if (bindingData.descriptorCount == 0)
                        {
                            srv++;
                            sampler++;
                        }
                        else
                        {
                            srv += bindingData.descriptorCount;
                            sampler += bindingData.descriptorCount;
                        }

                        compiler.add_hlsl_resource_binding(hlslb);
                    }

                    bindingIndex++;
                }

                setIndex++;
            }

            if (!layoutReflection.constants.empty())
            {
                size_t totalSize = 0;
                for (const auto& ct : layoutReflection.constants)
                    totalSize += ct.size;

                std::vector<spirv_cross::RootConstants> constants;
                spirv_cross::RootConstants              c;
                c.start   = 0;
                c.end     = static_cast<uint32>(totalSize);
                c.binding = layoutReflection.constantsBinding;
                c.space   = layoutReflection.constantsSet;
                constants.push_back(c);
                compiler.set_root_constant_layouts(constants);
            }

            // Perform the conversion
            out = compiler.compile();

            LINAGX_STRING searchString  = "int LGX_GET_DRAW_ID()\n{\n    return 0;\n}";
            LINAGX_STRING replaceString = "int LGX_GET_DRAW_ID()\n{\n    return LGX_DRAW_ID;\n}";

            size_t pos = out.find(searchString);
            if (pos != std::string::npos)
                out.replace(pos, searchString.length(), replaceString);
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

    bool SPIRVUtility::SPV2MSL(ShaderStage stg, const DataBlob& spv, LINAGX_STRING& out, ShaderLayout& layoutReflection)
    {

        try
        {
            spirv_cross::CompilerMSL compiler(reinterpret_cast<uint32*>(spv.ptr), spv.size / sizeof(uint32));

            spv::ExecutionModel exec = GetExecStage(stg);

            spirv_cross::CompilerMSL::Options options;
            options.set_msl_version(3);
            options.argument_buffers_tier = spirv_cross::CompilerMSL::Options::ArgumentBuffersTier::Tier2;
            // options.force_active_argument_buffer_resources = true;
            // options.runtime_array_rich_descriptor = true;
            // compiler.set_argument_buffer_device_address_space(0, true);
            // options.pad_argument_buffer_resources = true;

            uint32 bufferID = 0, textureID = 0, samplerID = 0;
            uint32 drawIDInputBufferID = 0;

            if (stg == ShaderStage::Vertex)
                bufferID++;

            uint32 setIndex = 0;
            for (auto& layout : layoutReflection.descriptorSetLayouts)
            {
                uint32 bindingIndex = 0;

                for (auto& bindingData : layout.bindings)
                {
                    // If not exists for this stage, or not active skip entirely.
                    auto it = bindingData.isActive.find(stg);
                    if (it == bindingData.isActive.end() || it->second == false)
                    {
                        bindingIndex++;
                        continue;
                    }

                    spirv_cross::MSLResourceBinding mslb;
                    mslb.stage    = exec;
                    mslb.desc_set = setIndex;
                    mslb.binding  = bindingIndex;

                    if (bindingData.type == DescriptorType::UBO)
                    {
                        mslb.basetype                = spirv_cross::SPIRType::BaseType::Struct;
                        mslb.count                   = bindingData.descriptorCount == 0 ? 1 : bindingData.descriptorCount;
                        mslb.msl_buffer              = bufferID;
                        bindingData.mslBufferID[stg] = bufferID;
                        bufferID += mslb.count;
                        compiler.add_msl_resource_binding(mslb);
                    }
                    else if (bindingData.type == DescriptorType::SSBO)
                    {
                        mslb.count                   = 1;
                        mslb.msl_buffer              = bufferID;
                        bindingData.mslBufferID[stg] = bufferID;
                        bufferID += mslb.count;
                        compiler.add_msl_resource_binding(mslb);
                    }
                    else if (bindingData.type == DescriptorType::SeparateImage)
                    {
                        if (bindingData.descriptorCount == 0)
                        {
                            mslb.count                   = 0;
                            mslb.msl_texture             = bufferID;
                            bindingData.mslBufferID[stg] = bufferID;
                            bufferID++;
                        }
                        else
                        {
                            mslb.count                   = bindingData.descriptorCount;
                            mslb.msl_texture             = textureID;
                            bindingData.mslBufferID[stg] = textureID;
                            textureID += mslb.count;
                        }

                        compiler.add_msl_resource_binding(mslb);
                    }
                    else if (bindingData.type == DescriptorType::SeparateSampler)
                    {
                        if (bindingData.descriptorCount == 0)
                        {
                            mslb.count                   = 0;
                            mslb.msl_texture             = bufferID;
                            bindingData.mslBufferID[stg] = bufferID;
                            bufferID++;
                        }
                        else
                        {
                            mslb.count                   = bindingData.descriptorCount;
                            mslb.msl_sampler             = samplerID;
                            bindingData.mslBufferID[stg] = samplerID;
                            samplerID += mslb.count;
                        }

                        compiler.add_msl_resource_binding(mslb);
                    }
                    else if (bindingData.type == DescriptorType::CombinedImageSampler)
                    {
                        if (bindingData.descriptorCount == 0)
                        {
                            mslb.count                   = 0;
                            mslb.msl_texture             = bufferID;
                            bindingData.mslBufferID[stg] = bufferID;
                            bufferID++;
                            compiler.add_msl_resource_binding(mslb);

                            mslb.msl_sampler = bufferID;
                            bufferID++;
                            compiler.add_msl_resource_binding(mslb);
                        }
                        else
                        {
                            mslb.count                   = bindingData.descriptorCount;
                            mslb.msl_texture             = textureID;
                            bindingData.mslBufferID[stg] = textureID;
                            textureID += mslb.count;
                            compiler.add_msl_resource_binding(mslb);

                            mslb.msl_sampler = samplerID;
                            samplerID += mslb.count;
                            compiler.add_msl_resource_binding(mslb);
                        }
                    }

                    bindingIndex++;
                }

                setIndex++;
            }

            // Pcb
            if (!layoutReflection.constants.empty())
            {
                for (auto& ct : layoutReflection.constants)
                {
                    auto it = ct.isActive.find(stg);

                    if (it != ct.isActive.end() && it->second == true)
                    {
                        spirv_cross::MSLResourceBinding mslb;
                        mslb.stage      = exec;
                        mslb.count      = 1;
                        mslb.binding    = spirv_cross::ResourceBindingPushConstantBinding;
                        mslb.desc_set   = spirv_cross::ResourceBindingPushConstantDescriptorSet;
                        mslb.basetype   = spirv_cross::SPIRType::BaseType::Struct;
                        mslb.msl_buffer = bufferID;
                        compiler.add_msl_resource_binding(mslb);
                        layoutReflection.constantsMSLBuffers[stg] = bufferID;
                        bufferID++;
                    }
                }
            }

            // DrawID last.
            if (layoutReflection.hasGLDrawID && stg == ShaderStage::Vertex)
            {
                drawIDInputBufferID = bufferID;
                bufferID++;
            }

            layoutReflection.mslMaxBufferIDs[stg] = bufferID;

            compiler.set_msl_options(options);
            out = compiler.compile();

            auto entry                        = compiler.get_entry_point("main", exec);
            layoutReflection.entryPoints[stg] = entry.name;

            if (layoutReflection.hasGLDrawID && stg == ShaderStage::Vertex)
            {
                layoutReflection.drawIDBinding = drawIDInputBufferID;

                LINAGX_STRING insertStr = "";
                if (!layoutReflection.vertexInputs.empty())
                    insertStr = layoutReflection.entryPoints[stg] + "(" + layoutReflection.entryPoints[stg] + "_in in [[stage_in]],";
                else
                    insertStr = layoutReflection.entryPoints[stg] + "(";

                // Insert struct definition.
                LINAGX_STRING structStr = "struct LGXDrawID\n{\n int value;\n};\n\n";

                LINAGX_STRING mainVertexLine    = "vertex " + layoutReflection.entryPoints[stg] + "_out";
                auto          mainVertexLinePos = out.find(mainVertexLine);
                LOGA(mainVertexLinePos != std::string::npos, "SPIRVUtility -> Failed inserting for gl_DrawID!");
                out.insert(mainVertexLinePos, structStr);

                // Insert struct argument.
                LINAGX_STRING toBeInserted = " constant LGXDrawID& lgxDrawID [[buffer(" + LINAGX_TOSTRING(drawIDInputBufferID) + ")]],";
                auto          pos          = out.find(insertStr);
                LOGA(pos != std::string::npos, "SPIRVUtility -> Failed inserting for gl_DrawID!");
                out.insert(pos + insertStr.length(), toBeInserted);

                const LINAGX_STRING lgxUsageSearchStr  = "LGX_GET_DRAW_ID()";
                const LINAGX_STRING lgxUsageReplaceStr = "lgxDrawID.value";
                size_t              lgxUsagePos        = 0;
                while ((pos = out.find(lgxUsageSearchStr, pos)) != std::string::npos)
                {
                    out.replace(pos, lgxUsageSearchStr.length(), lgxUsageReplaceStr);
                    lgxUsagePos += lgxUsageReplaceStr.length();
                }
            }
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

    void SPIRVUtility::PostFillReflection(ShaderLayout& outLayout, BackendAPI targetAPI)
    {
        // If contains push constants
        // Make sure we assign it to maxium set's maximum binding+1
        if (!outLayout.constants.empty())
        {
            outLayout.constantsSet = static_cast<uint32>(outLayout.descriptorSetLayouts.size());
            // outLayout.constantsBinding = outLayout.descriptorSetLayouts.empty() ? 0 : static_cast<uint32>(outLayout.descriptorSetLayouts[outLayout.constantsSet - 1].bindings.size() - 1);
            outLayout.constantsBinding = 0;
        }
    }

} // namespace LinaGX
