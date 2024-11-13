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

#include "LinaGX/Utility/PlatformUtility.hpp"
#include "LinaGX/Common/CommonGfx.hpp"
#include <sstream>
#include <fstream>
#include <codecvt>
#include <locale>
#include <string>
#include <iostream>

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
#ifdef LINAGX_PLATFORM_WINDOWS
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring                                     wideStr = converter.from_bytes(ch);

        wchar_t* wideStrCopy = new wchar_t[wideStr.size() + 1];
        wcscpy_s(wideStrCopy, wideStr.size() + 1, wideStr.c_str());

        return wideStrCopy;
#endif
        
#ifdef LINAGX_PLATFORM_APPLE
        // Convert the input char string to a wchar_t string on Apple platform
       size_t length = strlen(ch);
       wchar_t* wideStrCopy = new wchar_t[length + 1];

       mbstowcs(wideStrCopy, ch, length);
       wideStrCopy[length] = L'\0'; // Null-terminate the wide string

       return wideStrCopy;
#endif
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
        uint32 rowPitch = (width * bytesPerPixel + (alignment - 1)) & ~(alignment - 1);

        // create a new buffer with the adjusted pitch
        char* buffer = (char*)LINAGX_MALLOC(rowPitch * height);

        // copy each row from the original data to the new buffer
        char* src = static_cast<char*>(data);
        char* dst = buffer;

        if (dst != 0)
        {
            for (uint32 i = 0; i < height; ++i)
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
