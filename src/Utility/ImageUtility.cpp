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

#include "Utility/ImageUtility.hpp"
#include "Utility/MathUtility.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "Utility/stb/stb_image.h"
#include "Utility/stb/stb_image_resize.h"

namespace LinaGX
{
    int GetSTBIChannelMask(ImageChannelMask mask)
    {
        switch (mask)
        {
        case LinaGX::ImageChannelMask::Grey:
            return STBI_grey;
        case LinaGX::ImageChannelMask::GreyAlpha:
            return STBI_grey_alpha;
        case LinaGX::ImageChannelMask::Rgb:
            return STBI_rgb;
        case LinaGX::ImageChannelMask::Rgba:
            return STBI_rgb_alpha;
        default:
            return STBI_rgb_alpha;
        }
    }

    uint8* LoadImage(const char* path, uint32& outWidth, uint32& outHeight, uint32& outChannelCount, uint32& outMipLevel, ImageChannelMask channelMask)
    {
        int    w = 0, h = 0, ch = 0;
        uint8* data     = stbi_load(path, &w, &h, &ch, GetSTBIChannelMask(channelMask));
        outWidth        = w;
        outHeight       = h;
        outChannelCount = static_cast<uint32>(channelMask) + 1;
        outMipLevel     = FloorLog2(Max(outWidth, outHeight)) + 1;
        return data;
    }

    LINAGX_API void FreeImage(uint8* pixels)
    {
        stbi_image_free(pixels);
    }
} // namespace LinaGX
