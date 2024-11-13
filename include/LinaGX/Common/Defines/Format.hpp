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

namespace LinaGX
{

    enum class Format
    {
        UNDEFINED = 0,

        // 8 bit
        R8_SINT,
        R8_UINT,
        R8_UNORM,
        R8_SNORM,

        R8G8_SINT,
        R8G8_UINT,
        R8G8_UNORM,
        R8G8_SNORM,

        R8G8B8A8_SINT,
        R8G8B8A8_UINT,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        R8G8B8A8_SRGB,

        B8G8R8A8_UNORM,
        B8G8R8A8_SRGB,

        // 16 bit
        R16_SINT,
        R16_UINT,
        R16_UNORM,
        R16_SNORM,
        R16_SFLOAT,

        R16G16_SINT,
        R16G16_UINT,
        R16G16_UNORM,
        R16G16_SNORM,
        R16G16_SFLOAT,

        R16G16B16A16_SINT,
        R16G16B16A16_UINT,
        R16G16B16A16_UNORM,
        R16G16B16A16_SNORM,
        R16G16B16A16_SFLOAT,

        // 32 bit
        R32_SINT,
        R32_UINT,
        R32_SFLOAT,

        R32G32_SINT,
        R32G32_UINT,
        R32G32_SFLOAT,

        R32G32B32_SFLOAT,
        R32G32B32_SINT,
        R32G32B32_UINT,

        R32G32B32A32_SINT,
        R32G32B32A32_UINT,
        R32G32B32A32_SFLOAT,

        // depth-stencil
        D32_SFLOAT,
        D24_UNORM_S8_UINT,
        D16_UNORM,

        // Misc
        R11G11B10_SFLOAT,
        R10G0B10A2_INT,
        BC3_BLOCK_SRGB,
        BC3_BLOCK_UNORM,
        FORMAT_MAX,
    };
} // namespace LinaGX