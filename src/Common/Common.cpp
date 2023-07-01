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

/*

PushConstants are only supported as layout = 0, set = 0
No other shader resource should occupy that binding.
Only 1 type of PushConstants are supported.

*/

#pragma once

#include "Common/Common.hpp"
#include <sstream>
#include <fstream>

namespace LinaGX
{
    Configuration  Config  = {};
    GPUInformation GPUInfo = {};

    LINAGX_API char* Internal::WCharToChar(const wchar_t* wch)
    {
        // Count required buffer size (plus one for null-terminator).
        size_t size   = (wcslen(wch) + 1) * sizeof(wchar_t);
        char*  buffer = new char[size];

#ifdef __STDC_LIB_EXT1__
        // wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
        size_t convertedSize;
        std::wcstombs_s(&convertedSize, buffer, size, input, size);
#else
#pragma warning(disable : 4996)
        std::wcstombs(buffer, wch, size);
#endif
        return buffer;
    }

    uint32 FnvHash::fnvHash(const char* str)
    {
        const size_t length = strlen(str) + 1;
        uint32       hash   = OFFSET_BASIS;
        for (size_t i = 0; i < length; ++i)
        {
            hash ^= *str++;
            hash *= FNV_PRIME;
        }
        return hash;
    }

    LINAGX_STRING Internal::ReadFileContentsAsString(const char* filePath)
    {
        std::ifstream ifs(filePath);
        auto          a = std::istreambuf_iterator<char>(ifs);
        auto          b = (std::istreambuf_iterator<char>());
        std::string   content(a, b);
        return content.c_str();
    }

} // namespace LinaGX
