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

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"

namespace LinaGX
{
    class WindowManager;
    class Input;

    class Window
    {
    public:
        /// <summary>
        /// Check WindowStyle enumeration for descriptions of different styles.
        /// </summary>
        virtual void SetStyle(WindowStyle style) = 0;

        /// <summary>
        /// Check CursorStyle enumeration for descriptions of different styles.
        /// </summary>
        virtual void SetCursorType(CursorType type) = 0;

        /// <summary>
        /// Place the window in a position in OS-space.
        /// </summary>
        virtual void SetPosition(const LGXVector2i& pos) = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void SetSize(const LGXVector2ui& size) = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void CenterPositionToCurrentMonitor() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void SetVisible(bool isVisible) = 0;

        /// <summary>
        /// Only supported on windows with BorderlessAlpha style.
        /// </summary>
        /// <param name="alpha"></param>
        virtual void SetAlpha(float alpha) = 0;

        /// <summary>
        /// Don't set too frequently, especially on Windows, performance may surprise you :)
        /// </summary>
        virtual void SetTitle(const LINAGX_STRING& str) = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void Close() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void BringToFront() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void Maximize() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void Minimize() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void Restore() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual MonitorInfo GetMonitorInfoFromWindow() = 0;

        /// <summary>
        /// Work area is the monitor area excluding the taskbar.
        /// </summary>
        virtual LGXVector2ui GetMonitorWorkArea() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual LGXVector2ui GetMonitorSize() = 0;

        /// <summary>
        /// HWND in Windows, NSWindow in MacOS.
        /// </summary>
        /// <returns></returns>
        virtual void* GetWindowHandle() = 0;

        /// <summary>
        /// HINSTANCE on Windows, NSView in MacOS.
        /// </summary>
        /// <returns></returns>
        virtual void* GetOSHandle() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        virtual bool GetIsMaximized() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        virtual bool GetIsMinimized() = 0;

        /// <summary>
        /// Restricts the mouse movement to the entire window.
        /// </summary>
        /// <returns></returns>
        virtual void ConfineMouse() = 0;

        /// <summary>
        /// Restricts the mouse movement for a specific local-region in the window.
        /// </summary>
        /// <returns></returns>
        virtual void ConfineMouseToRegion(const LGXRectui& region) = 0;

        /// <summary>
        /// Restricts the mouse movement to a small region in the center of the window.
        /// </summary>
        /// <returns></returns>
        virtual void ConfineMouseToCenter() = 0;

        /// <summary>
        /// Removes all confine restrictions.
        /// </summary>
        /// <returns></returns>
        virtual void FreeMouse() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void SetMouseVisible(bool visible) = 0;

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackClose(CallbackNoArg&& cb)
        {
            m_cbClose = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackPositionChanged(CallbackPosChanged&& cb)
        {
            m_cbPosChanged = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackSizeChanged(CallbackSizeChanged&& cb)
        {
            m_cbSizeChanged = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackKey(CallbackKey&& cb)
        {
            m_cbKey = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackMouse(CallbackMouse&& cb)
        {
            m_cbMouse = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackMouseWheel(CallbackMouseWheel&& cb)
        {
            m_cbMouseWheel = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackMouseMove(CallbackMouseMove&& cb)
        {
            m_cbMouseMove = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackFocus(CallbackFocus&& cb)
        {
            m_cbFocus = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackHoverBegin(CallbackHoverBegin&& cb)
        {
            m_cbHoverBegin = cb;
        }

        /// <summary>
        ///
        /// </summary>
        inline void SetCallbackHoverEnd(CallbackHoverEnd&& cb)
        {
            m_cbHoverEnd = cb;
        }

        inline const LGXVector2i& GetPosition() const
        {
            return m_position;
        }

        inline const LGXVector2ui& GetSize() const
        {
            return m_size;
        }

        inline LINAGX_STRINGID GetSID() const
        {
            return m_sid;
        }

        inline float GetDPIScale() const
        {
            return m_dpiScale;
        }

        inline uint32 GetDPI() const
        {
            return m_dpi;
        }

        inline const LGXVector2ui GetMousePosition() const
        {
            return m_mousePosition;
        }

        inline bool GetIsVisible() const
        {
            return m_isVisible;
        }

        inline const LINAGX_STRING& GetTitle() const
        {
            return m_title;
        }

        inline bool GetIsTransparent() const
        {
            return m_isTransparent;
        }

        inline bool HasFocus() const
        {
            return m_hasFocus;
        }

        /// <summary>
        /// Drag rectangle is the local-region of the window that the operating system dragging takes control on Borderless windows.
        /// </summary>
        inline void SetDragRect(const LinaGX::LGXRectui& rect)
        {
            m_dragRect = rect;
        }

        inline bool GetIsHovered() const
        {
            return m_isHovered;
        }

        /// <summary>
        /// Sometimes you don't want to size the window immediately but after the current frame has completed. Use this then.
        /// </summary>
        /// <param name="req"></param>
        inline void AddSizeRequest(const LGXVector2ui& req)
        {
            m_sizeRequests.push_back(req);
        }

        inline const LGXRectui& GetDragRect() const
        {
            return m_dragRect;
        }

        inline Input* GetInput() const
        {
            return m_input;
        }

    protected:
        friend class WindowManager;
        Window(Input* input)
            : m_input(input){};
        virtual ~Window() = default;

        virtual bool Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent) = 0;
        virtual void Destroy()                                                                                                                        = 0;
        virtual void Tick()                                                                                                                           = 0;

    protected:
        CallbackNoArg       m_cbClose       = nullptr;
        CallbackPosChanged  m_cbPosChanged  = nullptr;
        CallbackSizeChanged m_cbSizeChanged = nullptr;
        CallbackKey         m_cbKey         = nullptr;
        CallbackMouse       m_cbMouse       = nullptr;
        CallbackMouseWheel  m_cbMouseWheel  = nullptr;
        CallbackMouseMove   m_cbMouseMove   = nullptr;
        CallbackFocus       m_cbFocus       = nullptr;
        CallbackHoverBegin  m_cbHoverBegin  = nullptr;
        CallbackHoverEnd    m_cbHoverEnd    = nullptr;

    protected:
        LINAGX_STRINGID          m_sid   = 0;
        Input*                   m_input = nullptr;
        LINAGX_STRING            m_title = "";
        LGXVector2i              m_position;
        LGXVector2ui             m_size;
        LGXVector2ui             m_trueSize;
        LGXVector2ui             m_mousePosition = {};
        LGXRectui                m_dragRect;
        uint32                   m_dpi           = 0;
        float                    m_dpiScale      = 0.0f;
        bool                     m_isVisible     = true;
        bool                     m_isTransparent = false;
        float                    m_alpha         = 0.0f;
        bool                     m_hasFocus      = true;
        bool                     m_isHovered     = false;
        CursorType               m_cursorType    = CursorType::Default;
        LINAGX_VEC<LGXVector2ui> m_sizeRequests;
        WindowStyle              m_style = WindowStyle::WindowedApplication;
    };
} // namespace LinaGX
