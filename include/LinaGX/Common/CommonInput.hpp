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

#include "LinaGX/Common/Types.hpp"
#include <functional>

namespace LinaGX
{

    enum class InputAction
    {
        Pressed,
        Released,
        Repeated
    };

    enum class CursorType
    {
        None,           // Default
        Default,        // Default
        SizeHorizontal, // That horizontal sizing cursor thingy
        SizeVertical,   // That vertical sizing cursor thingy
        Caret,          // Text/input caret
    };

    enum CharacterMask
    {
        Letter     = 1 << 0,
        Number     = 1 << 1,
        Separator  = 1 << 2,
        Symbol     = 1 << 4,
        Whitespace = 1 << 5,
        Control    = 1 << 6,
        Printable  = 1 << 7,
        Operator   = 1 << 8,
        Sign       = 1 << 9,
        Any        = Letter | Number | Separator | Whitespace | Control | Symbol | Printable | Operator | Sign,
    };

    struct LGXVector2;
    struct LGXVector2i;
    struct LGXVector2ui;
    class Window;

    class InputListener
    {
    public:
        InputListener()          = default;
        virtual ~InputListener() = default;

        virtual void OnKey(uint32 keycode, int32 scancode, InputAction action, LinaGX::Window* window){};
        virtual void OnMouse(uint32 button, InputAction action, LinaGX::Window* window){};
        virtual void OnMouseMove(const LGXVector2& pos, LinaGX::Window* window){};
        virtual void OnMouseWheel(float delta, LinaGX::Window* window){};
    };

    typedef std::function<void()>                                    CallbackNoArg;
    typedef std::function<void(const LGXVector2i&)>                  CallbackPosChanged;
    typedef std::function<void(const LGXVector2ui&, Window*)>        CallbackMouseMove;
    typedef std::function<void(const LGXVector2ui&)>                 CallbackSizeChanged;
    typedef std::function<void(uint32, int32, InputAction, Window*)> CallbackKey;
    typedef std::function<void(uint32, InputAction, Window*)>        CallbackMouse;
    typedef std::function<void(int32, Window*)>                      CallbackMouseWheel;
    typedef std::function<void(bool)>                                CallbackFocus;
    typedef std::function<void()>                                    CallbackHoverBegin;
    typedef std::function<void()>                                    CallbackHoverEnd;

} // namespace LinaGX
