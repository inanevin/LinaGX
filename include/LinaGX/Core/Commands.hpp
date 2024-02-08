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

#include "LinaGX/Common/CommonGfx.hpp"

namespace LinaGX
{
    /// <summary>
    /// Defines a color render target.
    /// </summary>
    struct RenderPassColorAttachment
    {
        LoadOp     loadOp      = LoadOp::Clear;
        StoreOp    storeOp     = StoreOp::Store;
        LGXVector4 clearColor  = {0, 0, 0, 1};
        uint32     texture     = 0; // Target texture to write to.
        uint32     viewIndex   = 0;
        bool       isSwapchain = false;
    };

    struct RenderPassDepthStencilAttachment
    {
        bool    useDepth       = false;
        bool    useStencil     = false;
        uint32  texture        = 0; // Target depth-stencil texture to write to.
        LoadOp  depthLoadOp    = LoadOp::Clear;
        StoreOp depthStoreOp   = StoreOp::Store;
        float   clearDepth     = 1.0f;
        LoadOp  stencilLoadOp  = LoadOp::Clear;
        StoreOp stencilStoreOp = StoreOp::Store;
        uint32  clearStencil   = 0;
        uint32  viewIndex      = 0;
    };

    /// <summary>
    /// Starts a render pass, at least 1 attachment (either color or depthStencil) is required.
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
    struct CMDEndRenderPass
    {
        inline void Init()
        {
        }
    };

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
    struct CMDBindPipeline
    {
        uint16 shader;

        inline void Init()
        {
            shader = 0;
        }
    };

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
    struct CMDDrawIndexedIndirect
    {
        uint32 indirectBuffer;
        uint32 indirectBufferOffset;
        uint32 count;

        inline void Init()
        {
            indirectBuffer       = 0;
            indirectBufferOffset = 0;
            count                = 0;
        }
    };

    /// <summary>
    ///
    /// </summary>
    struct CMDDrawIndirect
    {
        uint32 indirectBuffer;
        uint32 indirectBufferOffset;
        uint32 count;

        inline void Init()
        {
            indirectBuffer       = 0;
            indirectBufferOffset = 0;
            count                = 0;
        }
    };

    /// <summary>
    /// Use for copying from staging to gpu resources.
    /// </summary>
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

    /// <summary>
    /// Use for filling in texture pixel information from CPU.
    /// </summary>
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

    /// <summary>
    /// GPU-to-GPU texture copy.
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    /// LastBoundShader - uses the pipeline layout(either custom or auto-generated via reflection) of the last bound shader.
    /// CustomLayout - uses a specific layout user creates, need to supply it to customLayout parameter
    /// CustomShader - uses the pipeline layout of a specific shader, need to supply it to customLayoutShader parameter
    /// </summary>
    enum class DescriptorSetsLayoutSource
    {
        LastBoundShader,
        CustomLayout,
        CustomShader,
    };

    /// <summary>
    ///
    /// </summary>
    struct CMDBindDescriptorSets
    {
        uint32                     firstSet;
        uint32*                     allocationIndices;
        uint32                     setCount;
        uint16*                    descriptorSetHandles;
        bool                       isCompute;
        uint32                     dynamicOffsetCount;
        uint32*                    dynamicOffsets;
        DescriptorSetsLayoutSource layoutSource;       // determine where to take the pipeline layout for this binding.
        uint16                     customLayout;       // when layoutSource == ::CustomLayout,
        uint32                     customLayoutShader; // when layoutSource == ::CustomShader

        inline void Init()
        {
            firstSet             = 0;
            allocationIndices      = nullptr;
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

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
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

    /// <summary>
    ///
    /// </summary>
    struct CMDExecuteSecondaryStream
    {
        CommandStream* secondaryStream;

        inline void Init()
        {
            secondaryStream = nullptr;
        }
    };

    /// <summary>
    /// Used for break-point debugging.
    /// </summary>
    struct CMDDebug
    {
        uint32 id;

        inline void Init()
        {
            id = 0;
        }
    };

    /// <summary>
    /// Used for labeling using VK debug utilities, DX12 PIX events or Metal debug groups.
    /// Make sure to call CMDDebugEndLabel for every CMDDebugBeginLabel call.
    /// </summary>
    struct CMDDebugBeginLabel
    {
        const char* label;

        inline void Init()
        {
            label = nullptr;
        }
    };

    /// <summary>
    /// Used for labeling using VK debug utilities, DX12 PIX events or Metal debug groups.
    /// </summary>
    struct CMDDebugEndLabel
    {
        inline void Init()
        {
        }
    };

    /// <summary>
    /// Barrier operations in Vulkan, resource transitions in DX12, dull face of emptiness in Metal.
    /// If targeting Vulkan, srcStageFlags and dstStageFlags are required. Use PipelineStageFlags enumeration.
    /// In DX12 stage flags are not required to be filled. In Metal no transition/barriers are needed.
    /// </summary>
    struct CMDBarrier
    {
        uint32           textureBarrierCount;
        TextureBarrier*  textureBarriers;
        uint32           resourceBarrierCount;
        ResourceBarrier* resourceBarriers;
        uint32           memoryBarrierCount;
        MemBarrier*      memoryBarriers;
        uint32           srcStageFlags;
        uint32           dstStageFlags;

        inline void Init()
        {
            textureBarrierCount  = 0;
            textureBarriers      = nullptr;
            resourceBarrierCount = 0;
            resourceBarriers     = nullptr;
            memoryBarrierCount   = 0;
            memoryBarriers       = nullptr;
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
    m_cmdFunctions[LGX_GetTypeID<CMDBindConstants>()]          = &BACKEND::CMD_BindConstants;          \
    m_cmdFunctions[LGX_GetTypeID<CMDExecuteSecondaryStream>()] = &BACKEND::CMD_ExecuteSecondaryStream; \
    m_cmdFunctions[LGX_GetTypeID<CMDBarrier>()]                = &BACKEND::CMD_Barrier;                \
    m_cmdFunctions[LGX_GetTypeID<CMDDebug>()]                  = &BACKEND::CMD_Debug;                  \
    m_cmdFunctions[LGX_GetTypeID<CMDDebugBeginLabel>()]        = &BACKEND::CMD_DebugBeginLabel;        \
    m_cmdFunctions[LGX_GetTypeID<CMDDebugEndLabel>()]          = &BACKEND::CMD_DebugEndLabel;
} // namespace LinaGX
