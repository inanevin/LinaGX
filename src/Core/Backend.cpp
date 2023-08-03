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

#include "LinaGX/Core/Backend.hpp"

#ifdef LINAGX_PLATFORM_WINDOWS
#include "LinaGX/Platform/DX12/DX12Backend.hpp"
#include "LinaGX/Platform/Vulkan/VKBackend.hpp"
#endif

#ifdef LINAGX_PLATFORM_APPLE
#include "LinaGX/Platform/Metal/MTLBackend.hpp"
#endif

namespace LinaGX
{
    Backend* LinaGX::Backend::CreateBackend(BackendAPI api, Instance* renderer)
    {
        switch (api)
        {
        case BackendAPI::Vulkan:
            return new VKBackend(renderer);
        case BackendAPI::DX12:
            return new DX12Backend(renderer);

#ifdef LINAGX_PLATFORM_APPLE
        case BackendAPI::Metal:
            return new MTLBackend(renderer);
#endif
        default:
            return nullptr;
        }

        return nullptr;
    }
} // namespace LinaGX
