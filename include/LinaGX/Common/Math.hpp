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

        LGXVector3 Cross(const LGXVector3& other) const;
        float      Dot(const LGXVector3& other) const;
        void       Normalize();
        LGXVector3 Normalized() const;
    };

    struct LGXVector2
    {
        float x = 0.0f, y = 0.0f;
    };

    struct LGXVector4
    {
        float x = 0.0f, y = 0.0f, z = 0.0, w = 0.0f;

        static LGXVector4 Lerp(const LGXVector4& start, const LGXVector4& end, float t);

        void       Normalize();
        LGXVector4 Normalized() const;
        LGXVector3 Quat2Euler();
        LGXVector4 Euler2Quat(const LGXVector3& euler);
    };

    struct LGXVector4ui16
    {
        uint16 x = 0, y = 0, z = 0, w = 0;
    };

    struct LGXMatrix4
    {
        float values[16] = {0.0f};

        LGXMatrix4 operator*(const LGXMatrix4& other) const
        {
            LGXMatrix4 result;
            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                    result.values[row * 4 + col] = 0.0f;
                    for (int k = 0; k < 4; ++k)
                    {
                        result.values[row * 4 + col] += this->values[row * 4 + k] * other.values[k * 4 + col];
                    }
                }
            }
            return result;
        }

        static LGXMatrix4 Identity();
        static LGXMatrix4 Translate(const LGXMatrix4& in, const LGXVector3& translation);
        static LGXMatrix4 Rotate(const LGXMatrix4& in, const LGXVector4& rotation);
        static LGXMatrix4 Scale(const LGXMatrix4& in, const LGXVector3& scale);

        LGXMatrix4 Transpose() const;
        float      Cofactor(int row, int col) const;
        LGXMatrix4 Inverse() const;
        LGXMatrix4 QuaternionToLGXMatrix4(const LGXVector4& q) const;
        void       InitTranslationRotationScale(const LGXVector3& translation, const LGXVector4& quaternionRotation, const LGXVector3& scale);
        void       InitLookAtRH(const LGXVector3& eye, const LGXVector3& center, const LGXVector3& up);
        void       InitPerspectiveRH(float halfFov, float aspect, float nearZ, float farZ);
    };
} // namespace LinaGX

#endif
