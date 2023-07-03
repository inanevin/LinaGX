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

#include "Utility/MathUtility.hpp"
#include "Common/Common.hpp"

namespace LinaGX
{
    LINAGX_API uint32 FloorLog2(uint32 val)
    {
        uint32 pos = 0;
        if (val >= 1 << 16)
        {
            val >>= 16;
            pos += 16;
        }
        if (val >= 1 << 8)
        {
            val >>= 8;
            pos += 8;
        }
        if (val >= 1 << 4)
        {
            val >>= 4;
            pos += 4;
        }
        if (val >= 1 << 2)
        {
            val >>= 2;
            pos += 2;
        }
        if (val >= 1 << 1)
        {
            pos += 1;
        }
        return (val == 0) ? 0 : pos;
    }
} // namespace LinaGX