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
