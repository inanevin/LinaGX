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

#include "Core/CommandStream.hpp"
#include "Core/Commands.hpp"
#include "Core/Backend.hpp"

namespace LinaGX
{
    template <typename T>
    void LinaGX_AllocCommand(LINAGX_MAP<TypeID, CommandLinearAllocator>& map, uint32 commandCount)
    {
        auto& alloc = map[GetTypeID<T>()];
        alloc.size  = sizeof(T) * commandCount;
        alloc.ptr   = LINAGX_MALLOC(alloc.size);
        alloc.index = 0;
    }

    CommandStream::CommandStream(Backend* backend, CommandType type, uint32 commandCount, uint32 gpuHandle)
    {
        m_backend = backend;

        LinaGX_AllocCommand<CMDBeginRenderPass>(m_linearAllocators, 20);
        LinaGX_AllocCommand<CMDEndRenderPass>(m_linearAllocators, 20);
        LinaGX_AllocCommand<CMDBindPipeline>(m_linearAllocators, 20);
        LinaGX_AllocCommand<CMDSetViewport>(m_linearAllocators, 20);
        LinaGX_AllocCommand<CMDSetScissors>(m_linearAllocators, 20);
        LinaGX_AllocCommand<CMDDrawInstanced>(m_linearAllocators, 20);
        LinaGX_AllocCommand<CMDDrawIndexedInstanced>(m_linearAllocators, 20);

        m_commands = static_cast<uint64**>(malloc(sizeof(uint64*) * commandCount));
        m_types    = static_cast<TypeID*>(malloc(sizeof(TypeID) * commandCount));
        m_type     = type;
    }

    void CommandStream::Reset()
    {
        for (auto& [tid, alloc] : m_linearAllocators)
            alloc.index = 0;

        m_commandCount = 0;
    }

    CommandStream::~CommandStream()
    {
        m_backend->DestroyCommandStream(m_gpuHandle);
        LINAGX_FREE(m_commands);
        LINAGX_FREE(m_types);

        for (auto& [tid, alloc] : m_linearAllocators)
        {
            if (alloc.ptr != 0)
                LINAGX_FREE(alloc.ptr);
        }
    }
} // namespace LinaGX
