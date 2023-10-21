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
