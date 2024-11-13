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

#pragma once

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/CommonData.hpp"

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
