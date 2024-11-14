/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2023-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#pragma once

#include "CommonData.hpp"
#include "LinaGX/Common/Defines/Format.hpp"
#include "LinaGX/Common/Defines/DataBuffers.hpp"
#include "LinaGX/Common/Defines/SamplerDesc.hpp"
#include "Math.hpp"
#include <assert.h>

namespace LinaGX
{

    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Compute,
        Geometry,
        TesellationControl,
        TesellationEval,
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

    enum class VKVsync
    {
        None,
        FIFO,
        FIFO_RELAXED,
        MAILBOX,
    };

    enum class DXVsync
    {
        None,
        EveryVBlank,
        EverySecondVBlank,
    };

    struct VSyncStyle
    {
        VKVsync vulkanVsync = VKVsync::None;
        DXVsync dx12Vsync   = DXVsync::None;
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

    enum ResourceTypeHint
    {
        TH_None           = 1 << 0,
        TH_VertexBuffer   = 1 << 1,
        TH_ConstantBuffer = 1 << 2,
        TH_StorageBuffer  = 1 << 3,
        TH_IndexBuffer    = 1 << 4,
        TH_IndirectBuffer = 1 << 5,
        TH_ReadbackDest   = 1 << 6,
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

    enum class MipmapFilter
    {
        Default = 0,
        Box,
        Triangle,
        CubicSpline,
        CatmullRom,
        Mitchell
    };

    /// <summary>
    /// Equivalent to Vulkan pipeline stage flag bits.
    /// </summary>
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

    /// <summary>
    /// Equivalent to Vulkan access flag bits.
    /// </summary>
    enum AccessFlags
    {
        AF_IndirectCommandRead         = 0x00000001,
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
        DepthRead,
        StencilRead,
        DepthStencilRead,
        Present,
        TransferSource,
        TransferDestination,
    };

    enum class ResourceBarrierState
    {
        TransferRead,
        TransferWrite,
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
        TF_Readback              = 1 << 9,
    };

    struct DX12IndexedIndirectCommand
    {
        uint32 LGX_DrawID            = 0;
        uint32 indexCountPerInstance = 0;
        uint32 instanceCount         = 0;
        uint32 startIndexLocation    = 0;
        int32  baseVertexLocation    = 0;
        uint32 startInstanceLocation = 0;
    };

    struct DX12IndirectCommand
    {
        uint32 LGX_DrawID    = 0;
        uint32 vertexCount   = 0;
        uint32 instanceCount = 0;
        uint32 vertexStart   = 0;
        uint32 baseInstance  = 0;
    };

    struct IndexedIndirectCommand
    {
        uint32 indexCountPerInstance = 0;
        uint32 instanceCount         = 0;
        uint32 startIndexLocation    = 0;
        int32  baseVertexLocation    = 0;
        uint32 startInstanceLocation = 0;
    };

    struct IndirectCommand
    {
        uint32 vertexCount   = 0;
        uint32 instanceCount = 0;
        uint32 vertexStart   = 0;
        uint32 baseInstance  = 0;
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
        size_t                                     size    = 0;
        uint32                                     set     = 0;
        uint32                                     binding = 0;
        LINAGX_VEC<ShaderStructMember>             members;
        LINAGX_VEC<ShaderStage>                    stages;
        LINAGX_STRING                              name = "";
        LINAGX_VEC<LINAGX_PAIR<ShaderStage, bool>> isActive;
    };

    struct ShaderLayoutMSLBinding
    {
        uint32 bufferID          = 0;
        uint32 bufferIDSecondary = 0;
    };

    struct ShaderDescriptorSetBinding
    {
        DescriptorType                               type = DescriptorType::UBO;
        LINAGX_STRING                                name = "";
        LINAGX_VEC<ShaderStage>                      stages;
        LINAGX_VEC<ShaderStructMember>               structMembers;
        LINAGX_VEC<LINAGX_PAIR<ShaderStage, bool>>   isActive;
        LINAGX_VEC<LINAGX_PAIR<ShaderStage, uint32>> mslBufferID;
        uint32                                       spvID           = 0;
        uint32                                       binding         = 0;
        uint32                                       descriptorCount = 1;
        size_t                                       size            = 0;
        bool                                         isWritable      = false;
        bool                                         isArrayType     = false;
    };

    struct ShaderDescriptorSetLayout
    {
        LINAGX_VEC<ShaderDescriptorSetBinding> bindings;
    };

    struct ShaderLayout
    {
        LINAGX_VEC<ShaderStageInput>                        vertexInputs;
        LINAGX_VEC<ShaderDescriptorSetLayout>               descriptorSetLayouts;
        LINAGX_VEC<ShaderConstantBlock>                     constants;
        LINAGX_VEC<LINAGX_PAIR<ShaderStage, uint32>>        constantsMSLBuffers;
        LINAGX_VEC<LINAGX_PAIR<ShaderStage, LINAGX_STRING>> entryPoints;
        LINAGX_VEC<LINAGX_PAIR<ShaderStage, uint32>>        mslMaxBufferIDs;
        uint32                                              constantsSet     = 0;
        uint32                                              constantsBinding = 0;
        uint32                                              totalDescriptors = 0;
        bool                                                hasGLDrawID      = false;
        uint32                                              drawIDBinding    = 0;
    };

    struct Viewport
    {
        float  x;
        float  y;
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

    class CommandStream;

    /// <summary>
    /// If targeting Vulkan, need to use srcAccessFlags and dstAccessFlags. Use AccessFlags enumeration.
    /// In DX12 access flags are not required.
    /// In Metal no transition/barrier is required at all.
    /// </summary>
    struct TextureBarrier
    {
        uint32              texture;
        bool                isSwapchain;
        TextureBarrierState toState;
        uint32              srcAccessFlags;
        uint32              dstAccessFlags;
    };

    /// <summary>
    /// If targeting Vulkan, need to use srcAccessFlags and dstAccessFlags. Use AccessFlags enumeration.
    /// In DX12 access flags are not required.
    /// In Metal no transition/barrier is required at all.
    /// </summary>
    struct ResourceBarrier
    {
        uint32               resource;
        ResourceBarrierState toState;
        uint32               srcAccessFlags;
        uint32               dstAccessFlags;
    };

    /// <summary>
    /// If targeting Vulkan, need to use srcAccessFlags and dstAccessFlags. Use AccessFlags enumeration.
    /// In DX12 access flags are not required.
    /// In Metal no transition/barrier is required at all.
    /// </summary>
    struct MemBarrier
    {
        uint32 srcAccessFlags;
        uint32 dstAccessFlags;
    };

    enum VulkanFeatureFlags
    {
        VKF_Bindless          = 1 << 0,
        VKF_UpdateAfterBind   = 1 << 1,
        VKF_MultiDrawIndirect = 1 << 2,
        VKF_SamplerAnisotropy = 1 << 3,
        VKF_DepthClamp        = 1 << 4,
        VKF_DepthBiasClamp    = 1 << 5,
    };

    struct SubmitDesc
    {

        uint8           targetQueue          = 0; // Target queue either created with LinaGX::Instance::CreateQueue() or obtained with ::GetPrimaryQueue()
        CommandStream** streams              = nullptr;
        uint32          streamCount          = 0;
        bool            useWait              = 0; // Set to true if you want to wait for a user semaphore to be signalled.
        uint32          waitCount            = 0;
        uint16*         waitSemaphores       = 0;
        uint64*         waitValues           = 0;
        bool            useSignal            = 0; // Set to true if you want to signal a user semaphore when this submission is completed on gpu.
        uint32          signalCount          = 0;
        uint16*         signalSemaphores     = 0;
        uint64*         signalValues         = 0;
        bool            standaloneSubmission = false; // If you are rendering outside of main render loop (so outside of StartFrame() and EndFrame()), for example in a background thread, set to true.
    };

    struct PresentDesc
    {
        uint8* swapchains     = nullptr;
        uint32 swapchainCount = 0;
    };

    struct SwapchainDesc
    {
        Format     format       = Format::B8G8R8A8_UNORM;
        uint32     x            = 0;
        uint32     y            = 0;
        uint32     width        = 0;
        uint32     height       = 0;
        void*      window       = nullptr; // HWND handle on Windows, NSWindow handle on MacOS.
        void*      osHandle     = nullptr; // HINSTANCE handle on Windows, NSVIEW handle on MacOS.
        bool       isFullscreen = false;
        VSyncStyle vsyncStyle   = {};
    };

    struct SwapchainRecreateDesc
    {
        uint8      swapchain    = 0;
        uint32     width        = 0;
        uint32     height       = 0;
        bool       isFullscreen = false;
        VSyncStyle vsyncStyle   = {};
    };

    struct CommandStreamDesc
    {
        CommandType type              = CommandType::Graphics;
        uint32      commandCount      = 200;   // Maximum number of commands that can be recorded in this stream.
        size_t      totalMemoryLimit  = 24000; // Maximum allowed memory pool, this amount will be reserved for the lifetime of stream, so experiment and keep it sensible.
        size_t      auxMemorySize     = 4096;  // Maximum allowed auxilary memory poolthis amount will be reserved for the lifetime of stream, so experiment and keep it sensible.
        size_t      constantBlockSize = 64;    // Not used in Vulkan, but in DX12 and Metal used to store constant bindings data in the shaders. If constants to be bound is bigger than available space, they are MALLOC'ed directly.
        const char* debugName         = "LinaGXCommandStream";
    };

    struct ShaderCompileData
    {
        ShaderStage   stage       = {};
        LINAGX_STRING text        = "";
        LINAGX_STRING includePath = "";
        DataBlob      outBlob     = {};
    };

    struct ColorBlendAttachment
    {
        bool                            blendEnabled        = false;
        BlendFactor                     srcColorBlendFactor = BlendFactor::SrcAlpha;
        BlendFactor                     dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
        BlendOp                         colorBlendOp        = BlendOp::Add;
        BlendFactor                     srcAlphaBlendFactor = BlendFactor::One;
        BlendFactor                     dstAlphaBlendFactor = BlendFactor::Zero;
        BlendOp                         alphaBlendOp        = BlendOp::Add;
        LINAGX_VEC<ColorComponentFlags> componentFlags      = {ColorComponentFlags::R, ColorComponentFlags::G, ColorComponentFlags::B, ColorComponentFlags::A};
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

    struct UserDefinedVertexInput
    {
        uint32 location = 0;
        size_t offset   = 0;
        size_t size     = 0;
        Format format   = Format::UNDEFINED;
    };

    struct ShaderDesc
    {
        LINAGX_VEC<ShaderCompileData>      stages           = {};
        LINAGX_VEC<ShaderColorAttachment>  colorAttachments = {};
        ShaderDepthStencilDesc             depthStencilDesc;
        ShaderLayout                       layout                  = {};
        PolygonMode                        polygonMode             = PolygonMode::Fill;
        CullMode                           cullMode                = CullMode::None;
        FrontFace                          frontFace               = FrontFace::CW;
        Topology                           topology                = Topology::TriangleList;
        bool                               blendLogicOpEnabled     = false;
        LogicOp                            blendLogicOp            = LogicOp::Copy;
        bool                               depthBiasEnable         = false;
        float                              depthBiasConstant       = 0.0f;
        float                              depthBiasClamp          = 0.0f;
        float                              depthBiasSlope          = 0.0f;
        bool                               alphaToCoverage         = false;
        bool                               drawIndirectEnabled     = false;
        bool                               useCustomPipelineLayout = false; // If false, layout from the reflection info will be auto-generated and used.
        uint16                             customPipelineLayout    = 0;     // If useCustomPipelineLayout is true, set this to the handle of the layout you create with CreatePipelineLayout
        LINAGX_VEC<UserDefinedVertexInput> customVertexInputs      = {};    // If non-empty, use this to define your vertex shader inputs.
        const char*                        debugName               = "LinaGXShader";
    };

    struct ViewDesc
    {
        uint32 baseArrayLevel = 0;
        uint32 levelCount     = 0; // Leaving at 0 means it will cover all remaining array layers.
        uint32 baseMipLevel   = 0;
        uint32 mipCount       = 0; // Leaving at 0 means it will cover all remaining mip levels.
        bool   isCubemap      = false;
    };

    struct TextureDesc
    {
        TextureType          type                     = TextureType::Texture2D;
        Format               format                   = Format::R8G8B8A8_SRGB;
        Format               depthStencilSampleFormat = Format::R32_SFLOAT; // If this is a depthStencil texture, .format will be used as usual, but this format will be used in DX12 when sampling.
        LINAGX_VEC<ViewDesc> views                    = {{0, 0, 0, 0, false}};
        uint16               flags                    = 0; // Use TextureFlags enum to define how this texture will be used.
        uint32               width                    = 0;
        uint32               height                   = 0;
        uint32               mipLevels                = 1;
        uint32               arrayLength              = 1;
        const char*          debugName                = "LinaGXTexture";
    };

    struct ResourceDesc
    {
        uint64       size          = 0;
        uint32       typeHintFlags = 0; // Use TypeHintFlags enum to let LinaGX know the purpose of this resource, thus it can set necessary backend bits.
        ResourceHeap heapType      = ResourceHeap::StagingHeap;
        bool         isGPUWritable = false; // Needs to be true if want to use for non-read only resources and UAVs.
        const char*  debugName     = "LinaGXResource";
    };

    struct QueueDesc
    {
        CommandType type      = CommandType::Graphics;
        const char* debugName = "LinaGXQueue";
    };

    struct DescriptorBinding
    {
        uint32                  descriptorCount  = 1;
        DescriptorType          type             = DescriptorType::UBO;
        bool                    unbounded        = false; // True for dynamic arrays, e.g. uniform XXX [];
        bool                    useDynamicOffset = false;
        bool                    isWritable       = false;
        LINAGX_VEC<ShaderStage> stages;
    };

    struct DescriptorSetDesc
    {
        LINAGX_VEC<DescriptorBinding> bindings;
        uint32                        allocationCount = 1;
    };

    struct DescriptorUpdateImageDesc
    {
        uint16             setHandle          = 0;
        uint32             setAllocationIndex = 0; // You can allocate more than 1 set with single CreateDescriptorSet() call. This is the index of the allocation. Leave 0 if allocated only 1 set.
        uint32             binding            = 0;
        LINAGX_VEC<uint32> textures           = {};
        LINAGX_VEC<uint32> samplers           = {};
        LINAGX_VEC<uint32> textureViewIndices = {}; // If left empty, the first view (0) will be used.
    };

    struct DescriptorUpdateBufferDesc
    {
        uint16             setHandle          = 0;
        uint32             setAllocationIndex = 0; // You can allocate more than 1 set with single CreateDescriptorSet() call. This is the index of the allocation. Leave 0 if allocated only 1 set.
        uint32             binding            = 0;
        LINAGX_VEC<uint32> buffers            = {};
        LINAGX_VEC<uint32> ranges             = {}; // Can be left empty, whole size will be used.
        LINAGX_VEC<uint32> offsets            = {}; // Can be left empty, no offset will be used.
        bool               isWriteAccess      = false;
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
        bool                                        isCompute   = false;
        bool                                        hasGLDrawID = false;
        const char*                                 debugName   = "LinaGX PipelineLayout";
    };

} // namespace LinaGX
