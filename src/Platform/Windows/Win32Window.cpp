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
#include "LinaGX/Core/Input.hpp"
#include <Windows.h>
#include <windowsx.h>
#include <shellscalingapi.h>

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
            const uint32 dpi = static_cast<uint32>((short)LOWORD(lParam));
            win32Window->OnDPIChanged(dpi);
            break;
        }
        case WM_ACTIVATEAPP: {
            win32Window->m_input->WindowFeedActivateApp(wParam == TRUE ? true : false);
            break;
        }
        case WM_MOVE: {

            const int32 x = static_cast<int32>((short)LOWORD(lParam));
            const int32 y = static_cast<int32>((short)HIWORD(lParam));

            win32Window->m_position = {x, y};

            if (win32Window->m_cbPosChanged)
                win32Window->m_cbPosChanged(win32Window->m_position);

            break;
        }
        case WM_SIZE: {

            RECT rect;
            GetWindowRect(window, &rect);

            RECT clientRect;
            GetClientRect(win32Window->m_hwnd, &clientRect);

            win32Window->m_size.x = clientRect.right - clientRect.left;
            win32Window->m_size.y = clientRect.bottom - clientRect.top;

            win32Window->m_trueSize.x = rect.right - rect.left;
            win32Window->m_trueSize.y = rect.bottom - rect.top;

            if (win32Window->m_cbSizeChanged)
                win32Window->m_cbSizeChanged(win32Window->m_size);

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

            if (win32Window->m_cbKey)
                win32Window->m_cbKey(key, static_cast<int32>(scanCode), action);

            win32Window->m_input->WindowFeedKey(key, static_cast<int32>(scanCode), action);

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

            win32Window->m_input->WindowFeedKey(key, static_cast<uint32>(scanCode), InputAction::Released);

            break;
        }
        case WM_MOUSEMOVE: {

            uint32 xPos = static_cast<uint32>(GET_X_LPARAM(lParam));
            uint32 yPos = static_cast<uint32>(GET_Y_LPARAM(lParam));

            win32Window->m_mousePosition = {xPos, yPos};
            win32Window->m_input->WindowFeedMousePosition(win32Window->m_mousePosition);

            if (win32Window->m_cbMouseMove)
                win32Window->m_cbMouseMove(win32Window->m_mousePosition);

            break;
        }
        case WM_MOUSEWHEEL: {

            auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (win32Window->m_cbMouseWheel)
                win32Window->m_cbMouseWheel(static_cast<int32>(delta));

            win32Window->m_input->WindowFeedMouseWheel(static_cast<int32>(delta));

            break;
        }
        case WM_LBUTTONDOWN: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_LBUTTON, InputAction::Pressed);

            win32Window->m_input->WindowFeedMouseButton(VK_LBUTTON, InputAction::Pressed);

            break;
        }
        case WM_RBUTTONDOWN: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_RBUTTON, InputAction::Pressed);

            win32Window->m_input->WindowFeedMouseButton(VK_RBUTTON, InputAction::Pressed);

            break;
        }
        case WM_MBUTTONDOWN: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_MBUTTON, InputAction::Pressed);

            win32Window->m_input->WindowFeedMouseButton(VK_MBUTTON, InputAction::Pressed);

            break;
        }
        case WM_LBUTTONUP: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_LBUTTON, InputAction::Released);

            win32Window->m_input->WindowFeedMouseButton(VK_LBUTTON, InputAction::Released);

            break;
        }
        case WM_RBUTTONUP: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_RBUTTON, InputAction::Released);

            win32Window->m_input->WindowFeedMouseButton(VK_RBUTTON, InputAction::Released);

            break;
        }
        case WM_MBUTTONUP: {

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_RBUTTON, InputAction::Released);

            win32Window->m_input->WindowFeedMouseButton(VK_MBUTTON, InputAction::Released);

            break;
        }
        }

        return DefWindowProcA(window, msg, wParam, lParam);
    }

    bool Win32Window::Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style)
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
        m_trueSize.x       = static_cast<uint32>(adjustedWidth);
        m_trueSize.y       = static_cast<uint32>(adjustedHeight);
        m_size.x           = static_cast<uint32>(width);
        m_size.y           = static_cast<uint32>(height);

        m_hwnd      = CreateWindowExA(exStyle, title, title, stylew, x, y, adjustedWidth, adjustedHeight, NULL, NULL, m_hinst, NULL);
        m_isVisible = true;
        m_title     = title;
        ShowWindow(m_hwnd, SW_SHOW);

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

    void Win32Window::OnDPIChanged(uint32 dpi)
    {
        m_dpi      = GetDpiForWindow(m_hwnd);
        m_dpiScale = m_dpi / 96.0f;
    }

    void Win32Window::SetStyle(WindowStyle style)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, GetStyle(style));
    }

    void Win32Window::SetCursorType(CursorType type)
    {
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
        SetWindowPos(m_hwnd, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void Win32Window::SetSize(const LGXVector2ui& size)
    {
        m_size          = size;
        RECT windowRect = {0, 0, static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y)};
        AdjustWindowRect(&windowRect, GetWindowLong(m_hwnd, GWL_STYLE), FALSE);
        int adjustedWidth  = windowRect.right - windowRect.left;
        int adjustedHeight = windowRect.bottom - windowRect.top;
        m_trueSize.x       = static_cast<uint32>(adjustedWidth);
        m_trueSize.y       = static_cast<uint32>(adjustedHeight);
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

        int xPos = (monitorWidth - static_cast<int>(m_trueSize.x)) / 2;
        int yPos = (monitorHeight - static_cast<int>(m_trueSize.y)) / 2;

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

    void Win32Window::SetVisible(bool isVisible)
    {
        m_isVisible = isVisible;
        ShowWindow(m_hwnd, m_isVisible ? SW_SHOW : SW_HIDE);
    }

    MonitorInfo Win32Window::GetMonitorInfoFromWindow()
    {
        MonitorInfo info;

        HMONITOR      monitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(monitor, &monitorInfo);

        UINT    dpiX, dpiY;
        HRESULT temp2 = GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

        info.size          = {static_cast<uint32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left), static_cast<uint32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top)};
        info.workArea      = {static_cast<uint32>(monitorInfo.rcWork.right - monitorInfo.rcWork.left), static_cast<uint32>(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top)};
        info.workTopLeft   = {static_cast<int32>(monitorInfo.rcWork.left), static_cast<int32>(monitorInfo.rcWork.top)};
        info.isPrimary     = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
        info.monitorHandle = static_cast<void*>(monitor);
        info.dpi           = dpiX;
        info.dpiScale      = static_cast<float>(dpiX) / 96.0f;
        return info;
    }

    LGXVector2ui Win32Window::GetMonitorWorkArea()
    {
        HMONITOR    hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(hMonitor, &monitorInfo);
        LGXVector2ui ret;
        ret.x = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        ret.y = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
        return ret;
    }

    LGXVector2ui Win32Window::GetMonitorSize()
    {
        HMONITOR    hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(hMonitor, &monitorInfo);
        LGXVector2ui ret;
        ret.x = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        ret.y = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
        return ret;
    }

} // namespace LinaGX
