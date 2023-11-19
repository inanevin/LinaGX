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
    enum class ImageChannelMask
    {
        G = 0,
        GA,
        RGB,
        RGBA,
    };

    /// <summary>
    /// Returns the correct mip-levels for a texture with the given size.
    /// </summary>
    LINAGX_API uint32 CalculateMipLevels(uint32 width, uint32 height);

    /// <summary>
    /// Given path load the file and fill the pixel data in outData. Channel mask will be used to omit pixels or fill-empty pixels,
    /// e.g. on a texture with 2 channels, if loaded with RGBA, other 2 channels will be filled with stub pixel data.
    /// </summary>
    LINAGX_API void LoadImageFromFile(const char* path, TextureBuffer& outData, ImageChannelMask channelMask = ImageChannelMask::RGBA);

    /// <summary>
    /// Always use this on a texture data obtained via Load functions using LinaGX.
    /// </summary>
    LINAGX_API void FreeImage(uint8* pixels);

    /// <summary>
    /// If requestLevels is 0, will calculate how many maximum mip levels can be generated using CalculateMipLevels().
    /// If not, it will generate as requestLevels as possible.
    /// Given a source texture buffer, outMipData will be filled with buffers per-mip.
    /// outMipData is not cleared, so you can create one vector, fill the first element with original texture data, pass first element here and the vector,
    /// as a result you will have a vector containing all TextureBuffers, both the original and all mips generated.
    /// </summary>
    LINAGX_API void GenerateMipmaps(const TextureBuffer& sourceData, LINAGX_VEC<TextureBuffer>& outMipData, MipmapFilter filter, ImageChannelMask channelMask, bool linearColorSpace, uint32 requestLevels = 0);

} // namespace LinaGX
