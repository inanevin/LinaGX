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

#include "LinaGX/Common/Types.hpp"
#include "LinaGX/Common/Vectors.hpp"
#include "LinaGX/Common/Containers.hpp"

namespace LinaGX
{

#define NUM_KEY_STATES   256
#define NUM_MOUSE_STATES 12

    class Window;

#ifdef LINAGX_PLATFORM_WINDOWS
    class Win32Window;
#elif LINAGX_PLATFORM_APPLE
    class OSXWindow;
#endif

    enum class InputAction
    {
        Pressed,
        Released,
        Repeated
    };

    enum CharacterMask
    {
        Letter     = 1 << 0,
        Number     = 1 << 1,
        Separator  = 1 << 2,
        Symbol     = 1 << 4,
        Whitespace = 1 << 5,
        Control    = 1 << 6,
        Printable  = 1 << 7,
        Operator   = 1 << 8,
        Sign       = 1 << 9,
        Any        = Letter | Number | Separator | Whitespace | Control | Symbol | Printable | Operator | Sign,
    };

    class InputListener
    {
    public:
        InputListener()          = default;
        virtual ~InputListener() = default;

        virtual void OnKey(uint32 keycode, int32 scancode, InputAction action, LinaGX::Window* window){};
        virtual void OnMouse(uint32 button, InputAction action, LinaGX::Window* window){};
        virtual void OnMouseMove(const LGXVector2& pos, LinaGX::Window* window){};
        virtual void OnMouseWheel(float delta, LinaGX::Window* window){};
    };

    class Input
    {
    public:
        /// <summary>
        /// Fills character info, wide-character representation of the keycode and 16 bit unsigned integer representing LinaGX::CharacterMask.
        /// </summary>
        void GetCharacterInfoFromKeycode(uint32 keycode, wchar_t& outWChar, uint16& outMask);

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
        inline LGXVector2 GetMousePositionAbs()
        {
            return m_currentMousePositionAbs;
        }

        /// <summary>
        /// Delta-motion of the mouse.
        /// </summary>
        inline LGXVector2 GetMouseDelta()
        {
            return m_mouseDelta;
        }

        /// <summary>
        /// Mouse middle scroll value.
        /// </summary>
        inline float GetMouseScroll()
        {
            return m_mouseScroll;
        }

        inline void AddListener(InputListener* listener)
        {
            m_listeners.push_back(listener);
        }

        inline void RemoveListener(InputListener* listener)
        {
            auto it = LINAGX_FIND_IF(m_listeners.begin(), m_listeners.end(), [listener](InputListener* list) -> bool { return list == listener; });
            m_listeners.erase(it);
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
        void WindowFeedMouseButton(uint32 button, InputAction action, Window* window);
        void WindowFeedMousePosition(const LGXVector2& pos, Window* window);
        void WindowFeedActivateApp(bool activate);
        void WindowFeedMouseWheel(float delta, Window* window);
        void WindowFeedDelta(int32 deltaX, int32 deltaY);

    private:
        bool       m_appActive               = true;
        LGXVector2 m_currentMousePositionAbs = {0, 0};
        LGXVector2 m_previousMousePosition   = {0, 0};
        LGXVector2 m_mouseDelta              = {0, 0};
        float      m_mouseScroll             = 0.0f;

        LGXVector2 m_mousePosTrackingClick = {};

        bool m_currentMouseStates[NUM_MOUSE_STATES];
        bool m_prevMouseStates[NUM_MOUSE_STATES];
        bool m_currentStates[NUM_KEY_STATES]  = {0};
        bool m_previousStates[NUM_KEY_STATES] = {0};

        bool                       m_receivedDelta = false;
        LINAGX_VEC<InputListener*> m_listeners;
    };
} // namespace LinaGX
