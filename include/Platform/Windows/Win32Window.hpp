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

#ifndef LINAGX_WIN32WINDOW_HPP
#define LINAGX_WIN32WINDOW_HPP

#include "Common/Common.hpp"

struct HWND__;
struct HINSTANCE__;

namespace LinaGX
{
    class WindowManager;

    class Win32Window
    {
    public:
        static __int64 __stdcall WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
        static LINAGX_MAP<HWND__*, Win32Window*> s_win32Windows;

        inline void* GetWindowHandle()
        {
            return static_cast<void*>(m_hwnd);
        }

        inline void* GetOSHandle()
        {
            return static_cast<void*>(m_hinst);
        }

        inline void SetCallbackClose(CallbackNoArg&& cb)
        {
            m_cbClose = cb;
        }

    private:
        friend class WindowManager;
        Win32Window()  = default;
        ~Win32Window() = default;

        bool Create(StringID stringID, const char* title, uint32 x, uint32 y, uint32 width, uint32 height, WindowStyle style);
        void Destroy();

    private:
        CallbackNoArg m_cbClose = nullptr;

    private:
        LINAGX_STRING m_title    = "";
        uint32        m_posX     = 0;
        uint32        m_posY     = 0;
        uint32        m_width    = 0;
        uint32        m_height   = 0;
        uint32        m_dpi      = 0;
        float         m_dpiScale = 0.0f;
        HWND__*       m_hwnd     = nullptr;
        HINSTANCE__*  m_hinst    = nullptr;
    };
} // namespace LinaGX

#endif
