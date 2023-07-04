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

#ifndef LINAGX_IMAGE_UTILITY_HPP
#define LINAGX_IMAGE_UTILITY_HPP

#include "Common/Common.hpp"

namespace LinaGX
{
    enum class ImageChannelMask
    {
        Grey = 0,
        GreyAlpha,
        Rgb,
        Rgba
    };

    struct MipData
    {
        uint8* pixels = nullptr;
        uint32 width  = 0;
        uint32 height = 0;
    };

    struct TextureLoadData
    {
        uint8* pixels         = nullptr;
        uint32 width          = 0;
        uint32 height         = 0;
        uint32 totalMipLevels = 0;
    };

    /// <summary>
    ///
    /// </summary>
    /// <param name="path"></param>
    /// <param name="outData"></param>
    /// <param name="channelMask"></param>
    /// <returns></returns>
    LINAGX_API void LoadImage(const char* path, TextureLoadData& outData, ImageChannelMask channelMask = ImageChannelMask::Rgba);

    /// <summary>
    ///
    /// </summary>
    /// <param name="pixels"></param>
    /// <returns></returns>
    LINAGX_API void FreeImage(uint8* pixels);

    /// <summary>
    ///
    /// </summary>
    /// <param name="sourceData"></param>
    /// <param name="outMipData"></param>
    /// <param name="filter"></param>
    /// <param name="mipLevels">Total number of mips to generate (excluding the base texture).</param>
    /// <returns></returns>
    LINAGX_API void GenerateMipmaps(const TextureLoadData& sourceData, LINAGX_VEC<MipData>& outMipData, MipmapFilter filter, ImageChannelMask channelMask, bool linearColorSpace);

} // namespace LinaGX

#endif
