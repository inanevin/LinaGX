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
    class CommandStream;

    class Renderer
    {
    public:
        Renderer(){};
        virtual ~Renderer(){};

        /// <summary>
        ///
        /// </summary>
        /// <param name="info"></param>
        /// <returns></returns>
        bool Initialize(const InitInfo& info);

        /// <summary>
        ///
        /// </summary>
        void Shutdown();

        /// <summary>
        ///
        /// </summary>
        /// <param name="frameIndex"></param>
        void StartFrame(uint32 frameIndex = 0);

        /// <summary>
        ///
        /// </summary>
        void Flush();

        /// <summary>
        ///
        /// </summary>
        void EndFrame();

        /// <summary>
        ///
        /// </summary>
        void Present(const PresentDesc& present);

        /// <summary>
        /// Creates a swapchain for a window given necessary handles per-platform. e.g HWND and hInstance on windows.
        /// </summary>
        /// <returns>Handle to the created swapchain.</returns>
        uint8 CreateSwapchain(const SwapchainDesc& desc);

        /// <summary>
        /// Destroys the swapchain with given handle.
        /// </summary>
        /// <param name="handle"></param>
        void DestroySwapchain(uint8 handle);

        /// <summary>
        /// Compiles given GLSL text file into a blob. This blob is used to generate shader programs in the gpu. You can save the blob to file and pass it without compiling next time.
        /// </summary>
        /// <param name="stage">Target shader stage.</param>
        /// <param name="text">A valid raw GLSL for the shader stage.</param>
        /// <param name="outCompiledBlob">Out blob, SPIRV blob in Vulkan, IDXC blob in DirectX & metal blob in Metal.</param>
        bool CompileShader(ShaderStage stage, const char* text, const char* includePath, DataBlob& outCompiledBlob, ShaderLayout& outLayout);

        /// <summary>
        /// Generates a shader pipeline.
        /// </summary>
        /// <param name="stages">Hashmap containing all compiled blobs per shader stage.</param>
        uint16 CreateShader(const LINAGX_MAP<ShaderStage, DataBlob>& stages, const ShaderDesc& shaderDesc);

        /// <summary>
        /// Destroys the shader pipeline with given handle.
        /// </summary>
        /// <param name="handle"></param>
        void DestroyShader(uint16 handle);

        /// <summary>
        ///
        /// </summary>
        /// <param name="commandCount"></param>
        /// <returns></returns>
        CommandStream* CreateCommandStream(uint32 commandCount, CommandType type);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        /// <returns></returns>
        uint32 CreateTexture2D(const Texture2DDesc& desc);

    private:
        friend class VKBackend;
        friend class MTLBackend;
        friend class DX12Backend;

    private:
        InitInfo m_initInfo = {};
        Backend* m_backend  = nullptr;

        LINAGX_VEC<CommandStream*> m_commandStreams;
    };
} // namespace LinaGX

#endif
