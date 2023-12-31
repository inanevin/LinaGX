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
        case LinaGX::ImageChannelMask::G:
            return STBI_grey;
        case LinaGX::ImageChannelMask::GA:
            return STBI_grey_alpha;
        case LinaGX::ImageChannelMask::RGB:
            return STBI_rgb;
        case LinaGX::ImageChannelMask::RGBA:
            return STBI_rgb_alpha;
        default:
            return STBI_rgb_alpha;
        }
    }

    LINAGX_API uint32 CalculateMipLevels(uint32 width, uint32 height)
    {
        return FloorLog2(Max(width, height)) + 1;
    }

    void LoadImageFromFile(const char* path, TextureBuffer& outData, ImageChannelMask channelMask)
    {
        int        w = 0, h = 0, ch = 0;
        const bool is16 = stbi_is_16_bit(path);

        outData.bytesPerPixel = (static_cast<uint32>(channelMask) + 1) * (is16 ? 2 : 1);

        if (is16)
        {
            auto pixels    = stbi_load_16(path, &w, &h, &ch, GetSTBIChannelMask(channelMask));
            outData.pixels = new uint8[w * h * outData.bytesPerPixel];
            LINAGX_MEMCPY(outData.pixels, pixels, w * h * outData.bytesPerPixel);
            stbi_image_free(pixels);
        }
        else
        {
            outData.pixels = stbi_load(path, &w, &h, &ch, GetSTBIChannelMask(channelMask));
        }

        outData.width  = w;
        outData.height = h;

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

    LINAGX_API void GenerateMipmaps(const TextureBuffer& sourceData, LINAGX_VEC<TextureBuffer>& outMipData, MipmapFilter filter, ImageChannelMask channelMask, bool linearColorSpace, uint32 requestLevels)
    {
        uint8*       lastPixels = sourceData.pixels;
        uint32       lastWidth  = sourceData.width;
        uint32       lastHeight = sourceData.height;
        const uint32 levels     = requestLevels == 0 ? CalculateMipLevels(sourceData.width, sourceData.height) : requestLevels;

        for (uint32 i = 0; i < levels - 1; i++)
        {
            uint32 width  = lastWidth / 2;
            uint32 height = lastHeight / 2;

            if (width < 1)
                width = 1;

            if (height < 1)
                height = 1;

            const uint32  channels    = static_cast<int>(channelMask) + 1;
            TextureBuffer mipData     = {};
            mipData.width             = width;
            mipData.height            = height;
            mipData.pixels            = (uint8*)std::malloc(width * height * sourceData.bytesPerPixel);
            mipData.bytesPerPixel     = sourceData.bytesPerPixel;
            const stbir_colorspace cs = linearColorSpace ? stbir_colorspace::STBIR_COLORSPACE_LINEAR : stbir_colorspace::STBIR_COLORSPACE_SRGB;

            int retVal = 0;

            if (mipData.bytesPerPixel == 4)
                retVal = stbir_resize_uint8_generic(lastPixels, lastWidth, lastHeight, 0, mipData.pixels, width, height, 0, channels, STBIR_ALPHA_CHANNEL_NONE, 0, stbir_edge::STBIR_EDGE_CLAMP, static_cast<stbir_filter>(filter), cs, 0);
            else
                stbir_resize_uint16_generic((uint16*)lastPixels, lastWidth, lastHeight, 0, (uint16*)mipData.pixels, width, height, 0, channels, 0, 0, stbir_edge::STBIR_EDGE_CLAMP, static_cast<stbir_filter>(filter), cs, 0);

            lastWidth  = width;
            lastHeight = height;
            lastPixels = mipData.pixels;
            outMipData.push_back(mipData);
        }
    }
} // namespace LinaGX
