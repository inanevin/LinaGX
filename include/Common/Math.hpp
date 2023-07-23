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

    struct Vector3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        Vector3 operator-(const Vector3& other) const
        {
            return {x - other.x, y - other.y, z - other.z};
        }

        Vector3 Cross(const Vector3& other) const;
        float   Dot(const Vector3& other) const;
        void    Normalize();
        Vector3 Normalized() const;
    };

    struct Vector2
    {
        float x = 0.0f, y = 0.0f;
    };

    struct Vector4
    {
        float x = 0.0f, y = 0.0f, z = 0.0, w = 0.0f;

        static Vector4 Lerp(const Vector4& start, const Vector4& end, float t);

        void    Normalize();
        Vector4 Normalized() const;
        Vector3 Quat2Euler();
        Vector4 Euler2Quat(const Vector3& euler);
    };

    struct Vector4ui16
    {
        uint16 x = 0, y = 0, z = 0, w = 0;
    };

    struct Matrix4
    {
        float values[16] = {0.0f};

        Matrix4 operator*(const Matrix4& other) const
        {
            Matrix4 result;
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

        static Matrix4 Identity();
        static Matrix4 Translate(const Matrix4& in, const Vector3& translation);
        static Matrix4 Rotate(const Matrix4& in, const Vector4& rotation);
        static Matrix4 Scale(const Matrix4& in, const Vector3& scale);

        Matrix4 Transpose() const;
        float   Cofactor(int row, int col) const;
        Matrix4 Inverse() const;
        Matrix4 QuaternionToMatrix4(const Vector4& q) const;
        void    InitTranslationRotationScale(const Vector3& translation, const Vector4& quaternionRotation, const Vector3& scale);
        void    InitLookAtRH(const Vector3& eye, const Vector3& center, const Vector3& up);
        void    InitPerspectiveRH(float halfFov, float aspect, float nearZ, float farZ);
    };
} // namespace LinaGX

#endif
