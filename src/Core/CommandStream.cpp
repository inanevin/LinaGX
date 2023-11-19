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

#include "LinaGX/Core/CommandStream.hpp"
#include "LinaGX/Core/Commands.hpp"
#include "LinaGX/Core/Backend.hpp"

namespace LinaGX
{
    CommandStream::CommandStream(Backend* backend, const CommandStreamDesc& desc, uint32 gpuHandle)
    {
        m_backend           = backend;
        m_commandBufferSize = static_cast<uint32>(desc.totalMemoryLimit);
        m_commandBuffer     = (uint8*)LINAGX_MALLOC(desc.totalMemoryLimit);
        m_commands          = static_cast<uint8**>(LINAGX_MALLOC(sizeof(uint8*) * desc.commandCount));
        m_type              = desc.type;
        m_maxCommands       = desc.commandCount;
        m_gpuHandle         = gpuHandle;
        m_auxMemoryIndex    = 0;
        m_auxMemorySize     = static_cast<uint32>(desc.auxMemorySize);
        m_auxMemory         = (uint8*)LINAGX_MALLOC(desc.auxMemorySize);
        m_constantBlockSize = static_cast<uint32>(desc.constantBlockSize);

        if (Config.api != BackendAPI::Vulkan && desc.constantBlockSize != 0)
            m_constantBlockMemory = (uint8*)LINAGX_MALLOC(desc.constantBlockSize);
    }

    void CommandStream::Reset()
    {
        m_commandIndex   = 0;
        m_commandCount   = 0;
        m_auxMemoryIndex = 0;
    }

    void CommandStream::WriteToConstantBlock(void* data, size_t size)
    {
        LINAGX_MEMCPY(m_constantBlockMemory, data, size);
    }

    CommandStream::~CommandStream()
    {
        m_backend->DestroyCommandStream(m_gpuHandle);
        LINAGX_FREE(m_commands);
        LINAGX_FREE(m_commandBuffer);
        LINAGX_FREE(m_auxMemory);

        if (m_constantBlockMemory != nullptr)
            LINAGX_FREE(m_constantBlockMemory);
    }
} // namespace LinaGX
