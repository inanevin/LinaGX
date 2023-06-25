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

#include "Common/Common.hpp"

namespace LinaGX
{
    class Renderer;

    class Backend
    {
    public:
        Backend(){};
        virtual ~Backend(){};

        virtual bool Initialize(const InitInfo& initInfo) = 0;
        virtual void Shutdown()                           = 0;

        virtual bool CompileShader(ShaderStage stage, const LINAGX_STRING& source, DataBlob& outBlob)
        {
            return false;
        };

        virtual uint8 CreateSwapchain(const SwapchainDesc& desc)
        {
            return 0;
        }

        virtual void DestroySwapchain(uint8 handle){};

        virtual uint16 GenerateShader(const LINAGX_MAP<ShaderStage, DataBlob>& stages, const ShaderDesc& shaderDesc)
        {
            return 0;
        };
        virtual void DestroyShader(uint16 handle){};

        virtual uint32 CreateTexture2D(const Texture2DDesc& desc)
        {
            return 0;
        };
        virtual void DestroyTexture2D(uint32 handle){};

        static Backend* CreateBackend(BackendAPI api);

    protected:
    };
} // namespace LinaGX

#endif
