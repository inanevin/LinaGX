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

#ifndef LINAGX_Backend_HPP
#define LINAGX_Backend_HPP

#include "LinaGX/Common/CommonGfx.hpp"

namespace LinaGX
{
    class Instance;
    class CommandStream;

    class Backend
    {
    public:
        Backend()          = default;
        virtual ~Backend() = default;

        virtual bool Initialize()                        = 0;
        virtual void Shutdown()                          = 0;
        virtual void Join()                              = 0;
        virtual void StartFrame(uint32 frameIndex)       = 0;
        virtual void EndFrame()                          = 0;
        virtual void Present(const PresentDesc& present) = 0;

        virtual uint16 CreateUserSemaphore()                                                            = 0;
        virtual void   DestroyUserSemaphore(uint16 handle)                                              = 0;
        virtual void   WaitForUserSemaphore(uint16 handle, uint64 value)                                = 0;
        virtual bool   CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob) = 0;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc)                                       = 0;
        virtual void   DestroySwapchain(uint8 handle)                                                   = 0;
        virtual void   RecreateSwapchain(const SwapchainRecreateDesc& desc)                             = 0;
        virtual void   SetSwapchainActive(uint8 swp, bool isActive)                                     = 0;
        virtual uint16 CreateShader(const ShaderDesc& shaderDesc)                                       = 0;
        virtual void   DestroyShader(uint16 handle)                                                     = 0;
        virtual uint32 CreateTexture(const TextureDesc& desc)                                           = 0;
        virtual void   DestroyTexture(uint32 handle)                                                    = 0;
        virtual uint32 CreateSampler(const SamplerDesc& desc)                                           = 0;
        virtual void   DestroySampler(uint32 handle)                                                    = 0;
        virtual uint32 CreateResource(const ResourceDesc& desc)                                         = 0;
        virtual void   DestroyResource(uint32 handle)                                                   = 0;
        virtual void   MapResource(uint32 resource, uint8*& ptr)                                        = 0;
        virtual void   UnmapResource(uint32 resource)                                                   = 0;
        virtual uint16 CreateDescriptorSet(const DescriptorSetDesc& desc)                               = 0;
        virtual void   DestroyDescriptorSet(uint16 handle)                                              = 0;
        virtual void   DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc& desc)                   = 0;
        virtual void   DescriptorUpdateImage(const DescriptorUpdateImageDesc& desc)                     = 0;
        virtual uint16 CreatePipelineLayout(const PipelineLayoutDesc& desc)                             = 0;
        virtual void   DestroyPipelineLayout(uint16 layout)                                             = 0;
        virtual uint32 CreateCommandStream(const CommandStreamDesc& desc)                               = 0;
        virtual void   DestroyCommandStream(uint32 handle)                                              = 0;
        virtual void   SetCommandStreamImpl(uint32 handle, CommandStream* stream)                       = 0;
        virtual void   CloseCommandStreams(CommandStream** streams, uint32 streamCount)                 = 0;
        virtual void   SubmitCommandStreams(const SubmitDesc& desc)                                     = 0;
        virtual uint8  CreateQueue(const QueueDesc& desc)                                               = 0;
        virtual void   DestroyQueue(uint8 queue)                                                        = 0;
        virtual uint8  GetPrimaryQueue(CommandType type)                                                = 0;

        static Backend* CreateBackend();
    };
} // namespace LinaGX

#endif
