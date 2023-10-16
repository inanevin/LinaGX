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


#include "LinaGX/Common/CommonGfx.hpp"
#include "WindowManager.hpp"
#include "Input.hpp"

namespace LinaGX
{
    class Backend;
    class CommandStream;

    class Instance
    {
    public:
        Instance()
            : m_windowManager(&m_input){};
        virtual ~Instance();

        /// <summary>
        ///
        /// </summary>
        /// <param name="info"></param>
        /// <returns></returns>
        bool Initialize(const InitInfo& info);

        /// <summary>
        ///
        /// </summary>
        void Join();

        /// <summary>
        ///
        /// </summary>
        /// <param name="frameIndex"></param>
        void StartFrame();

        /// <summary>
        ///
        /// </summary>
        /// <param name="streams"></param>
        /// <param name="streamCount"></param>
        void CloseCommandStreams(CommandStream** streams, uint32 streamCount);

        /// <summary>
        ///
        /// </summary>
        /// <param name="sterams"></param>
        /// <param name="streamCount"></param>
        void SubmitCommandStreams(const SubmitDesc& execute);

        /// <summary>
        ///
        /// </summary>
        void EndFrame();

        /// <summary>
        ///
        /// </summary>
        void Present(const PresentDesc& present);

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        uint16 CreateUserSemaphore();

        /// <summary>
        ///
        /// </summary>
        void DestroyUserSemaphore(uint16 handle);

        /// <summary>
        ///
        /// </summary>
        /// <param name="handle"></param>
        void WaitForUserSemaphore(uint16 handle, uint64 value);

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
        ///
        /// </summary>
        /// <param name="handle"></param>
        void RecreateSwapchain(const SwapchainRecreateDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="swp"></param>
        /// <param name="isActive"></param>
        void SetSwapchainActive(uint8 swp, bool isActive);

        /// <summary>
        /// Compiles given GLSL text file into a blob. This blob is used to generate shader programs in the gpu. You can save the blob to file and pass it without compiling next time.
        /// </summary>
        /// <param name="stage">Target shader stage.</param>
        /// <param name="text">A valid raw GLSL for the shader stage.</param>
        /// <param name="outCompiledBlob">Out blob, SPIRV blob in Vulkan, IDXC blob in DirectX & metal blob in Metal.</param>
        //  bool CompileShader(ShaderStage stage, const char* text, const char* includePath, DataBlob& outCompiledBlob, ShaderLayout& outLayout);

        bool CompileShader(const LINAGX_MAP<ShaderStage, ShaderCompileData>& compileData, LINAGX_MAP<ShaderStage, DataBlob>& outCompiledBlobs, ShaderLayout& outLayout);

        /// <summary>
        /// Generates a shader pipeline.
        /// </summary>
        /// <param name="stages">Hashmap containing all compiled blobs per shader stage.</param>
        uint16 CreateShader(const ShaderDesc& shaderDesc);

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
        CommandStream* CreateCommandStream(const CommandStreamDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="stream"></param>k
        void DestroyCommandStream(CommandStream* stream);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        /// <returns></returns>
        uint32 CreateTexture(const TextureDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="handle"></param>
        void DestroyTexture(uint32 handle);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        /// <returns></returns>
        uint32 CreateSampler(const SamplerDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="handle"></param>
        void DestroySampler(uint32 handle);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        /// <returns></returns>
        uint32 CreateResource(const ResourceDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="handle"></param>
        void DestroyResource(uint32 handle);

        /// <summary>
        ///
        /// </summary>
        /// <param name="resource"></param>
        /// <param name="ptr"></param>
        void MapResource(uint32 resource, uint8*& ptr);

        /// <summary>
        ///
        /// </summary>
        /// <param name="resource"></param>
        void UnmapResource(uint32 resource);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        /// <returns></returns>
        uint16 CreateDescriptorSet(const DescriptorSetDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="handle"></param>
        void DestroyDescriptorSet(uint16 handle);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        void DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        void DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="desc"></param>
        /// <returns></returns>
        uint16 CreatePipelineLayout(const PipelineLayoutDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="layout"></param>
        void DestroyPipelineLayout(uint16 layout);

        /// <summary>
        ///
        /// </summary>
        /// <param name="type"></param>
        uint8 CreateQueue(const QueueDesc& desc);

        /// <summary>
        ///
        /// </summary>
        /// <param name="queue"></param>
        void DestroyQueue(uint8 queue);

        /// <summary>
        ///
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        uint8 GetPrimaryQueue(CommandType type);

        /// <summary>
        ///
        /// </summary>
        void PollWindowsAndInput()
        {
            m_windowManager.PollWindowsAndInput();
        }

        inline uint32 GetCurrentFrameIndex()
        {
            return m_currentFrameIndex;
        }

        inline WindowManager& GetWindowManager()
        {
            return m_windowManager;
        }

        inline Input& GetInput()
        {
            return m_input;
        }

    private:
        /// <summary>
        ///
        /// </summary>
        void Shutdown();

    private:
        friend class VKBackend;
        friend class MTLBackend;
        friend class DX12Backend;

    private:
        InitInfo      m_initInfo = {};
        Backend*      m_backend  = nullptr;
        WindowManager m_windowManager;
        Input         m_input;
        uint32        m_currentFrameIndex = 0;

        LINAGX_VEC<CommandStream*> m_commandStreams;
    };
} // namespace LinaGX

