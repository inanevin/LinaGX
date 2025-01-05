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

#include "LinaGX/Core/WindowManager.hpp"
#include "LinaGX/Core/Input.hpp"
#include <algorithm>

#ifdef LINAGX_PLATFORM_WINDOWS
#include <Windows.h>
#include <shellscalingapi.h>

#endif

namespace LinaGX
{
    Window* WindowManager::CreateApplicationWindow(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent)
    {
        auto it = LINAGX_FIND_IF(m_windows.begin(), m_windows.end(), [sid](const auto& pair) -> bool { return pair.first == sid; });
        LOGA((it == m_windows.end()), "Window Manager -> Window with the same sid already exists! %d", sid);

#ifdef LINAGX_PLATFORM_WINDOWS
        Window* win = new Win32Window(m_input, this);
#else
        Window* win = new OSXWindow(m_input, this);
#endif

        if (!win->Create(sid, title, x, y, width, height, style, parent))
        {
            LOGE("Window Manager -> Failed creating window!");
            delete win;
            return nullptr;
        }

        m_windows.push_back({sid, win});

        return win;
    }

    void WindowManager::DestroyApplicationWindow(LINAGX_STRINGID sid)
    {
        auto it = LINAGX_FIND_IF(m_windows.begin(), m_windows.end(), [sid](const auto& pair) -> bool { return pair.first == sid; });
        LOGA((it != m_windows.end()), "Window Manager -> Window with the sid %d could not be found!", sid);
        it->second->Destroy();
        delete it->second;
        m_windows.erase(it);
        PopWindowFromList(sid);
    }

    void WindowManager::SetWindowFocused(Window* window)
    {
        PushWindowToList(window->GetSID());
    }

    void WindowManager::PushWindowToList(LINAGX_STRINGID id)
    {
        auto it = std::find_if(m_windowList.begin(), m_windowList.end(), [id](LINAGX_STRINGID sid) -> bool { return sid == id; });
        if (it != m_windowList.end())
            m_windowList.erase(it);
        m_windowList.push_back(id);
    }

    void WindowManager::PopWindowFromList(LINAGX_STRINGID id)
    {
        auto it = std::find_if(m_windowList.begin(), m_windowList.end(), [id](LINAGX_STRINGID sid) -> bool { return sid == id; });
        if (it != m_windowList.end())
            m_windowList.erase(it);
    }

    Window* WindowManager::GetTopWindow()
    {
        if (m_windowList.empty())
            return nullptr;

        LINAGX_STRINGID id = m_windowList.back();
        auto            it = LINAGX_FIND_IF(m_windows.begin(), m_windows.end(), [id](const auto& pair) -> bool { return pair.first == id; });
        if (it == m_windows.end())
            return nullptr;

        return it->second;
    }

    void WindowManager::TickWindowSystem()
    {
        m_input->Tick();

        for (const auto& [sid, w] : m_windows)
        {
            w->SetLastMouseDelta(m_input->GetMouseDelta());
            w->Tick();

            if (w->GetWrapMouse())
            {
                const MonitorInfo& mi = w->GetMonitorInfoFromWindow();
                const LGXVector2   mp = m_input->GetMousePositionAbs();

                if (mp.x > mi.workTopLeft.x + mi.workSize.x - 5)
                {
                    m_input->SetMousePosition({mi.workTopLeft.x + 6, static_cast<int>(mp.y)});
                }
                else if (mp.x < mi.workTopLeft.x + 5)
                {
                    m_input->SetMousePosition({mi.workTopLeft.x + static_cast<int>(mi.workSize.x) - 6, static_cast<int>(mp.y)});
                }
                else if (mp.y > mi.workTopLeft.y + mi.workSize.y - 5)
                {
                    m_input->SetMousePosition({static_cast<int>(mp.x), mi.workTopLeft.y + 6});
                }
                else if (mp.y < mi.workTopLeft.y + 5)
                {
                    m_input->SetMousePosition({static_cast<int>(mp.x), mi.workTopLeft.y + static_cast<int>(mi.workSize.y) - 6});
                }
            }
        }
    }

    void WindowManager::EndFrame()
    {
        m_input->EndFrame();
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
                data.workSize    = LGXVector2ui{static_cast<uint32>(monitorInfo.rcWork.right - monitorInfo.rcWork.left), static_cast<uint32>(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top)};
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
