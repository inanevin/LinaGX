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

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"

namespace LinaGX
{

#define NUM_KEY_STATES   380
#define NUM_MOUSE_STATES 8

    class Window;

#ifdef LINAGX_PLATFORM_WINDOWS
    class Win32Window;
#elif LINAGX_PLATFORM_APPLE
    class OSXWindow;
#endif

    class Input
    {
    public:
        /// <summary>
        /// Returns 16 bit unsigned integer representing LinaGX::CharacterMask for the given wide-character.
        /// </summary>
        uint16 GetCharacterMask(wchar_t ch);

        /// <summary>
        /// 32-bit unsigned integer code for the given 32-bit character.
        /// </summary>
        uint32 GetKeycode(char32_t c);

        /// <summary>
        /// Tries to find the wide-character representation of the given keycode.
        /// </summary>
        wchar_t GetCharacterFromKey(uint32 key);

        /// <summary>
        /// This is NOT only CTRL keys, it's all "control" keys, e.g. CTRLs, ALTs, TAB and CAPSLOCK.
        /// </summary>
        bool IsControlPressed();

        /// <summary>
        /// True if given key is pressed.
        /// </summary>
        bool GetKey(int button);

        /// <summary>
        /// True for a single-frame, on the frame that the given key is pressed.
        /// </summary>
        bool GetKeyDown(int button);

        /// <summary>
        /// True for a single-frame, on the frame that the given key is released.
        /// </summary>
        bool GetKeyUp(int button);

        /// <summary>
        /// True if given button is pressed.
        /// </summary>
        bool GetMouseButton(int button);

        /// <summary>
        /// True for a single-frame, on the frame that the given button is pressed.
        /// </summary>
        bool GetMouseButtonDown(int button);

        /// <summary>
        /// True for a single-frame, on the frame that the given button is released.
        /// </summary>
        bool GetMouseButtonUp(int button);

        /// <summary>
        /// Absolute mouse position across all monitors, OS-space.
        /// </summary>
        inline LGXVector2i GetMousePositionAbs()
        {
            return m_currentMousePositionAbs;
        }

        /// <summary>
        /// Delta-motion of the mouse.
        /// </summary>
        inline LGXVector2i GetMouseDelta()
        {
            return m_mouseDelta;
        }

        /// <summary>
        /// Mouse middle scroll value.
        /// </summary>
        inline int32 GetMouseScroll()
        {
            return m_mouseScroll;
        }

        /// <summary>
        /// Set to receive callbacks on key presses and releases.
        /// </summary>
        inline void SetCallbackKey(CallbackKey&& cb)
        {
            m_cbKey = cb;
        }

        /// <summary>
        /// Set to receive callbacks on mouse presses and releases.
        /// </summary>
        inline void SetCallbackMouse(CallbackMouse&& cb)
        {
            m_cbMouse = cb;
        }

        /// <summary>
        /// Set to receive callbacks on mouse wheel.
        /// </summary>
        inline void SetCallbackMouseWheel(CallbackMouseWheel&& cb)
        {
            m_cbMouseWheel = cb;
        }

        /// <summary>
        /// Set to receive callbacks on mouse motion.
        /// </summary>
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
        friend class OSXWindow;
#endif

        void Tick();
        void EndFrame();
        void WindowFeedKey(uint32 key, int32 scanCode, InputAction action, Window* window);
        void WindowFeedMouseButton(uint32 button, InputAction action);
        void WindowFeedMousePosition(const LGXVector2ui& pos);
        void WindowFeedActivateApp(bool activate);
        void WindowFeedMouseWheel(int32 delta);
        void WindowFeedDelta(int32 deltaX, int32 deltaY);

    private:
        int         m_keyStatesDown[NUM_KEY_STATES]     = {0};
        int         m_keyStatesUp[NUM_KEY_STATES]       = {0};
        int         m_mouseStatesDown[NUM_MOUSE_STATES] = {0};
        int         m_mouseStatesUp[NUM_MOUSE_STATES]   = {0};
        bool        m_appActive                         = true;
        LGXVector2i m_currentMousePositionAbs           = {0, 0};
        LGXVector2i m_previousMousePosition             = {0, 0};
        LGXVector2i m_mouseDelta                        = {0, 0};
        int32       m_mouseScroll                       = 0;
        bool        m_currentStates[256]                = {0};
        bool        m_previousStates[256]               = {0};
        LGXVector2i m_mousePosTrackingClick             = {};

        CallbackKey              m_cbKey        = nullptr;
        CallbackMouse            m_cbMouse      = nullptr;
        CallbackMouseWheel       m_cbMouseWheel = nullptr;
        CallbackMouseMove        m_cbMouseMove  = nullptr;
        LINAGX_MAP<uint32, bool> m_globalMouseStates;
        LINAGX_MAP<uint32, bool> m_globalPrevMouseStates;
        bool                     m_receivedDelta = false;
    };
} // namespace LinaGX
