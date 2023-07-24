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

#include "LinaGX/Common/Math.hpp"
#include "LinaGX/Common/CommonGfx.hpp"

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

    LGXVector3 LGXVector3::Cross(const LGXVector3& other) const
    {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

    float LGXVector3::Dot(const LGXVector3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    void LGXVector3::Normalize()
    {
        float magnitude = sqrt(x * x + y * y + z * z);
        if (magnitude > 0.0f)
        {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
    }

    LGXVector3 LGXVector3::Normalized() const
    {
        LGXVector3 norm = *this;
        norm.Normalize();
        return norm;
    }

    LGXVector4 LGXVector4::Lerp(const LGXVector4& start, const LGXVector4& end, float t)
    {
        LGXVector4 result;
        result.x = start.x + (end.x - start.x) * t;
        result.y = start.y + (end.y - start.y) * t;
        result.z = start.z + (end.z - start.z) * t;
        result.w = start.w + (end.w - start.w) * t;
        return result;
    }

    void LGXVector4::Normalize()
    {
        float magnitude = sqrt(x * x + y * y + z * z + w * w);
        if (magnitude > 0.0f)
        {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
            w /= magnitude;
        }
    }

    LGXVector4 LGXVector4::Normalized() const
    {
        LGXVector4 norm = *this;
        norm.Normalize();
        return norm;
    }

    LGXVector3 LGXVector4::Quat2Euler()
    {
        LGXVector3 euler;

        // Roll (X-axis rotation)
        float sinr_cosp = 2 * (w * x + y * z);
        float cosr_cosp = 1 - 2 * (x * x + y * y);
        euler.x         = std::atan2(sinr_cosp, cosr_cosp);

        // Pitch (Y-axis rotation)
        float sinp = 2 * (w * y - z * x);
        if (std::abs(sinp) >= 1)
            euler.y = std::copysign(LPI / 2, sinp); // Use 90 degrees if out of range
        else
            euler.y = std::asin(sinp);

        // Yaw (Z-axis rotation)
        float siny_cosp = 2 * (w * z + x * y);
        float cosy_cosp = 1 - 2 * (y * y + z * z);
        euler.z         = std::atan2(siny_cosp, cosy_cosp);

        return euler;
    }

    LGXVector4 LGXVector4::Euler2Quat(const LGXVector3& euler)
    {
        LGXVector4 quaternion;

        // Abbreviations for the various angular functions
        float cy = cos(euler.z * 0.5f);
        float sy = sin(euler.z * 0.5f);
        float cp = cos(euler.y * 0.5f);
        float sp = sin(euler.y * 0.5f);
        float cr = cos(euler.x * 0.5f);
        float sr = sin(euler.x * 0.5f);

        quaternion.w = cr * cp * cy + sr * sp * sy;
        quaternion.x = sr * cp * cy - cr * sp * sy;
        quaternion.y = cr * sp * cy + sr * cp * sy;
        quaternion.z = cr * cp * sy - sr * sp * cy;

        return quaternion;
    }

    LGXMatrix4 LGXMatrix4::Identity()
    {
        LGXMatrix4 identity;

        identity.values[0]  = 1.0f;
        identity.values[5]  = 1.0f;
        identity.values[10] = 1.0f;
        identity.values[15] = 1.0f;

        return identity;
    }

    LGXMatrix4 LGXMatrix4::Translate(const LGXMatrix4& in, const LGXVector3& translation)
    {
        LGXMatrix4 res = LGXMatrix4::Identity();

        res.values[12] = translation.x;
        res.values[13] = translation.y;
        res.values[14] = translation.z;

        return in * res;
    }

    LGXMatrix4 LGXMatrix4::Scale(const LGXMatrix4& in, const LGXVector3& scale)
    {
        LGXMatrix4 res = LGXMatrix4::Identity();

        res.values[0]  = scale.x;
        res.values[5]  = scale.y;
        res.values[10] = scale.z;

        // Assuming you have implemented matrix multiplication
        return in * res;
    }

    LGXMatrix4 LGXMatrix4::Rotate(const LGXMatrix4& matrix, const LGXVector4& rot)
    {
        LGXMatrix4 rotation;

        float qx = rot.x, qy = rot.y, qz = rot.z, qw = rot.w;

        rotation.values[0] = 1 - 2 * qy * qy - 2 * qz * qz;
        rotation.values[1] = 2 * qx * qy - 2 * qz * qw;
        rotation.values[2] = 2 * qx * qz + 2 * qy * qw;

        rotation.values[4] = 2 * qx * qy + 2 * qz * qw;
        rotation.values[5] = 1 - 2 * qx * qx - 2 * qz * qz;
        rotation.values[6] = 2 * qy * qz - 2 * qx * qw;

        rotation.values[8]  = 2 * qx * qz - 2 * qy * qw;
        rotation.values[9]  = 2 * qy * qz + 2 * qx * qw;
        rotation.values[10] = 1 - 2 * qx * qx - 2 * qy * qy;

        rotation.values[15] = 1.0f;

        return matrix * rotation;
    }

    LGXMatrix4 LGXMatrix4::Transpose() const
    {
        LGXMatrix4 result;
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                result.values[row * 4 + col] = this->values[col * 4 + row];
            }
        }
        return result;
    }

    float LGXMatrix4::Cofactor(int row, int col) const
    {
        float miniDet[9];
        int   colCount = 0, rowCount = 0;

        for (int i = 0; i < 4; i++)
        {
            if (i != row)
            {
                colCount = 0;
                for (int j = 0; j < 4; j++)
                {
                    if (j != col)
                    {
                        miniDet[rowCount * 3 + colCount] = values[i * 4 + j];
                        colCount++;
                    }
                }
                rowCount++;
            }
        }

        float determinant = miniDet[0] * (miniDet[4] * miniDet[8] - miniDet[7] * miniDet[5]) - miniDet[1] * (miniDet[3] * miniDet[8] - miniDet[5] * miniDet[6]) + miniDet[2] * (miniDet[3] * miniDet[7] - miniDet[4] * miniDet[6]);
        int   sign        = ((row + col) % 2 == 0) ? 1 : -1;
        return sign * determinant;
    }

    LGXMatrix4 LGXMatrix4::Inverse() const
    {
        LGXMatrix4 result;

        // Compute the determinant
        float determinant = 0.0f;
        for (int i = 0; i < 4; ++i)
        {
            determinant += values[0 * 4 + i] * Cofactor(0, i);
        }

        // Check if the determinant is zero (with some tolerance)
        if (abs(determinant) < 1e-6)
        {
            // The matrix is singular
            LOGE("Cannot invert a singular matrix.");
        }

        // Compute the adjugate matrix and divide by the determinant
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                // Cofactor includes (-1)^(row+col) factor
                result.values[col * 4 + row] = static_cast<float>(pow(-1, row + col)) * Cofactor(row, col) / determinant;
            }
        }

        return result;
    }

    LGXMatrix4 LGXMatrix4::QuaternionToLGXMatrix4(const LGXVector4& q) const
    {
        LGXMatrix4 result;
        float   xx = q.x * q.x;
        float   yy = q.y * q.y;
        float   zz = q.z * q.z;
        float   xy = q.x * q.y;
        float   xz = q.x * q.z;
        float   yz = q.y * q.z;
        float   wx = q.w * q.x;
        float   wy = q.w * q.y;
        float   wz = q.w * q.z;

        result.values[0]  = 1.0f - 2.0f * (yy + zz);
        result.values[4]  = 2.0f * (xy - wz);
        result.values[8]  = 2.0f * (xz + wy);
        result.values[12] = 0.0f;
        result.values[1]  = 2.0f * (xy + wz);
        result.values[5]  = 1.0f - 2.0f * (xx + zz);
        result.values[9]  = 2.0f * (yz - wx);
        result.values[13] = 0.0f;
        result.values[2]  = 2.0f * (xz - wy);
        result.values[6]  = 2.0f * (yz + wx);
        result.values[10] = 1.0f - 2.0f * (xx + yy);
        result.values[14] = 0.0f;
        result.values[3]  = 0.0f;
        result.values[7]  = 0.0f;
        result.values[11] = 0.0f;
        result.values[15] = 1.0f;

        return result;
    }

    void LGXMatrix4::InitTranslationRotationScale(const LGXVector3& translation, const LGXVector4& quaternionRotation, const LGXVector3& scale)
    {
        // Compute rotation matrix from quaternion
        LGXMatrix4 rotation = QuaternionToLGXMatrix4(quaternionRotation.Normalized());

        // Create scale matrix
        LGXMatrix4 scaleMatrix;
        scaleMatrix.values[0]  = scale.x;
        scaleMatrix.values[5]  = scale.y;
        scaleMatrix.values[10] = scale.z;
        scaleMatrix.values[15] = 1.0f;

        // Create translation matrix
        LGXMatrix4 translationMatrix;
        translationMatrix.values[0]  = 1.0f;
        translationMatrix.values[5]  = 1.0f;
        translationMatrix.values[10] = 1.0f;
        translationMatrix.values[12] = translation.x;
        translationMatrix.values[13] = translation.y;
        translationMatrix.values[14] = translation.z;
        translationMatrix.values[15] = 1.0f;

        // Compute model matrix
        *this = scaleMatrix * rotation * translationMatrix;
    }

    void LGXMatrix4::InitLookAtRH(const LGXVector3& eye, const LGXVector3& center, const LGXVector3& up)
    {
        LGXVector3 f = (center - eye).Normalized();
        LGXVector3 r = up.Cross(f).Normalized();
        LGXVector3 u = f.Cross(r).Normalized();

        values[0]  = r.x;
        values[4]  = r.y;
        values[8]  = r.z;
        values[12] = -r.Dot(eye);
        values[1]  = u.x;
        values[5]  = u.y;
        values[9]  = u.z;
        values[13] = -u.Dot(eye);
        values[2]  = -f.x;
        values[6]  = -f.y;
        values[10] = -f.z;
        values[14] = f.Dot(eye);
        values[3]  = 0.0f;
        values[7]  = 0.0f;
        values[11] = 0.0f;
        values[15] = 1.0f;
    }

    void LGXMatrix4::InitPerspectiveRH(float halfFov, float aspect, float nearZ, float farZ)
    {
        const float tanHalfFovy = tan(halfFov);

        values[0] = 1.0f / (aspect * tanHalfFovy);
        values[1] = 0.0f;
        values[2] = 0.0f;
        values[3] = 0.0f;

        values[4] = 0.0f;
        values[5] = 1.0f / tanHalfFovy;
        values[6] = 0.0f;
        values[7] = 0.0f;

        values[8]  = 0.0f;
        values[9]  = 0.0f;
        values[10] = farZ / (nearZ - farZ);
        values[11] = -1.0f;

        values[12] = 0.0f;
        values[13] = 0.0f;
        values[14] = (farZ * nearZ) / (nearZ - farZ);
        values[15] = 0.0f;
    }

} // namespace LinaGX
