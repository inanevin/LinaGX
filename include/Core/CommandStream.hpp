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

#ifndef LINAGX_COMMAND_STREAM_HPP
#define LINAGX_COMMAND_STREAM_HPP

#include "Common/Common.hpp"

namespace LinaGX
{
    class Backend;
    class Renderer;
    class DX12Backend;
    class MTLBackend;
    class VKBackend;

    class CommandStream
    {
    public:
        CommandStream(Backend* backend, QueueType type, uint32 commandCount, uint32 gpuHandle);

        template <typename T>
        T* AddCommand()
        {
            LOGA((m_commandCount < m_maxCommands), "Command Stream -> Max command count (%d) is exceeded (%d)!", m_maxCommands, m_commandCount);
            const TypeID tid      = GetTypeID<T>();
            const size_t typeSize = sizeof(TypeID);

            uint8* currentHead = m_commandBuffer + m_commandIndex;

            // Place type header.
            LINAGX_MEMCPY(currentHead, &tid, typeSize);

            // Find command start.
            uint8* ptr = currentHead + typeSize;

            // Assign command ptr
            m_commands[m_commandCount] = currentHead;
            m_commandIndex += sizeof(T) + typeSize;

            m_commandCount++;
            T* retVal = reinterpret_cast<T*>(ptr);

            // Commons
            retVal->extension = nullptr;

            return retVal;
        }

    private:
        friend class Renderer;
        friend class VKBackend;
        friend class MTLBackend;
        friend class DX12Backend;
        ~CommandStream();
        void Reset();

    private:
        uint8**   m_commands     = nullptr;
        uint32    m_commandCount = 0;
        uint32    m_gpuHandle    = 0;
        Backend*  m_backend      = nullptr;
        QueueType m_type         = QueueType::Graphics;

        uint8* m_commandBuffer = nullptr;
        size_t m_commandIndex  = 0;
        uint32 m_maxCommands   = 0;
    };
} // namespace LinaGX

#endif
