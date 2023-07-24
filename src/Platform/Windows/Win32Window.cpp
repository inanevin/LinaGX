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

#include "LinaGX/Platform/Windows/Win32Window.hpp"
#include <Windows.h>
#include <windowsx.h>

namespace LinaGX
{
    LINAGX_MAP<HWND__*, Win32Window*> Win32Window::s_win32Windows;

    LRESULT __stdcall Win32Window::WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam)
    {
        auto* win32Window = s_win32Windows[window];

        if (win32Window == nullptr)
            return DefWindowProcA(window, msg, wParam, lParam);

        switch (msg)
        {
        case WM_CLOSE: {
            if (win32Window->m_cbClose != nullptr)
                win32Window->m_cbClose();

            return 0;
        }
        case WM_KILLFOCUS: {
            break;
        }
        case WM_SETFOCUS: {
            break;
        }
        case WM_WINDOWPOSCHANGED: {
            break;
        }
        case WM_DISPLAYCHANGE: {
            break;
        }
        case WM_DPICHANGED: {
            break;
        }
        case WM_ACTIVATEAPP: {
            break;
        }
        case WM_MOVE: {

            const uint32 x = static_cast<uint32>((short)LOWORD(lParam));
            const uint32 y = static_cast<uint32>((short)HIWORD(lParam));

            win32Window->m_posX = x;
            win32Window->m_posY = y;

            if (win32Window->m_cbPosChanged)
                win32Window->m_cbPosChanged(x, y);

            break;
        }
        case WM_SIZE: {

            RECT rect;
            GetWindowRect(window, &rect);

            RECT clientRect;
            GetClientRect(win32Window->m_hwnd, &clientRect);

            win32Window->m_width  = clientRect.right - clientRect.left;
            win32Window->m_height = clientRect.bottom - clientRect.top;

            win32Window->m_trueWidth  = rect.right - rect.left;
            win32Window->m_trueHeight = rect.bottom - rect.top;

            if (win32Window->m_cbSizeChanged)
                win32Window->m_cbSizeChanged(win32Window->m_width, win32Window->m_height);

            break;
        }
        case WM_KEYDOWN: {

            WORD   keyFlags   = HIWORD(lParam);
            WORD   scanCode   = LOBYTE(keyFlags);
            uint32 key        = static_cast<uint32>(wParam);
            int    extended   = (lParam & 0x01000000) != 0;
            bool   isRepeated = (HIWORD(lParam) & KF_REPEAT) != 0;

            if (wParam == VK_SHIFT)
                key = extended == 0 ? VK_LSHIFT : VK_RSHIFT;
            else if (wParam == VK_CONTROL)
                key = extended == 0 ? VK_LCONTROL : VK_RCONTROL;

            if (win32Window->m_cbKey)
                win32Window->m_cbKey(key, static_cast<uint32>(scanCode), isRepeated ? InputAction::Repeated : InputAction::Pressed);

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

            if (win32Window->m_cbKey)
                win32Window->m_cbKey(key, static_cast<uint32>(scanCode), InputAction::Released);

            break;
        }
        case WM_MOUSEMOVE: {

            int32 xPos = static_cast<int32>(GET_X_LPARAM(lParam));
            int32 yPos = static_cast<int32>(GET_Y_LPARAM(lParam));

            if (win32Window->m_cbMouseMove)
                win32Window->m_cbMouseMove(xPos, yPos);

            break;
        }
        case WM_MOUSEWHEEL: {

            auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (win32Window->m_cbMouseWheel)
                win32Window->m_cbMouseWheel(static_cast<uint32>(delta));

            break;
        }
        case WM_LBUTTONDOWN: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_LBUTTON, InputAction::Pressed);

            break;
        }
        case WM_RBUTTONDOWN: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_RBUTTON, InputAction::Pressed);

            break;
        }
        case WM_MBUTTONDOWN: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_MBUTTON, InputAction::Pressed);

            break;
        }
        case WM_LBUTTONUP: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_LBUTTON, InputAction::Released);

            break;
        }
        case WM_RBUTTONUP: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_RBUTTON, InputAction::Released);

            break;
        }
        case WM_MBUTTONUP: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_RBUTTON, InputAction::Released);
            break;
        }
        }

        return DefWindowProcA(window, msg, wParam, lParam);
    }

    bool Win32Window::Create(StringID stringID, const char* title, uint32 x, uint32 y, uint32 width, uint32 height, WindowStyle style)
    {
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

            if (!RegisterClassA(&wc))
            {
                LOGE("Window -> Failed registering window class!");
                return false;
            }
        }

        DWORD exStyle = WS_EX_APPWINDOW;
        DWORD stylew  = GetStyle(style);

        RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        AdjustWindowRect(&windowRect, stylew, FALSE);

        int adjustedWidth  = windowRect.right - windowRect.left;
        int adjustedHeight = windowRect.bottom - windowRect.top;
        m_trueWidth        = adjustedWidth;
        m_trueHeight       = adjustedHeight;
        m_width            = width;
        m_height           = height;

        m_hwnd = CreateWindowExA(exStyle, title, title, stylew, x, y, adjustedWidth, adjustedHeight, NULL, NULL, m_hinst, NULL);
        ShowWindow(m_hwnd, SW_SHOW);
        m_title = title;

        if (m_hwnd == nullptr)
        {
            LOGE("Window -> Failed creating window!");
            return false;
        }

        s_win32Windows[m_hwnd] = this;
        m_dpi                  = GetDpiForWindow(m_hwnd);
        m_dpiScale             = m_dpi / 96.0f;

        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return true;
    }

    void Win32Window::Destroy()
    {
        DestroyWindow(m_hwnd);
    }

    uint32 Win32Window::GetStyle(WindowStyle style)
    {
        if (style == WindowStyle::Borderless)
            return WS_POPUP;

        return WS_OVERLAPPEDWINDOW;
    }

    void Win32Window::SetStyle(WindowStyle style)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, GetStyle(style));
    }

    void Win32Window::GetMonitorWorkArea(uint32& width, uint32& height)
    {
        HMONITOR    hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(hMonitor, &monitorInfo);
        width  = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
    }

    void Win32Window::GetMonitorSize(uint32& width, uint32& height)
    {
        HMONITOR    hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(hMonitor, &monitorInfo);
        width  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    }

    void Win32Window::SetPosition(uint32 x, uint32 y)
    {
        m_posX = x;
        m_posY = y;
        SetWindowPos(m_hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void Win32Window::SetSize(uint32 width, uint32 height)
    {
        m_width         = width;
        m_height        = height;
        RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        AdjustWindowRect(&windowRect, GetWindowLong(m_hwnd, GWL_STYLE), FALSE);
        int adjustedWidth  = windowRect.right - windowRect.left;
        int adjustedHeight = windowRect.bottom - windowRect.top;
        m_trueWidth        = adjustedWidth;
        m_trueHeight       = adjustedHeight;
        SetWindowPos(m_hwnd, NULL, 0, 0, adjustedWidth, adjustedHeight, SWP_NOMOVE | SWP_NOZORDER);
    }

    void Win32Window::CenterPositionToCurrentMonitor()
    {
        HMONITOR    hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor, &monitorInfo);

        int monitorWidth  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        int monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

        int xPos = (monitorWidth - m_trueWidth) / 2;
        int yPos = (monitorHeight - m_trueHeight) / 2;

        SetWindowPos(m_hwnd, NULL, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void Win32Window::SetFullscreen()
    {
        SetWindowLong(m_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        HMONITOR    hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(hMonitor, &monitorInfo);
        SetWindowPos(m_hwnd, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                     monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                     monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                     SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowWindow(m_hwnd, SW_MAXIMIZE);
    }

} // namespace LinaGX
