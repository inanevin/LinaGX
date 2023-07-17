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

#ifndef LINAGX_DATASTRUCTURES_HPP
#define LINAGX_DATASTRUCTURES_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <deque>
#include <typeinfo>
#include <functional>

namespace LinaGX
{
#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#include <stdint.h>
#elif defined(_MSC_VER)
    typedef signed __int8    int8_t;
    typedef unsigned __int8  uint8_t;
    typedef signed __int16   int16_t;
    typedef unsigned __int16 uint16_t;
    typedef signed __int32   int32_t;
    typedef unsigned __int32 uint32_t;
    typedef signed __int64   int64_t;
    typedef unsigned __int64 uint64_t;
    typedef uint64_t         uintptr_t;
    typedef int64_t          intptr_t;
    typedef int16_t          wchar_t;
#else
    typedef signed char        int8_t;
    typedef unsigned char      uint8_t;
    typedef signed short int   int16_t;
    typedef unsigned short int uint16_t;
    typedef signed int         int32_t;
    typedef unsigned int       uint32_t;
    typedef long long          int64_t;
    typedef unsigned long long uint64_t;
    typedef uint64_t           uintptr_t;
    typedef int64_t            intptr_t;
    typedef int16_t            wchar_t;
#endif

    typedef uint8_t   CHART;
    typedef int8_t    int8;
    typedef int16_t   int16;
    typedef int32_t   int32;
    typedef int64_t   int64;
    typedef uint8_t   uint8;
    typedef uint16_t  uint16;
    typedef uint32_t  uint32;
    typedef uint64_t  uint64;
    typedef intptr_t  intptr;
    typedef uintptr_t uintptr;

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

#define DEG2RAD(X) X * 0.0174533
#define LPI        3.14159265359f

    struct Vector3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        Vector3 operator-(const Vector3& other) const
        {
            return {x - other.x, y - other.y, z - other.z};
        }

        Vector3 Cross(const Vector3& other) const
        {
            return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
        }

        float Dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

        void Normalize()
        {
            float magnitude = sqrt(x * x + y * y + z * z);
            if (magnitude > 0.0f)
            {
                x /= magnitude;
                y /= magnitude;
                z /= magnitude;
            }
        }

        Vector3 Normalized() const
        {
            Vector3 norm = *this;
            norm.Normalize();
            return norm;
        }
    };

    struct Vector2
    {
        float x = 0.0f, y = 0.0f;
    };

    struct Vector4
    {
        float x = 0.0f, y = 0.0f, z = 0.0, w = 0.0f;

        void Normalize()
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

        Vector4 Normalized() const
        {
            Vector4 norm = *this;
            norm.Normalize();
            return norm;
        }

        Vector3 Quat2Euler()
        {
            Vector3 euler;

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

        Vector4 Euler2Quat(const Vector3& euler)
        {
            Vector4 quaternion;

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
    };

#ifndef LINAGX_VEC
#define LINAGX_VEC std::vector
#endif

#ifndef INAGX_QUEUE
#define LINAGX_QUEUE std::queue
#endif

#ifndef LINAGX_STRING
#define LINAGX_STRING std::string
#endif

#ifndef LINAGX_WSTRING
#define LINAGX_WSTRING std::wstring
#endif

#ifndef LINAGX_MAP
#define LINAGX_MAP std::unordered_map
#endif

#ifndef LINAGX_DEQUE
#define LINAGX_DEQUE std::deque
#endif

#ifndef LINAGX_MALLOC
#define LINAGX_MALLOC(...) malloc(__VA_ARGS__)
#endif

#ifndef LINAGX_MEMCPY
#define LINAGX_MEMCPY(...) memcpy(__VA_ARGS__)
#endif

#ifndef LINAGX_FREE
#define LINAGX_FREE(...) free(__VA_ARGS__)
#endif

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

        Matrix4 Transpose() const
        {
            Matrix4 result;
            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                    result.values[row * 4 + col] = this->values[col * 4 + row];
                }
            }
            return result;
        }

        float Cofactor(int row, int col) const
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

        Matrix4 Inverse() const
        {
            Matrix4 result;
            // This is a very simplified inversion for 4x4 matrices,
            // and doesn't handle cases where the matrix is singular or near-singular
            float determinant = 0.0f;
            for (int i = 0; i < 4; i++)
                determinant += values[0 * 4 + i] * Cofactor(0, i);
            for (int row = 0; row < 4; row++)
            {
                for (int col = 0; col < 4; col++)
                {
                    result.values[col * 4 + row] = Cofactor(row, col) / determinant;
                }
            }
            return result;
        }

        Matrix4 QuaternionToMatrix4(const Vector4& q) const
        {
            Matrix4 result;
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
            result.values[1]  = 2.0f * (xy - wz);
            result.values[2]  = 2.0f * (xz + wy);
            result.values[3]  = 0.0f;
            result.values[4]  = 2.0f * (xy + wz);
            result.values[5]  = 1.0f - 2.0f * (xx + zz);
            result.values[6]  = 2.0f * (yz - wx);
            result.values[7]  = 0.0f;
            result.values[8]  = 2.0f * (xz - wy);
            result.values[9]  = 2.0f * (yz + wx);
            result.values[10] = 1.0f - 2.0f * (xx + yy);
            result.values[11] = 0.0f;
            result.values[12] = 0.0f;
            result.values[13] = 0.0f;
            result.values[14] = 0.0f;
            result.values[15] = 1.0f;

            return result;
        }

        void InitTranslationRotationScale(const Vector3& translation, const Vector4& quaternionRotation, const Vector3& scale)
        {
            // Compute rotation matrix from quaternion
            Matrix4 rotation = QuaternionToMatrix4(quaternionRotation.Normalized());

            // Create scale matrix
            Matrix4 scaleMatrix;
            scaleMatrix.values[0]  = scale.x;
            scaleMatrix.values[5]  = scale.y;
            scaleMatrix.values[10] = scale.z;
            scaleMatrix.values[15] = 1.0f;

            // Create translation matrix
            Matrix4 translationMatrix;
            translationMatrix.values[0]  = 1.0f;
            translationMatrix.values[5]  = 1.0f;
            translationMatrix.values[10] = 1.0f;
            translationMatrix.values[12] = translation.x;
            translationMatrix.values[13] = translation.y;
            translationMatrix.values[14] = translation.z;
            translationMatrix.values[15] = 1.0f;

            // Compute model matrix
            *this = translationMatrix * rotation * scaleMatrix;
        }

        void InitLookAtRH(const Vector3& eye, const Vector3& center, const Vector3& up)
        {
            Vector3 f = (center - eye).Normalized();
            Vector3 r = up.Cross(f).Normalized();
            Vector3 u = f.Cross(r).Normalized();

            values[0]  = r.x;
            values[1]  = r.y;
            values[2]  = r.z;
            values[3]  = -r.Dot(eye);
            values[4]  = u.x;
            values[5]  = u.y;
            values[6]  = u.z;
            values[7]  = -u.Dot(eye);
            values[8]  = -f.x;
            values[9]  = -f.y;
            values[10] = -f.z;
            values[11] = f.Dot(eye);
            values[12] = 0.0f;
            values[13] = 0.0f;
            values[14] = 0.0f;
            values[15] = 1.0f;
        }

        void InitPerspectiveRH(float halfFov, float aspect, float nearZ, float farZ)
        {
            float tangentHalfFov = tan(halfFov);

            values[0]  = 1.0f / (tangentHalfFov * aspect);
            values[1]  = 0.0f;
            values[2]  = 0.0f;
            values[3]  = 0.0f;
            values[4]  = 0.0f;
            values[5]  = 1.0f / tangentHalfFov;
            values[6]  = 0.0f;
            values[7]  = 0.0f;
            values[8]  = 0.0f;
            values[9]  = 0.0f;
            values[10] = farZ / (nearZ - farZ);
            values[11] = (farZ * nearZ) / (nearZ - farZ);
            values[12] = 0.0f;
            values[13] = 0.0f;
            values[14] = -1.0f;
            values[15] = 0.0f;
        }
    };

#define ALIGN_SIZE_POW(sizeToAlign, PowerOfTwo) (((sizeToAlign) + (PowerOfTwo)-1) & ~((PowerOfTwo)-1))
#define ALIGN_SIZE(sizeToAlign, Alignment)      (sizeToAlign + Alignment - 1) - sizeToAlign % Alignment;
#define IS_SIZE_ALIGNED(sizeToTest, PowerOfTwo) (((sizeToTest) & ((PowerOfTwo)-1)) == 0)

    template <typename T>
    struct Handle
    {
        Handle(T it)
        {
            item = it;
        }

        T    item    = T();
        bool isValid = true;
    };

    template <typename U, typename T>
    class IDList
    {
    public:
        IDList(uint32 step)
        {
            m_defaultStep = step;
            m_items.resize(step, T());
        }

        ~IDList() = default;

        typename LINAGX_VEC<T>::iterator begin()
        {
            return m_items.begin();
        }

        typename LINAGX_VEC<T>::iterator end()
        {
            return m_items.end();
        }

        inline U AddItem(T item)
        {
            U id = 0;

            if (!m_availableIDs.empty())
            {
                id = m_availableIDs.front();
                m_availableIDs.pop();
            }
            else
                id = m_nextFreeID++;

            const U currentSize = static_cast<U>(m_items.size());
            if (id >= currentSize)
                m_items.resize(m_defaultStep + currentSize);

            m_items[id] = item;
            return id;
        }

        inline void AddItem(T item, U index)
        {
            m_items[index] = item;
        }

        inline void RemoveItem(U id)
        {
            m_items[id] = T();
            m_availableIDs.push(id);
        }

        inline const LINAGX_VEC<T>& GetItems() const
        {
            return m_items;
        }

        inline LINAGX_VEC<T>& GetItems()
        {
            return m_items;
        }

        inline T GetItem(int index)
        {
            return m_items[index];
        }

        inline T& GetItemR(int index)
        {
            return m_items[index];
        }

        inline U GetNextFreeID()
        {
            return m_nextFreeID;
        }

        inline void Clear()
        {
            m_items.clear();
        }

        inline void Reset()
        {
            Clear();
            m_items.resize(m_defaultStep, T());
        }

        inline T* GetRaw()
        {
            return m_items.data();
        }

    private:
        LINAGX_VEC<T>   m_items;
        LINAGX_QUEUE<U> m_availableIDs;
        U               m_nextFreeID  = 0;
        uint32          m_defaultStep = 50;
    };

    typedef uint32 StringID;
    typedef uint32 TypeID;

    // https://gist.github.com/hwei/1950649d523afd03285c
    class FnvHash
    {
    public:
        static const unsigned int FNV_PRIME    = 16777619u;
        static const unsigned int OFFSET_BASIS = 2166136261u;
        template <unsigned int N>
        static constexpr unsigned int fnvHashConst(const char (&str)[N], unsigned int I = N)
        {
            return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnvHashConst(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
        }
        static uint32 fnvHash(const char* str);
        struct Wrapper
        {
            Wrapper(const char* str)
                : str(str)
            {
            }
            const char* str;
        };
        unsigned int hash_value;

    public:
        // calulate in run-time
        FnvHash(Wrapper wrapper)
            : hash_value(fnvHash(wrapper.str))
        {
        }
        // calulate in compile-time
        template <unsigned int N>
        constexpr FnvHash(const char (&str)[N])
            : hash_value(fnvHashConst(str))
        {
        }
        // output result
        constexpr operator unsigned int() const
        {
            return this->hash_value;
        }
    };

    template <typename T>
    TypeID GetTypeID()
    {
        return FnvHash(typeid(T).name());
    }

    constexpr StringID operator"" _hs(const char* str, std::size_t) noexcept
    {
        return FnvHash(str);
    }

    template <typename T>
    class RingBuffer
    {
    public:
        inline void Initialize(uint32 size)
        {
            m_elements.resize(size, T());
            m_size = size;
        }

        inline uint32 GetSize()
        {
            return m_size;
        }

        inline void Place(uint32 index, T item)
        {
            m_elements[index] = item;
        }

        inline T GetElement(uint32 index)
        {
            return m_elements[index % m_size];
        }

        inline const T& GetElementRef(uint32 index)
        {
            return m_elements[index % m_size];
        }

        inline T GetHead()
        {
            return m_elements[m_head];
        }

        inline void Increment()
        {
            m_head = (m_head + 1) % m_size;
        }

        inline uint32 GetHeadIndex()
        {
            return m_head;
        }

        inline void Clear()
        {
            m_elements.clear();
        }

    private:
        LINAGX_VEC<T> m_elements;
        uint32        m_head = 0;
        uint32        m_size = 0;
    };

} // namespace LinaGX

#endif
