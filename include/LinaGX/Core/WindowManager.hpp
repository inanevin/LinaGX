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

#include "LinaGX/Common/CommonGfx.hpp"

#ifdef LINAGX_PLATFORM_WINDOWS
#include "LinaGX/Platform/Windows/Win32Window.hpp"
#elif LINAGX_PLATFORM_APPLE
#include "LinaGX/Platform/Apple/OSXWindow.hpp"
#endif

namespace LinaGX
{
    class Instance;

    class WindowManager
    {
    public:
        Window* CreateApplicationWindow(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style = WindowStyle::WindowedApplication, Window* parent = nullptr);
        void    DestroyApplicationWindow(LINAGX_STRINGID sid);

        inline Window* GetWindow(LINAGX_STRINGID sid)
        {
            return m_windows[sid];
        }

        MonitorInfo GetPrimaryMonitorInfo();

        inline const LINAGX_MAP<LINAGX_STRINGID, Window*>& GetWindows() const
        {
            return m_windows;
        }

        inline const LINAGX_VEC<MonitorInfo>& GetMonitors() const
        {
            return m_monitors;
        }

    private:
        friend class Instance;

        WindowManager(Input* inp)
            : m_input(inp){};
        ~WindowManager() = default;

        void Initialize();
        void Shutdown();
        void PollWindowsAndInput();
        void EndFrame();
        
    private:
        LINAGX_MAP<LINAGX_STRINGID, Window*> m_windows = {};
        LINAGX_VEC<MonitorInfo>              m_monitors;
        Input*                               m_input = nullptr;
        int                                  a       = 0;
    };
} // namespace LinaGX

#endif
