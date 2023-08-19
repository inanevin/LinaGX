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
struct HICON__;

namespace LinaGX
{
    class Win32Window : public Window
    {
    public:
        static __int64 __stdcall WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
        static LINAGX_MAP<HWND__*, Win32Window*> s_win32Windows;

        virtual void         Close() override;
        virtual void         SetStyle(WindowStyle style) override;
        virtual void         SetCursorType(CursorType type) override;
        virtual void         SetPosition(const LGXVector2i& pos) override;
        virtual void         SetSize(const LGXVector2ui& size) override;
        virtual void         CenterPositionToCurrentMonitor() override;
        virtual void         SetVisible(bool isVisible) override;
        virtual void         BringToFront() override;
        virtual void         SetAlpha(float alpha) override;
        virtual void         SetTitle(const LINAGX_STRING& str) override;
        virtual void         SetForceIsDragged(bool isDragged, const LGXVector2ui& offset) override;
        virtual void         SetIcon(const LINAGX_STRING& name) override;
        virtual void         Maximize() override;
        virtual void         Minimize() override;
        virtual void         Restore() override;
        virtual MonitorInfo  GetMonitorInfoFromWindow() override;
        virtual LGXVector2ui GetMonitorWorkArea() override;
        virtual LGXVector2ui GetMonitorSize() override;
        virtual bool         GetIsMaximized() override;
        virtual bool         GetIsMinimized() override;

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

        virtual bool Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent) override;
        virtual void Destroy() override;
        virtual void PreTick() override;
        virtual void Tick() override;

    private:
        uint32 GetStyle(WindowStyle style);
        void   OnDPIChanged(uint32 dpi);
        void   OnDrag(bool begin);

    private:
        HWND__*      m_hwnd                 = nullptr;
        HINSTANCE__* m_hinst                = nullptr;
        bool         m_titleChangeRequested = false;
        bool         m_markedDestroy        = false;
        LGXVector2ui m_dragMouseDelta       = {};
        LGXVector2i  m_restorePos           = {};
        LGXVector2ui m_restoreSize          = {};
        HICON__*     m_icon                 = nullptr;
        bool         m_isMaximizeFullscreen = false;
        Win32Window* m_parent               = nullptr;
        bool         m_setToFullscreen      = false;
    };
} // namespace LinaGX

#endif
