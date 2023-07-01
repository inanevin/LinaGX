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
#include "Core/DataStructures.hpp"

namespace LinaGX
{
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

    enum class QueueType
    {
        Graphics,
        Transfer,
        Compute
    };

    enum class VsyncMode
    {
        None,
        EveryVBlank,
        EverySecondVBlank,
    };

    enum class LoadOp
    {
        Load,
        Clear,
        DontCare,
        None,
    };

    enum class StoreOp
    {
        Store,
        DontCare,
        None,
    };

    enum class Filter
    {
        Anisotropic,
        Nearest,
        Linear,
    };

    enum class SamplerAddressMode
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge
    };

    enum class ImageTransitionType
    {
        Present2RT,
        RT2Present,
    };

    enum class WindowStyle
    {
        Windowed,
        Borderless,
        Fullscreen,
        WorkArea
    };

    enum ResourceTypeHint
    {
        LGX_VertexBuffer = 0,
        LGX_ConstantBuffer,
        LGX_StorageBuffer,
        LGX_IndexBuffer,
        LGX_IndirectBuffer,
    };

    enum class ResourceHeap
    {
        StagingHeap,
        GPUOnly,
        CPUVisibleGPUMemory,
    };

    struct DataBlob
    {
        uint8* ptr  = nullptr;
        size_t size = 0;
    };

    struct SwapchainDesc
    {
        uint32 x        = 0;
        uint32 y        = 0;
        uint32 width    = 0;
        uint32 height   = 0;
        void*  window   = nullptr;
        void*  osHandle = nullptr;
    };

    struct ColorBlendAttachment
    {
        bool                blendEnabled = false;
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
        LINAGX_MAP<ShaderStage, DataBlob> stages;
        ShaderLayout                      layout              = {};
        PolygonMode                       polygonMode         = PolygonMode::Fill;
        CullMode                          cullMode            = CullMode::None;
        FrontFace                         frontFace           = FrontFace::CW;
        bool                              depthTest           = false;
        bool                              depthWrite          = false;
        CompareOp                         depthCompare        = CompareOp::LEqual;
        Topology                          topology            = Topology::TriangleList;
        ColorBlendAttachment              blendAttachment     = {};
        bool                              blendLogicOpEnabled = false;
        LogicOp                           blendLogicOp        = LogicOp::Copy;
    };

    struct Viewport
    {
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
        float  minDepth;
        float  maxDepth;
    };

    struct ScissorsRect
    {
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
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

    struct ResourceDesc
    {
        /// <summary>
        ///
        /// </summary>
        uint64 size = 0;

        /// <summary>
        ///
        /// </summary>
        uint32 typeHintFlags = 0;

        /// <summary>
        ///
        /// </summary>
        ResourceHeap heapType = ResourceHeap::StagingHeap;

        /// <summary>
        ///
        /// </summary>
        const wchar_t* debugName = L"Resource";
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

        /// <summary>
        ///
        /// </summary>
        uint32 maxSubmitsPerFrame = 30;
    };

    struct GPUInformation
    {
        /// <summary>
        ///
        /// </summary>
        LINAGX_STRING deviceName = "";

        /// <summary>
        ///
        /// </summary>
        uint64 totalCPUVisibleGPUMemorySize = 0;

        /// <summary>
        ///
        /// </summary>
        uint64 dedicatedVideoMemory = 0;
    };

    typedef void (*LogCallback)(const char*, ...);

    struct VulkanConfiguration
    {
        /// <summary>
        ///
        /// </summary>
        bool flipViewport = true;
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
        VulkanConfiguration vulkanConfig = {};
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
        uint32 framesInFlight = 2;

        /// <summary>
        ///
        /// </summary>
        uint32 backbufferCount = 2;

        /// <summary>
        ///
        /// </summary>
        GPULimits gpuLimits = {};

        /// <summary>
        ///
        /// </summary>
        Format rtSwapchainFormat = Format::B8G8R8A8_UNORM;

        /// <summary>
        ///
        /// </summary>
        Format rtColorFormat = Format::R8G8B8A8_SRGB;

        /// <summary>
        ///
        /// </summary>
        Format rtDepthFormat = Format::D32_SFLOAT;
    };

    struct PresentDesc
    {
        /// <summary>
        ///
        /// </summary>
        uint8 swapchain = 0;

        /// <summary>
        ///
        /// </summary>
        VsyncMode vsync = VsyncMode::None;
    };

    class CommandStream;

    struct ExecuteDesc
    {
        /// <summary>
        ///
        /// </summary>
        QueueType queue = QueueType::Graphics;

        /// <summary>
        ///
        /// </summary>
        CommandStream** streams = nullptr;

        /// <summary>
        ///
        /// </summary>
        uint32 streamCount = 0;

        /// <summary>
        ///
        /// </summary>
        bool useWait = 0;

        /// <summary>
        ///
        /// </summary>
        uint16 waitSemaphore = 0;

        /// <summary>
        ///
        /// </summary>
        uint64 waitValue = 0;

        /// <summary>
        ///
        /// </summary>
        bool useSignal = 0;

        /// <summary>
        ///
        /// </summary>
        uint16 signalSemaphore = 0;

        /// <summary>
        ///
        /// </summary>
        uint64 signalValue = 0;
    };

    extern LINAGX_API Configuration  Config;
    extern LINAGX_API GPUInformation GPUInfo;

#define LOGT(...)                                                                                            \
    if (Config.infoCallback && Config.logLevel != LogLevel::None && Config.logLevel != LogLevel::OnlyErrors) \
        Config.infoCallback(__VA_ARGS__);
#define LOGE(...)                                                       \
    if (Config.errorCallback && Config.logLevel != LogLevel::None)      \
    {                                                                   \
        Config.errorCallback(__VA_ARGS__);                              \
        Config.errorCallback("File: %s, Line: %d", __FILE__, __LINE__); \
    }

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

    typedef std::function<void()> CallbackNoArg;

} // namespace LinaGX

#endif
