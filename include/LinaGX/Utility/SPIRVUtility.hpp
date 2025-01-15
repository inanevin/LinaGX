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

#pragma once

#include "LinaGX/Common/CommonGfx.hpp"
#include "SPIRV/GlslangToSpv.h"

namespace LinaGX
{
    enum class BackendAPI;

    class SPIRVUtility
    {
    public:
        static void Initialize();
        static void Shutdown();
        static bool GLSL2SPV(ShaderStage stg, const LINAGX_STRING& pShader, const LINAGX_STRING& includePath, DataBlob& spirv, ShaderLayout& outLayout, BackendAPI targetAPI);
        static bool SPV2HLSL(ShaderStage stg, const DataBlob& spv, LINAGX_STRING& out, const ShaderLayout& layoutReflection);
        static bool SPV2MSL(ShaderStage stg, const DataBlob& spv, LINAGX_STRING& out, ShaderLayout& layoutReflection);
        static void PostFillReflection(ShaderLayout& outLayout, BackendAPI targetAPI);
        static void GetShaderTextWithIncludes(LINAGX_STRING& outStr, const LINAGX_STRING& shader, const LINAGX_STRING& includePath);

    private:
        static void InitResources(TBuiltInResource& resources);
    };
} // namespace LinaGX
