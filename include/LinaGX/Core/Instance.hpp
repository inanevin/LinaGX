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
#include "WindowManager.hpp"
#include "Input.hpp"
#include <mutex>

namespace LinaGX
{
    class Backend;
    class CommandStream;

    class Instance
    {
    public:
        Instance()
            : m_windowManager(&m_input){};

        /// <summary>
        /// Deleting LinaGX instance requires you to DestroyXXX all resources you have created.
        /// Prior to destroying resources, make sure to use Join() so all operations on the GPU will be completed.
        /// </summary>
        virtual ~Instance();

        /// <summary>
        /// Call once before your rendering loop to initialize LinaGX. Make sure you setup your LinaGX::Config. values accordingly
        /// before calling.
        /// </summary>
        /// <param name="info"></param>
        /// <returns></returns>
        bool Initialize();

        /// <summary>
        /// Waits the calling thread for all the frames-in-flight operations to finish on the GPU.
        /// Definitely use when deleting a resource that might be still processing on the GPU, before shutting down and destroying LinaGX instance.
        /// </summary>
        void Join();

        /// <summary>
        /// Call once to let LinaGX know you are starting a draw frame, before calling any other draw operations.
        /// </summary>
        void StartFrame();

        /// <summary>
        /// Call once to let LinagX know you are ending a frame, after calling all other operations.
        /// </summary>
        void EndFrame();

        /// <summary>
        /// This records the given command streams in the actual underlying graphics API. You must call this before submitting the streams.
        /// </summary>
        void CloseCommandStreams(CommandStream** streams, uint32 streamCount);

        /// <summary>
        /// Begins work on the GPU for the given command streams.
        /// </summary>
        void SubmitCommandStreams(const SubmitDesc& execute);

        /// <summary>
        /// Present given swapchains.
        /// </summary>
        void Present(const PresentDesc& present);

        /// <summary>
        /// Check the current GPU support for the given format.
        /// </summary>
        /// <returns>Info structure where you can check if .format != Format::UNDEFINED, which means the format is supported. Boolean
        /// values determine where does this format can be used. </returns>
        FormatSupportInfo GetFormatSupport(Format format);

        /// <summary>
        /// Create a semaphore to be used in queue submit operations, as well as individually waiting on the CPU.
        /// </summary>
        uint16 CreateUserSemaphore();

        /// <summary>
        /// Pass the handle obtained via CreateUserSemaphore() to destroy it. Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        void DestroyUserSemaphore(uint16 handle);

        /// <summary>
        /// Will block the calling thread until the given semaphore reaches the given value on the GPU, e.g. waiting for the GPU operations that signal that value.
        /// </summary>
        void WaitForUserSemaphore(uint16 handle, uint64 value);

        /// <summary>
        /// Creates a swapchain for a window given necessary handles per-platform.
        /// </summary>
        uint8 CreateSwapchain(const SwapchainDesc& desc);

        /// <summary>
        /// Destroys the swapchain with given handle. If you want to re-create the swapchain, don't use this, use RecreateSwapchain().
        /// </summary>
        void DestroySwapchain(uint8 handle);

        /// <summary>
        /// Use this for re-creating your swapchain on window resize operations. This will Join() the graphics operations, so expect frames-in-flight stall.
        /// </summary>
        void RecreateSwapchain(const SwapchainRecreateDesc& desc);

        /// <summary>
        /// You can a swapchain per-window in your application, but not all windows will be visible and not all swapchains will be presenting. Use this to
        /// control the image acquire behavior. System won't acquire backbuffer images for inactive swapchains.
        /// </summary>
        void SetSwapchainActive(uint8 swp, bool isActive);

        /// <summary>
        /// Given raw GLSL shader texts per-stage, outputs data blobs representing the compiled version of the shaders per-stage along with reflection information.
        /// The compiled blobs are necessary parameters for CreateShader() function.
        /// You can compile once, store the compiled blobs offline and use them directly from memory next time creating a shader, thus omitting this step for the successive launches of your app.
        /// </summary>
        /// <param name="compileData">Per-stage map representing the stage text and root directory for any #include directives.</param>
        /// <param name="outCompiledBlobs">Per-stage map giving compiled blobs. On Vulkan, blobs will be SPIR-V. On DX12 they will be DXIL, on Metal they will be binary representation of cross-compiled MSL. </param>
        /// <param name="outLayout">ShaderLayout describing the reflection information of the shader. You need to use this while calling CreateShader(). </param>
        bool CompileShader(const LINAGX_MAP<ShaderStage, ShaderCompileData>& compileData, LINAGX_MAP<ShaderStage, DataBlob>& outCompiledBlobs, ShaderLayout& outLayout);

        /// <summary>
        /// Creates a shader pipeline state object.
        /// </summary>
        uint16 CreateShader(const ShaderDesc& shaderDesc);
        uint16 CreateShaderMT(const ShaderDesc& shaderDesc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        /// <param name="handle"></param>
        void DestroyShader(uint16 handle);
        void DestroyShaderMT(uint16 handle);

        /// <summary>
        /// Create a command stream, which are LinaGX representations of CommandBuffers (VK) or CommandLists (DX12). Use them to record all GPU operations.
        /// </summary>
        /// <returns></returns>
        CommandStream* CreateCommandStream(const CommandStreamDesc& desc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        /// <param name="stream"></param>k
        void DestroyCommandStream(CommandStream* stream);

        /// <summary>
        /// Create a texture resource on the GPU.
        /// </summary>
        uint32 CreateTexture(const TextureDesc& desc);
        uint32 CreateTextureMT(const TextureDesc& desc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        void DestroyTexture(uint32 handle);
        void DestroyTextureMT(uint32 handle);

        /// <summary>
        /// Create a sampler resource on the GPU.
        /// </summary>
        /// <param name="desc"></param>
        uint32 CreateSampler(const SamplerDesc& desc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        void DestroySampler(uint32 handle);

        /// <summary>
        /// Create a resource on the GPU, you can use ResourceDesc to customize memory and allocation behaviors.
        /// </summary>
        uint32 CreateResource(const ResourceDesc& desc);
        uint32 CreateResourceMT(const ResourceDesc& desc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        void DestroyResource(uint32 handle);
        void DestroyResourceMT(uint32 handle);

        /// <summary>
        /// Map a GPU resource to given CPU pointer to write data to GPU from CPU.
        /// </summary>
        void MapResource(uint32 resource, uint8*& ptr);

        /// <summary>
        /// Unmaps a previously mapped GPU resource. You don't need to use Unmap() if absolutely necessary, resources can stay mapped.
        /// They will automatically be unmapped upon destruction.
        /// </summary>
        /// <param name="resource"></param>
        void UnmapResource(uint32 resource);

        /// <summary>
        /// Create a Vulkan style descriptor set for binding resources.
        /// </summary>
        uint16 CreateDescriptorSet(const DescriptorSetDesc& desc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        /// <param name="handle"></param>
        void DestroyDescriptorSet(uint16 handle);

        /// <summary>
        /// Use to update a buffer resource for a descriptor.
        /// </summary>
        /// <param name="desc"></param>
        void DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc);

        /// <summary>
        /// Use to update a texture or sampler resource for a descriptor.
        /// </summary>
        /// <param name="desc"></param>
        void DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc);

        /// <summary>
        /// Creating a shader automatically creates it's pipeline layout from reflection information, and you can use that layout for binding descriptors.
        /// However if you want full control on shader pipeline layouts as well as descriptor binding you can use this function to create custom layouts.
        /// </summary>
        /// <param name="desc"></param>
        /// <returns></returns>
        uint16 CreatePipelineLayout(const PipelineLayoutDesc& desc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        /// <param name="layout"></param>
        void DestroyPipelineLayout(uint16 layout);

        /// <summary>
        /// Although LinaGX creates 3 queues (Graphics, Transfer and Compute) for you, if you want additional queues you can use this to create them.
        /// System handles underlying queue pointers in Vulkan so you don't have to think about queue families and queue limitations, as long as you set LinaGX::Config.gpuFeatures.extraGraphicsQueueCount to maximum amount of
        /// extra queues you will create. Even if your GPU does not support this amount, LinaGX will handle pointers and operations in the background.
        /// </summary>
        /// <param name="type"></param>
        uint8 CreateQueue(const QueueDesc& desc);

        /// <summary>
        /// Make sure all frames-in-flight operations are complete prior.
        /// </summary>
        /// <param name="queue"></param>
        void DestroyQueue(uint8 queue);

        /// <summary>
        /// LinaGX creates 3 queues for you automatically, Graphics, Transfer and Compute queues. If you don't want to work on separate queues you should use these.
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        uint8 GetPrimaryQueue(CommandType type);


        /// <summary>
        /// Vulkan Only, queries feature support and returns a bitmask containing VulkanFeatureFlags of supported features on at least 1 of the preffered devices.
        /// </summary>
        /// <param name="gpuType"></param>
        /// <returns></returns>
        uint32 VKQueryFeatureSupport(PreferredGPUType gpuType);

        /// <summary>
        /// If you are using LinaGX as Windowing and Input system, make sure to call this function to after you poll all events from the operating system before your update loop (or however you want to handle input polling).
        /// </summary>
        void TickWindowSystem()
        {
            m_windowManager.TickWindowSystem();
        }

        /// <summary>
        /// Index of the current frame-in-flight.
        /// </summary>
        inline uint32 GetCurrentFrameIndex()
        {
            return m_currentFrameIndex;
        }

        /// <summary>
        /// WindowManager is responsible for all window related operations.
        /// </summary>
        inline WindowManager& GetWindowManager()
        {
            return m_windowManager;
        }

        /// <summary>
        /// Input is responsible for querying input state and everything related.
        /// </summary>
        inline Input& GetInput()
        {
            return m_input;
        }

    private:
        void Shutdown();

    private:
        friend class VKBackend;
        friend class MTLBackend;
        friend class DX12Backend;

    private:
        Backend*      m_backend = nullptr;
        WindowManager m_windowManager;
        Input         m_input;
        uint32        m_currentFrameIndex = 0;
        std::mutex m_textureMtx;
        std::mutex m_resourceMtx;
        std::mutex m_shaderMtx;

        LINAGX_VEC<CommandStream*> m_commandStreams;
    };
} // namespace LinaGX
