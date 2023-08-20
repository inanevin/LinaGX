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


#include "LinaGX/Utility/ImageUtility.hpp"
#include "LinaGX/Common/Math.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "LinaGX/Utility/stb/stb_image.h"
#include "LinaGX/Utility/stb/stb_image_write.h"
#include "LinaGX/Utility/stb/stb_image_resize.h"

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

    void LoadImageFromFile(const char* path, TextureLoadData& outData, ImageChannelMask channelMask)
    {
        int w = 0, h = 0, ch = 0;
        outData.pixels         = stbi_load(path, &w, &h, &ch, GetSTBIChannelMask(channelMask));
        outData.width          = w;
        outData.height         = h;
        outData.totalMipLevels = FloorLog2(Max(outData.width, outData.height)) + 1;

        if (outData.pixels)
        {
            LOGV("Loaded image: %s", path);
        }
        else
        {
            LOGE("Failed loading image: %s", path);
        }
    }

    LINAGX_API void FreeImage(uint8* pixels)
    {
        stbi_image_free(pixels);
    }

    LINAGX_API void GenerateMipmaps(const TextureLoadData& sourceData, LINAGX_VEC<MipData>& outMipData, MipmapFilter filter, ImageChannelMask channelMask, bool linearColorSpace)
    {
        outMipData.clear();

        uint8* lastPixels = sourceData.pixels;
        uint32 lastWidth  = sourceData.width;
        uint32 lastHeight = sourceData.height;

        for (uint32 i = 0; i < sourceData.totalMipLevels - 1; i++)
        {
            uint32 width  = lastWidth / 2;
            uint32 height = lastHeight / 2;

            if (width < 1)
                width = 1;

            if (height < 1)
                height = 1;

            const uint32 channels         = static_cast<int>(channelMask) + 1;
            MipData      mipData          = {};
            mipData.width                 = width;
            mipData.height                = height;
            mipData.pixels                = (uint8*)std::malloc(width * height * channels);
            const stbir_colorspace cs     = linearColorSpace ? stbir_colorspace::STBIR_COLORSPACE_LINEAR : stbir_colorspace::STBIR_COLORSPACE_SRGB;
            int                    retval = stbir_resize_uint8_generic(lastPixels, lastWidth, lastHeight, 0, mipData.pixels, width, height, 0, channels, STBIR_ALPHA_CHANNEL_NONE, 0, stbir_edge::STBIR_EDGE_CLAMP, static_cast<stbir_filter>(filter), cs, 0);

            lastWidth  = width;
            lastHeight = height;
            lastPixels = mipData.pixels;
            outMipData.push_back(mipData);
        }
    }
} // namespace LinaGX
