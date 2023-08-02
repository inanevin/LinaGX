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

#ifndef LINAGX_MATH_HPP
#define LINAGX_MATH_HPP

#include "CommonData.hpp"

namespace LinaGX
{

#define DEG2RAD(X) X * 0.0174533f
#define LPI        3.14159265359f

    extern LINAGX_API uint32 FloorLog2(uint32 val);

    template <typename T>
    inline constexpr T Max(const T& val1, const T& val2)
    {
        return val1 >= val2 ? val1 : val2;
    }

    template <typename T>
    inline constexpr T Min(const T& val1, const T& val2)
    {
        return val1 <= val2 ? val1 : val2;
    }

    template <typename T, typename U>
    static T Lerp(const T& val1, const T& val2, const U& amt)
    {
        return (T)(val1 * ((U)(1) - amt) + val2 * amt);
    }

    struct LGXVector3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        LGXVector3 operator-(const LGXVector3& other) const
        {
            return {x - other.x, y - other.y, z - other.z};
        }
    };

    struct LGXVector2
    {
        float x = 0.0f, y = 0.0f;
    };

    struct LGXVector2ui
    {
        uint32 x = 0, y = 0;
    };

    struct LGXVector2i
    {
        int32 x = 0, y = 0;
    };

    struct LGXVector4
    {
        float x = 0.0f, y = 0.0f, z = 0.0, w = 0.0f;
    };

    struct LGXVector4ui16
    {
        uint16 x = 0, y = 0, z = 0, w = 0;
    };

    struct LGXRectui
    {
        LGXVector2ui pos;
        LGXVector2ui size;
    };
} // namespace LinaGX

#endif
