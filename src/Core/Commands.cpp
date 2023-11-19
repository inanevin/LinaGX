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
