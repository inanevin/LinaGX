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

#include "LinaGX/Common/CommonGfx.hpp"

namespace LinaGX
{
    class Backend;
    class Instance;
    class DX12Backend;
    class MTLBackend;
    class VKBackend;

    class CommandStream
    {
    public:
        CommandStream(Backend* backend, CommandType type, uint32 commandCount, uint32 gpuHandle, uint32 auxMemorySize);

        template <typename T>
        T* AddCommand()
        {
            LOGA((m_commandCount < m_maxCommands), "Command Stream -> Max command count (%d) is exceeded (%d)!", m_maxCommands, m_commandCount);
            const LINAGX_TYPEID tid      = LGX_GetTypeID<T>();
            const size_t        typeSize = sizeof(LINAGX_TYPEID);

            uint8* currentHead = m_commandBuffer + m_commandIndex;

            // Place type header.
            LINAGX_MEMCPY(currentHead, &tid, typeSize);

            // Find command start.
            uint8* ptr = currentHead + typeSize;

            // Assign command ptr
            m_commands[m_commandCount] = currentHead;
            m_commandIndex += sizeof(T) + typeSize;
            LOGA(m_commandIndex < m_commandBufferSize, "Exceeded command memory limit!");

            m_commandCount++;
            T* retVal = reinterpret_cast<T*>(ptr);

            // Commons
            retVal->extension = nullptr;

            return retVal;
        }

        template <typename T, typename... Args>
        T* EmplaceAuxMemory(T firstValue, Args... remainingValues)
        {
            uint8* initialHead = m_auxMemory + m_auxMemoryIndex;

            // Place the first value in memory
            uint8* currentHead = initialHead;
            LINAGX_MEMCPY(currentHead, &firstValue, sizeof(T));
            m_auxMemoryIndex += sizeof(T);
            LOGA(m_auxMemoryIndex < m_auxMemorySize, "Exceeded aux memory limit!");

            // Recursively place the remaining values in memory
            if constexpr (sizeof...(remainingValues) > 0)
            {
                EmplaceAuxMemory<T>(remainingValues...);
            }

            return reinterpret_cast<T*>(initialHead);
        }

        template <typename T>
        T* EmplaceAuxMemory(void* data, size_t size)
        {
            uint8* initialHead = m_auxMemory + m_auxMemoryIndex;
            LINAGX_MEMCPY(initialHead, data, size);
            m_auxMemoryIndex += static_cast<uint32>(size);
            LOGA(m_auxMemoryIndex < m_auxMemorySize, "Exceeded aux memory limit!");
            return reinterpret_cast<T*>(initialHead);
        }

        template <typename T>
        T* EmplaceAuxMemorySizeOnly(size_t size)
        {
            uint8* initialHead = m_auxMemory + m_auxMemoryIndex;
            m_auxMemoryIndex += static_cast<uint32>(size);
            LOGA(m_auxMemoryIndex < m_auxMemorySize, "Exceeded aux memory limit!");
            return reinterpret_cast<T*>(initialHead);
        }

    private:
        friend class Instance;
        friend class VKBackend;
        friend class MTLBackend;
        friend class DX12Backend;
        ~CommandStream();
        void Reset();

    private:
        uint8**     m_commands     = nullptr;
        uint32      m_commandCount = 0;
        uint32      m_gpuHandle    = 0;
        Backend*    m_backend      = nullptr;
        CommandType m_type         = CommandType::Graphics;

        uint8* m_commandBuffer     = nullptr;
        size_t m_commandIndex      = 0;
        uint32 m_maxCommands       = 0;
        uint32 m_commandBufferSize = 0;

        uint8* m_auxMemory      = nullptr;
        uint32 m_auxMemoryIndex = 0;
        uint32 m_auxMemorySize  = 0;
    };
} // namespace LinaGX

#endif
