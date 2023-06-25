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

    struct CommandLinearAllocator
    {
        void*  ptr   = nullptr;
        size_t size  = 0;
        uint32 index = 0;
    };

    class CommandStream
    {
    public:
        CommandStream(Backend* backend, CommandType type, uint32 commandCount, uint32 gpuHandle);
        ~CommandStream();

        template <typename T>
        T* AddCommand()
        {
            TypeID tid   = GetTypeID<T>();
            auto&  alloc = m_linearAllocators[tid];
            T*     start = static_cast<T*>(alloc.ptr);
            T*     ptr   = start + alloc.index;
            alloc.index++;

            m_commands[m_commandCount] = static_cast<uint64*>(alloc.ptr);
            m_types[m_commandCount]    = tid;
            m_commandCount++;
            return ptr;
        }

    private:
        friend class Renderer;
        friend class VKBackend;
        friend class MTLBackend;
        friend class DX12Backend;

        void Reset();

    private:
        LINAGX_MAP<TypeID, CommandLinearAllocator> m_linearAllocators;
        uint64**                                   m_commands     = nullptr;
        TypeID*                                    m_types        = nullptr;
        uint32                                     m_commandCount = 0;
        uint32                                     m_gpuHandle    = 0;
        Backend*                                   m_backend      = nullptr;
        CommandType                                m_type         = CommandType::Graphics;
    };
} // namespace LinaGX

#endif
