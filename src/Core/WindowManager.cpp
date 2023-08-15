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
#include <shellscalingapi.h>
#else

#endif

namespace LinaGX
{
    Window* WindowManager::CreateApplicationWindow(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style)
    {
        auto it = m_windows.find(sid);
        LOGA((it == m_windows.end()), "Window Manager -> Window with the same sid already exists! %d", sid);

#ifdef LINAGX_PLATFORM_WINDOWS
        Window* win = new Win32Window(m_input);
#else
        Window* win = new AppleWindow(m_input);
#endif

        if (!win->Create(sid, title, x, y, width, height, style))
        {
            LOGE("Window Manager -> Failed creating window!");
            delete win;
            return nullptr;
        }

        m_windows[sid] = win;
        return win;
    }

    void WindowManager::DestroyApplicationWindow(LINAGX_STRINGID sid)
    {
        auto it = m_windows.find(sid);
        LOGA((it != m_windows.end()), "Window Manager -> Window with the sid %d could not be found!", sid);
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

        for (const auto [sid, w] : m_windows)
            w->Tick();
    }

    MonitorInfo WindowManager::GetPrimaryMonitorInfo()
    {
        for (const auto& m : m_monitors)
        {
            if (m.isPrimary)
                return m;
        }
        return MonitorInfo();
    }

    void WindowManager::Initialize()
    {
#ifdef LINAGX_PLATFORM_WINDOWS
        EnumDisplayMonitors(
            NULL,
            NULL,
            [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
                LINAGX_VEC<MonitorInfo>& monitors = *reinterpret_cast<LINAGX_VEC<MonitorInfo>*>(dwData);
                MONITORINFOEX            monitorInfo;
                monitorInfo.cbSize = sizeof(monitorInfo);
                GetMonitorInfo(hMonitor, &monitorInfo);

                UINT    dpiX, dpiY;
                HRESULT temp2 = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

                MonitorInfo data;
                data.size        = LGXVector2ui{static_cast<uint32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left), static_cast<uint32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top)};
                data.workArea    = LGXVector2ui{static_cast<uint32>(monitorInfo.rcWork.right - monitorInfo.rcWork.left), static_cast<uint32>(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top)};
                data.workTopLeft = LGXVector2i{static_cast<int32>(monitorInfo.rcWork.left), static_cast<int32>(monitorInfo.rcWork.top)};
                data.isPrimary   = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
                data.dpi         = dpiX;
                data.dpiScale    = static_cast<float>(dpiY) / 96.0f;

                monitors.push_back(data);

                return TRUE;
            },
            reinterpret_cast<LPARAM>(&m_monitors));
#endif
    }

    void WindowManager::Shutdown()
    {
        LOGA(m_windows.empty(), "Window Manager -> Some windows were not destroyed!");
    }

} // namespace LinaGX
