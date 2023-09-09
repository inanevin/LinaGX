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


#include "LinaGX/Core/Input.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include <Windows.h>

namespace LinaGX
{
    uint16 Input::GetCharacterMask(wchar_t ch)
    {
        uint16       mask    = 0;
        const uint32 keycode = GetKeycode(ch);


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

        return mask;
    }

    uint32 Input::GetKeycode(char32_t c)
    {
        return VkKeyScanExW(static_cast<WCHAR>(c), GetKeyboardLayout(0));
    }

    wchar_t Input::GetCharacterFromKey(uint32 key)
    {
        wchar_t ch = 0;
        BYTE keyboardState[256];

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

    bool Input::IsControlPressed()
    {
        return GetKey(LINAGX_KEY_LCTRL) || GetKey(LINAGX_KEY_RCTRL) || GetKey(LINAGX_KEY_LALT) || GetKey(LINAGX_KEY_RALT) || GetKey(LINAGX_KEY_TAB) || GetKey(LINAGX_KEY_CAPSLOCK);
    }

    bool Input::GetKey(int keycode)
    {
        if (!m_appActive)
            return false;

        int keyState = GetKeyState(keycode) & 0x8000 ? 1 : 0;
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

        int keyState = GetKeyState(button) & 0x8000 ? 1 : 0;
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

    void Input::SetCursorMode(CursorMode mode)
    {
        if (mode == m_cursorMode)
            return;

        m_cursorMode = mode;

        switch (mode)
        {
        case CursorMode::Visible:
            ShowCursor(true);
            break;

        case CursorMode::Hidden:
            ShowCursor(false);
            break;

        case CursorMode::Disabled:
            ShowCursor(false);
            break;
        }
    }

    void Input::Tick()
    {
        POINT point;
        GetCursorPos(&point);
        m_previousMousePosition     = m_currentMousePositionAbs;
        m_currentMousePositionAbs.x = point.x;
        m_currentMousePositionAbs.y = point.y;
        m_mouseDelta.x              = m_currentMousePositionAbs.x - m_previousMousePosition.x;
        m_mouseDelta.y              = m_currentMousePositionAbs.y - m_previousMousePosition.y;
    }

    void Input::EndFrame()
    {
        for (int i = 0; i < 256; i++)
            m_previousStates[i] = m_currentStates[i];
    }

    void Input::WindowFeedKey(uint32 key, int32 scanCode, InputAction action, Window* window)
    {
        if (m_cbKey)
            m_cbKey(key, scanCode, action, window);
    }

    void Input::WindowFeedMouseButton(uint32 button, InputAction action)
    {
        if (m_cbMouse)
            m_cbMouse(button, action);
    }

    void Input::WindowFeedActivateApp(bool activate)
    {
        m_appActive = activate;
    }

    void Input::WindowFeedMouseWheel(int32 delta)
    {
        m_mouseScroll = delta;

        if (m_cbMouseWheel)
            m_cbMouseWheel(delta);
    }

    void Input::WindowFeedMousePosition(const LGXVector2ui& pos)
    {
        if (m_cbMouseMove)
            m_cbMouseMove(pos);
    }
} // namespace LinaGX
