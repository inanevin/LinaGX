/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2023-] [Inan Evin]

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
#ifndef LINAGX_INPUT_HPP
#define LINAGX_INPUT_HPP

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"

namespace LinaGX
{

#define NUM_KEY_STATES   380
#define NUM_MOUSE_STATES 8

#ifdef LINAGX_PLATFORM_WINDOWS
    class Win32Window;
#elif LINAGX_PLATFORM_APPLE
    class AppleWindow;
#endif

    class Input
    {
    public:
        /// <summary>
        ///
        /// </summary>
        /// <param name="ch"></param>
        /// <returns></returns>
        uint16 GetCharacterMask(wchar_t ch);

        /// <summary>
        ///
        /// </summary>
        /// <param name="c"></param>
        /// <returns></returns>
        uint32 GetKeycode(char32_t c);

        /// <summary>
        ///
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        wchar_t GetCharacterFromKey(uint32 key);

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        bool IsControlPressed();

        /// <summary>
        ///
        /// </summary>
        /// <param name="point"></param>
        /// <param name="rect"></param>
        /// <returns></returns>
        bool IsPointInRect(const LGXVector2ui& point, const LGXRectui& rect);

        /// <summary>
        ///
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        bool GetKey(int button);

        /// <summary>
        ///
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        bool GetKeyDown(int button);

        /// <summary>
        ///
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        bool GetKeyUp(int button);

        /// <summary>
        ///
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        bool GetMouseButton(int button);

        /// <summary>
        ///
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        bool GetMouseButtonDown(int button);

        /// <summary>
        ///
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        bool GetMouseButtonUp(int button);

        /// <summary>
        ///
        /// </summary>
        /// <param name="mode"></param>
        void SetCursorMode(CursorMode mode);

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        inline LGXVector2i GetMousePositionAbs()
        {
            return m_currentMousePositionAbs;
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        inline LGXVector2i GetMouseDelta()
        {
            return m_mouseDelta;
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        inline LGXVector2i GetMouseDeltaRaw()
        {
            return m_mouseDeltaRaw;
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        inline int32 GetMouseScroll()
        {
            return m_mouseScroll;
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        inline CursorMode GetCursorMode()
        {
            return m_cursorMode;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="cb"></param>
        inline void SetCallbackKey(CallbackKey&& cb)
        {
            m_cbKey = cb;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="cb"></param>
        inline void SetCallbackMouse(CallbackMouse&& cb)
        {
            m_cbMouse = cb;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="cb"></param>
        inline void SetCallbackMouseWheel(CallbackMouseWheel&& cb)
        {
            m_cbMouseWheel = cb;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="cb"></param>
        inline void SetCallbackMouseMove(CallbackMouseMove&& cb)
        {
            m_cbMouseMove = cb;
        }

    private:
        friend class WindowManager;
        friend class Instance;

        Input()  = default;
        ~Input() = default;

#ifdef LINAGX_PLATFORM_WINDOWS
        friend class Win32Window;
#elif LINAGX_PLATFORM_APPLE
        friend class AppleWindow;
#endif

        void PreTick();
        void Tick();
        void WindowFeedKey(uint32 key, int32 scanCode, InputAction action);
        void WindowFeedMouseButton(uint32 button, InputAction action);
        void WindowFeedMousePosition(const LGXVector2ui& pos);
        void WindowFeedActivateApp(bool activate);
        void WindowFeedMouseWheel(int32 delta);

    private:
        int         m_keyStatesDown[NUM_KEY_STATES]     = {0};
        int         m_keyStatesUp[NUM_KEY_STATES]       = {0};
        int         m_mouseStatesDown[NUM_MOUSE_STATES] = {0};
        int         m_mouseStatesUp[NUM_MOUSE_STATES]   = {0};
        bool        m_appActive                         = true;
        CursorMode  m_cursorMode                        = CursorMode::Visible;
        LGXVector2i m_currentMousePositionAbs           = {0, 0};
        LGXVector2i m_previousMousePosition             = {0, 0};
        LGXVector2i m_mouseDelta                        = {0, 0};
        LGXVector2i m_mouseDeltaRaw                     = {0, 0};
        int32       m_mouseScroll                       = 0;
        bool        m_currentStates[256]                = {0};
        bool        m_previousStates[256]               = {0};
        LGXVector2i m_mousePosTrackingClick             = {};

        CallbackKey        m_cbKey        = nullptr;
        CallbackMouse      m_cbMouse      = nullptr;
        CallbackMouseWheel m_cbMouseWheel = nullptr;
        CallbackMouseMove  m_cbMouseMove  = nullptr;
    };
} // namespace LinaGX

#endif
