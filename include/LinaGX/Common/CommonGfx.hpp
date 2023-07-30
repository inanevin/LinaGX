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

#ifndef LINAGX_COMMON_HPP
#define LINAGX_COMMON_HPP

#include "CommonData.hpp"

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
        Fragment,
        Compute,
        Geometry,
        Tesellation,
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
        R16G16B16A16_SFLOAT,
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
        R32_UINT,
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

    enum class DepthStencilAspect
    {
        DepthOnly,
        StencilOnly,
        DepthStencil
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

    enum class MipmapMode
    {
        Nearest,
        Linear
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
    };

    enum ResourceTypeHint
    {
        TH_None           = 1 << 0,
        TH_VertexBuffer   = 1 << 1,
        TH_ConstantBuffer = 1 << 2,
        TH_StorageBuffer  = 1 << 3,
        TH_IndexBuffer    = 1 << 4,
        TH_IndirectBuffer = 1 << 5,
    };

    enum class ResourceHeap
    {
        StagingHeap,
        GPUOnly,
        CPUVisibleGPUMemory,
    };

    enum class IndexType
    {
        Uint16,
        Uint32,
    };

    enum class DescriptorType
    {
        CombinedImageSampler,
        SeparateSampler,
        SeparateImage,
        SSBO,
        UBO,
        ConstantBlock,
    };

    enum class BorderColor
    {
        BlackTransparent,
        BlackOpaque,
        WhiteOpaque
    };

    enum class MipmapFilter
    {
        Default = 0,
        Box,
        Triangle,
        CubicSpline,
        CatmullRom,
        Mitchell
    };

    struct IndexedIndirectCommand
    {
        uint32 LGX_DrawID            = 0;
        uint32 indexCountPerInstance = 0;
        uint32 instanceCount         = 0;
        uint32 startIndexLocation    = 0;
        uint32 baseVertexLocation    = 0;
        uint32 startInstanceLocation = 0;
    };

    struct DataBlob
    {
        uint8* ptr  = nullptr;
        size_t size = 0;
    };

    struct SwapchainDesc
    {
        Format    format       = Format::B8G8R8A8_UNORM;
        Format    depthFormat  = Format::D32_SFLOAT;
        uint32    x            = 0;
        uint32    y            = 0;
        uint32    width        = 0;
        uint32    height       = 0;
        void*     window       = nullptr;
        void*     osHandle     = nullptr;
        bool      isFullscreen = false;
        VsyncMode vsyncMode    = VsyncMode::None;
    };

    struct SwapchainRecreateDesc
    {
        uint8     swapchain    = 0;
        uint32    width        = 0;
        uint32    height       = 0;
        bool      isFullscreen = false;
        VsyncMode vsyncMode    = VsyncMode::None;
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

    struct ShaderStageInput
    {
        LINAGX_STRING name     = "";
        uint32        location = 0;
        uint32        elements = 0;
        size_t        size     = 0;
        Format        format   = Format::UNDEFINED;
        size_t        offset   = 0;
    };

    struct ShaderUBOMember
    {
        ShaderMemberType type;
        size_t           size         = 0;
        size_t           offset       = 0;
        LINAGX_STRING    name         = "";
        uint32           elementSize  = 0;
        size_t           arrayStride  = 0;
        size_t           matrixStride = 0;
    };

    struct ShaderConstantBlock
    {
        size_t                      size    = 0;
        uint32                      set     = 0;
        uint32                      binding = 0;
        LINAGX_VEC<ShaderUBOMember> members;
        LINAGX_VEC<ShaderStage>     stages;
        LINAGX_STRING               name = "";
    };

    struct ShaderUBO
    {
        uint32                      set;
        uint32                      binding;
        size_t                      size;
        LINAGX_VEC<ShaderUBOMember> members;
        LINAGX_VEC<ShaderStage>     stages;
        LINAGX_STRING               name        = "";
        uint32                      elementSize = 1;
    };

    struct ShaderSSBO
    {
        uint32                  set;
        uint32                  binding;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name       = "";
        bool                    isReadOnly = true;
    };

    struct ShaderSRVTexture2D
    {
        uint32                  set;
        uint32                  binding;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name        = "";
        uint32                  elementSize = 1;
    };

    struct ShaderSampler
    {
        uint32                  set;
        uint32                  binding;
        LINAGX_VEC<ShaderStage> stages;
        LINAGX_STRING           name        = "";
        uint32                  elementSize = 1;
    };

    struct ShaderLayout
    {
        LINAGX_VEC<ShaderStageInput>           vertexInputs;
        LINAGX_VEC<ShaderUBO>                  ubos;
        LINAGX_VEC<ShaderSSBO>                 ssbos;
        LINAGX_VEC<ShaderSRVTexture2D>         combinedImageSamplers;
        LINAGX_VEC<ShaderSRVTexture2D>         separateImages;
        LINAGX_VEC<ShaderSampler>              samplers;
        LINAGX_MAP<uint32, LINAGX_VEC<uint32>> setsAndBindings;
        ShaderConstantBlock                    constantBlock;
        uint32                                 totalDescriptors = 0;
        bool                                   hasGLDrawID      = false;
        uint32                                 drawIDBinding    = 0;
    };

    struct ShaderCompileData
    {
        const char* text        = "";
        const char* includePath = "";
    };

    struct ShaderDesc
    {
        LINAGX_MAP<ShaderStage, DataBlob> stages;
        Format                            colorAttachmentFormat = Format::B8G8R8A8_UNORM;
        Format                            depthAttachmentFormat = Format::D32_SFLOAT;
        ShaderLayout                      layout                = {};
        PolygonMode                       polygonMode           = PolygonMode::Fill;
        CullMode                          cullMode              = CullMode::None;
        FrontFace                         frontFace             = FrontFace::CW;
        bool                              depthTest             = false;
        bool                              depthWrite            = false;
        CompareOp                         depthCompare          = CompareOp::LEqual;
        Topology                          topology              = Topology::TriangleList;
        ColorBlendAttachment              blendAttachment       = {};
        bool                              blendLogicOpEnabled   = false;
        LogicOp                           blendLogicOp          = LogicOp::Copy;
        const char*                       debugName             = "LinaGXShader";
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

    struct TextureBuffer
    {
        uint8* pixels;
        uint32 width;
        uint32 height;
        uint32 bytesPerPixel;
    };

    struct Texture2DDesc
    {
        Texture2DUsage     usage              = Texture2DUsage::ColorTexture;
        DepthStencilAspect depthStencilAspect = DepthStencilAspect::DepthStencil;
        uint32             width              = 0;
        uint32             height             = 0;
        uint32             mipLevels          = 0;
        Format             format             = Format::R8G8B8A8_SRGB;
        const char*        debugName          = "LinaGXTexture";
    };

    struct SamplerDesc
    {
        Filter             minFilter  = Filter::Linear;
        Filter             magFilter  = Filter::Linear;
        SamplerAddressMode mode       = SamplerAddressMode::ClampToEdge;
        MipmapMode         mipmapMode = MipmapMode::Linear;
        uint32             anisotropy = 0;
        float              minLod     = 0.0f;
        float              maxLod     = 1.0f;
        float              mipLodBias = 0.0f;
        BorderColor        borderColor;
    };

    struct DescriptorBinding
    {
        uint32                  binding         = 0;
        uint32                  descriptorCount = 1;
        DescriptorType          type            = DescriptorType::UBO;
        LINAGX_VEC<ShaderStage> stages;
        bool                    bindless = false;
    };

    struct DescriptorSetDesc
    {
        DescriptorBinding* bindings      = nullptr;
        uint32             bindingsCount = 0;
    };

    struct DescriptorUpdateImageDesc
    {
        uint16         setHandle;
        uint32         binding;
        uint32         descriptorCount;
        uint32*        textures;
        uint32*        samplers;
        DescriptorType descriptorType;
    };

    struct DescriptorUpdateBufferDesc
    {
        uint16         setHandle       = 0;
        uint32         binding         = 0;
        uint32         descriptorCount = 0;
        uint32*        resources       = nullptr;
        DescriptorType descriptorType  = DescriptorType::UBO;
        bool           isWriteAccess   = false;
    };

    struct ResourceDesc
    {
        uint64       size          = 0;
        uint32       typeHintFlags = 0;
        ResourceHeap heapType      = ResourceHeap::StagingHeap;
        bool         isGPUWritable = false;
        const char*  debugName     = "LinaGXResource";
    };

    struct PresentDesc
    {
        uint8 swapchain = 0;
    };

    class CommandStream;

    struct SubmitDesc
    {
        QueueType       queue           = QueueType::Graphics;
        CommandStream** streams         = nullptr;
        uint32          streamCount     = 0;
        bool            useWait         = 0;
        uint16          waitSemaphore   = 0;
        uint64          waitValue       = 0;
        bool            useSignal       = 0;
        uint16          signalSemaphore = 0;
        uint64          signalValue     = 0;
    };

    struct GPULimits
    {
        uint32 textureLimit       = 512;
        uint32 samplerLimit       = 512;
        uint32 bufferLimit        = 512;
        uint32 maxSubmitsPerFrame = 30;
    };

    struct GPUInformation
    {
        LINAGX_STRING      deviceName = "";
        LINAGX_VEC<Format> supportedImageFormats;
        uint64             totalCPUVisibleGPUMemorySize = 0;
        uint64             dedicatedVideoMemory         = 0;
    };

    struct PerformanceStatistics
    {
        uint64 totalFrames = 0;
    };

    typedef void (*LogCallback)(const char*, ...);

    struct VulkanConfiguration
    {
        bool flipViewport = true;
    };

    struct Configuration
    {
        LogCallback         errorCallback = nullptr;
        LogCallback         infoCallback  = nullptr;
        LogLevel            logLevel      = LogLevel::Normal;
        VulkanConfiguration vulkanConfig  = {};
    };

    struct GPUFeatures
    {
        bool enableBindless = false;
    };

    struct InitInfo
    {
        BackendAPI         api             = BackendAPI::Vulkan;
        PreferredGPUType   gpu             = PreferredGPUType::Discrete;
        const char*        appName         = "LinaGX App";
        uint32             framesInFlight  = 2;
        uint32             backbufferCount = 2;
        GPULimits          gpuLimits       = {};
        GPUFeatures        gpuFeatures     = {};
        LINAGX_VEC<Format> checkForFormatSupport;
    };

    extern LINAGX_API Configuration         Config;
    extern LINAGX_API GPUInformation        GPUInfo;
    extern LINAGX_API PerformanceStatistics PerformanceStats;

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

#define LOGA(condition, ...)                  \
    if (!(condition) && Config.errorCallback) \
        Config.errorCallback(__VA_ARGS__);    \
    _ASSERT(condition);

    typedef std::function<void()>                            CallbackNoArg;
    typedef std::function<void(uint32, uint32)>              CallbackUint2;
    typedef std::function<void(int32, int32)>                CallbackInt2;
    typedef std::function<void(uint32, uint32, InputAction)> CallbackKey;
    typedef std::function<void(uint32, InputAction)>         CallbackMouse;
    typedef std::function<void(uint32)>                      CallbackMouseWheel;

} // namespace LinaGX

#endif
