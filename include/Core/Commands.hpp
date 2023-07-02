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

#include "Common/Common.hpp"

namespace LinaGX
{

    struct CMDBeginRenderPass
    {
        bool         isSwapchain;
        uint8        swapchain;
        uint32       colorTexture;
        uint32       depthTexture;
        float        clearColor[4];
        Viewport     viewport;
        ScissorsRect scissors;
    };

    struct CMDEndRenderPass
    {
        uint32 texture;
        uint8  swapchain;
        bool   isSwapchain;
    };

    struct CMDSetViewport
    {
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
        float  minDepth;
        float  maxDepth;
    };

    struct CMDSetScissors
    {
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
    };

    struct CMDBindPipeline
    {
        uint16 shader;
    };

    struct CMDDrawInstanced
    {
        uint32 vertexCountPerInstance;
        uint32 instanceCount;
        uint32 startVertexLocation;
        uint32 startInstanceLocation;
    };

    struct CMDDrawIndexedInstanced
    {
        uint32 indexCountPerInstance;
        uint32 instanceCount;
        uint32 startIndexLocation;
        uint32 baseVertexLocation;
        uint32 startInstanceLocation;
    };

    struct CMDCopyResource
    {
        uint32 source;
        uint32 destination;
    };

    struct CMDBindVertexBuffers
    {
        uint32 slot;
        uint32 resource;
        uint32 vertexSize;
        uint64 offset;
    };

    struct CMDBindIndexBuffers
    {
    };

#define BACKEND_BIND_COMMANDS(BACKEND)                                                         \
    m_cmdFunctions[GetTypeID<CMDBeginRenderPass>()]      = &BACKEND::CMD_BeginRenderPass;      \
    m_cmdFunctions[GetTypeID<CMDEndRenderPass>()]        = &BACKEND::CMD_EndRenderPass;        \
    m_cmdFunctions[GetTypeID<CMDSetViewport>()]          = &BACKEND::CMD_SetViewport;          \
    m_cmdFunctions[GetTypeID<CMDSetScissors>()]          = &BACKEND::CMD_SetScissors;          \
    m_cmdFunctions[GetTypeID<CMDBindPipeline>()]         = &BACKEND::CMD_BindPipeline;         \
    m_cmdFunctions[GetTypeID<CMDDrawInstanced>()]        = &BACKEND::CMD_DrawInstanced;        \
    m_cmdFunctions[GetTypeID<CMDDrawIndexedInstanced>()] = &BACKEND::CMD_DrawIndexedInstanced; \
    m_cmdFunctions[GetTypeID<CMDBindVertexBuffers>()]    = &BACKEND::CMD_BindVertexBuffers;    \
    m_cmdFunctions[GetTypeID<CMDBindIndexBuffers>()]     = &BACKEND::CMD_BindIndexBuffers;     \
    m_cmdFunctions[GetTypeID<CMDCopyResource>()]         = &BACKEND::CMD_CopyResource;
} // namespace LinaGX

#endif
