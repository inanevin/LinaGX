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
        RenderPassColorAttachment*       colorAttachments;
        uint32                           colorAttachmentCount;
        RenderPassDepthStencilAttachment depthStencilAttachment;
        Viewport                         viewport;
        ScissorsRect                     scissors;

        inline void Init()
        {
            colorAttachmentCount   = 0;
            colorAttachments       = nullptr;
            depthStencilAttachment = {};
            viewport               = {};
            scissors               = {};
        }
    };

    struct CMDEndRenderPass
    {
        inline void Init()
        {

        }
    };

    struct CMDSetViewport
    {
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
        float  minDepth;
        float  maxDepth;

        inline void Init()
        {
            x = y = width = height = 0;
            minDepth               = 0.0f;
            maxDepth               = 1.0f;
        }
    };

    struct CMDSetScissors
    {
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;

        inline void Init()
        {
            x = y = width = height = 0;
        }
    };

    struct CMDBindPipeline
    {
        uint16 shader;

        inline void Init()
        {
            shader = 0;
        }
    };

    struct CMDDrawInstanced
    {
        uint32 vertexCountPerInstance;
        uint32 instanceCount;
        uint32 startVertexLocation;
        uint32 startInstanceLocation;

        inline void Init()
        {
            vertexCountPerInstance = 0;
            instanceCount          = 0;
            startVertexLocation    = 0;
            startInstanceLocation  = 0;
        }
    };

    struct CMDDrawIndexedInstanced
    {
        uint32 indexCountPerInstance;
        uint32 instanceCount;
        uint32 startIndexLocation;
        uint32 baseVertexLocation;
        uint32 startInstanceLocation;

        inline void Init()
        {
            indexCountPerInstance = 0;
            instanceCount         = 0;
            startIndexLocation    = 0;
            baseVertexLocation    = 0;
            startInstanceLocation = 0;
        }
    };

    struct CMDDrawIndexedIndirect
    {
        uint32 indirectBuffer;
        uint32 count;
        uint32 stride;

        inline void Init()
        {
            indirectBuffer = 0;
            count          = 0;
            stride         = 0;
        }
    };

    struct CMDDrawIndirect
    {
        uint32 indirectBuffer;
        uint32 count;
        uint32 stride;

        inline void Init()
        {
            indirectBuffer = 0;
            count          = 0;
            stride         = 0;
        }
    };

    struct CMDCopyResource
    {
        uint32 source;
        uint32 destination;

        inline void Init()
        {
            source      = 0;
            destination = 0;
        }
    };

    struct CMDCopyBufferToTexture2D
    {
        uint32         destTexture;
        uint32         mipLevels;
        uint32         destinationSlice;
        TextureBuffer* buffers;

        inline void Init()
        {
            destTexture      = 0;
            mipLevels        = 0;
            destinationSlice = 0;
            buffers          = nullptr;
        }
    };

    struct CMDCopyTexture
    {
        uint32 srcTexture;
        uint32 dstTexture;
        uint32 srcLayer;
        uint32 dstLayer;
        uint32 srcMip;
        uint32 dstMip;

        inline void Init()
        {
            srcTexture = 0;
            dstTexture = 0;
            srcLayer   = 0;
            dstLayer   = 0;
            srcMip     = 0;
            dstMip     = 0;
        }
    };

    struct CMDBindVertexBuffers
    {
        uint32 slot;
        uint32 resource;
        uint32 vertexSize;
        uint64 offset;

        inline void Init()
        {
            slot       = 0;
            resource   = 0;
            vertexSize = 0;
            offset     = 0;
        }
    };

    struct CMDBindIndexBuffers
    {
        uint32    resource;
        uint64    offset;
        IndexType indexType;

        inline void Init()
        {
            resource  = 0;
            offset    = 0;
            indexType = IndexType::Uint16;
        }
    };

    enum class DescriptorSetsLayoutSource
    {
        LastBoundShader,
        CustomLayout,
        CustomShader,
    };

    struct CMDBindDescriptorSets
    {
        uint32                     firstSet;
        uint32                     setCount;
        uint16*                    descriptorSetHandles;
        bool                       isCompute;
        uint32                     dynamicOffsetCount;
        uint32*                    dynamicOffsets;
        DescriptorSetsLayoutSource layoutSource;
        uint16                     customLayout;
        uint32                     customLayoutShader;

        inline void Init()
        {
            firstSet             = 0;
            setCount             = 0;
            descriptorSetHandles = nullptr;
            isCompute            = false;
            dynamicOffsetCount   = 0;
            dynamicOffsets       = nullptr;
            layoutSource         = {};
            customLayout         = 0;
            customLayoutShader   = 0;
        }
    };

    struct CMDBindConstants
    {
        void*        data;
        uint32       offset;
        uint32       size;
        ShaderStage* stages;
        uint32       stagesSize;

        inline void Init()
        {
            data       = nullptr;
            offset     = 0;
            size       = 0;
            stages     = nullptr;
            stagesSize = 0;
        }
    };

    struct CMDDispatch
    {
        uint32 groupSizeX;
        uint32 groupSizeY;
        uint32 groupSizeZ;

        inline void Init()
        {
            groupSizeX = 0;
            groupSizeY = 0;
            groupSizeZ = 0;
        }
    };

    struct CMDComputeBarrier
    {
    };

    struct CMDExecuteSecondaryStream
    {
        CommandStream* secondaryStream;

        inline void Init()
        {
            secondaryStream = nullptr;
        }
    };

    struct CMDDebug
    {
        uint32 id;

        inline void Init()
        {
            id = 0;
        }
    };

    struct CMDDebugBeginLabel
    {
        const char* label;

        inline void Init()
        {
            label = nullptr;
        }
    };

    struct CMDDebugEndLabel
    {
        inline void Init()
        {

        }
    };

    struct CMDBarrier
    {
        uint32           textureBarrierCount;
        TextureBarrier*  textureBarriers;
        uint32           resourceBarrierCount;
        ResourceBarrier* resourceBarriers;
        uint32           srcStageFlags;
        uint32           dstStageFlags;

        inline void Init()
        {
            textureBarrierCount  = 0;
            textureBarriers      = nullptr;
            resourceBarrierCount = 0;
            resourceBarriers     = nullptr;
            srcStageFlags        = 0;
            dstStageFlags        = 0;
        }
    };

    extern LINAGX_STRING GetCMDDebugName(LINAGX_TYPEID tid);

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
    m_cmdFunctions[LGX_GetTypeID<CMDBarrier>()]                = &BACKEND::CMD_Barrier;                \
    m_cmdFunctions[LGX_GetTypeID<CMDDebug>()]                  = &BACKEND::CMD_Debug;                  \
    m_cmdFunctions[LGX_GetTypeID<CMDDebugBeginLabel>()]        = &BACKEND::CMD_DebugBeginLabel;        \
    m_cmdFunctions[LGX_GetTypeID<CMDDebugEndLabel>()]          = &BACKEND::CMD_DebugEndLabel;
} // namespace LinaGX

