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
    struct LGXVector3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
    };

    struct LGXVector2
    {
        float x = 0.0f, y = 0.0f;
    };

    struct LGXVector2ui
    {
        unsigned int x = 0, y = 0;
    };

    struct LGXVector2i
    {
        int x = 0, y = 0;
    };

    struct LGXVector4
    {
        float x = 0.0f, y = 0.0f, z = 0.0, w = 0.0f;
    };

    struct LGXVector4ui16
    {
        unsigned short x = 0, y = 0, z = 0, w = 0;
    };

    struct LGXVector4ui8
    {
        uint8 x = 0, y = 0, z = 0, w = 0;
    };

    struct LGXRectui
    {
        LGXVector2ui pos;
        LGXVector2ui size;

        bool IsPointInside(const LGXVector2ui& p) const
        {
            return p.x > pos.x && p.x < pos.x + size.x && p.y > pos.y && p.y < pos.y + size.y;
        }

        bool IsPointInside(const LGXVector2& p) const
        {
            return p.x > static_cast<float>(pos.x) && p.x < static_cast<float>(pos.x + size.x) && p.y > static_cast<float>(pos.y) && p.y < static_cast<float>(pos.y + size.y);
        }
    };

} // namespace LinaGX
