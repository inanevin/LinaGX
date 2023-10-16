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


#include "CommonData.hpp"
#include "Math.hpp"
#include <assert.h>

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
        R11G11B10_FLOAT,
        R10G0B10A2_INT,
        R32G32B32_FLOAT,
        R32G32B32_INT,
        R32G32B32A32_FLOAT,
        R32G32B32A32_INT,
        R16G16B16A16_FLOAT,
        R16G16B16A16_UNORM,
        R32G32_FLOAT,
        R32G32_INT,
        D32_SFLOAT,
        R8G8B8A8_UNORM,
        R8G8B8A8_SRGB,
        R8G8_UNORM,
        R8_UNORM,
        R8_UINT,
        R16_FLOAT,
        R16_INT,
        R32_FLOAT,
        R32_INT,
        R32_UINT,
        BC3_BLOCK_SRGB,
        BC3_BLOCK_UNORM,
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
        A
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

    enum class TextureUsage
    {
        ColorTexture,
        ColorTextureDynamic,
        ColorTextureRenderTarget,
        DepthStencilTexture
    };

    enum class TextureType
    {
        Texture1D,
        Texture2D,
        Texture3D,
    };

    enum class DepthStencilAspect
    {
        DepthOnly,
        StencilOnly,
        DepthStencil
    };

    enum class CommandType
    {
        Graphics = 0,
        Transfer,
        Compute,
        Secondary,
    };

    enum class VsyncMode
    {
        None,
        EveryVBlank,
        EverySecondVBlank,
    };

    enum class StencilOp
    {
        Keep,
        Zero,
        Replace,
        IncrementClamp,
        DecrementClamp,
        Invert,
        IncrementWrap,
        DecrementWrap,
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
        WindowedApplication,
        BorderlessApplication,
        Borderless,
        BorderlessAlpha,
        BorderlessFullscreen,
        Fullscreen,
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

    enum PipelineStageFlags
    {
        PSF_TopOfPipe       = 0x00000001,
        PSF_DrawIndirect    = 0x00000002,
        PSF_VertexInput     = 0x00000004,
        PSF_VertexShader    = 0x00000008,
        PSF_FragmentShader  = 0x00000080,
        PSF_EarlyFragment   = 0x00000100,
        PSF_LateFragment    = 0x00000200,
        PSF_ColorAttachment = 0x00000400,
        PSF_Compute         = 0x00000800,
        PSF_Transfer        = 0x00001000,
        PSF_BottomOfPipe    = 0x00002000,
        PSF_Host            = 0x00004000,
        PSF_AllGraphics     = 0x00008000,
        PSF_AllCommands     = 0x00010000,
    };

    enum AccessFlags
    {
        AF_IndirectCommand             = 0x00000001,
        AF_IndexRead                   = 0x00000002,
        AF_VertexAttributeRead         = 0x00000004,
        AF_UniformRead                 = 0x00000008,
        AF_InputAttachmentRead         = 0x00000010,
        AF_ShaderRead                  = 0x00000020,
        AF_ShaderWrite                 = 0x00000040,
        AF_ColorAttachmentRead         = 0x00000080,
        AF_ColorAttachmentWrite        = 0x00000100,
        AF_DepthStencilAttachmentRead  = 0x00000200,
        AF_DepthStencilAttachmentWrite = 0x00000400,
        AF_TransferRead                = 0x00000800,
        AF_TransferWrite               = 0x00001000,
        AF_HostRead                    = 0x00002000,
        AF_HostWrite                   = 0x00004000,
        AF_MemoryRead                  = 0x00008000,
        AF_MemoryWrite                 = 0x00010000,
    };

    enum class TextureBarrierState
    {
        ColorAttachment,
        DepthStencilAttachment,
        ShaderRead,
        Present,
        TransferSource,
        TransferDestination,
    };

    enum class ResourceBarrierState
    {
        TransferRead,
        TransferWrite,
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

    struct IndirectCommand
    {
        uint32 LGX_DrawID    = 0;
        uint32 vertexCount   = 0;
        uint32 instanceCount = 0;
        uint32 vertexStart   = 0;
        uint32 baseInstance  = 0;
    };

    struct DataBlob
    {
        uint8* ptr  = nullptr;
        size_t size = 0;
    };

    struct SwapchainDesc
    {
        uint32    queue        = 0;
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
        bool                            blendEnabled = false;
        BlendFactor                     srcColorBlendFactor;
        BlendFactor                     dstColorBlendFactor;
        BlendOp                         colorBlendOp;
        BlendFactor                     srcAlphaBlendFactor;
        BlendFactor                     dstAlphaBlendFactor;
        BlendOp                         alphaBlendOp;
        LINAGX_VEC<ColorComponentFlags> componentFlags = {ColorComponentFlags::R, ColorComponentFlags::G, ColorComponentFlags::B, ColorComponentFlags::A};
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

    struct ShaderStructMember
    {
        ShaderMemberType type;
        size_t           size         = 0;
        size_t           offset       = 0;
        size_t           alignment    = 0;
        LINAGX_STRING    name         = "";
        uint32           elementSize  = 0;
        size_t           arrayStride  = 0;
        size_t           matrixStride = 0;
    };

    struct ShaderConstantBlock
    {
        size_t                         size    = 0;
        uint32                         set     = 0;
        uint32                         binding = 0;
        LINAGX_VEC<ShaderStructMember> members;
        LINAGX_VEC<ShaderStage>        stages;
        LINAGX_STRING                  name = "";
        LINAGX_MAP<ShaderStage, bool>  isActive;
    };

    struct ShaderLayoutMSLBinding
    {
        uint32 bufferID          = 0;
        uint32 bufferIDSecondary = 0;
    };

    struct ShaderDescriptorSetBinding
    {
        DescriptorType                  type            = DescriptorType::UBO;
        LINAGX_STRING                   name            = "";
        uint32                          spvID           = 0;
        uint32                          binding         = 0;
        uint32                          descriptorCount = 1;
        size_t                          size            = 0;
        LINAGX_VEC<ShaderStage>         stages;
        bool                            isWritable  = false;
        bool                            isArrayType = false;
        LINAGX_VEC<ShaderStructMember>  structMembers;
        LINAGX_MAP<ShaderStage, bool>   isActive;
        LINAGX_MAP<ShaderStage, uint32> mslBufferID;
    };

    struct ShaderDescriptorSetLayout
    {
        LINAGX_VEC<ShaderDescriptorSetBinding> bindings;
    };

    struct ShaderLayout
    {
        LINAGX_VEC<ShaderStageInput>           vertexInputs;
        LINAGX_VEC<ShaderDescriptorSetLayout>  descriptorSetLayouts;
        LINAGX_VEC<ShaderConstantBlock>        constants;
        uint32                                 constantsSet     = 0;
        uint32                                 constantsBinding = 0;
        LINAGX_MAP<ShaderStage, uint32>        constantsMSLBuffers;
        uint32                                 totalDescriptors = 0;
        bool                                   hasGLDrawID      = false;
        uint32                                 drawIDBinding    = 0;
        LINAGX_MAP<ShaderStage, LINAGX_STRING> entryPoints;
        LINAGX_MAP<ShaderStage, uint32>        mslMaxBufferIDs;
    };

    struct ShaderCompileData
    {
        LINAGX_STRING text        = "";
        LINAGX_STRING includePath = "";
    };

    struct ShaderColorAttachment
    {
        Format               format          = Format::B8G8R8A8_UNORM;
        ColorBlendAttachment blendAttachment = {};
    };

    struct StencilState
    {
        StencilOp failOp      = StencilOp::Keep;
        StencilOp passOp      = StencilOp::Keep;
        StencilOp depthFailOp = StencilOp::Keep;
        CompareOp compareOp   = CompareOp::Always;
    };

    struct ShaderDepthStencilDesc
    {
        Format       depthStencilAttachmentFormat = Format::D32_SFLOAT;
        bool         depthWrite                   = false;
        bool         depthTest                    = false;
        CompareOp    depthCompare                 = CompareOp::LEqual;
        bool         stencilEnabled               = false;
        StencilState backStencilState             = {};
        StencilState frontStencilState            = {};
        uint32       stencilCompareMask           = 0xFF;
        uint32       stencilWriteMask             = 0xFF;
    };

    struct ShaderDesc
    {
        LINAGX_MAP<ShaderStage, DataBlob> stages           = {};
        LINAGX_VEC<ShaderColorAttachment> colorAttachments = {};
        ShaderDepthStencilDesc            depthStencilDesc;
        ShaderLayout                      layout                  = {};
        PolygonMode                       polygonMode             = PolygonMode::Fill;
        CullMode                          cullMode                = CullMode::None;
        FrontFace                         frontFace               = FrontFace::CW;
        Topology                          topology                = Topology::TriangleList;
        bool                              blendLogicOpEnabled     = false;
        LogicOp                           blendLogicOp            = LogicOp::Copy;
        bool                              alphaToCoverage         = false;
        bool                              useCustomPipelineLayout = false;
        uint16                            customPipelineLayout    = 0;
        const char*                       debugName               = "LinaGXShader";
    };

    struct CommandStreamDesc
    {
        uint32      commandCount     = 0;
        CommandType type             = CommandType::Graphics;
        uint32      totalMemoryLimit = 48000;
        uint32      auxMemorySize    = 4096;
        const char* debugName        = "LinaGXCommandStream";
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

    enum TextureFlags
    {
        TF_ColorAttachment       = 1 << 0,
        TF_DepthTexture          = 1 << 1,
        TF_StencilTexture        = 1 << 2,
        TF_LinearTiling          = 1 << 3,
        TF_Sampled               = 1 << 4,
        TF_SampleOutsideFragment = 1 << 5,
        TF_CopySource            = 1 << 6,
        TF_CopyDest              = 1 << 7,
        TF_Cubemap               = 1 << 8,
    };

    /// <summary>
    /// mipCount && levelCount -> leave at 0 for REMAINING_MIPS && REMAINING_LAYERS
    /// </summary>
    struct ViewDesc
    {
        uint32 baseArrayLevel = 0;
        uint32 levelCount     = 0;
        uint32 baseMipLevel   = 0;
        uint32 mipCount       = 0;
        bool   isCubemap      = false;
    };

    struct TextureDesc
    {
        TextureType          type                     = TextureType::Texture2D;
        Format               format                   = Format::R8G8B8A8_SRGB;
        Format               depthStencilSampleFormat = Format::R32_FLOAT;
        LINAGX_VEC<ViewDesc> views                    = {{0, 0, 0, 0, false}};
        uint16               flags                    = 0;
        uint32               width                    = 0;
        uint32               height                   = 0;
        uint32               mipLevels                = 1;
        uint32               arrayLength              = 1;
        const char*          debugName                = "LinaGXTexture";
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
        const char*        debugName = "LinaGXSampler";
    };

    struct DescriptorBinding
    {
        uint32                  descriptorCount  = 1;
        DescriptorType          type             = DescriptorType::UBO;
        bool                    unbounded        = false;
        bool                    useDynamicOffset = false;
        bool                    isWritable       = false;
        LINAGX_VEC<ShaderStage> stages;
    };

    struct DescriptorSetDesc
    {
        LINAGX_VEC<DescriptorBinding> bindings;
    };

    struct DescriptorUpdateImageDesc
    {
        uint16             setHandle          = 0;
        uint32             binding            = 0;
        LINAGX_VEC<uint32> textures           = {};
        LINAGX_VEC<uint32> samplers           = {};
        LINAGX_VEC<uint32> textureViewIndices = {};
    };

    struct DescriptorUpdateBufferDesc
    {
        uint16             setHandle     = 0;
        uint32             binding       = 0;
        LINAGX_VEC<uint32> buffers       = {};
        LINAGX_VEC<uint32> ranges        = {};
        LINAGX_VEC<uint32> offsets       = {};
        bool               isWriteAccess = false;
    };

    struct PipelineLayoutPushConstantRange
    {
        LINAGX_VEC<ShaderStage> stages;
        uint32                  size = 0;
    };

    struct PipelineLayoutDesc
    {
        LINAGX_VEC<DescriptorSetDesc>               descriptorSetDescriptions;
        LINAGX_VEC<PipelineLayoutPushConstantRange> constantRanges;
        bool                                        isCompute           = false;
        bool                                        indirectDrawEnabled = false;
        const char*                                 debugName           = "LinaGX PipelineLayout";
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
        uint8* swapchains     = nullptr;
        uint32 swapchainCount = 0;
    };

    struct QueueDesc
    {
        CommandType type      = CommandType::Graphics;
        const char* debugName = "LinaGXQueue";
    };

    class CommandStream;

    struct SubmitDesc
    {
        uint8           targetQueue      = 0;
        CommandStream** streams          = nullptr;
        uint32          streamCount      = 0;
        bool            useWait          = 0;
        uint32          waitCount        = 0;
        uint16*         waitSemaphores   = 0;
        uint64*         waitValues       = 0;
        bool            useSignal        = 0;
        uint32          signalCount      = 0;
        uint16*         signalSemaphores = 0;
        uint64*         signalValues     = 0;
        bool            isMultithreaded  = false;
    };

    struct TextureBarrier
    {
        uint32              texture;
        bool                isSwapchain;
        TextureBarrierState toState;
        uint32              srcAccessFlags;
        uint32              dstAccessFlags;
    };

    struct ResourceBarrier
    {
        uint32               resource;
        ResourceBarrierState toState;
        uint32               srcAccessFlags;
        uint32               dstAccessFlags;
    };

    struct GPULimits
    {
        uint32 textureLimit       = 512;
        uint32 samplerLimit       = 512;
        uint32 bufferLimit        = 512;
        uint32 maxSubmitsPerFrame = 30;
        uint32 maxDescriptorSets  = 512;
    };

    struct GPUInformation
    {
        LINAGX_STRING      deviceName = "";
        LINAGX_VEC<Format> supportedImageFormats;
        uint64             totalCPUVisibleGPUMemorySize     = 0;
        uint64             dedicatedVideoMemory             = 0;
        uint64             minConstantBufferOffsetAlignment = 0;
        uint64             minStorageBufferOffsetAlignment  = 0;
    };

    struct PerformanceStatistics
    {
        uint64 totalFrames = 0;
    };

    typedef void (*LogCallback)(const char*, ...);

    struct VulkanConfiguration
    {
        bool flipViewport           = true;
        bool enableValidationLayers = true;
    };

    struct DX12Configuration
    {
        bool allowTearing                 = true;
        bool enableDebugLayers            = true;
        bool enableShaderDebugInformation = true;
        bool serializeShaderDebugSymbols  = false;
    };

    struct MetalConfiguration
    {
    };

    struct Configuration
    {
        LogCallback         errorCallback = nullptr;
        LogCallback         infoCallback  = nullptr;
        LogLevel            logLevel      = LogLevel::Normal;
        VulkanConfiguration vulkanConfig  = {};
        DX12Configuration   dx12Config    = {};
        MetalConfiguration  mtlConfig     = {};
    };

    struct GPUFeatures
    {
        bool   enableBindless          = false;
        uint32 extraGraphicsQueueCount = 0;
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

    struct MonitorInfo
    {
        void*        monitorHandle = nullptr;
        bool         isPrimary     = false;
        float        dpiScale      = 0.0f;
        uint32       dpi           = 0;
        LGXVector2ui size          = {};
        LGXVector2ui workArea      = {};
        LGXVector2i  workTopLeft   = {};
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
    assert(condition);

    class Window;

    typedef std::function<void()>                                    CallbackNoArg;
    typedef std::function<void(const LGXVector2i&)>                  CallbackPosChanged;
    typedef std::function<void(const LGXVector2ui&)>                 CallbackMouseMove;
    typedef std::function<void(const LGXVector2ui&)>                 CallbackSizeChanged;
    typedef std::function<void(uint32, int32, InputAction, Window*)> CallbackKey;
    typedef std::function<void(uint32, InputAction)>                 CallbackMouse;
    typedef std::function<void(int32)>                               CallbackMouseWheel;
    typedef std::function<void(bool)>                                CallbackFocus;
    typedef std::function<void()>                                    CallbackHoverBegin;
    typedef std::function<void()>                                    CallbackHoverEnd;

} // namespace LinaGX

