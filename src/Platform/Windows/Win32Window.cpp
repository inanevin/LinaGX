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
#include <chrono>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace LinaGX
{
    LINAGX_MAP<HWND__*, Win32Window*> Win32Window::s_win32Windows;

    auto AdjustMaximizedClientRect(Win32Window* win32, HWND window, RECT& rect) -> void
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

        rect = monitor_info.rcWork;
    }

    auto composition_enabled() -> bool
    {
        BOOL composition_enabled = FALSE;
        bool success             = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
        return composition_enabled && success;
    }

    LRESULT HandleNonclientHitTest(HWND wnd, LPARAM lparam, const LGXRectui& dragRect)
    {
        RECT wnd_rect;
        GetWindowRect(wnd, &wnd_rect);

        int        width    = wnd_rect.right - wnd_rect.left;
        int        height   = wnd_rect.bottom - wnd_rect.top;
        const LONG margin   = 10;
        const LONG trMargin = 5;

        RECT title_bar = {dragRect.pos.x, dragRect.pos.y, dragRect.pos.x + dragRect.size.x, dragRect.pos.y + dragRect.size.y};
        RECT tl        = {0, 0, margin, margin};
        RECT tr        = {width - trMargin, 0, width, trMargin};
        RECT bl        = {0, height - margin, margin, height};
        RECT br        = {width - margin, height - margin, width, height};
        RECT left      = {0, tl.bottom, margin, bl.top};
        RECT right     = {width - margin, tr.bottom, width, br.top};
        RECT bottom    = {bl.right, bl.top, br.left, br.bottom};
        RECT top       = {tl.right, tl.top, tr.left, tr.bottom};

        // std::tuple<RECT, LRESULT> rects[] = {{title_bar, HTCAPTION}, {left, HTLEFT}, {right, HTRIGHT}, {bottom, HTBOTTOM}};
        std::tuple<RECT, LRESULT> rects[] = {{tl, HTTOPLEFT}, {tr, HTTOPRIGHT}, {bl, HTBOTTOMLEFT}, {br, HTBOTTOMRIGHT}, {left, HTLEFT}, {right, HTRIGHT}, {bottom, HTBOTTOM}, {top, HTTOP}};

        POINT pt = {GET_X_LPARAM(lparam) - wnd_rect.left, GET_Y_LPARAM(lparam) - wnd_rect.top};

        for (const auto& [r, code] : rects)
        {
            if (PtInRect(&r, pt))
                return code;
        }
        return HTCLIENT;
    }

    LRESULT HandleNonclientHitTest2(POINT cursor, HWND wnd)
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

        const auto drag = HTCAPTION;

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

        switch (result)
        {
        case left:
            return borderless_resize ? HTLEFT : drag;
        case right:
            return borderless_resize ? HTRIGHT : drag;
        case top:
            return borderless_resize ? HTTOP : drag;
        case bottom:
            return borderless_resize ? HTBOTTOM : drag;
        case top | left:
            return borderless_resize ? HTTOPLEFT : drag;
        case top | right:
            return borderless_resize ? HTTOPRIGHT : drag;
        case bottom | left:
            return borderless_resize ? HTBOTTOMLEFT : drag;
        case bottom | right:
            return borderless_resize ? HTBOTTOMRIGHT : drag;
        case client:
            return HTCLIENT;
        default:
            return HTNOWHERE;
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

        switch (msg)
        {

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
            if (win32Window->m_cbClose != nullptr)
                win32Window->m_cbClose();

            return 0;
        }
            // case WM_NCHITTEST: {
            //     // When we have no border or title bar, we need to perform our
            //     // own hit testing to allow resizing and moving.
            //     if (win32Window->m_style == WindowStyle::Borderless)
            //     {
            //         return HandleNonclientHitTest2(POINT{
            //                                            GET_X_LPARAM(lParam),
            //                                            GET_Y_LPARAM(lParam)},
            //                                        hwnd);
            //     }
            //     break;
            // }
        case WM_NCHITTEST: {
            if (win32Window->m_style == WindowStyle::Borderless)
            {
                auto res = HandleNonclientHitTest(hwnd, lParam, win32Window->m_dragRect);
                if (res == HTCLIENT)
                {
                    win32Window->SetCursorType(win32Window->m_cursorType);
                }

                return res;
            }

            break;
        }
        case WM_NCCALCSIZE: {
            if (wParam == TRUE)
            {
                auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                AdjustMaximizedClientRect(win32Window, hwnd, params.rgrc[0]);
                return 0;
            }
            break;
        }
        case WM_KILLFOCUS: {

            if (!win32Window->m_markedDestroy)
            {
                if (win32Window->m_cbFocus != nullptr)
                    win32Window->m_cbFocus(false);

                win32Window->m_hasFocus = false;
            }

            break;
        }
        case WM_SETFOCUS: {
            if (win32Window->m_cbFocus != nullptr)
                win32Window->m_cbFocus(true);

            win32Window->m_hasFocus = true;

            break;
        }
        case WM_WINDOWPOSCHANGED: {
            break;
        }
        case WM_DISPLAYCHANGE: {
            break;
        }
        case WM_DESTROY: {
            if (win32Window->m_icon)
                DestroyIcon(win32Window->m_icon);
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

            const LGXVector2i mp    = {x, y};
            win32Window->m_position = mp;

            if (win32Window->m_cbPosChanged)
                win32Window->m_cbPosChanged(win32Window->m_position);

            break;
        }
        case WM_SIZE: {

            UINT width  = LOWORD(lParam);
            UINT height = HIWORD(lParam);

            RECT rect;
            GetWindowRect(hwnd, &rect);

            RECT clientRect;
            GetClientRect(win32Window->m_hwnd, &clientRect);

            win32Window->m_size.x = clientRect.right - clientRect.left;
            win32Window->m_size.y = clientRect.bottom - clientRect.top;

            if (win32Window->m_style == WindowStyle::Windowed)
            {
                win32Window->m_trueSize.x = rect.right - rect.left;
                win32Window->m_trueSize.y = rect.bottom - rect.top;
            }
            else
            {
                win32Window->m_trueSize = win32Window->m_size;
            }

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

            if (key == VK_SPACE)
            {
                win32Window->Minimize();
            }
            if (win32Window->m_cbKey)
                win32Window->m_cbKey(key, static_cast<int32>(scanCode), action, win32Window);

            win32Window->m_input->WindowFeedKey(key, static_cast<int32>(scanCode), action, win32Window);

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
                win32Window->m_cbKey(key, static_cast<uint32>(scanCode), InputAction::Released, win32Window);

            win32Window->m_input->WindowFeedKey(key, static_cast<uint32>(scanCode), InputAction::Released, win32Window);

            break;
        }
        case WM_MOUSEMOVE: {

            uint32 xPos = static_cast<uint32>(GET_X_LPARAM(lParam));
            uint32 yPos = static_cast<uint32>(GET_Y_LPARAM(lParam));

            const LGXVector2ui mp        = {xPos, yPos};
            win32Window->m_mouseDelta    = {static_cast<int32>(mp.x - win32Window->m_mousePosition.x), static_cast<int32>(mp.y - win32Window->m_mousePosition.y)};
            win32Window->m_mousePosition = mp;
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

            static std::chrono::steady_clock::time_point lastTime = {};
            auto                                         current  = std::chrono::high_resolution_clock::now();
            auto                                         elapsed  = std::chrono::duration_cast<std::chrono::nanoseconds>(current - lastTime);
            lastTime                                              = current;

            bool repeated = elapsed.count() < 250000000;

            if (win32Window->m_cbMouse)
                win32Window->m_cbMouse(VK_LBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed);

            win32Window->m_input->WindowFeedMouseButton(VK_LBUTTON, repeated ? InputAction::Repeated : InputAction::Pressed);

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
            wc.style         = CS_HREDRAW | CS_VREDRAW;

            // wc.style         = CS_DBLCLKS;

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

        if (style == WindowStyle::Borderless)
        {
            m_trueSize.x = width;
            m_trueSize.y = height;
        }
        else
        {
            int adjustedWidth  = windowRect.right - windowRect.left;
            int adjustedHeight = windowRect.bottom - windowRect.top;
            m_trueSize.x       = static_cast<uint32>(adjustedWidth);
            m_trueSize.y       = static_cast<uint32>(adjustedHeight);
        }

        Win32Window* parentWindow = nullptr;

        if (parent != nullptr)
        {
            parentWindow = static_cast<Win32Window*>(parent);
            exStyle |= WS_EX_LAYERED;
        }

        m_hwnd = CreateWindowExA(exStyle, title, title, stylew, x, y, m_trueSize.x, m_trueSize.y, parentWindow ? parentWindow->m_hwnd : NULL, NULL, m_hinst, this);

        if (m_hwnd == nullptr)
        {
            LOGE("Window -> Failed creating window!");
            return false;
        }

        m_size.x        = static_cast<uint32>(width);
        m_size.y        = static_cast<uint32>(height);
        m_title         = title;
        m_style         = style;
        m_dpi           = GetDpiForWindow(m_hwnd);
        m_dpiScale      = m_dpi / 96.0f;
        m_restoreSize   = m_trueSize;
        m_restorePos    = m_position;
        SetFocus(m_hwnd);
        BringToFront();
        SetStyle(style);
        SetVisible(true);

        return true;
    }

    void Win32Window::Destroy()
    {
        m_markedDestroy = true;
        DestroyWindow(m_hwnd);
    }

    void Win32Window::PreTick()
    {
        // Reset as this will be called pre-pump messages
        m_mouseDelta = {0, 0};
    }

    void Win32Window::Tick()
    {
        if (!m_sizeRequests.empty())
        {
            SetSize(*m_sizeRequests.end());
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

            if (m_cbHoverBegin)
                m_cbHoverBegin();
        }
        else if (m_isHovered && !isInside)
        {
            m_isHovered = false;

            if (m_cbHoverEnd)
                m_cbHoverEnd();
        }

        // Custom dragging by drag rect.
        {
            if (!m_isDragged)
            {
                if (m_input->GetMouseButtonDown(VK_LBUTTON) && m_dragRect.IsPointInside(m_mousePosition) && m_hasFocus)
                {
                    m_isDragged      = true;
                    m_dragMouseDelta = m_mousePosition;

                    if (m_cbDragBegin)
                        m_cbDragBegin();
                }
            }

            if (m_isDragged)
            {
                if (!m_input->GetMouseButton(VK_LBUTTON))
                {
                    m_isDragged = false;

                    if (m_cbDragEnd)
                        m_cbDragEnd();

                    return;
                }

                const LGXVector2i absMouse  = m_input->GetMousePositionAbs();
                const LGXVector2i targetPos = {absMouse.x - static_cast<int32>(m_dragMouseDelta.x), absMouse.y - static_cast<int32>(m_dragMouseDelta.y)};
                SetPosition(targetPos);
            }
        }
    }

    uint32 Win32Window::GetStyle(WindowStyle style)
    {
        // THICKFRAME: aero snap
        // CAPTION: transitions
        if (style == WindowStyle::Windowed)
            return static_cast<uint32>(WS_OVERLAPPEDWINDOW);
        else
        {
            if (composition_enabled())
                return WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
            else
                return WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
        }
    }

    void Win32Window::OnDPIChanged(uint32 dpi)
    {
        m_dpi      = GetDpiForWindow(m_hwnd);
        m_dpiScale = m_dpi / 96.0f;
    }

    void Win32Window::Close()
    {
        if (m_cbClose != nullptr)
            m_cbClose();

        CloseWindow(m_hwnd);
    }

    void Win32Window::SetStyle(WindowStyle style)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, GetStyle(style));
        ::SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        //::ShowWindow(m_hwnd, SW_SHOW);
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
        SetWindowPos(m_hwnd, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    void Win32Window::SetSize(const LGXVector2ui& size)
    {
        m_size = size;

        if (m_style == WindowStyle::Borderless)
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

    void Win32Window::SetFullscreen()
    {
        SetWindowLong(m_hwnd, GWL_STYLE, WS_POPUPWINDOW | WS_VISIBLE);
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

    void Win32Window::BringToFront()
    {
        OpenIcon(m_hwnd);
        SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    void Win32Window::SetAlpha(float alpha)
    {
        const BYTE     finalAlpha = static_cast<BYTE>(alpha * 255.0f);
        const COLORREF colorKey   = RGB(1, 1, 1);
        SetLayeredWindowAttributes(m_hwnd, colorKey, finalAlpha, LWA_ALPHA);
        m_isTransparent = alpha < 0.99f;
    }

    void Win32Window::SetTitle(const LINAGX_STRING& str)
    {
        m_title                = str;
        m_titleChangeRequested = true;
    }

    void Win32Window::SetForceIsDragged(bool isDragged, const LGXVector2ui& offset)
    {
        m_isDragged = true;

        if (m_isDragged)
        {
            m_isDragged      = true;
            m_dragMouseDelta = offset;

            if (m_cbDragBegin)
                m_cbDragBegin();
        }
        else
        {
            if (m_cbDragEnd)
                m_cbDragEnd();
        }
    }

    void Win32Window::SetIcon(const LINAGX_STRING& name)
    {
        if (m_icon)
            DestroyIcon(m_icon);

        const LINAGX_STRING fullName = name + ".ico";
        m_icon                       = (HICON)LoadImage(NULL, fullName.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_icon);
        SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_icon);
    }

    void Win32Window::Maximize()
    {
        if (!GetIsMaximized())
            ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
        else
            ShowWindow(m_hwnd, SW_RESTORE);
    }

    void Win32Window::Minimize()
    {
        ShowWindow(m_hwnd, SW_SHOWMINIMIZED);
    }

    void Win32Window::Restore()
    {
        if (!GetIsMaximized())
            ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
        else
            ShowWindow(m_hwnd, SW_RESTORE);
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

    bool Win32Window::GetIsMaximized()
    {
        WINDOWPLACEMENT placement;
        if (!::GetWindowPlacement(m_hwnd, &placement))
            return false;

        return placement.showCmd == SW_MAXIMIZE;
    }

} // namespace LinaGX
