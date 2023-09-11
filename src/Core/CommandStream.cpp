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
    CommandStream::CommandStream(Backend* backend, CommandType type, uint32 commandCount, uint32 gpuHandle, uint32 auxMemorySize)
    {
        m_backend           = backend;
        m_commandBufferSize = commandCount * 40;
        m_commandBuffer     = (uint8*)LINAGX_MALLOC(m_commandBufferSize);
        m_commands          = static_cast<uint8**>(malloc(sizeof(uint8*) * commandCount));
        m_type              = type;
        m_maxCommands       = commandCount;
        m_gpuHandle         = gpuHandle;
        m_auxMemoryIndex    = 0;
        m_auxMemorySize     = auxMemorySize;
        m_auxMemory         = (uint8*)LINAGX_MALLOC(m_auxMemorySize);
    }

    void CommandStream::Reset()
    {
        m_commandIndex   = 0;
        m_commandCount   = 0;
        m_auxMemoryIndex = 0;
    }

    CommandStream::~CommandStream()
    {
        m_backend->DestroyCommandStream(m_gpuHandle);
        LINAGX_FREE(m_commands);
        LINAGX_FREE(m_commandBuffer);
        LINAGX_FREE(m_auxMemory);
    }
} // namespace LinaGX
