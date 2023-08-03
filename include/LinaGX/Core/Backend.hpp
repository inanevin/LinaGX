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
        Backend(Instance* renderer)
            : m_lgx(renderer){};
        virtual ~Backend(){};

        virtual bool Initialize(const InitInfo& initInfo) = 0;
        virtual void Shutdown()                           = 0;
        virtual void Join()                               = 0;
        virtual void StartFrame(uint32 frameIndex)        = 0;
        virtual void EndFrame()                           = 0;
        virtual void Present(const PresentDesc& present)  = 0;

        virtual uint16 CreateUserSemaphore()                                                            = 0;
        virtual void   DestroyUserSemaphore(uint16 handle)                                              = 0;
        virtual void   WaitForUserSemaphore(uint16 handle, uint64 value)                                = 0;
        virtual bool   CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob) = 0;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc)                                       = 0;
        virtual void   DestroySwapchain(uint8 handle)                                                   = 0;
        virtual void   RecreateSwapchain(const SwapchainRecreateDesc& desc)                             = 0;
        virtual uint16 CreateShader(const ShaderDesc& shaderDesc)                                       = 0;
        virtual void   DestroyShader(uint16 handle)                                                     = 0;
        virtual uint32 CreateTexture2D(const Texture2DDesc& desc)                                       = 0;
        virtual void   DestroyTexture2D(uint32 handle)                                                  = 0;
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
        virtual uint32 CreateCommandStream(QueueType type)                                              = 0;
        virtual void   DestroyCommandStream(uint32 handle)                                              = 0;
        virtual void   CloseCommandStreams(CommandStream** streams, uint32 streamCount)                 = 0;
        virtual void   SubmitCommandStreams(const SubmitDesc& desc)                                     = 0;

        static Backend* CreateBackend(BackendAPI api, Instance* renderer);

    protected:
        Instance* m_lgx = nullptr;
    };
} // namespace LinaGX

#endif
