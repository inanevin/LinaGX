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

#ifndef LINAGX_COMMANDS_HPP
#define LINAGX_COMMANDS_HPP

#include "LinaGX/Common/CommonGfx.hpp"

namespace LinaGX
{
    struct RenderPassColorAttachment
    {
        LoadOp     loadOp;
        StoreOp    storeOp;
        LGXVector4 clearColor;
        uint32     texture;
        uint32     viewIndex;
        bool       isSwapchain = false;
    };

    struct RenderPassDepthStencilAttachment
    {
        bool    useDepth;
        bool    useStencil;
        uint32  texture;
        LoadOp  depthLoadOp;
        StoreOp depthStoreOp;
        float   clearDepth;
        LoadOp  stencilLoadOp;
        StoreOp stencilStoreOp;
        uint32  clearStencil;
        uint32  viewIndex;
    };

    struct CMDBeginRenderPass
    {
        void*                            extension;
        RenderPassColorAttachment*       colorAttachments;
        uint32                           colorAttachmentCount;
        RenderPassDepthStencilAttachment depthStencilAttachment;
        Viewport                         viewport;
        ScissorsRect                     scissors;
    };

    struct CMDEndRenderPass
    {
        void* extension;
    };

    struct CMDSetViewport
    {
        void*  extension;
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
        float  minDepth;
        float  maxDepth;
    };

    struct CMDSetScissors
    {
        void*  extension;
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
    };

    struct CMDBindPipeline
    {
        void*  extension;
        uint16 shader;
    };

    struct CMDDrawInstanced
    {
        void*  extension;
        uint32 vertexCountPerInstance;
        uint32 instanceCount;
        uint32 startVertexLocation;
        uint32 startInstanceLocation;
    };

    struct CMDDrawIndexedInstanced
    {
        void*  extension;
        uint32 indexCountPerInstance;
        uint32 instanceCount;
        uint32 startIndexLocation;
        uint32 baseVertexLocation;
        uint32 startInstanceLocation;
    };

    struct CMDDrawIndexedIndirect
    {
        void*  extension;
        uint32 indirectBuffer;
        uint32 count;
        uint32 stride;
    };

    struct CMDDrawIndirect
    {
        void*  extension;
        uint32 indirectBuffer;
        uint32 count;
        uint32 stride;
    };

    struct CMDCopyResource
    {
        void*  extension;
        uint32 source;
        uint32 destination;
    };

    struct CMDCopyBufferToTexture2D
    {
        void*          extension;
        uint32         destTexture;
        uint32         mipLevels;
        uint32         destinationSlice;
        TextureBuffer* buffers;
    };

    struct CMDCopyTexture
    {
        void*  extension;
        uint32 srcTexture;
        uint32 dstTexture;
        uint32 srcLayer;
        uint32 dstLayer;
        uint32 srcMip;
        uint32 dstMip;
    };

    struct CMDBindVertexBuffers
    {
        void*  extension;
        uint32 slot;
        uint32 resource;
        uint32 vertexSize;
        uint64 offset;
    };

    struct CMDBindIndexBuffers
    {
        void*     extension;
        uint32    resource;
        uint64    offset;
        IndexType indexType;
    };

    enum class DescriptorSetsLayoutSource
    {
        LastBoundShader,
        CustomLayout,
        CustomShader,
    };

    struct CMDBindDescriptorSets
    {
        void*                      extension;
        uint32                     firstSet;
        uint32                     setCount;
        uint16*                    descriptorSetHandles;
        bool                       isCompute;
        uint32                     dynamicOffsetCount;
        uint32*                    dynamicOffsets;
        DescriptorSetsLayoutSource layoutSource;
        uint16                     customLayout;
        uint32                     customLayoutShader;
    };

    struct CMDBindConstants
    {
        void*        extension;
        void*        data;
        uint32       offset;
        uint32       size;
        ShaderStage* stages;
        uint32       stagesSize;
    };

    struct CMDDispatch
    {
        void*  extension;
        uint32 groupSizeX;
        uint32 groupSizeY;
        uint32 groupSizeZ;
    };

    struct CMDComputeBarrier
    {
        void* extension;
    };

    struct CMDExecuteSecondaryStream
    {
        void*          extension;
        CommandStream* secondaryStream;
        bool           recordInThreads;
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

    struct CMDBarrier
    {
        void*            extension;
        uint32           textureBarrierCount;
        TextureBarrier*  textureBarriers;
        uint32           resourceBarrierCount;
        ResourceBarrier* resourceBarriers;
        uint32           srcStageFlags;
        uint32           dstStageFlags;
    };

#define BACKEND_BIND_COMMANDS(BACKEND)                                                                 \
    m_cmdFunctions[LGX_GetTypeID<CMDBeginRenderPass>()]        = &BACKEND::CMD_BeginRenderPass;        \
    m_cmdFunctions[LGX_GetTypeID<CMDEndRenderPass>()]          = &BACKEND::CMD_EndRenderPass;          \
    m_cmdFunctions[LGX_GetTypeID<CMDSetViewport>()]            = &BACKEND::CMD_SetViewport;            \
    m_cmdFunctions[LGX_GetTypeID<CMDSetScissors>()]            = &BACKEND::CMD_SetScissors;            \
    m_cmdFunctions[LGX_GetTypeID<CMDBindPipeline>()]           = &BACKEND::CMD_BindPipeline;           \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawInstanced>()]          = &BACKEND::CMD_DrawInstanced;          \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawIndexedInstanced>()]   = &BACKEND::CMD_DrawIndexedInstanced;   \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawIndexedIndirect>()]    = &BACKEND::CMD_DrawIndexedIndirect;    \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawIndirect>()]           = &BACKEND::CMD_DrawIndirect;           \
    m_cmdFunctions[LGX_GetTypeID<CMDBindVertexBuffers>()]      = &BACKEND::CMD_BindVertexBuffers;      \
    m_cmdFunctions[LGX_GetTypeID<CMDBindIndexBuffers>()]       = &BACKEND::CMD_BindIndexBuffers;       \
    m_cmdFunctions[LGX_GetTypeID<CMDCopyResource>()]           = &BACKEND::CMD_CopyResource;           \
    m_cmdFunctions[LGX_GetTypeID<CMDCopyBufferToTexture2D>()]  = &BACKEND::CMD_CopyBufferToTexture2D;  \
    m_cmdFunctions[LGX_GetTypeID<CMDCopyTexture>()]            = &BACKEND::CMD_CopyTexture;            \
    m_cmdFunctions[LGX_GetTypeID<CMDBindDescriptorSets>()]     = &BACKEND::CMD_BindDescriptorSets;     \
    m_cmdFunctions[LGX_GetTypeID<CMDDispatch>()]               = &BACKEND::CMD_Dispatch;               \
    m_cmdFunctions[LGX_GetTypeID<CMDComputeBarrier>()]         = &BACKEND::CMD_ComputeBarrier;         \
    m_cmdFunctions[LGX_GetTypeID<CMDBindConstants>()]          = &BACKEND::CMD_BindConstants;          \
    m_cmdFunctions[LGX_GetTypeID<CMDExecuteSecondaryStream>()] = &BACKEND::CMD_ExecuteSecondaryStream; \
    m_cmdFunctions[LGX_GetTypeID<CMDBarrier>()]                = &BACKEND::CMD_Barrier;
} // namespace LinaGX

#endif
