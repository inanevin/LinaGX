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

namespace LinaGX
{
    struct LGXVector3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
    };

    struct LGXVector2
    {
        float x = 0.0f, y = 0.0f;
    };

    struct LGXVector2ui
    {
        unsigned int x = 0, y = 0;
    };

    struct LGXVector2i
    {
        int x = 0, y = 0;
    };

    struct LGXVector4
    {
        float x = 0.0f, y = 0.0f, z = 0.0, w = 0.0f;
    };

    struct LGXVector4ui16
    {
        unsigned short x = 0, y = 0, z = 0, w = 0;
    };

    struct LGXVector4ui8
    {
        uint8 x = 0, y = 0, z = 0, w = 0;
    };

    struct LGXRectui
    {
        LGXVector2ui pos;
        LGXVector2ui size;

        bool IsPointInside(const LGXVector2ui& p) const
        {
            return p.x > pos.x && p.x < pos.x + size.x && p.y > pos.y && p.y < pos.y + size.y;
        }
    };

} // namespace LinaGX

