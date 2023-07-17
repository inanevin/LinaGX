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

#ifndef LINAGX_WINDOW_MANAGER_HPP
#define LINAGX_WINDOW_MANAGER_HPP

#include "Common/CommonGfx.hpp"

#ifdef LINAGX_PLATFORM_WINDOWS
#include "Platform/Windows/Win32Window.hpp"
typedef LinaGX::Win32Window Window;
#elif LINAGX_PLATFORM_APPLE
#include "Platform/MacOS/MacOSWindow.hpp"
typedef LinaGX::MacOSWindow Window;
#endif

namespace LinaGX
{
    class Renderer;

    class WindowManager
    {
    public:
        Window* CreateApplicationWindow(StringID stringID, const char* title, uint32 x, uint32 y, uint32 width, uint32 height, WindowStyle style = WindowStyle::Windowed);
        void    DestroyApplicationWindow(StringID stringID);
        void    PollWindow();

    private:
        friend class Renderer;
        void Initialize();
        void Shutdown();

    private:
        LINAGX_MAP<StringID, Window*> m_windows;
    };
} // namespace LinaGX

#endif
