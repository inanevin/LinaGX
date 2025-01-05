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
    
namespace
{
    NSString* GetCharacterFromKey(uint32 key)
    {
        TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
        CFDataRef layoutData = (CFDataRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
        
            const UCKeyboardLayout *keyboardLayout = (const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);
        
        // Get the current modifier key state
        UInt32 modifierKeyState = GetCurrentKeyModifiers();
        
        UInt32 keysDown = 0;
        UniChar characters[4];
        UniCharCount realLength;
        
        UCKeyTranslate(keyboardLayout,
                        key,
                        kUCKeyActionDown,
                        modifierKeyState >> 8,
                        LMGetKbdType(),
                        kUCKeyTranslateNoDeadKeysBit,
                        &keysDown,
                        sizeof(characters) / sizeof(characters[0]),
                        &realLength,
                        characters);
        
        CFRelease(currentKeyboard);
        
        NSString* nsString = (realLength > 0) ? [NSString stringWithCharacters:characters length:1] : nil;
        if (nsString == nil || [nsString length] == 0) {
            return nil;
        }
        
        return nsString;
        // unichar uniChar = [nsString characterAtIndex:0];
        // if (CFStringIsSurrogateHighCharacter(uniChar) || CFStringIsSurrogateLowCharacter(uniChar)) {
        //     return L'?'; // Placeholder for surrogate pairs.
        // }
        // return (wchar_t)uniChar;
    }
}

    void Input::GetCharacterInfoFromKeycode(uint32 keycode, wchar_t &outWChar, uint16 &outMask)
    {
        NSString* characterString = GetCharacterFromKey(keycode);
        unichar uniChar = [characterString characterAtIndex:0];
        if (CFStringIsSurrogateHighCharacter(uniChar) || CFStringIsSurrogateLowCharacter(uniChar)) {
            return; // Placeholder for surrogate pairs.
        }
        wchar_t ch = (wchar_t)uniChar;
        uint16 mask = 0;
        
        if (ch == L' ')
            mask |= Whitespace;
        else
        {
            if ([characterString rangeOfCharacterFromSet:[NSCharacterSet whitespaceCharacterSet]].location != NSNotFound) {
                    mask |= Whitespace;
                } else if ([characterString rangeOfCharacterFromSet:[NSCharacterSet alphanumericCharacterSet]].location != NSNotFound) {
                    if ([characterString rangeOfCharacterFromSet:[NSCharacterSet decimalDigitCharacterSet]].location != NSNotFound)
                        mask |= Number;
                    else
                        mask |= Letter;
                } else if ([characterString rangeOfCharacterFromSet:[NSCharacterSet punctuationCharacterSet]].location != NSNotFound) {
                    mask |= Symbol;

                    if ([@[@"-", @"+", @"*", @"/"] containsObject:characterString])
                        mask |= Operator;

                    if ([@[@"-", @"+"] containsObject:characterString])
                        mask |= Sign;
                } else {
                    mask |= Control;
                }
        }

        if (ch == L'.' || ch == L',')
            mask |= Separator;

        if (mask & (Letter | Number | Whitespace | Separator | Symbol))
            mask |= Printable;
        
        outMask = mask;
        outWChar = ch;
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
        
        CGMouseButton but = (CGMouseButton)button;
        bool isActive = CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, but);;
        m_currentMouseStates[button] = isActive;
        return isActive;
    }

    bool Input::GetMouseButtonDown(int button)
    {
        if (!m_appActive)
            return false;

        int keyState = GetMouseButton(button);
        return keyState == 1 && m_prevMouseStates[button] == 0;
    }
    bool Input::GetMouseButtonUp(int button)
    {
        if (!m_appActive)
            return false;

        int keyState =  GetMouseButton(button);
        return keyState == 0 && m_prevMouseStates[button] == 1;
    }

    void Input::SetMousePosition(const LGXVector2i& mp)
    {
        CGPoint point;
        point.x = static_cast<float>(mp.x);
        point.y = static_cast<float>(mp.y);
        CGWarpMouseCursorPosition(point);
        m_currentMousePositionAbs.x = static_cast<float>(mp.x);
        m_currentMousePositionAbs.y = static_cast<float>(mp.y);
        m_previousMousePosition     = m_currentMousePositionAbs;
    }
    void Input::Tick()
    {
        int32 mdx = 0, mdy = 0;
        CGGetLastMouseDelta(&mdx, &mdy);
        m_mouseDelta.x = static_cast<float>(mdx);
        m_mouseDelta.y = static_cast<float>(mdy);
        NSPoint point = [NSEvent mouseLocation];
        m_previousMousePosition     = m_currentMousePositionAbs;
        m_currentMousePositionAbs.x = point.x;
        m_currentMousePositionAbs.y = [[NSScreen mainScreen] frame].size.height - point.y;
    }

    void Input::EndFrame()
    {
        for (int i = 0; i < NUM_KEY_STATES; i++)
            m_previousStates[i] = m_currentStates[i];
        
        for (int i = 0; i < NUM_MOUSE_STATES; i++)
            m_prevMouseStates[i] = m_currentMouseStates[i];
    }


    void Input::WindowFeedKey(uint32 key, int32 scanCode, InputAction action, Window* window)
    {
        for(auto* l : m_listeners)
            l->OnKey(key, scanCode, action, window);
    }

    void Input::WindowFeedMouseButton(uint32 button, InputAction action, Window* window)
    {
        // m_currentMouseStates[button] = action == InputAction::Released ? false : true;
        
        for(auto* l : m_listeners)
            l->OnMouse(button, action, window);
    }

    void Input::WindowFeedActivateApp(bool activate)
    {
        m_appActive = activate;
    }

    void Input::WindowFeedMouseWheel(float delta, Window* window)
    {
        m_mouseScroll = delta;

        for(auto* l : m_listeners)
            l->OnMouseWheel(delta, window);
    }

    void Input::WindowFeedMousePosition(const LGXVector2& pos, Window* window)
    {
        for(auto* l : m_listeners)
            l->OnMouseMove(pos, window);
    }
} // namespace LinaGX
