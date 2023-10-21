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
        CommandStream(Backend* backend, const CommandStreamDesc& desc, uint32 gpuHandle);
        void WriteToConstantBlock(void* data, size_t size);

        /// <summary>
        /// Use LinaGX::CMDXXX structures to perform GPU operations by adding those commands to the stream.
        /// </summary>
        /// <returns>Pointer to the allocated command structure where you can edit the parameters directly.</returns>
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
            retVal->Init();

            return retVal;
        }

        /// <summary>
        /// Some commands you add require memory addresses for some parameters, specifically arrays.
        /// But none of the commands will be executed immediately as you add them, but will be executed upon calling CloseCommandStreams().
        /// If you for example use addresses of local stack variables in functions and alike, those variables might have been destroyed by the CloseCommandStreams()
        /// call, leading to undefined behavior.
        /// In such cases, use EmplaceAuxMemory to place your variables' values alongside the command stream's persistenly mapped memory space. The space will only be
        /// cleared once all the commands are executed.
        /// </summary>
        /// <returns>The beginning address of the memory block containing the values you've passed.</returns>
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

        /// <summary>
        /// Some commands you add require memory addresses for some parameters, specifically arrays.
        /// But none of the commands will be executed immediately as you add them, but will be executed upon calling CloseCommandStreams().
        /// If you for example use addresses of local stack variables in functions and alike, those variables might have been destroyed by the CloseCommandStreams()
        /// call, leading to undefined behavior.
        /// In such cases, use EmplaceAuxMemory to place your variables' values alongside the command stream's persistenly mapped memory space. The space will only be
        /// cleared once all the commands are executed.
        /// </summary>
        /// <returns>The beginning address of the memory block containing the data you've passed.</returns>
        template <typename T>
        T* EmplaceAuxMemory(void* data, size_t size)
        {
            uint8* initialHead = m_auxMemory + m_auxMemoryIndex;
            LINAGX_MEMCPY(initialHead, data, size);
            m_auxMemoryIndex += static_cast<uint32>(size);
            LOGA(m_auxMemoryIndex < m_auxMemorySize, "Exceeded aux memory limit!");
            return reinterpret_cast<T*>(initialHead);
        }

        /// <summary>
        /// Some commands you add require memory addresses for some parameters, specifically arrays.
        /// But none of the commands will be executed immediately as you add them, but will be executed upon calling CloseCommandStreams().
        /// If you for example use addresses of local stack variables in functions and alike, those variables might have been destroyed by the CloseCommandStreams()
        /// call, leading to undefined behavior.
        /// In such cases, use EmplaceAuxMemory to place your variables' values alongside the command stream's persistenly mapped memory space. The space will only be
        /// cleared once all the commands are executed.
        /// </summary>
        /// <returns>The beginning address of an empty memory block of size 'size', you can fill the data yourself.</returns>
        template <typename T>
        T* EmplaceAuxMemorySizeOnly(size_t size)
        {
            uint8* initialHead = m_auxMemory + m_auxMemoryIndex;
            m_auxMemoryIndex += static_cast<uint32>(size);
            LOGA(m_auxMemoryIndex < m_auxMemorySize, "Exceeded aux memory limit!");
            return reinterpret_cast<T*>(initialHead);
        }

        inline uint32 GetConstantBlockSize() const
        {
            return m_constantBlockSize;
        }

        inline uint8* GetConstantBlockMemory()
        {
            return m_constantBlockMemory;
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

        uint8* m_constantBlockMemory = nullptr;
        uint32 m_constantBlockSize   = 0;
    };
} // namespace LinaGX
