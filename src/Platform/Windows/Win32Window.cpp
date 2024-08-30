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

#include "LinaGX/Platform/Windows/Win32Window.hpp"
#include "LinaGX/Core/Input.hpp"
#include <Windows.h>
#include <windowsx.h>
#include <shellscalingapi.h>
#include <chrono>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

/* RAW INPUT
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif
 */

namespace LinaGX
{
    LINAGX_MAP<HWND__*, Win32Window*> Win32Window::s_win32Windows;

    auto AdjustMaximizedClientRect(Win32Window* win32, HWND window, RECT& rect, bool fs) -> void
    {
        if (!win32->GetIsMaximized())
            return;

        auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
        if (!monitor)
            return;

        MONITORINFO monitor_info{};
        monitor_info.cbSize = sizeof(monitor_info);
        if (!::GetMonitorInfoW(monitor, &monitor_info))
            return;

        rect = fs ? monitor_info.rcMonitor : monitor_info.rcWork;
    }

    auto composition_enabled() -> bool
    {
        BOOL composition_enabled = FALSE;
        bool success             = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
        return composition_enabled && success;
    }

    LRESULT HandleNonclientHitTest2(Win32Window* win32Window, LPARAM lParam, POINT cursor, HWND wnd)
    {
        // identify borders and corners to allow resizing the window.
        // Note: On Windows 10, windows behave differently and
        // allow resizing outside the visible window frame.
        // This implementation does not replicate that behavior.
        const POINT border{
            ::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
            ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)};
        RECT window;
        if (!::GetWindowRect(wnd, &window))
        {
            return HTNOWHERE;
        }

        enum region_mask
        {
            client = 0b0000,
            left   = 0b0001,
            right  = 0b0010,
            top    = 0b0100,
            bottom = 0b1000,
        };

        const auto result =
            left * (cursor.x < (window.left + border.x)) |
            right * (cursor.x >= (window.right - border.x)) |
            top * (cursor.y < (window.top + border.y)) |
            bottom * (cursor.y >= (window.bottom - border.y));

        bool borderless_resize = true;

        // Detect the mouse position
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        ScreenToClient(wnd, &pt);

        auto dragRect = win32Window->GetDragRect();

        RECT rcClient;
        // GetClientRect(hwnd, &rcClient);
        rcClient.left   = dragRect.pos.x;
        rcClient.right  = dragRect.pos.x + dragRect.size.x;
        rcClient.top    = dragRect.pos.y;
        rcClient.bottom = dragRect.pos.y + dragRect.size.y;

        DWORD drag = HTCLIENT;
        if (PtInRect(&rcClient, pt))
            drag = HTCAPTION;

        switch (result)
        {
        case left:
            return HTLEFT;
        case right:
            return HTRIGHT;
        case top:
            return HTTOP;
        case bottom:
            return HTBOTTOM;
        case top | left:
            return HTTOPLEFT;
        case top | right:
            return HTTOPRIGHT;
        case bottom | left:
            return HTBOTTOMLEFT;
        case bottom | right:
            return HTBOTTOMRIGHT;
        case client:
            return drag;
        default:
            return HTCLIENT;
        }
    }

    LRESULT __stdcall Win32Window::WndProc(HWND__* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam)
    {
        auto* win32Window = s_win32Windows[hwnd];

        if (msg == WM_NCCREATE)
        {
            auto userData        = reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams;
            s_win32Windows[hwnd] = static_cast<Win32Window*>(userData);
        }

        if (win32Window == nullptr)
            return DefWindowProcA(hwnd, msg, wParam, lParam);

        auto handleMouseMove = [](Win32Window* win32Window, float xPos, float yPos) {
            const LGXVector2 mp          = {xPos, yPos};
            win32Window->m_mousePosition = mp;
            win32Window->m_input->WindowFeedMousePosition(win32Window->m_mousePosition, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouseMove(win32Window, win32Window->m_mousePosition);
        };

        switch (msg)
        {
        case WM_DISPLAYCHANGE: {
            win32Window->CalculateMonitorInfo();
            break;
        }
        case WM_NCACTIVATE: {

            if (!composition_enabled())
            {
                // Prevents window frame reappearing on window activation
                // in "basic" theme, where no aero shadow is present.
                return 1;
            }
            break;
        }
        case WM_CLOSE: {
            for (auto* l : win32Window->m_listeners)
                l->OnWindowClose(win32Window);
            return 0;
        }
        case WM_NCHITTEST: {
            if (win32Window->m_style == WindowStyle::BorderlessApplication)
            {
                // auto res = HandleNonClientArea(win32Window->m_hwnd, lParam, win32Window->m_dragRect, msg, wParam, true);
                auto res = HandleNonclientHitTest2(win32Window, lParam, POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)},
                                                   hwnd);

                win32Window->m_hittestOwnsCursor = res != HTCLIENT;

                if (res == HTCLIENT)
                    win32Window->SetCursorType(win32Window->m_cursorType);
                return res;
            }
            break;
        }
        case WM_NCCALCSIZE: {
            if (win32Window->m_style == WindowStyle::BorderlessApplication)
            {
                if (wParam == TRUE)
                {
                    auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                    AdjustMaximizedClientRect(win32Window, hwnd, params.rgrc[0], win32Window->m_isMaximizeFullscreen);
                    return 0;
                }
            }
            break;
        }
        case WM_KILLFOCUS: {

            if (!win32Window->m_markedDestroy)
            {
                for (auto* l : win32Window->m_listeners)
                    l->OnWindowFocus(win32Window, false);
                win32Window->m_manager->SetWindowFocused(this);
                win32Window->m_hasFocus = false;
            }

            break;
        }
        case WM_SETFOCUS: {
            for (auto* l : win32Window->m_listeners)
                l->OnWindowFocus(win32Window, true);

            win32Window->m_hasFocus = true;
            break;
        }
        case WM_INPUT: {

            if (win32Window->m_hasFocus)
            {
                UINT        dwSize = sizeof(RAWINPUT);
                static BYTE lpb[sizeof(RAWINPUT)];

                GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

                RAWINPUT* raw = (RAWINPUT*)lpb;

                if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    int xPosRelative = raw->data.mouse.lLastX;
                    int yPosRelative = raw->data.mouse.lLastY;
                    win32Window->m_input->WindowFeedDelta(xPosRelative, yPosRelative);
                }
            }

            return 0;
        }
        case WM_DPICHANGED: {
            const uint32 dpi = static_cast<uint32>((short)LOWORD(lParam));
            win32Window->OnDPIChanged(dpi);
            win32Window->CalculateMonitorInfo();
            break;
        }
        case WM_ACTIVATEAPP: {
            win32Window->m_input->WindowFeedActivateApp(wParam == TRUE ? true : false);
            break;
        }
        case WM_ACTIVATE: {
            if (win32Window->m_style == WindowStyle::BorderlessFullscreen)
            {
                win32Window->m_style = WindowStyle::WindowedApplication;
                win32Window->SetStyle(WindowStyle::BorderlessFullscreen);
            }
            break;
        }
        case WM_MOVE: {

            const int32 x = static_cast<int32>((short)LOWORD(lParam));
            const int32 y = static_cast<int32>((short)HIWORD(lParam));

            const LGXVector2i mp    = {x, y};
            win32Window->m_position = mp;

            for (auto* l : win32Window->m_listeners)
                l->OnWindowPosChanged(win32Window, win32Window->m_position);

            break;
        }
        case WM_SIZE: {

            UINT width  = LOWORD(lParam);
            UINT height = HIWORD(lParam);

            RECT rect;
            GetWindowRect(hwnd, &rect);

            RECT clientRect;
            GetClientRect(win32Window->m_hwnd, &clientRect);

            win32Window->m_size.x = width;
            win32Window->m_size.y = height;

            if (win32Window->m_style == WindowStyle::WindowedApplication)
            {
                win32Window->m_trueSize.x = rect.right - rect.left;
                win32Window->m_trueSize.y = rect.bottom - rect.top;
            }
            else
            {
                win32Window->m_trueSize = win32Window->m_size;
            }

            for (auto* l : win32Window->m_listeners)
                l->OnWindowSizeChanged(win32Window, win32Window->m_size);

            break;
        }
        case WM_KEYDOWN: {

            WORD  keyFlags   = HIWORD(lParam);
            WORD  scanCode   = LOBYTE(keyFlags);
            int32 key        = static_cast<int32>(wParam);
            int   extended   = (lParam & 0x01000000) != 0;
            bool  isRepeated = (HIWORD(lParam) & KF_REPEAT) != 0;

            if (wParam == VK_SHIFT)
                key = extended == 0 ? VK_LSHIFT : VK_RSHIFT;
            else if (wParam == VK_CONTROL)
                key = extended == 0 ? VK_LCONTROL : VK_RCONTROL;

            const InputAction action = isRepeated ? InputAction::Repeated : InputAction::Pressed;

            if (key == VK_SPACE)
            {
                win32Window->Minimize();
            }

            win32Window->m_input->WindowFeedKey(key, static_cast<int32>(scanCode), action, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowKey(win32Window, key, static_cast<int32>(scanCode), action);

            break;
        }
        case WM_KEYUP: {

            WORD   keyFlags = HIWORD(lParam);
            WORD   scanCode = LOBYTE(keyFlags);
            uint32 key      = static_cast<uint32>(wParam);
            int    extended = (lParam & 0x01000000) != 0;

            if (wParam == VK_SHIFT)
                key = extended ? VK_LSHIFT : VK_RSHIFT;
            else if (wParam == VK_CONTROL)
                key = extended ? VK_LCONTROL : VK_RCONTROL;

            win32Window->m_input->WindowFeedKey(key, static_cast<uint32>(scanCode), InputAction::Released, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowKey(win32Window, key, static_cast<uint32>(scanCode), InputAction::Released);

            break;
        }
        case WM_MOUSEMOVE: {
            float xPos = static_cast<float>(GET_X_LPARAM(lParam));
            float yPos = static_cast<float>(GET_Y_LPARAM(lParam));
            handleMouseMove(win32Window, xPos, yPos);
            return 0;
        }
        case WM_NCMOUSEMOVE: {
            // if (win32Window->m_style == WindowStyle::BorderlessApplication)
            //     handleMouseMove(win32Window, 0, 0);
            break;
        }
        case WM_MOUSEWHEEL: {

            auto delta = GET_WHEEL_DELTA_WPARAM(wParam);

            win32Window->m_input->WindowFeedMouseWheel(static_cast<float>(delta), win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouseWheel(win32Window, static_cast<float>(delta));

            break;
        }
        case WM_LBUTTONDOWN: {

            static std::chrono::steady_clock::time_point lastTime   = {};
            static LGXVector2                            lastPos    = {};
            const LGXVector2                             currentPos = win32Window->m_mousePosition;
            auto                                         current    = std::chrono::high_resolution_clock::now();
            auto                                         elapsed    = std::chrono::duration_cast<std::chrono::nanoseconds>(current - lastTime);
            lastTime                                                = current;
            bool repeated                                           = elapsed.count() < 250000000;

            if (repeated)
            {
                if (std::abs(lastPos.x - currentPos.x) + std::abs(lastPos.y - currentPos.y) > 1.0f)
                    repeated = false;
            }
            lastPos = currentPos;

            win32Window->m_input->WindowFeedMouseButton(VK_LBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouse(win32Window, VK_LBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed);

            break;
        }
        case WM_RBUTTONDOWN: {

            static std::chrono::steady_clock::time_point lastTime   = {};
            static LGXVector2                            lastPos    = {};
            const LGXVector2                             currentPos = win32Window->m_mousePosition;
            auto                                         current    = std::chrono::high_resolution_clock::now();
            auto                                         elapsed    = std::chrono::duration_cast<std::chrono::nanoseconds>(current - lastTime);
            lastTime                                                = current;
            bool repeated                                           = elapsed.count() < 250000000;

            if (repeated)
            {
                if (std::abs(lastPos.x - currentPos.x) + std::abs(lastPos.y - currentPos.y) > 1.0f)
                    repeated = false;
            }
            lastPos = currentPos;

            win32Window->m_input->WindowFeedMouseButton(VK_RBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouse(win32Window, VK_RBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed);

            break;
        }
        case WM_MBUTTONDOWN: {

            static std::chrono::steady_clock::time_point lastTime   = {};
            static LGXVector2                            lastPos    = {};
            const LGXVector2                             currentPos = win32Window->m_mousePosition;
            auto                                         current    = std::chrono::high_resolution_clock::now();
            auto                                         elapsed    = std::chrono::duration_cast<std::chrono::nanoseconds>(current - lastTime);
            lastTime                                                = current;
            bool repeated                                           = elapsed.count() < 250000000;

            if (repeated)
            {
                if (std::abs(lastPos.x - currentPos.x) + std::abs(lastPos.y - currentPos.y) > 1.0f)
                    repeated = false;
            }
            lastPos = currentPos;

            win32Window->m_input->WindowFeedMouseButton(VK_MBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouse(win32Window, VK_MBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed);
            break;
        }
        case WM_LBUTTONUP: {

            win32Window->m_input->WindowFeedMouseButton(VK_LBUTTON, InputAction::Released, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouse(win32Window, VK_LBUTTON, InputAction::Released);
            break;
        }
        case WM_RBUTTONUP: {

            win32Window->m_input->WindowFeedMouseButton(VK_RBUTTON, InputAction::Released, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouse(win32Window, VK_RBUTTON, InputAction::Released);
            break;
        }
        case WM_MBUTTONUP: {

            win32Window->m_input->WindowFeedMouseButton(VK_MBUTTON, InputAction::Released, win32Window);

            for (auto* l : win32Window->m_listeners)
                l->OnWindowMouse(win32Window, VK_MBUTTON, InputAction::Released);

            break;
        }
        }
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    bool Win32Window::Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent)
    {
        m_sid   = sid;
        m_hinst = GetModuleHandle(0);
        WNDCLASSEX wcx;
        BOOL       exists = GetClassInfoEx(m_hinst, title, &wcx);

        if (!exists)
        {
            WNDCLASS wc      = {};
            wc.lpfnWndProc   = Win32Window::WndProc;
            wc.hInstance     = m_hinst;
            wc.lpszClassName = title;
            wc.hCursor       = NULL;
            // wc.style         = CS_HREDRAW | CS_VREDRAW;
            // wc.style         = CS_DBLCLKS;

            if (!RegisterClassA(&wc))
            {
                LOGE("Window -> Failed registering window class!");
                return false;
            }
        }

        Win32Window* parentWindow = nullptr;
        if (parent != nullptr)
            parentWindow = static_cast<Win32Window*>(parent);
        m_parent = parentWindow;

        DWORD exStyle = WS_EX_APPWINDOW;
        DWORD stylew  = GetStyle(style);

        RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        AdjustWindowRect(&windowRect, stylew, FALSE);

        if (style == WindowStyle::WindowedApplication)
        {
            int adjustedWidth  = windowRect.right - windowRect.left;
            int adjustedHeight = windowRect.bottom - windowRect.top;
            m_trueSize.x       = static_cast<uint32>(adjustedWidth);
            m_trueSize.y       = static_cast<uint32>(adjustedHeight);
        }
        else
        {
            m_trueSize.x = width;
            m_trueSize.y = height;
        }

        if (style == WindowStyle::BorderlessAlpha)
            exStyle |= WS_EX_LAYERED;

        m_hwnd = CreateWindowExA(exStyle, title, title, stylew, x, y, m_trueSize.x, m_trueSize.y, parentWindow ? parentWindow->m_hwnd : NULL, NULL, m_hinst, this);

        if (m_hwnd == nullptr)
        {
            LOGE("Window -> Failed creating window!");
            return false;
        }

        m_size.x      = static_cast<uint32>(width);
        m_size.y      = static_cast<uint32>(height);
        m_title       = title;
        m_style       = style;
        m_dpi         = GetDpiForWindow(m_hwnd);
        m_dpiScale    = m_dpi / 96.0f;
        m_restoreSize = m_trueSize;
        m_restorePos  = m_position;
        m_isVisible   = true;
        CalculateMonitorInfo();
        SetFocus(m_hwnd);
        BringToFront();
        SetStyle(style);
        SetVisible(true);

        /*
        RAWINPUTDEVICE Rid[1];
        Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
        Rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
        Rid[0].dwFlags     = RIDEV_INPUTSINK;
        Rid[0].hwndTarget  = m_hwnd;
        RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
        */

        return true;
    }

    void Win32Window::Destroy()
    {
        m_markedDestroy = true;
        DestroyWindow(m_hwnd);
    }

    void Win32Window::Tick()
    {
        if (!m_sizeRequests.empty())
        {
            SetSize(m_sizeRequests[m_sizeRequests.size() - 1]);
            m_sizeRequests.clear();
        }

        if (m_titleChangeRequested)
        {
            SetWindowTextA(m_hwnd, m_title.c_str());
            m_titleChangeRequested = false;
        }

        const auto absMouse = m_input->GetMousePositionAbs();
        const bool isInside = absMouse.x > m_position.x && absMouse.y > m_position.y && absMouse.x < m_position.x + static_cast<int>(m_size.x) && absMouse.y < m_position.y + static_cast<int>(m_size.y);

        if (!m_isHovered && isInside)
        {
            m_isHovered = true;

            for (auto* l : m_listeners)
                l->OnWindowHoverBegin(this);
        }
        else if (m_isHovered && !isInside)
        {
            m_isHovered = false;
            for (auto* l : m_listeners)
                l->OnWindowHoverEnd(this);
        }

        if (m_hasFocus && m_confineStyle != ConfineStyle::None)
        {
            RECT rect = {};
            if (m_confineStyle == ConfineStyle::Region)
                rect = {m_position.x + static_cast<LONG>(m_confineRect.pos.x), m_position.y + static_cast<LONG>(m_confineRect.pos.y), m_position.x + static_cast<LONG>(m_confineRect.pos.x + m_confineRect.size.x), m_position.y + static_cast<LONG>(m_confineRect.pos.y + m_confineRect.size.y)};
            if (m_confineStyle == ConfineStyle::Window)
                rect = {m_position.x, m_position.y, m_position.x + static_cast<LONG>(m_size.x), m_position.y + static_cast<LONG>(m_size.y)};
            else if (m_confineStyle == ConfineStyle::Center)
                rect = {m_position.x + static_cast<LONG>(m_size.x) / 2 - 5, m_position.y + static_cast<LONG>(m_size.y) / 2 - 5, m_position.x + static_cast<LONG>(m_size.x) / 2 + 5, m_position.y + static_cast<LONG>(m_size.y) / 2 + 5};

            ClipCursor(&rect);
        }
    }

    uint32 Win32Window::GetStyle(WindowStyle s)
    {
        // THICKFRAME: aero snap
        // CAPTION: transitions
        if (s == WindowStyle::WindowedApplication)
            return static_cast<uint32>(WS_OVERLAPPEDWINDOW);
        else if (s == WindowStyle::BorderlessAlpha || s == WindowStyle::Borderless)
            return WS_POPUP;
        else if (s == WindowStyle::BorderlessFullscreen || s == WindowStyle::Fullscreen)
            return WS_POPUPWINDOW;
        else
        {
            DWORD style = 0;
            if (composition_enabled())
                style = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
            else
                style = WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

            if (m_parent == nullptr)
                style |= WS_CLIPCHILDREN;

            return style;
        }
    }

    void Win32Window::SetIsFloating(bool isFloating)
    {
        SetWindowPos(
               m_hwnd,
               isFloating ? HWND_TOPMOST : HWND_NOTOPMOST,
               0, 0, 0, 0,
               SWP_NOMOVE |
               SWP_NOSIZE |
               SWP_NOACTIVATE
           );
    }

    void Win32Window::OnDPIChanged(uint32 dpi)
    {
        m_dpi      = GetDpiForWindow(m_hwnd);
        m_dpiScale = m_dpi / 96.0f;
    }

    namespace
    {
        MonitorInfo GetMonitorInfo(HMONITOR monitor)
        {
            MonitorInfo info;

            MONITORINFOEX monitorInfo;
            monitorInfo.cbSize = sizeof(monitorInfo);
            GetMonitorInfo(monitor, &monitorInfo);

            UINT    dpiX, dpiY;
            HRESULT temp2 = GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

            info.size          = {static_cast<uint32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left), static_cast<uint32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top)};
            info.workSize      = {static_cast<uint32>(monitorInfo.rcWork.right - monitorInfo.rcWork.left), static_cast<uint32>(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top)};
            info.workTopLeft   = {static_cast<int32>(monitorInfo.rcWork.left), static_cast<int32>(monitorInfo.rcWork.top)};
            info.isPrimary     = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
            info.monitorHandle = static_cast<void*>(monitor);
            info.dpi           = dpiX;
            info.dpiScale      = static_cast<float>(dpiX) / 96.0f;
            return info;
        }
    } // namespace

    MonitorInfo Window::GetPrimaryMonitorInfo()
    {
        const POINT ptZero  = {0, 0};
        HMONITOR    monitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
        return GetMonitorInfo(monitor);
    }

    void Win32Window::CalculateMonitorInfo()
    {
        HMONITOR monitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTOPRIMARY);
        m_monitorInfo    = GetMonitorInfo(monitor);
    }

    void Win32Window::Close()
    {
        for (auto* l : m_listeners)
            l->OnWindowClose(this);

        CloseWindow(m_hwnd);
    }

    void Win32Window::SetStyle(WindowStyle style)
    {
        const bool  wasExclusiveFullscreen  = m_style == WindowStyle::Fullscreen;
        const bool  wasBorderlessFullscreen = m_style == WindowStyle::BorderlessFullscreen;
        const auto& mi                      = GetMonitorInfoFromWindow();

        m_style = style;
        SetWindowLong(m_hwnd, GWL_STYLE, GetStyle(style));
        ::SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(m_hwnd, SW_SHOW);

        if (style != WindowStyle::Fullscreen && wasExclusiveFullscreen)
        {
            int result = ChangeDisplaySettings(NULL, CDS_RESET);

            if (result != DISP_CHANGE_SUCCESSFUL)
            {
                MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
                PostQuitMessage(0);
            }
        }

        if (style == WindowStyle::BorderlessFullscreen)
        {
            SetWindowPos(m_hwnd, HWND_NOTOPMOST, mi.workTopLeft.x, mi.workTopLeft.y, mi.size.x, mi.size.y, SWP_SHOWWINDOW);
        }
        else if (style == WindowStyle::Fullscreen)
        {
            DEVMODE dmSettings;
            memset(&dmSettings, 0, sizeof(dmSettings));

            if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings))
            {
                MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
                return;
            }

            dmSettings.dmPelsWidth  = mi.size.x;
            dmSettings.dmPelsHeight = mi.size.y;
            dmSettings.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
            int result              = ChangeDisplaySettings(&dmSettings, CDS_FULLSCREEN);
            SetWindowPos(m_hwnd, HWND_TOPMOST, mi.workTopLeft.x, mi.workTopLeft.y, mi.size.x, mi.size.y, SWP_SHOWWINDOW);

            if (result != DISP_CHANGE_SUCCESSFUL)
            {
                MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
                PostQuitMessage(0);
            }
        }

        if ((wasExclusiveFullscreen || wasBorderlessFullscreen) && m_style != WindowStyle::Fullscreen && m_style != WindowStyle::BorderlessFullscreen)
        {
            Maximize();
        }
    }

    void Win32Window::SetCursorType(CursorType type)
    {
        if (m_hittestOwnsCursor)
            return;

        m_cursorType = type;

        HCURSOR cursor = NULL;

        switch (m_cursorType)
        {
        case CursorType::None:
        case CursorType::Default:
            cursor = LoadCursor(NULL, IDC_ARROW);
            break;
        case CursorType::SizeHorizontal:
            cursor = LoadCursor(NULL, IDC_SIZEWE);
            break;
        case CursorType::SizeVertical:
            cursor = LoadCursor(NULL, IDC_SIZENS);
            break;
        case CursorType::Caret:
            cursor = LoadCursor(NULL, IDC_IBEAM);
            break;
        default:
            break;
        }

        if (cursor != NULL)
            SetCursor(cursor);
    }

    void Win32Window::SetPosition(const LGXVector2i& pos)
    {
        m_position = pos;
        SetWindowPos(m_hwnd, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    void Win32Window::SetSize(const LGXVector2ui& size)
    {
        m_setToFullscreen = false;
        m_size            = size;

        if (m_style == WindowStyle::BorderlessApplication)
        {
            m_trueSize.x = size.x;
            m_trueSize.y = size.y;
        }
        else
        {
            RECT windowRect = {0, 0, static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y)};
            AdjustWindowRect(&windowRect, GetWindowLong(m_hwnd, GWL_STYLE), FALSE);
            int adjustedWidth  = windowRect.right - windowRect.left;
            int adjustedHeight = windowRect.bottom - windowRect.top;
            m_trueSize.x       = static_cast<uint32>(adjustedWidth);
            m_trueSize.y       = static_cast<uint32>(adjustedHeight);
        }

        SetWindowPos(m_hwnd, NULL, 0, 0, m_trueSize.x, m_trueSize.y, SWP_NOMOVE | SWP_NOZORDER);
    }

    void Win32Window::CenterPositionToCurrentMonitor()
    {
        HMONITOR    hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor, &monitorInfo);

        int monitorWidth  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        int monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

        int xPos = (monitorWidth - static_cast<int>(m_trueSize.x)) / 2;
        int yPos = (monitorHeight - static_cast<int>(m_trueSize.y)) / 2;

        SetWindowPos(m_hwnd, NULL, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void Win32Window::SetVisible(bool isVisible)
    {
        m_isVisible = isVisible;
        ShowWindow(m_hwnd, m_isVisible ? SW_SHOW : SW_HIDE);
    }

    void Win32Window::BringToFront()
    {
        OpenIcon(m_hwnd);
        SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        m_manager->SetWindowFocused(this);
    }

    void Win32Window::SetAlpha(float alpha)
    {
        if (m_style != WindowStyle::BorderlessAlpha)
        {
            LOGE("Window -> Alpha can only be set on plain layered windows!");
            return;
        }

        const BYTE     finalAlpha = static_cast<BYTE>(alpha * 255.0f);
        const COLORREF colorKey   = RGB(1, 1, 1);
        SetLayeredWindowAttributes(m_hwnd, colorKey, finalAlpha, LWA_ALPHA | LWA_COLORKEY);
        m_isTransparent = alpha < 0.99f;
    }

    void Win32Window::SetTitle(const LINAGX_STRING& str)
    {
        m_title                = str;
        m_titleChangeRequested = true;
    }

    void Win32Window::Maximize()
    {
        m_restoreSize = m_trueSize;
        m_restorePos  = m_position;

        if (m_style == WindowStyle::WindowedApplication || m_style == WindowStyle::BorderlessApplication)
        {
            ShowWindow(m_hwnd, SW_MAXIMIZE);
        }
        else
        {
            const auto& mi = GetMonitorInfoFromWindow();
            SetPosition({mi.workTopLeft.x, mi.workTopLeft.y});
            SetSize({mi.workSize.x, mi.workSize.y});
        }
    }

    void Win32Window::Minimize()
    {
        ShowWindow(m_hwnd, SW_MINIMIZE);
    }

    void Win32Window::Restore()
    {
        if (m_style == WindowStyle::WindowedApplication || m_style == WindowStyle::BorderlessApplication)
        {
            ShowWindow(m_hwnd, SW_RESTORE);
        }
        else
        {
            SetPosition(m_restorePos);
            SetSize(m_restoreSize);
        }
    }

    bool Win32Window::GetIsMaximized()
    {
        WINDOWPLACEMENT placement = {};
        if (!::GetWindowPlacement(m_hwnd, &placement))
            return false;

        return placement.showCmd == SW_MAXIMIZE;
    }

    bool Win32Window::GetIsMinimized()
    {
        return IsIconic(m_hwnd);
    }

    void Win32Window::ConfineMouse()
    {
        m_confineStyle = ConfineStyle::Window;
    }

    void Win32Window::ConfineMouseToRegion(const LGXRectui& region)
    {
        m_confineStyle = ConfineStyle::Region;
        m_confineRect  = region;
    }

    void Win32Window::ConfineMouseToCenter()
    {
        m_confineStyle = ConfineStyle::Center;
    }

    void Win32Window::FreeMouse()
    {
        m_confineStyle = ConfineStyle::None;
        ClipCursor(NULL);
    }

    void Win32Window::SetMouseVisible(bool visible)
    {
        ShowCursor(visible);
    }
} // namespace LinaGX
