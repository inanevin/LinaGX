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
#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Carbon/Carbon.h>

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
            if (iswalnum(ch))
            {
                if (keycode >= '0' && keycode <= '9')
                    mask |= Number;
                else
                    mask |= Letter;
            }
            else if (iswpunct(ch))
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
        LOGA(false, "Not implemented on macOS!");
        return 0;
    }

    wchar_t Input::GetCharacterFromKey(uint32 key)
    {
        wchar_t ch = 0;
        LOGA(false, "Not implemented on macOS!");
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

        int keyState = CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, keycode) ? 1 : 0;
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
        
        int keyState = m_globalMouseStates[button] ? 1 : 0;
        m_currentStates[button] = keyState;
        return keyState == 1;
    }

    bool Input::GetMouseButtonDown(int button)
    {
        if (!m_appActive)
            return false;

        int keyState = m_globalMouseStates[button];

        return keyState == 1 && m_globalPrevMouseStates[button] == 0;
    }
    bool Input::GetMouseButtonUp(int button)
    {
        if (!m_appActive)
            return false;

        int keyState = m_globalMouseStates[button];

        return keyState == 0 && m_globalPrevMouseStates[button] == 1;
    }

    void Input::Tick()
    {
        CGGetLastMouseDelta(&m_mouseDelta.x, &m_mouseDelta.y);
        NSPoint point = [NSEvent mouseLocation]; 
        m_previousMousePosition     = m_currentMousePositionAbs;
        m_currentMousePositionAbs.x = point.x;
        m_currentMousePositionAbs.y = [[NSScreen mainScreen] frame].size.height - point.y;
    }

    void Input::EndFrame()
    {
        for (int i = 0; i < 256; i++)
            m_previousStates[i] = m_currentStates[i];
        
        for(int i = 0; i < 8; i++)
            m_globalPrevMouseStates[i] = m_globalMouseStates[i];
    }


    void Input::WindowFeedKey(uint32 key, int32 scanCode, InputAction action, Window* window)
    {
        if (m_cbKey)
            m_cbKey(key, scanCode, action, window);
    }

    void Input::WindowFeedMouseButton(uint32 button, InputAction action)
    {
        m_globalMouseStates[button] = action == InputAction::Released ? false : true;
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
