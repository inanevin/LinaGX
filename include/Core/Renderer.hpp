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

#ifndef LINAGX_Renderer_HPP
#define LINAGX_Renderer_HPP

#include "Common/Common.hpp"

namespace LinaGX
{
    class Backend;

    class Renderer
    {
    public:
        Renderer(){};
        virtual ~Renderer(){};

        bool Initialize(const InitInfo& info);
        void Shutdown();

        /// <summary>
        /// Compiles given shader into a blob.
        /// </summary>
        /// <param name="stage">Target shader stage.</param>
        /// <param name="text">A valid raw GLSL for the shader stage.</param>
        /// <param name="outCompiledBlob">Out blob, SPIRV blob in Vulkan, IDXC blob in DirectX & metal blob in Metal.</param>
        bool CompileShader(ShaderStage stage, const char* text, CompiledShaderBlob& outCompiledBlob);

    private:
        InitInfo m_initInfo = {};
        Backend* m_backend  = nullptr;
    };
} // namespace LinaGX

#endif
