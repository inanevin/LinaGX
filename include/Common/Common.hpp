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

#ifndef Common_HPP
#define Common_HPP

#include "LinaGXExports.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <deque>

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

#define LINAGX_VEC         std::vector
#define LINAGX_QUEUE       std::queue
#define LINAGX_STRING      std::string
#define LINAGX_MAP         std::unordered_map
#define LINAGX_DEQUE       std::deque
#define LINAGX_MALLOC(...) malloc(__VA_ARGS__)
#define LINAGX_MEMCPY(...) memcpy(__VA_ARGS__)
#define LINAGX_FREE(...)   free(__VA_ARGS__)

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

    enum class BackendAPI
    {
        Vulkan,
        DX12,
        Metal
    };

    enum class ShaderStage
    {
        Vertex,
        Pixel,
        Compute,
        Geometry,
        Tesellation
    };

    enum class PreferredGPUType
    {
        CPU,
        Integrated,
        Discrete,
    };

    enum class LogLevel
    {
        None,
        OnlyErrors,
        Normal,
        Verbose,
    };

    enum class Format
    {
        UNDEFINED = 0,
        B8G8R8A8_SRGB,
        B8G8R8A8_UNORM,
        R32G32B32_SFLOAT,
        R32G32B32_SINT,
        R32G32B32A32_SFLOAT,
        R32G32B32A32_SINT,
        R32G32_SFLOAT,
        R32G32_SINT,
        D32_SFLOAT,
        R8G8B8A8_UNORM,
        R8G8B8A8_SRGB,
        R8G8_UNORM,
        R8_UNORM,
        R8_UINT,
        R16_SFLOAT,
        R16_SINT,
        R32_SFLOAT,
        R32_SINT,
        FORMAT_MAX,
    };

    enum class PolygonMode
    {
        Fill,
        Line,
        Point
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
        FrontAndBack,
    };

    enum class FrontFace
    {
        CCW,
        CW,
    };

    enum class CompareOp
    {
        Never,
        Less,
        Equal,
        LEqual,
        Greater,
        NotEqual,
        GEqual,
        Always
    };

    enum class Topology
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        TriangleListAdjacency,
        TriangleStripAdjacency,
    };

    enum class LogicOp
    {
        Clear,
        And,
        AndReverse,
        Copy,
        AndInverted,
        NoOp,
        XOR,
        OR,
        NOR,
        Equivalent
    };

    typedef void (*LogCallback)(const char*, ...);

    struct DataBlob
    {
        uint8* ptr  = nullptr;
        size_t size = 0;
    };

    struct SwapchainDesc
    {
        uint32 x           = 0;
        uint32 y           = 0;
        uint32 width       = 0;
        uint32 height      = 0;
        void*  window      = nullptr;
        void*  osHandle    = nullptr;
        Format format      = Format::R8G8B8A8_UNORM;
        Format depthFormat = Format::D32_SFLOAT;
    };

    enum class BlendOp
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
    };

    enum class ColorComponentFlags
    {
        R,
        G,
        B,
        A,
        RG,
        RGB,
        RGBA
    };

    enum class ShaderMemberType
    {
        Float,
        Int,
        Float4,
        Int4,
        Float2,
        Int2,
        Float3,
        Int3,
        MatUnknown,
        Mat2x2,
        Mat4x4,
        Mat3x3,
    };

    enum class Texture2DUsage
    {
        ColorTexture,
        ColorTextureDynamic,
        ColorTextureRenderTarget,
        DepthStencilTexture
    };

    struct ColorBlendAttachment
    {
        bool                blendEnabled;
        BlendFactor         srcColorBlendFactor;
        BlendFactor         dstColorBlendFactor;
        BlendOp             colorBlendOp;
        BlendFactor         srcAlphaBlendFactor;
        BlendFactor         dstAlphaBlendFactor;
        BlendOp             alphaBlendOp;
        ColorComponentFlags componentFlags;
    };

    struct StageInput
    {
        LINAGX_STRING name;
        uint32        location;
        uint32        elements;
        size_t        size;
        Format        format;
        size_t        offset;
    };

    struct UBOMember
    {
        ;
        ShaderMemberType type;
        size_t           size;
        size_t           offset;
        LINAGX_STRING    name;
        bool             isArray;
        uint32           arraySize;
        size_t           arrayStride;
        size_t           matrixStride;
    };

    struct ConstantBlock
    {
        size_t                  size;
        LINAGX_VEC<UBOMember>   members;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name;
    };

    struct UBO
    {
        uint32                  set;
        uint32                  binding;
        size_t                  size;
        LINAGX_VEC<UBOMember>   members;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name = "";
    };

    struct SSBO
    {
        uint32                  set;
        uint32                  binding;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name = "";
    };

    struct SRVTexture2D
    {
        uint32                  set;
        uint32                  binding;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name = "";
    };

    struct Sampler
    {
        uint32                  set;
        uint32                  binding;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name = "";
    };

    struct ShaderLayout
    {
        LINAGX_VEC<StageInput>   vertexInputs;
        LINAGX_VEC<UBO>          ubos;
        LINAGX_VEC<SSBO>         ssbos;
        LINAGX_VEC<SRVTexture2D> combinedImageSamplers;
        LINAGX_VEC<Sampler>      samplers;
        ConstantBlock            constantBlock;
    };

    struct ShaderDesc
    {
        ShaderLayout         layout;
        PolygonMode          polgyonMode;
        CullMode             cullMode;
        FrontFace            frontFace;
        bool                 depthTest;
        bool                 depthWrite;
        CompareOp            depthCompare;
        Topology             topology;
        ColorBlendAttachment blendAttachment;
        bool                 blendLogicOpEnabled;
        LogicOp              blendLogicOp;
    };

    struct Texture2DDesc
    {
        /// <summary>
        ///
        /// </summary>
        Texture2DUsage usage;

        /// <summary>
        ///
        /// </summary>
        uint32 width = 0;

        /// <summary>
        ///
        /// </summary>
        uint32 height = 0;

        /// <summary>
        ///
        /// </summary>
        uint32 mipLevels = 0;

        /// <summary>
        ///
        /// </summary>
        uint32 channels = 4;

        /// <summary>
        ///
        /// </summary>
        Format format = Format::R8G8B8A8_SRGB;

        /// <summary>
        ///
        /// </summary>
        const wchar_t* debugName = L"Texture";
    };

    struct GPULimits
    {
        /// <summary>
        ///
        /// </summary>
        uint32 textureLimit = 1024;

        /// <summary>
        ///
        /// </summary>
        uint32 samplerLimit = 1024;

        /// <summary>
        ///
        /// </summary>
        uint32 bufferLimit = 1024;
    };

    struct Configuration
    {
        /// <summary>
        ///
        /// </summary>
        LogCallback errorCallback = nullptr;

        /// <summary>
        ///
        /// </summary>
        LogCallback infoCallback = nullptr;

        /// <summary>
        ///
        /// </summary>
        LogLevel logLevel = LogLevel::Normal;

        /// <summary>
        ///
        /// </summary>
        Format defaultRTFormat = Format::R8G8B8A8_SRGB;

        /// <summary>
        ///
        /// </summary>
        uint32 backBufferCount = 2;

        /// <summary>
        ///
        /// </summary>
        uint32 framesInFlight = 2;
    };

    struct InitInfo
    {
        /// <summary>
        ///
        /// </summary>
        BackendAPI api = BackendAPI::Vulkan;

        /// <summary>
        ///
        /// </summary>
        PreferredGPUType gpu = PreferredGPUType::Discrete;

        /// <summary>
        ///
        /// </summary>
        const char* appName = "LinaGX App";

        /// <summary>
        ///
        /// </summary>
        GPULimits gpuLimits = {};
    };

    extern LINAGX_API Configuration Config;

#define LOGT(...)                                                                                            \
    if (Config.infoCallback && Config.logLevel != LogLevel::None && Config.logLevel != LogLevel::OnlyErrors) \
        Config.infoCallback(__VA_ARGS__);
#define LOGE(...)                                                  \
    if (Config.errorCallback && Config.logLevel != LogLevel::None) \
        Config.errorCallback(__VA_ARGS__);

#define LOGV(...)                                                    \
    if (Config.infoCallback && Config.logLevel == LogLevel::Verbose) \
        Config.infoCallback(__VA_ARGS__);

#define LOGA(condition, ...)                \
    if (!condition && Config.errorCallback) \
        Config.errorCallback(__VA_ARGS__);  \
    _ASSERT(condition);

    namespace Internal
    {
        extern LINAGX_API char*         WCharToChar(const wchar_t* wch);
        extern LINAGX_API LINAGX_STRING ReadFileContentsAsString(const char* filePath);

    } // namespace Internal

} // namespace LinaGX

#endif
