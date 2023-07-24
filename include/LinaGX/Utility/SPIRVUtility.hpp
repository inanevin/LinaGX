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

#pragma once

#ifndef LINAGX_SPIRVUtility_HPP
#define LINAGX_SPIRVUtility_HPP

#include "LinaGX/Common/CommonGfx.hpp"
#include "glslang/SPIRV/GlslangToSpv.h"

namespace LinaGX
{
    class SPIRVUtility
    {
    public:
        static void Initialize();
        static void Shutdown();
        static bool GLSL2SPV(ShaderStage stg, const char* pShader, const char* includePath, DataBlob& spirv, ShaderLayout& outLayout);
        static bool SPV2HLSL(ShaderStage stg, const DataBlob& spv, LINAGX_STRING& out, const ShaderLayout& layoutReflection);
        static bool SPV2MSL(ShaderStage stg, const DataBlob& spv, LINAGX_STRING& out, const ShaderLayout& layoutReflection);

    private:
        static void InitResources(TBuiltInResource& resources);
        static void GetShaderTextWithIncludes(LINAGX_STRING& outStr, const char* shader, const char* includePath);
    };
} // namespace LinaGX

#endif
