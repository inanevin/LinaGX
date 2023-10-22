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
        /// <summary>
        /// Creates a window given parameters. StringID is a unique unsigned 64-bit integer representing the id of the window, which will necessary when destroying the window.
        /// You can choose a style for the window which could later be updated through the returned window object.
        /// </summary>
        /// <returns>Pointer to the allocated window object, can't delete yourself, use DestroyApplicationWindow().</returns>
        Window* CreateApplicationWindow(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style = WindowStyle::WindowedApplication, Window* parent = nullptr);

        /// <summary>
        /// Destroy a window with a given id, make sure all gpu and swapchain operations are completed prior.
        /// </summary>
        void DestroyApplicationWindow(LINAGX_STRINGID sid);

        /// <summary>
        /// Find a window with the given id, will return uninitialized memory if not found.
        /// </summary>
        inline Window* GetWindow(LINAGX_STRINGID sid)
        {
            return m_windows[sid];
        }

        /// <summary>
        /// Primary monitor will be detected only upon app launch, return it's information.
        /// </summary>
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
        void TickWindowSystem();
        void EndFrame();

    private:
        LINAGX_MAP<LINAGX_STRINGID, Window*> m_windows = {};
        LINAGX_VEC<MonitorInfo>              m_monitors;
        Input*                               m_input = nullptr;
        int                                  a       = 0;
    };
} // namespace LinaGX
