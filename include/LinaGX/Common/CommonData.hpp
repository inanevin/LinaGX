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

#include <string>
#include <typeinfo>

namespace LinaGX
{

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

#ifndef LINAGX_STRINGID
#define LINAGX_STRINGID uint64_t
#endif

#ifndef LINAGX_TYPEID
#define LINAGX_TYPEID uint32_t
#endif

#define ALIGN_SIZE_POW(sizeToAlign, PowerOfTwo) (((sizeToAlign) + (PowerOfTwo)-1) & ~((PowerOfTwo)-1))
#define ALIGN_SIZE(sizeToAlign, Alignment)      (sizeToAlign + Alignment - 1) - sizeToAlign % Alignment;
#define IS_SIZE_ALIGNED(sizeToTest, PowerOfTwo) (((sizeToTest) & ((PowerOfTwo)-1)) == 0)

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

} // namespace LinaGX
