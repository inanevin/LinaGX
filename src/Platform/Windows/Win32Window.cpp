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

#include "Platform/Windows/Win32Window.hpp"
#include <Windows.h>

namespace LinaGX
{
    LRESULT __stdcall Win32Window::WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam)
    {
        switch (msg)
        {
        case WM_CLOSE:
            return 0;
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
            break;
        }
        case WM_SIZE: {
            break;
        }
        case WM_KEYDOWN: {
            break;
        }
        case WM_KEYUP: {
            break;
        }
        case WM_MOUSEMOVE: {
            break;
        }
        case WM_MOUSEWHEEL: {
            break;
        }
        case WM_LBUTTONDOWN: {
            break;
        }
        case WM_RBUTTONDOWN: {

            break;
        }
        case WM_MBUTTONDOWN: {
            break;
        }
        case WM_LBUTTONUP: {

            break;
        }
        case WM_RBUTTONUP: {
            break;
        }
        case WM_MBUTTONUP: {
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
        DWORD stylew  = WS_OVERLAPPEDWINDOW;

        RECT  windowRect  = {0, 0, width, height};
        DWORD windowStyle = WS_OVERLAPPEDWINDOW;
        AdjustWindowRect(&windowRect, windowStyle, FALSE);

        int adjustedWidth  = windowRect.right - windowRect.left;
        int adjustedHeight = windowRect.bottom - windowRect.top;

        m_window = CreateWindowExA(exStyle, title, title, stylew, x, y, adjustedWidth, adjustedHeight, NULL, NULL, m_hinst, NULL);
        ShowWindow(m_window, SW_SHOW);
        m_title = title;

        if (m_window == nullptr)
        {
            LOGE("Window -> Failed creating window!");
            return false;
        }

        m_dpi      = GetDpiForWindow(m_window);
        m_dpiScale = m_dpi / 96.0f;
        return true;
    }

    void Win32Window::Destroy()
    {
        DestroyWindow(m_window);
    }
} // namespace LinaGX
