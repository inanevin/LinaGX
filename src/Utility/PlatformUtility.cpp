/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2023-] [Inan Evin]

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

#include "Utility/PlatformUtility.hpp"
#include "Common/Common.hpp"
#include <sstream>
#include <fstream>
#include <codecvt>
#include <locale>
#include <string>
namespace LinaGX
{

    char* WCharToChar(const wchar_t* wch)
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

    LINAGX_API const wchar_t* CharToWChar(const char* ch)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring                                     wideStr = converter.from_bytes(ch);

        wchar_t* wideStrCopy = new wchar_t[wideStr.size() + 1];
        wcscpy_s(wideStrCopy, wideStr.size() + 1, wideStr.c_str());

        return wideStrCopy;
    }

    LINAGX_STRING ReadFileContentsAsString(const char* filePath)
    {
        std::ifstream ifs(filePath);
        auto          a = std::istreambuf_iterator<char>(ifs);
        auto          b = (std::istreambuf_iterator<char>());
        std::string   content(a, b);
        return content.c_str();
    }

    LINAGX_API void* AdjustBufferPitch(void* data, uint32 width, uint32 height, uint32 bytesPerPixel, uint32 alignment)
    {
        // calculate the adjusted row pitch
        uint32_t rowPitch = (width * bytesPerPixel + (alignment - 1)) & ~(alignment - 1);

        // create a new buffer with the adjusted pitch
        char* buffer = (char*)LINAGX_MALLOC(rowPitch * height);

        // copy each row from the original data to the new buffer
        char* src = static_cast<char*>(data);
        char* dst = buffer;

        if (dst != 0)
        {
            for (uint32_t i = 0; i < height; ++i)
            {
                memcpy(dst, src, width * bytesPerPixel);
                dst += rowPitch;
                src += width * bytesPerPixel;
            }
        }
        else
        {
            LOGE("Malloc failed!");
        }

        return buffer;
    }

} // namespace LinaGX