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

#include "LinaGX/Core/WindowManager.hpp"
#include "LinaGX/Core/Input.hpp"

#ifdef LINAGX_PLATFORM_WINDOWS
#include <Windows.h>
#else

#endif

namespace LinaGX
{
    Window* WindowManager::CreateApplicationWindow(LINAGX_STRINGID LINAGX_STRINGID, const char* title, uint32 x, uint32 y, uint32 width, uint32 height, WindowStyle style)
    {
        auto it = m_windows.find(LINAGX_STRINGID);
        LOGA((it == m_windows.end()), "Window Manager -> Window with the same LINAGX_STRINGID already exists! %d", LINAGX_STRINGID);
        Window* win = new Window(m_input);

        if (!win->Create(LINAGX_STRINGID, title, x, y, width, height, style))
        {
            LOGE("Window Manager -> Failed creating window!");
            delete win;
            return nullptr;
        }

        m_windows[LINAGX_STRINGID] = win;
        return win;
    }

    void WindowManager::DestroyApplicationWindow(LINAGX_STRINGID LINAGX_STRINGID)
    {
        auto it = m_windows.find(LINAGX_STRINGID);
        LOGA((it != m_windows.end()), "Window Manager -> Window with the LINAGX_STRINGID %d could not be found!", LINAGX_STRINGID);
        delete it->second;
        m_windows.erase(it);
    }

    void WindowManager::PollWindow()
    {
        m_input->PreTick();

#ifdef LINAGX_PLATFORM_WINDOWS
        MSG msg    = {0};
        msg.wParam = 0;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#else

#endif
        m_input->Tick();
    }

    void WindowManager::Initialize()
    {
    }

    void WindowManager::Shutdown()
    {
        LOGA(m_windows.empty(), "Window Manager -> Some windows were not destroyed!");
    }

} // namespace LinaGX
