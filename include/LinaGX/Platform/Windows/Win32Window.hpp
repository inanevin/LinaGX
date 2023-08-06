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
#include "LinaGX/Core/Window.hpp"

struct HWND__;
struct HINSTANCE__;

namespace LinaGX
{
    class Win32Window : public Window
    {
    public:
        static __int64 __stdcall WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
        static LINAGX_MAP<HWND__*, Win32Window*> s_win32Windows;

        virtual void SetStyle(WindowStyle style) override;
        virtual void SetCursorType(CursorType type) override;

        virtual void         SetPosition(const LGXVector2i& pos);
        virtual void         SetSize(const LGXVector2ui& size);
        virtual void         CenterPositionToCurrentMonitor();
        virtual void         SetFullscreen();
        virtual void         SetVisible(bool isVisible);
        virtual MonitorInfo  GetMonitorInfoFromWindow();
        virtual LGXVector2ui GetMonitorWorkArea() override;
        virtual LGXVector2ui GetMonitorSize();

        virtual void* GetWindowHandle() override
        {
            return static_cast<void*>(m_hwnd);
        }

        virtual void* GetOSHandle() override
        {
            return static_cast<void*>(m_hinst);
        }

    protected:
        friend class WindowManager;
        Win32Window(Input* input)
            : Window(input){};
        virtual ~Win32Window() = default;

        virtual bool Create(LINAGX_STRINGID sid, const char* title, uint32 x, uint32 y, uint32 width, uint32 height, WindowStyle style) override;
        virtual void Destroy() override;

    private:
        uint32 GetStyle(WindowStyle style);
        void   OnDPIChanged(uint32 dpi);

    private:
        HWND__*      m_hwnd  = nullptr;
        HINSTANCE__* m_hinst = nullptr;
    };
} // namespace LinaGX

#endif
