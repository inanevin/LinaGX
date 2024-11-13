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

#include "LinaGX/Core/Backend.hpp"
#include "LinaGX/Common/CommonData.hpp"

#ifdef LINAGX_PLATFORM_WINDOWS

#ifndef LINAGX_DISABLE_DX12
#include "LinaGX/Platform/DX12/DX12Backend.hpp"
#endif

#ifndef LINAGX_DISABLE_VK
#include "LinaGX/Platform/Vulkan/VKBackend.hpp"
#endif

#endif

#ifdef LINAGX_PLATFORM_APPLE
#include "LinaGX/Platform/Metal/MTLBackend.hpp"
#endif

namespace LinaGX
{
    Backend* LinaGX::Backend::CreateBackend()
    {
        
#ifdef LINAGX_PLATFORM_WINDOWS

        if (Config.api == BackendAPI::Vulkan)
        {
#ifndef LINAGX_DISABLE_VK
            return new VKBackend();
#else
            return nullptr;
#endif
        }
        else if (Config.api == BackendAPI::DX12)
        {
#ifndef LINAGX_DISABLE_DX12
            return new DX12Backend();
#else
            return nullptr;
#endif
        }
#endif

#ifdef LINAGX_PLATFORM_APPLE
        return new MTLBackend();
#endif
        return nullptr;
    }
} // namespace LinaGX
