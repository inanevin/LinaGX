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

    private:
        enum class ConfineStyle
        {
            None,
            Window,
            Region,
            Center,
        };

    public:
        static __int64 __stdcall WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
        static LINAGX_MAP<HWND__*, Win32Window*> s_win32Windows;

        virtual void Close() override;
        virtual void SetStyle(WindowStyle style) override;
        virtual void SetCursorType(CursorType type) override;
        virtual void SetPosition(const LGXVector2i& pos) override;
        virtual void SetSize(const LGXVector2ui& size) override;
        virtual void CenterPositionToCurrentMonitor() override;
        virtual void SetVisible(bool isVisible) override;
        virtual void BringToFront() override;
        virtual void SetAlpha(float alpha) override;
        virtual void SetTitle(const LINAGX_STRING& str) override;
        virtual void Maximize() override;
        virtual void Minimize() override;
        virtual void Restore() override;
        virtual bool GetIsMaximized() override;
        virtual bool GetIsMinimized() override;
        virtual void ConfineMouse() override;
        virtual void ConfineMouseToRegion(const LGXRectui& region) override;
        virtual void ConfineMouseToCenter() override;
        virtual void FreeMouse() override;
        virtual void SetMouseVisible(bool visible) override;
        virtual void SetIsFloating(bool isFloating) override;

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
        Win32Window(Input* input, WindowManager* wm)
            : Window(input, wm){};
        virtual ~Win32Window() = default;

        virtual bool Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent) override;
        virtual void Destroy() override;
        virtual void Tick() override;

    private:
        uint32 GetStyle(WindowStyle style);
        void   OnDPIChanged(uint32 dpi);
        void   CalculateMonitorInfo();

    private:
        HWND__*      m_hwnd                 = nullptr;
        HINSTANCE__* m_hinst                = nullptr;
        bool         m_titleChangeRequested = false;
        bool         m_markedDestroy        = false;
        LGXVector2i  m_restorePos           = {};
        LGXVector2ui m_restoreSize          = {};
        HICON__*     m_icon                 = nullptr;
        bool         m_isMaximizeFullscreen = false;
        Win32Window* m_parent               = nullptr;
        bool         m_setToFullscreen      = false;
        bool         m_mouseMoved           = false;
        ConfineStyle m_confineStyle         = ConfineStyle::None;
        LGXRectui    m_confineRect          = {};
        bool         m_hittestOwnsCursor    = false;
    };
} // namespace LinaGX

#endif
