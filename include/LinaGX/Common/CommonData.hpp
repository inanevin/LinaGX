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

#include "LinaGX/LinaGXExports.hpp"
#include <string>
#include <string_view>
#include <typeinfo>
#include <functional>

#ifndef LINAGX_VEC
#include <vector>
#endif

#ifndef LINAGX_MAP
#include <unordered_map>
#endif

#ifndef LINAGX_QUEUE
#include <queue>
#endif

#ifndef LINAGX_DEQUE
#include <deque>
#endif

namespace LinaGX
{

#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#include <stdint.h>
#elif defined(_MSC_VER)
    typedef signed __int8    int8_t;
    typedef unsigned __int8  uint8_t;
    typedef signed __int16   int16_t;
    typedef unsigned __int16 uint16_t;
    typedef signed __int32   int32_t;
    typedef unsigned __int32 uint32_t;
    typedef signed __int64   int64_t;
    typedef unsigned __int64 uint64_t;
    typedef uint64_t         uintptr_t;
    typedef int64_t          intptr_t;
    typedef int16_t          wchar_t;
#else
    typedef signed char        int8_t;
    typedef unsigned char      uint8_t;
    typedef signed short int   int16_t;
    typedef unsigned short int uint16_t;
    typedef signed int         int32_t;
    typedef unsigned int       uint32_t;
    typedef long long          int64_t;
    typedef unsigned long long uint64_t;
    typedef uint64_t           uintptr_t;
    typedef int64_t            intptr_t;
    typedef int16_t            wchar_t;
#endif

    typedef uint8_t   CHART;
    typedef int8_t    int8;
    typedef int16_t   int16;
    typedef int32_t   int32;
    typedef int64_t   int64;
    typedef uint8_t   uint8;
    typedef uint16_t  uint16;
    typedef uint32_t  uint32;
    typedef uint64_t  uint64;
    typedef intptr_t  intptr;
    typedef uintptr_t uintptr;

#ifndef LINAGX_DISABLE_VC_WARNING
#if defined(_MSC_VER)
#define LINAGX_DISABLE_VC_WARNING(w) __pragma(warning(push)) __pragma(warning(disable \
                                                                              : w))
#else
#define LINAGX_DISABLE_VC_WARNING(w)
#endif
#endif

#ifndef LINAGX_RESTORE_VC_WARNING
#if defined(_MSC_VER)
#define LINAGX_RESTORE_VC_WARNING() __pragma(warning(pop))
#else
#define LINAGX_RESTORE_VC_WARNING()
#endif
#endif

#ifndef LINAGX_VEC
#define LINAGX_VEC std::vector
#endif

#ifndef LINAGX_FIND_IF
#define LINAGX_FIND_IF std::find_if
#endif

#ifndef LINAGX_QUEUE
#define LINAGX_QUEUE std::queue
#endif

#ifndef LINAGX_STRING
#define LINAGX_STRING std::string
#endif

#ifndef LINAGX_TOSTRING
#define LINAGX_TOSTRING std::to_string
#endif

#ifndef LINAGX_WSTRING
#define LINAGX_WSTRING std::wstring
#endif

#ifndef LINAGX_MAP
#define LINAGX_MAP std::unordered_map
#endif

#ifndef LINAGX_DEQUE
#define LINAGX_DEQUE std::deque
#endif

#ifndef LINAGX_MALLOC
#define LINAGX_MALLOC(...) malloc(__VA_ARGS__)
#endif

#ifndef LINAGX_MEMCPY
#define LINAGX_MEMCPY(...) memcpy(__VA_ARGS__)
#endif

#ifndef LINAGX_FREE
#define LINAGX_FREE(...) free(__VA_ARGS__)
#endif

#ifndef LINAGX_STRINGID
#define LINAGX_STRINGID uint64_t
#endif

#ifndef LINAGX_TYPEID
#define LINAGX_TYPEID uint32_t
#endif

#define ALIGN_SIZE_POW(sizeToAlign, PowerOfTwo) (((sizeToAlign) + (PowerOfTwo)-1) & ~((PowerOfTwo)-1))
#define ALIGN_SIZE(sizeToAlign, Alignment)      (sizeToAlign + Alignment - 1) - sizeToAlign % Alignment;
#define IS_SIZE_ALIGNED(sizeToTest, PowerOfTwo) (((sizeToTest) & ((PowerOfTwo)-1)) == 0)

    enum class InputAction
    {
        Pressed,
        Released,
        Repeated
    };

    enum class CursorType
    {
        None,           // Default
        Default,        // Default
        SizeHorizontal, // That horizontal sizing cursor thingy
        SizeVertical,   // That vertical sizing cursor thingy
        Caret,          // Text/input caret
    };

    enum CharacterMask
    {
        Letter     = 1 << 0,
        Number     = 1 << 1,
        Separator  = 1 << 2,
        Symbol     = 1 << 4,
        Whitespace = 1 << 5,
        Control    = 1 << 6,
        Printable  = 1 << 7,
        Operator   = 1 << 8,
        Sign       = 1 << 9,
        Any        = Letter | Number | Separator | Whitespace | Control | Symbol | Printable | Operator | Sign,
    };

    template <typename U, typename T>
    class IDList
    {
    public:
        IDList(uint32 step)
        {
            m_defaultStep = step;
            m_items.resize(step, T());
        }

        ~IDList() = default;

        typename LINAGX_VEC<T>::iterator begin()
        {
            return m_items.begin();
        }

        typename LINAGX_VEC<T>::iterator end()
        {
            return m_items.end();
        }

        inline U AddItem(T item)
        {
            U id = 0;

            if (!m_availableIDs.empty())
            {
                id = m_availableIDs.front();
                m_availableIDs.pop();
            }
            else
                id = m_nextFreeID++;

            const U currentSize = static_cast<U>(m_items.size());
            if (id >= currentSize)
                m_items.resize(m_defaultStep + currentSize);

            m_items[id] = item;
            return id;
        }

        inline void AddItem(T item, U index)
        {
            m_items[index] = item;
        }

        inline void RemoveItem(U id)
        {
            m_items[id] = T();
            m_availableIDs.push(id);
        }

        inline const LINAGX_VEC<T>& GetItems() const
        {
            return m_items;
        }

        inline LINAGX_VEC<T>& GetItems()
        {
            return m_items;
        }

        inline T GetItem(int index)
        {
            return m_items[index];
        }

        inline T& GetItemR(int index)
        {
            return m_items[index];
        }

        inline U GetNextFreeID()
        {
            return m_nextFreeID;
        }

        inline void Clear()
        {
            m_items.clear();
        }

        inline void Reset()
        {
            Clear();
            m_items.resize(m_defaultStep, T());
        }

        inline T* GetRaw()
        {
            return m_items.data();
        }

    private:
        LINAGX_VEC<T>   m_items;
        LINAGX_QUEUE<U> m_availableIDs;
        U               m_nextFreeID  = 0;
        uint32          m_defaultStep = 50;
    };

    // https://gist.github.com/hwei/1950649d523afd03285c
    class FnvHash
    {
        static const unsigned int FNV_PRIME    = 16777619u;
        static const unsigned int OFFSET_BASIS = 2166136261u;
        template <unsigned int N>
        static constexpr unsigned int fnvHashConst(const char (&str)[N], unsigned int I = N)
        {
            return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnvHashConst(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
        }
        static unsigned int fnvHash(const char* str)
        {
            const size_t length = strlen(str) + 1;
            unsigned int hash   = OFFSET_BASIS;
            for (size_t i = 0; i < length; ++i)
            {
                hash ^= *str++;
                hash *= FNV_PRIME;
            }
            return hash;
        }
        struct Wrapper
        {
            Wrapper(const char* str)
                : str(str)
            {
            }
            const char* str;
        };

        // static constexpr unsigned int fnvHashConstexpr(const char* str, std::size_t len, unsigned int hash = OFFSET_BASIS)
        // {
        //     return len == 0 ? hash : fnvHashConstexpr(str + 1, len - 1, (hash ^ *str) * FNV_PRIME);
        // }

        static constexpr unsigned int fnvHashConstexpr(const char* str)
        {
            const size_t length = std::char_traits<char>::length(str) + 1;
            unsigned int hash   = OFFSET_BASIS;
            for (size_t i = 0; i < length; ++i)
            {
                hash ^= *str++;
                hash *= FNV_PRIME;
            }
            return hash;
        }

        unsigned int hash_value;

    public:
        // calulate in run-time
        FnvHash(Wrapper wrapper)
            : hash_value(fnvHash(wrapper.str))
        {
        }

        //// calulate in compile-time
        // template <unsigned int N>
        // constexpr FnvHash(const char (&str)[N])
        //     : hash_value(fnvHashConst(str))
        //{
        // }

        constexpr FnvHash(const char* str)
            : hash_value(fnvHashConstexpr(str))
        {
        }

        // output result
        constexpr operator unsigned int() const
        {
            return this->hash_value;
        }
    };

    template <typename T>
    LINAGX_TYPEID LGX_GetTypeID()
    {
        return FnvHash(typeid(T).name());
    }

    constexpr LINAGX_STRINGID operator"" _hs(const char* str, size_t len) noexcept
    {
        return FnvHash(str);
    }

    extern LINAGX_STRINGID LGX_ToSID(const char* ty);

    template <typename T>
    class RingBuffer
    {
    public:
        inline void Initialize(uint32 size)
        {
            m_elements.resize(size, T());
            m_size = size;
        }

        inline uint32 GetSize()
        {
            return m_size;
        }

        inline void Place(uint32 index, T item)
        {
            m_elements[index] = item;
        }

        inline T GetElement(uint32 index)
        {
            return m_elements[index % m_size];
        }

        inline const T& GetElementRef(uint32 index)
        {
            return m_elements[index % m_size];
        }

        inline T GetHead()
        {
            return m_elements[m_head];
        }

        inline void Increment()
        {
            m_head = (m_head + 1) % m_size;
        }

        inline uint32 GetHeadIndex()
        {
            return m_head;
        }

        inline void Clear()
        {
            m_elements.clear();
        }

    private:
        LINAGX_VEC<T> m_elements;
        uint32        m_head = 0;
        uint32        m_size = 0;
    };

} // namespace LinaGX
