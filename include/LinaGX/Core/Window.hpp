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

#ifndef LINAGX_WINDOW_HPP
#define LINAGX_WINDOW_HPP

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
        ///
        /// </summary>
        /// <param name="style"></param>
        virtual void SetStyle(WindowStyle style) = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="type"></param>
        virtual void SetCursorType(CursorType type) = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        virtual void SetPosition(const LGXVector2i& pos) = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        virtual void SetSize(const LGXVector2ui& size) = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void CenterPositionToCurrentMonitor() = 0;

        /// <summary>
        ///
        /// </summary>
        virtual void SetFullscreen() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        virtual void SetVisible(bool isVisible) = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="alpha"></param>
        virtual void SetAlpha(float alpha) = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="str"></param>
        virtual void SetTitle(const LINAGX_STRING& str) = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="isDragged"></param>
        virtual void SetForceIsDragged(bool isDragged, const LGXVector2ui& offset) = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="name"></param>
        virtual void SetIcon(const LINAGX_STRING& name) = 0;

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
        /// <returns></returns>
        virtual MonitorInfo GetMonitorInfoFromWindow() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        virtual LGXVector2ui GetMonitorWorkArea() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        virtual LGXVector2ui GetMonitorSize() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        virtual void* GetWindowHandle() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        virtual void* GetOSHandle() = 0;

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        virtual bool GetIsMaximized() = 0;

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

        inline void SetCallbackFocus(CallbackFocus&& cb)
        {
            m_cbFocus = cb;
        }

        inline void SetCallbackDragBegin(CallbackDragBegin&& cb)
        {
            m_cbDragBegin = cb;
        }

        inline void SetCallbackDragEnd(CallbackDragEnd&& cb)
        {
            m_cbDragEnd = cb;
        }

        inline void SetCallbackHoverBegin(CallbackHoverBegin&& cb)
        {
            m_cbHoverBegin = cb;
        }

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

        inline void SetDragRect(const LinaGX::LGXRectui& rect)
        {
            m_dragRect = rect;
        }

        inline bool GetIsDragged() const
        {
            return m_isDragged;
        }

        inline bool GetIsHovered() const
        {
            return m_isHovered;
        }

        inline void AddSizeRequest(const LGXVector2ui& req)
        {
            m_sizeRequests.push_back(req);
        }

        inline const LGXVector2i& GetMouseDelta() const
        {
            return m_mouseDelta;
        }

        inline const LGXRectui& GetDragRect() const
        {
            return m_dragRect;
        }

        inline void SetMaximizeToWorkArea(bool max)
        {
            m_maximizeToWorkArea = max;
        }

    protected:
        friend class WindowManager;
        Window(Input* input)
            : m_input(input){};
        virtual ~Window() = default;

        virtual bool Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent) = 0;
        virtual void Destroy()                                                                                                                        = 0;
        virtual void PreTick()                                                                                                                        = 0;
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
        CallbackDragBegin   m_cbDragBegin   = nullptr;
        CallbackDragEnd     m_cbDragEnd     = nullptr;

    protected:
        LINAGX_STRINGID          m_sid   = 0;
        Input*                   m_input = nullptr;
        LINAGX_STRING            m_title = "";
        LGXVector2i              m_position;
        LGXVector2ui             m_size;
        LGXVector2ui             m_trueSize;
        LGXVector2ui             m_mousePosition = {};
        LGXVector2i              m_mouseDelta    = {};
        LGXRectui                m_dragRect;
        uint32                   m_dpi                = 0;
        float                    m_dpiScale           = 0.0f;
        bool                     m_isVisible          = true;
        bool                     m_isTransparent      = false;
        float                    m_alpha              = 0.0f;
        bool                     m_hasFocus           = true;
        bool                     m_isDragged          = false;
        bool                     m_isHovered          = false;
        bool                     m_isInputPassThrough = false;
        bool                     m_maximizeToWorkArea = false;
        CursorType               m_cursorType         = CursorType::Default;
        LINAGX_VEC<LGXVector2ui> m_sizeRequests;
        WindowStyle              m_style = WindowStyle::Windowed;
    };
} // namespace LinaGX

#endif
