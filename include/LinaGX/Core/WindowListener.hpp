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
    class Window;
    struct LGXVector2i;
    struct LGXVector2ui;
    enum class InputAction;

    class WindowListener
    {
    public:
        WindowListener()          = default;
        virtual ~WindowListener() = default;

        virtual void OnWindowClose(Window* window){};
        virtual void OnWindowPosChanged(Window* window, const LGXVector2i&){};
        virtual void OnWindowSizeChanged(Window* window, const LGXVector2ui&){};
        virtual void OnWindowKey(Window* window, uint32 keycode, int32 scancode, InputAction inputAction){};
        virtual void OnWindowMouse(Window* window, uint32 button, InputAction inputAction){};
        virtual void OnWindowMouseWheel(Window* window, float delta){};
        virtual void OnWindowMouseMove(Window* window, const LGXVector2&){};
        virtual void OnWindowFocus(Window* window, bool gainedFocus){};
        virtual void OnWindowHoverBegin(Window* window){};
        virtual void OnWindowHoverEnd(Window* window){};

    protected:
        friend class Window;
    };

} // namespace LinaGX
