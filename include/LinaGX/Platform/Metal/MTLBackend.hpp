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

#include "LinaGX/Core/Backend.hpp"
#include "LinaGX/Core/Commands.hpp"
#include <atomic>

namespace LinaGX
{

    struct MTLTexture
    {
        bool              isValid     = false;
        void*             ptr         = nullptr;
        uint32            arrayLength = 1;
        LINAGX_VEC<void*> views;
        Format            format;
        uint32            flags;
        uint32            bytesPerPixel;
        LGXVector2ui      size;
        LINAGX_STRING debugName = "";
    };

    struct MTLBoundDescriptorSet
    {
        uint16             handle  = 0;
        bool               isDirty = false;
        LINAGX_VEC<uint32> dynamicOffsets;
        uint32             setAllocIndex = 0;
    };

    struct MTLBoundConstant
    {
        uint8*       data            = nullptr;
        uint32       offset          = 0;
        uint32       size            = 0;
        uint32       stagesSize      = 0;
        ShaderStage* stages          = nullptr;
        bool         usesStreamAlloc = false;
    };

    struct MTLCommandStream
    {
        bool                                                   isValid                          = false;
        bool                                                   currentRenderPassUseDepth        = false;
        CommandType                                            type                             = CommandType::Graphics;
        void*                                                  currentBuffer                    = nullptr;
        void*                                                  currentEncoder                   = nullptr;
        void*                                                  currentBlitEncoder               = nullptr;
        void*                                                  currentComputeEncoder            = nullptr;
        void*                                                  currentEncoderDepthStencil       = nullptr;
        void*                                                  indirectCommandBuffer            = nullptr;
        uint32                                                 indirectCommandBufferMaxCommands = 0;
        LINAGX_VEC<void*>                                      allBlitEncoders;
        LINAGX_VEC<void*>                                      allRenderEncoders;
        LINAGX_VEC<void*>                                      allComputeEncoders;
        LINAGX_VEC<uint8>                                      writtenSwapchains;
        uint32                                                 currentShader          = 0;
        uint32                                                 currentIndexBuffer     = 0;
        bool                                                   currentShaderExists    = false;
        uint8                                                  indexBufferType        = 0;
        bool                                                   currentShaderIsCompute = false;
        LINAGX_VEC<LINAGX_PAIR<uint32, uint64>>                intermediateResources;
        LINAGX_VEC<LINAGX_PAIR<uint32, MTLBoundDescriptorSet>> boundSets;
        CMDBindVertexBuffers                                   lastVertexBind;
        MTLBoundConstant                                       boundConstants;
        LINAGX_STRING                                          lastDebugLabel = "";
        CommandStream*                                         streamImpl     = nullptr;
    };

    struct MTLSwapchain
    {
        bool   isValid               = false;
        bool   isActive              = true;
        void*  layer                 = nullptr;
        void*  _currentDrawable      = nullptr;
        void*  window                = nullptr;
        uint32 width                 = 0;
        uint32 height                = 0;
        uint32 _currentDrawableIndex = 0;
        Format format;
        void*  osHandle = nullptr;
    };

    struct MTLArgEncoder
    {
        void* encoder          = nullptr;
        void* encoderSecondary = nullptr;
    };

    struct MTLShader
    {
        bool               isValid     = false;
        bool               isCompute   = false;
        PolygonMode        polygonMode = PolygonMode::Fill;
        CullMode           cullMode    = CullMode::None;
        FrontFace          frontFace   = FrontFace::CW;
        Topology           topology    = Topology::TriangleList;
        void*              pso         = nullptr;
        void*              dsso        = nullptr;
        void*              cso         = nullptr;
        ShaderLayout       layout      = {};
        LINAGX_VEC<Format> colorAttachmentFormats;
        Format             depthFormat;
        Format             stencilFormat;
        float              depthBias  = 0.0f;
        float              depthSlope = 0.0f;
        float              depthClamp = 0.0f;
        LINAGX_STRING      debugName  = "";
    };

    struct MTLFence
    {
        bool isValid = false;
    };

    struct MTLResource
    {
        bool          isValid   = false;
        void*         ptr       = nullptr;
        ResourceHeap  heapType  = ResourceHeap::StagingHeap;
        size_t        size      = 0;
        LINAGX_STRING debugName = "";
    };

    struct MTLUserSemaphore
    {
        bool   isValid    = false;
        void*  semaphore  = nullptr;
        uint64 reachValue = 0;
    };

    struct MTLSampler
    {
        bool  isValid = false;
        void* ptr     = nullptr;
    };

    struct MTLDescriptorSetPerStageData
    {
        uint32 localBufferID  = 0;
        uint32 localTextureID = 0;
        uint32 localSamplerID = 0;
    };

    struct MTLBinding
    {
        DescriptorBinding  lgxBinding;
        LINAGX_VEC<uint32> resources;
        LINAGX_VEC<uint32> additionalResources;
        LINAGX_VEC<uint32> viewIndices;
        void*              argBuffer           = nullptr;
        void*              argBufferSecondary  = nullptr;
        void*              argEncoder          = nullptr;
        void*              argEncoderSecondary = nullptr;
    };

    struct MTLDescriptorSet
    {
        bool                               isValid = false;
        void*                              buffer  = nullptr;
        DescriptorSetDesc                  desc    = {};
        LINAGX_VEC<LINAGX_VEC<MTLBinding>> bindings;
    };

    struct MTLQueue
    {
        bool        isValid = false;
        void*       queue   = nullptr;
        CommandType type    = CommandType::Graphics;
    };

    struct MTLPerFrameData
    {
        uint64 submits        = 0;
        std::atomic<uint64> reachedSubmits = 0;
        
        MTLPerFrameData() : reachedSubmits(0) {};
        
        MTLPerFrameData(MTLPerFrameData&& other) noexcept
               : reachedSubmits(other.reachedSubmits.load()) {} // Transfers value

           MTLPerFrameData& operator=(MTLPerFrameData&& other) noexcept {
               reachedSubmits.store(other.reachedSubmits.load());
               return *this;
           }

           // Delete copy constructor (already implicitly deleted)
           MTLPerFrameData(const MTLPerFrameData&) = delete;
           MTLPerFrameData& operator=(const MTLPerFrameData&) = delete;
    };

    struct MTLPipelineLayout
    {
        bool isValid = false;
    };

    class MTLBackend : public Backend
    {
    private:
        typedef void (MTLBackend::*CommandFunction)(uint8*, MTLCommandStream& stream);

    public:
        MTLBackend()
            : Backend(){};
        virtual ~MTLBackend(){};

        virtual uint16 CreateUserSemaphore() override;
        virtual void   DestroyUserSemaphore(uint16 handle) override;
        virtual void   WaitForUserSemaphore(uint16 handle, uint64 value) override;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual void   RecreateSwapchain(const SwapchainRecreateDesc& desc) override;
        virtual void   SetSwapchainActive(uint8 swp, bool isActive) override;
        static bool    CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob);
        virtual uint16 CreateShader(const ShaderDesc& shaderDesc) override;
        virtual void   DestroyShader(uint16 handle) override;
        virtual uint32 CreateTexture(const TextureDesc& desc) override;
        virtual void   DestroyTexture(uint32 handle) override;
        virtual uint32 CreateSampler(const SamplerDesc& desc) override;
        virtual void   DestroySampler(uint32 handle) override;
        virtual uint32 CreateResource(const ResourceDesc& desc) override;
        virtual void   MapResource(uint32 handle, uint8*& ptr) override;
        virtual void   UnmapResource(uint32 handle) override;
        virtual void   DestroyResource(uint32 handle) override;
        virtual uint16 CreateDescriptorSet(const DescriptorSetDesc& desc) override;
        virtual void   DestroyDescriptorSet(uint16 handle) override;
        virtual void   DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc) override;
        virtual void   DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc) override;
        virtual uint16 CreatePipelineLayout(const PipelineLayoutDesc& desc) override;
        virtual void   DestroyPipelineLayout(uint16 handle) override;
        virtual uint32 CreateCommandStream(const CommandStreamDesc& desc) override;
        virtual void   DestroyCommandStream(uint32 handle) override;
        virtual void   SetCommandStreamImpl(uint32 handle, CommandStream* stream) override;
        virtual void   CloseCommandStreams(CommandStream** streams, uint32 streamCount) override;
        virtual void   SubmitCommandStreams(const SubmitDesc& desc) override;
        virtual uint8  CreateQueue(const QueueDesc& desc) override;
        virtual void   DestroyQueue(uint8 queue) override;
        virtual uint8  GetPrimaryQueue(CommandType type) override;

    private:
        void BindDescriptorSets(MTLCommandStream& stream);

    public:
        virtual bool Initialize() override;
        virtual void Shutdown() override;
        virtual void Join() override;
        virtual void StartFrame(uint32 frameIndex) override;
        virtual void Present(const PresentDesc& present) override;
        virtual void EndFrame() override;

    private:
        void CMD_BeginRenderPass(uint8* data, MTLCommandStream& stream);
        void CMD_EndRenderPass(uint8* data, MTLCommandStream& stream);
        void CMD_SetViewport(uint8* data, MTLCommandStream& stream);
        void CMD_SetScissors(uint8* data, MTLCommandStream& stream);
        void CMD_BindPipeline(uint8* data, MTLCommandStream& stream);
        void CMD_DrawInstanced(uint8* data, MTLCommandStream& stream);
        void CMD_DrawIndexedInstanced(uint8* data, MTLCommandStream& stream);
        void CMD_DrawIndexedIndirect(uint8* data, MTLCommandStream& stream);
        void CMD_DrawIndirect(uint8* data, MTLCommandStream& stream);
        void CMD_BindVertexBuffers(uint8* data, MTLCommandStream& stream);
        void CMD_BindIndexBuffers(uint8* data, MTLCommandStream& stream);
        void CMD_CopyResource(uint8* data, MTLCommandStream& stream);
        void CMD_CopyBufferToTexture2D(uint8* data, MTLCommandStream& stream);
        void CMD_CopyTexture(uint8* data, MTLCommandStream& stream);
        void CMD_CopyTexture2DToBuffer(uint8* data, MTLCommandStream& stream);
        void CMD_BindDescriptorSets(uint8* data, MTLCommandStream& stream);
        void CMD_BindConstants(uint8* data, MTLCommandStream& stream);
        void CMD_Dispatch(uint8* data, MTLCommandStream& stream);
        void CMD_ExecuteSecondaryStream(uint8* data, MTLCommandStream& stream);
        void CMD_Barrier(uint8* data, MTLCommandStream& stream);
        void CMD_Debug(uint8* data, MTLCommandStream& stream);
        void CMD_DebugBeginLabel(uint8* data, MTLCommandStream& stream);
        void CMD_DebugEndLabel(uint8* data, MTLCommandStream& stream);

    private:
        IDList<uint8, MTLSwapchain>       m_swapchains      = {10};
        IDList<uint16, MTLShader>         m_shaders         = {20};
        IDList<uint32, MTLTexture>        m_textures        = {50};
        IDList<uint32, MTLCommandStream>  m_cmdStreams      = {50};
        IDList<uint16, MTLFence>          m_fences          = {20};
        IDList<uint32, MTLResource>       m_resources       = {100};
        IDList<uint16, MTLUserSemaphore>  m_userSemaphores  = {20};
        IDList<uint32, MTLSampler>        m_samplers        = {100};
        IDList<uint16, MTLDescriptorSet>  m_descriptorSets  = {20};
        IDList<uint8, MTLQueue>           m_queues          = {5};
        IDList<uint16, MTLPipelineLayout> m_pipelineLayouts = {10};

        uint32 m_currentFrameIndex = 0;
        uint32 m_currentImageIndex = 0;

        LINAGX_VEC<MTLPerFrameData> m_perFrameData;

        std::atomic<uint32> m_submissionPerFrame = 0;
        void*               m_device             = nullptr;

        uint8 m_primaryQueues[3];

        LINAGX_VEC<LINAGX_PAIR<LINAGX_TYPEID, CommandFunction>> m_cmdFunctions;
        std::atomic_flag                                        m_submissionFlag;
    };

} // namespace LinaGX
