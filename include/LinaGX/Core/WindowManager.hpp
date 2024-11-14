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
        /// Used internally by windows to keep track of focus hierarcy
        /// </summary>
        void SetWindowFocused(Window* window);

        /// <summary>
        /// Returns the window in the back of the focus list, can be nullptr.
        /// </summary>
        Window* GetTopWindow();

        /// <summary>
        /// Find a window with the given id, will return uninitialized memory if not found.
        /// </summary>
        inline Window* GetWindow(LINAGX_STRINGID sid)
        {
            auto it = LINAGX_FIND_IF(m_windows.begin(), m_windows.end(), [sid](const auto& pair) -> bool {  return pair.first == sid; });
            return it->second;
        }

        /// <summary>
        /// Primary monitor will be detected only upon app launch, return it's information.
        /// </summary>
        MonitorInfo GetPrimaryMonitorInfo();

        /// <summary>
        /// Done automatically when a window gains focus.
        /// </summary>
        void PushWindowToList(LINAGX_STRINGID id);

        /// <summary>
        /// Done automatically when a window is destroyed.
        /// </summary>
        void PopWindowFromList(LINAGX_STRINGID id);

        inline const LINAGX_VEC<LINAGX_PAIR<LINAGX_STRINGID, Window*>>& GetWindows() const
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
        LINAGX_VEC<LINAGX_STRINGID>                       m_windowList;
        LINAGX_VEC<LINAGX_PAIR<LINAGX_STRINGID, Window*>> m_windows = {};
        LINAGX_VEC<MonitorInfo>                           m_monitors;
        Input*                                            m_input = nullptr;
    };
} // namespace LinaGX
