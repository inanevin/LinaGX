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

#include "LinaGX/Core/Commands.hpp"

namespace LinaGX
{
    LINAGX_STRING GetCMDDebugName(LINAGX_TYPEID tid)
    {
        static LINAGX_MAP<LINAGX_TYPEID, LINAGX_STRING> map = {
            {LGX_GetTypeID<CMDBeginRenderPass>(), "CMDBeginRenderPass"},
            {LGX_GetTypeID<CMDEndRenderPass>(), "CMDEndRenderPass"},
            {LGX_GetTypeID<CMDSetViewport>(), "CMDSetViewport"},
            {LGX_GetTypeID<CMDSetScissors>(), "CMDSetScissors"},
            {LGX_GetTypeID<CMDBindPipeline>(), "CMDBindPipeline"},
            {LGX_GetTypeID<CMDDrawInstanced>(), "CMDDrawInstanced"},
            {LGX_GetTypeID<CMDDrawIndexedInstanced>(), "CMDDrawIndexedInstanced"},
            {LGX_GetTypeID<CMDDrawIndexedIndirect>(), "CMDDrawIndexedIndirect"},
            {LGX_GetTypeID<CMDDrawIndirect>(), "CMDDrawIndirect"},
            {LGX_GetTypeID<CMDBindVertexBuffers>(), "CMDBindVertexBuffers"},
            {LGX_GetTypeID<CMDBindIndexBuffers>(), "CMDBindIndexBuffers"},
            {LGX_GetTypeID<CMDCopyResource>(), "CMDCopyResource"},
            {LGX_GetTypeID<CMDCopyBufferToTexture2D>(), "CMDCopyBufferToTexture2D"},
            {LGX_GetTypeID<CMDCopyTexture>(), "CMDCopyTexture"},
            {LGX_GetTypeID<CMDBindDescriptorSets>(), "CMDBindDescriptorSets"},
            {LGX_GetTypeID<CMDDispatch>(), "CMDDispatch"},
            {LGX_GetTypeID<CMDBindConstants>(), "CMDBindConstants"},
            {LGX_GetTypeID<CMDExecuteSecondaryStream>(), "CMDExecuteSecondaryStream"},
            {LGX_GetTypeID<CMDBarrier>(), "CMDBarrier"},
            {LGX_GetTypeID<CMDDebug>(), "CMDDebug"},
            {LGX_GetTypeID<CMDDebugBeginLabel>(), "CMDDebugBeginLabel"},
            {LGX_GetTypeID<CMDDebugEndLabel>(), "CMDDebugEndLabel"},
        };

        return map[tid];
    } // namespace LinaGX
} // namespace LinaGX
