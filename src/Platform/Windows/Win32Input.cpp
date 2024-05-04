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

#include "LinaGX/Core/Input.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include <Windows.h>

namespace LinaGX
{

    namespace
    {
        wchar_t GetCharacterFromKey(uint32 key)
        {
            wchar_t ch = 0;
            BYTE    keyboardState[256];

            // Get the current keyboard state
            if (!GetKeyboardState(keyboardState))
            {
                return ch;
            }

            // Set the keyboard state to a known state
            // memset(keyboardState, 0, sizeof(keyboardState));

            // Map the virtual keycode to a scan code
            UINT scanCode = MapVirtualKeyEx(key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));

            // Convert the scan code to a character
            WCHAR unicodeChar[2] = {0};
            int   result         = ToUnicodeEx(key, scanCode, keyboardState, unicodeChar, 2, 0, GetKeyboardLayout(0));

            // If the conversion is successful and the character is a printable character
            if (result == 1)
            {
                ch = unicodeChar[0];
            }

            return ch;
        }
    } // namespace

    void Input::GetCharacterInfoFromKeycode(uint32 keycode, wchar_t& outWChar, uint16& outMask)
    {
        wchar_t ch   = GetCharacterFromKey(keycode);
        uint16  mask = 0;

        if (ch == L' ')
            mask |= Whitespace;
        else
        {
            if (IsCharAlphaNumericW(ch))
            {
                if (keycode >= '0' && keycode <= '9')
                    mask |= Number;
                else
                    mask |= Letter;
            }
            else if (iswctype(ch, _PUNCT))
            {
                mask |= Symbol;

                if (ch == '-' || ch == '+' || ch == '*' || ch == '/')
                    mask |= Operator;

                if (ch == L'-' || ch == L'+')
                    mask |= Sign;
            }
            else
                mask |= Control;
        }

        if (ch == L'.')
            mask |= Separator;

        if (mask & (Letter | Number | Whitespace | Separator | Symbol))
            mask |= Printable;

        outWChar = ch;
        outMask  = mask;
    }

    bool Input::IsControlPressed()
    {
        return GetKey(LINAGX_KEY_LCTRL) || GetKey(LINAGX_KEY_RCTRL) || GetKey(LINAGX_KEY_LALT) || GetKey(LINAGX_KEY_RALT) || GetKey(LINAGX_KEY_TAB) || GetKey(LINAGX_KEY_CAPSLOCK);
    }

    bool Input::GetKey(int keycode)
    {
        if (!m_appActive)
            return false;

        int keyState             = GetKeyState(keycode) & 0x8000 ? 1 : 0;
        m_currentStates[keycode] = keyState;
        return keyState == 1;
    }

    bool Input::GetKeyDown(int keyCode)
    {
        if (!m_appActive)
            return false;

        int keyState = GetKey(keyCode);

        return keyState == 1 && m_previousStates[keyCode] == 0;
    }

    bool Input::GetKeyUp(int keyCode)
    {
        if (!m_appActive)
            return false;

        int keyState = GetKey(keyCode);

        return keyState == 0 && m_previousStates[keyCode] == 1;
    }

    bool Input::GetMouseButton(int button)
    {
        if (!m_appActive)
            return false;

        int keyState            = GetKeyState(button) & 0x8000 ? 1 : 0;
        m_currentStates[button] = keyState;
        return keyState == 1;
    }

    bool Input::GetMouseButtonDown(int button)
    {
        if (!m_appActive)
            return false;

        int keyState = GetKey(button);

        return keyState == 1 && m_previousStates[button] == 0;
    }
    bool Input::GetMouseButtonUp(int button)
    {
        if (!m_appActive)
            return false;

        int keyState = GetKey(button);

        return keyState == 0 && m_previousStates[button] == 1;
    }

    void Input::Tick()
    {
        POINT point;
        GetCursorPos(&point);
        m_previousMousePosition     = m_currentMousePositionAbs;
        m_currentMousePositionAbs.x = static_cast<float>(point.x);
        m_currentMousePositionAbs.y = static_cast<float>(point.y);
        if (!m_receivedDelta)
            m_mouseDelta = {};
        m_receivedDelta = false;
        // m_mouseDelta.x              = m_currentMousePositionAbs.x - m_previousMousePosition.x;
        // m_mouseDelta.y              = m_currentMousePositionAbs.y - m_previousMousePosition.y;
    }

    void Input::EndFrame()
    {
        for (int i = 0; i < NUM_KEY_STATES; i++)
            m_previousStates[i] = m_currentStates[i];
    }

    void Input::WindowFeedKey(uint32 key, int32 scanCode, InputAction action, Window* window)
    {
        for (auto* l : m_listeners)
            l->OnKey(key, scanCode, action, window);
    }

    void Input::WindowFeedMouseButton(uint32 button, InputAction action, Window* window)
    {
        for (auto* l : m_listeners)
            l->OnMouse(button, action, window);
    }

    void Input::WindowFeedActivateApp(bool activate)
    {
        m_appActive = activate;
    }

    void Input::WindowFeedMouseWheel(float delta, Window* window)
    {
        m_mouseScroll = delta;

        for (auto* l : m_listeners)
            l->OnMouseWheel(delta, window);
    }

    void Input::WindowFeedDelta(int32 deltaX, int32 deltaY)
    {
        m_mouseDelta.x  = static_cast<float>(deltaX);
        m_mouseDelta.y  = static_cast<float>(deltaY);
        m_receivedDelta = true;
    }

    void Input::WindowFeedMousePosition(const LGXVector2& pos, Window* window)
    {
        for (auto* l : m_listeners)
            l->OnMouseMove(pos, window);
    }
} // namespace LinaGX
