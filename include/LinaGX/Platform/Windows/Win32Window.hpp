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

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"

struct HWND__;
struct HINSTANCE__;

namespace LinaGX
{
    class WindowManager;
    class Input;

    class Win32Window
    {
    public:
        static __int64 __stdcall WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
        static LINAGX_MAP<HWND__*, Win32Window*> s_win32Windows;

        /// <summary>
        ///
        /// </summary>
        /// <param name="style"></param>
        void SetStyle(WindowStyle style);

        /// <summary>
        ///
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        LGXVector2ui GetMonitorWorkArea();

        /// <summary>
        ///
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        LGXVector2ui GetMonitorSize();

        /// <summary>
        ///
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetPosition(const LGXVector2i& pos);

        /// <summary>
        ///
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        void SetSize(const LGXVector2ui& size);

        /// <summary>
        ///
        /// </summary>
        void CenterPositionToCurrentMonitor();

        /// <summary>
        ///
        /// </summary>
        void SetFullscreen();

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        void SetVisible(bool isVisible);

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

        inline void SetCallbackPositionChanged(CallbackPosChanged&& cb)
        {
            m_cbPosChanged = cb;
        }

        inline void SetCallbackSizeChanged(CallbackSizeChanged&& cb)
        {
            m_cbSizeChanged = cb;
        }

        inline void SetCallbackKey(CallbackKey&& cb)
        {
            m_cbKey = cb;
        }

        inline void SetCallbackMouse(CallbackMouse&& cb)
        {
            m_cbMouse = cb;
        }

        inline void SetCallbackMouseWheel(CallbackMouseWheel&& cb)
        {
            m_cbMouseWheel = cb;
        }

        inline void SetCallbackMouseMove(CallbackMouseMove&& cb)
        {
            m_cbMouseMove = cb;
        }

        inline LGXVector2i GetPosition()
        {
            return m_position;
        }

        inline LGXVector2ui GetSize()
        {
            return m_size;
        }

    private:
        friend class WindowManager;
        Win32Window(Input* input)
            : m_input(input){};
        ~Win32Window() = default;

        bool Create(StringID stringID, const char* title, uint32 x, uint32 y, uint32 width, uint32 height, WindowStyle style);
        void Destroy();

        uint32 GetStyle(WindowStyle style);
        void   OnDPIChanged(uint32 dpi);

    private:
        CallbackNoArg       m_cbClose       = nullptr;
        CallbackPosChanged  m_cbPosChanged  = nullptr;
        CallbackSizeChanged m_cbSizeChanged = nullptr;
        CallbackKey         m_cbKey         = nullptr;
        CallbackMouse       m_cbMouse       = nullptr;
        CallbackMouseWheel  m_cbMouseWheel  = nullptr;
        CallbackMouseMove   m_cbMouseMove   = nullptr;

    private:
        /* Window Interface */
        Input*        m_input = nullptr;
        LINAGX_STRING m_title = "";
        LGXVector2i   m_position;
        LGXVector2ui  m_size;
        LGXVector2ui  m_trueSize;
        LGXVector2ui  m_mousePosition = {};
        uint32        m_dpi           = 0;
        float         m_dpiScale      = 0.0f;
        bool          m_isVisible     = true;
        /* Window Interface */

        HWND__*      m_hwnd  = nullptr;
        HINSTANCE__* m_hinst = nullptr;
    };
} // namespace LinaGX

#endif
