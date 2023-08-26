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

#ifndef LINAGX_OSXWINDOW_HPP
#define LINAGX_OSXWINDOW_HPP

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"
#include "LinaGX/Core/Window.hpp"


namespace LinaGX
{
    class OSXWindow : public Window
    {
    public:

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
            return m_nsWindow;
        }

        virtual void* GetOSHandle() override
        {
            return m_nsView;
        }

    protected:
        friend class WindowManager;
        OSXWindow(Input* input)
            : Window(input){};
        virtual ~OSXWindow() = default;

        virtual bool Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent) override;
        virtual void Destroy() override;
        virtual void PreTick() override;
        virtual void Tick() override;
        
    private:
        
        float ConvertY(float desiredY);
        uint32 ConvertYInt(uint32 desiredY);
        void CalculateDPI();
        uint32 GetStyle(WindowStyle style);

    private:
        bool         m_titleChangeRequested = false;
        OSXWindow*   m_parent               = nullptr;
        void* m_nsWindow = nullptr;
        void* m_nsView = nullptr;
        bool m_lmDownForDrag = false;
        LGXVector2ui m_lmDragDelta = {};
        bool m_mouseMoved = false;
    };
} // namespace LinaGX

#endif
