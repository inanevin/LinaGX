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
        LoadOp loadOp;
        StoreOp storeOp;
        LGXVector4 clearColor;
        uint32 texture = 0;
        bool isSwapchain = false;
    };

    struct RenderPassDepthStencilAttachment
    {
        bool depthWrite;
        uint32 depthTexture;
        LoadOp depthLoadOp;
        StoreOp depthStoreOp;
        float clearDepth;

        bool useStencil;
        uint32 stencilTexture;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        uint32 clearStencil;
    };

    struct CMDBeginRenderPass
    {
        void*        extension;
        RenderPassColorAttachment* colorAttachments;
        uint32 colorAttachmentCount;
        RenderPassDepthStencilAttachment depthStencilAttachment;
        Viewport     viewport;
        ScissorsRect scissors;
    };

    struct CMDEndRenderPass
    {
        void*  extension;
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
        IndexType indexFormat;
    };

    struct CMDCopyBufferToTexture2D
    {
        void*          extension;
        uint32         destTexture;
        uint32         mipLevels;
        uint32         destinationSlice;
        TextureBuffer* buffers;
    };

    struct CMDBindDescriptorSets
    {
        void*   extension;
        uint32  firstSet;
        uint32  setCount;
        uint16* descriptorSetHandles;
        bool    isCompute;
        uint32 dynamicOffsetCount;
        uint32* dynamicOffsets;
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
        void*   extension;
    };

    struct CMDExecuteSecondaryStream
    {
        void* extension;
        CommandStream* secondaryStream;
        bool recordInThreads;
    };

#define BACKEND_BIND_COMMANDS(BACKEND)                                                           \
    m_cmdFunctions[LGX_GetTypeID<CMDBeginRenderPass>()]         = &BACKEND::CMD_BeginRenderPass;       \
    m_cmdFunctions[LGX_GetTypeID<CMDEndRenderPass>()]           = &BACKEND::CMD_EndRenderPass;         \
    m_cmdFunctions[LGX_GetTypeID<CMDSetViewport>()]             = &BACKEND::CMD_SetViewport;           \
    m_cmdFunctions[LGX_GetTypeID<CMDSetScissors>()]             = &BACKEND::CMD_SetScissors;           \
    m_cmdFunctions[LGX_GetTypeID<CMDBindPipeline>()]            = &BACKEND::CMD_BindPipeline;          \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawInstanced>()]           = &BACKEND::CMD_DrawInstanced;         \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawIndexedInstanced>()]    = &BACKEND::CMD_DrawIndexedInstanced;  \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawIndexedIndirect>()]     = &BACKEND::CMD_DrawIndexedIndirect;   \
    m_cmdFunctions[LGX_GetTypeID<CMDDrawIndirect>()]            = &BACKEND::CMD_DrawIndirect;   \
    m_cmdFunctions[LGX_GetTypeID<CMDBindVertexBuffers>()]       = &BACKEND::CMD_BindVertexBuffers;     \
    m_cmdFunctions[LGX_GetTypeID<CMDBindIndexBuffers>()]        = &BACKEND::CMD_BindIndexBuffers;      \
    m_cmdFunctions[LGX_GetTypeID<CMDCopyResource>()]            = &BACKEND::CMD_CopyResource;          \
    m_cmdFunctions[LGX_GetTypeID<CMDCopyBufferToTexture2D>()]   = &BACKEND::CMD_CopyBufferToTexture2D; \
    m_cmdFunctions[LGX_GetTypeID<CMDBindDescriptorSets>()]      = &BACKEND::CMD_BindDescriptorSets;    \
    m_cmdFunctions[LGX_GetTypeID<CMDDispatch>()]                = &BACKEND::CMD_Dispatch;              \
    m_cmdFunctions[LGX_GetTypeID<CMDComputeBarrier>()]          = &BACKEND::CMD_ComputeBarrier;        \
    m_cmdFunctions[LGX_GetTypeID<CMDBindConstants>()]           = &BACKEND::CMD_BindConstants;        \
    m_cmdFunctions[LGX_GetTypeID<CMDExecuteSecondaryStream>()]  = &BACKEND::CMD_ExecuteSecondaryStream;
} // namespace LinaGX

#endif
