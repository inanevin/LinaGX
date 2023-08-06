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

#ifndef LINAGX_DATASTRUCTURES_HPP
#define LINAGX_DATASTRUCTURES_HPP

#include "LinaGX/LinaGXExports.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <deque>
#include <functional>
#include <typeinfo>
#include <functional>

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
#define LINAGX_STRINGID uint32_t
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

    enum class CursorMode
    {
        Visible  = 1 << 0,
        Hidden   = 1 << 1,
        Disabled = 1 << 2
    };

    enum class CursorType
    {
        None,
        Default,
        SizeHorizontal,
        SizeVertical,
        Caret,
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

    template <typename T>
    struct Handle
    {
        Handle(T it)
        {
            item = it;
        }

        T    item    = T();
        bool isValid = true;
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
    public:
        static const unsigned int FNV_PRIME    = 16777619u;
        static const unsigned int OFFSET_BASIS = 2166136261u;
        template <unsigned int N>
        static constexpr unsigned int fnvHashConst(const char (&str)[N], unsigned int I = N)
        {
            return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnvHashConst(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
        }
        static uint32 fnvHash(const char* str);
        struct Wrapper
        {
            Wrapper(const char* str)
                : str(str)
            {
            }
            const char* str;
        };
        unsigned int hash_value;

    public:
        // calulate in run-time
        FnvHash(Wrapper wrapper)
            : hash_value(fnvHash(wrapper.str))
        {
        }
        // calulate in compile-time
        template <unsigned int N>
        constexpr FnvHash(const char (&str)[N])
            : hash_value(fnvHashConst(str))
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

    constexpr LINAGX_STRINGID operator"" _hs(const char* str, std::size_t) noexcept
    {
        return FnvHash(str);
    }

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

#endif
