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
#ifndef LINAGX_InputMappings_HPP
#define LINAGX_InputMappings_HPP

#ifdef LINAGX_PLATFORM_WINDOWS
#include <Windows.h>
#include <Xinput.h>
#endif

namespace LinaGX
{
#ifdef LINAGX_PLATFORM_WINDOWS

#define LINAGX_MOUSE_0      VK_LBUTTON
#define LINAGX_MOUSE_1      VK_RBUTTON
#define LINAGX_MOUSE_2      VK_MBUTTON
#define LINAGX_MOUSE_3      VK_XBUTTON1
#define LINAGX_MOUSE_4      VK_XBUTTON2
#define LINAGX_MOUSE_5      VK_LBUTTON
#define LINAGX_MOUSE_6      VK_LBUTTON
#define LINAGX_MOUSE_7      VK_LBUTTON
#define LINAGX_MOUSE_LAST   LINAGX_MOUSE_7
#define LINAGX_MOUSE_LEFT   LINAGX_MOUSE_0
#define LINAGX_MOUSE_RIGHT  LINAGX_MOUSE_1
#define LINAGX_MOUSE_MIDDLE LINAGX_MOUSE_2

#define LINAGX_GAMEPAD_A             VK_PAD_A
#define LINAGX_GAMEPAD_B             VK_PAD_B
#define LINAGX_GAMEPAD_X             VK_PAD_X
#define LINAGX_GAMEPAD_Y             VK_PAD_Y
#define LINAGX_GAMEPAD_LEFT_BUMPER   VK_PAD_LSHOULDER
#define LINAGX_GAMEPAD_RIGHT_BUMPER  VK_PAD_RSHOULDER
#define LINAGX_GAMEPAD_BACK          VK_PAD_BACK
#define LINAGX_GAMEPAD_START         VK_PAD_START
#define LINAGX_GAMEPAD_GUIDE         VK_PAD_START
#define LINAGX_GAMEPAD_LEFT_THUMB    VK_PAD_LTHUMB_PRESS
#define LINAGX_GAMEPAD_RIGHT_THUMB   VK_PAD_RTHUMB_PRESS
#define LINAGX_GAMEPAD_DPAD_UP       VK_PAD_DPAD_UP
#define LINAGX_GAMEPAD_DPAD_RIGHT    VK_PAD_DPAD_RIGHT
#define LINAGX_GAMEPAD_DPAD_DOWN     VK_PAD_DPAD_DOWN
#define LINAGX_GAMEPAD_DPAD_LEFT     VK_PAD_DPAD_LEFT
#define LINAGX_GAMEPAD_LEFT_TRIGGER  4
#define LINAGX_GAMEPAD_RIGHT_TRIGGER 5
#define LINAGX_GAMEPAD_LAST          LINAGX_GAMEPAD_RIGHT_TRIGGER

#define LINAGX_KEY_UNKNOWN -1
#define LINAGX_KEY_A       0x41
#define LINAGX_KEY_B       0x42
#define LINAGX_KEY_C       0x43
#define LINAGX_KEY_D       0x44
#define LINAGX_KEY_E       0x45
#define LINAGX_KEY_F       0x46
#define LINAGX_KEY_G       0x47
#define LINAGX_KEY_H       0x48
#define LINAGX_KEY_I       0x49
#define LINAGX_KEY_J       0x4A
#define LINAGX_KEY_K       0x4B
#define LINAGX_KEY_L       0x4C
#define LINAGX_KEY_M       0x4D
#define LINAGX_KEY_N       0x4E
#define LINAGX_KEY_O       0x4F
#define LINAGX_KEY_P       0x50
#define LINAGX_KEY_Q       0x51
#define LINAGX_KEY_R       0x52
#define LINAGX_KEY_S       0x53
#define LINAGX_KEY_T       0x54
#define LINAGX_KEY_U       0x55
#define LINAGX_KEY_V       0x56
#define LINAGX_KEY_W       0x57
#define LINAGX_KEY_X       0x58
#define LINAGX_KEY_Y       0x59
#define LINAGX_KEY_Z       0x5A

#define LINAGX_KEY_0 0x30
#define LINAGX_KEY_1 0x31
#define LINAGX_KEY_2 0x32
#define LINAGX_KEY_3 0x33
#define LINAGX_KEY_4 0x34
#define LINAGX_KEY_5 0x35
#define LINAGX_KEY_6 0x36
#define LINAGX_KEY_7 0x37
#define LINAGX_KEY_8 0x38
#define LINAGX_KEY_9 0x39

#define LINAGX_KEY_RETURN    VK_RETURN
#define LINAGX_KEY_ESCAPE    VK_ESCAPE
#define LINAGX_KEY_BACKSPACE VK_BACK
#define LINAGX_KEY_TAB       VK_TAB
#define LINAGX_KEY_SPACE     VK_SPACE

#define LINAGX_KEY_MINUS VK_OEM_MINUS

#define LINAGX_KEY_GRAVE VK_OEM_3 // 41 // ?

#define LINAGX_KEY_COMMA    VK_OEM_COMMA
#define LINAGX_KEY_PERIOD   VK_OEM_PERIOD
#define LINAGX_KEY_SLASH    VK_DIVIDE
#define LINAGX_KEY_CAPSLOCK VK_CAPITAL

#define LINAGX_KEY_F1  VK_F1
#define LINAGX_KEY_F2  VK_F2
#define LINAGX_KEY_F3  VK_F3
#define LINAGX_KEY_F4  VK_F4
#define LINAGX_KEY_F5  VK_F5
#define LINAGX_KEY_F6  VK_F6
#define LINAGX_KEY_F7  VK_F7
#define LINAGX_KEY_F8  VK_F8
#define LINAGX_KEY_F9  VK_F9
#define LINAGX_KEY_F10 VK_F10
#define LINAGX_KEY_F11 VK_F11
#define LINAGX_KEY_F12 VK_F12
#define LINAGX_KEY_F13 VK_F13
#define LINAGX_KEY_F14 VK_F14
#define LINAGX_KEY_F15 VK_F15
#define LINAGX_KEY_F16 VK_F16
#define LINAGX_KEY_F17 VK_F17
#define LINAGX_KEY_F18 VK_F18
#define LINAGX_KEY_F19 VK_F19
#define LINAGX_KEY_F20 VK_F20
#define LINAGX_KEY_F21 VK_F21
#define LINAGX_KEY_F22 VK_F22
#define LINAGX_KEY_F23 VK_F23
#define LINAGX_KEY_F24 VK_F24

#define LINAGX_KEY_LWIN VK_LWIN
#define LINAGX_KEY_RWIN VK_RWIN

#define LINAGX_KEY_PRINTSCREEN VK_PRINT
#define LINAGX_KEY_SCROLLLOCK  VK_SCROLL
#define LINAGX_KEY_PAUSE       VK_PAUSE
#define LINAGX_KEY_INSERT      VK_INSERT

#define LINAGX_KEY_HOME         VK_HOME
#define LINAGX_KEY_PAGEUP       VK_PRIOR
#define LINAGX_KEY_DELETE       VK_DELETE
#define LINAGX_KEY_END          VK_END
#define LINAGX_KEY_PAGEDOWN     VK_NEXT
#define LINAGX_KEY_RIGHT        VK_RIGHT
#define LINAGX_KEY_LEFT         VK_LEFT
#define LINAGX_KEY_DOWN         VK_DOWN
#define LINAGX_KEY_UP           VK_UP
#define LINAGX_KEY_NUMLOCKCLEAR VK_CLEAR

#define LINAGX_KEY_KP_DECIMAL  VK_DECIMAL
#define LINAGX_KEY_KP_DIVIDE   VK_DIVIDE
#define LINAGX_KEY_KP_MULTIPLY VK_MULTIPLY
#define LINAGX_KEY_KP_MINUS    VK_OEM_MINUS
#define LINAGX_KEY_KP_PLUS     VK_OEM_PLUS
#define LINAGX_KEY_KP_ENTER    VK_RETURN
#define LINAGX_KEY_KP_1        VK_NUMPAD1
#define LINAGX_KEY_KP_2        VK_NUMPAD2
#define LINAGX_KEY_KP_3        VK_NUMPAD3
#define LINAGX_KEY_KP_4        VK_NUMPAD4
#define LINAGX_KEY_KP_5        VK_NUMPAD5
#define LINAGX_KEY_KP_6        VK_NUMPAD6
#define LINAGX_KEY_KP_7        VK_NUMPAD7
#define LINAGX_KEY_KP_8        VK_NUMPAD8
#define LINAGX_KEY_KP_9        VK_NUMPAD9
#define LINAGX_KEY_KP_0        VK_NUMPAD0

#define LINAGX_KEY_LCTRL  VK_LCONTROL
#define LINAGX_KEY_LSHIFT VK_LSHIFT
#define LINAGX_KEY_LALT   VK_LMENU
#define LINAGX_KEY_LGUI   VK_LMENU
#define LINAGX_KEY_RCTRL  VK_RCONTROL
#define LINAGX_KEY_RSHIFT VK_RSHIFT
#define LINAGX_KEY_RALT   VK_RMENU
#define LINAGX_KEY_RGUI   VK_RMENU

#else

#define LINAGX_MOUSE_0      0
#define LINAGX_MOUSE_1      1
#define LINAGX_MOUSE_2      2
#define LINAGX_MOUSE_3      3
#define LINAGX_MOUSE_4      4
#define LINAGX_MOUSE_5      5
#define LINAGX_MOUSE_6      6
#define LINAGX_MOUSE_7      7
#define LINAGX_MOUSE_LAST   LINAGX_MOUSE_7
#define LINAGX_MOUSE_LEFT   LINAGX_MOUSE_0
#define LINAGX_MOUSE_RIGHT  LINAGX_MOUSE_1
#define LINAGX_MOUSE_MIDDLE LINAGX_MOUSE_2

#define LINAGX_GAMEPAD_A             0
#define LINAGX_GAMEPAD_B             1
#define LINAGX_GAMEPAD_X             2
#define LINAGX_GAMEPAD_Y             3
#define LINAGX_GAMEPAD_LEFT_BUMPER   4
#define LINAGX_GAMEPAD_RIGHT_BUMPER  5
#define LINAGX_GAMEPAD_BACK          6
#define LINAGX_GAMEPAD_START         7
#define LINAGX_GAMEPAD_GUIDE         8
#define LINAGX_GAMEPAD_LEFT_THUMB    9
#define LINAGX_GAMEPAD_RIGHT_THUMB   10
#define LINAGX_GAMEPAD_DPAD_UP       11
#define LINAGX_GAMEPAD_DPAD_RIGHT    12
#define LINAGX_GAMEPAD_DPAD_DOWN     13
#define LINAGX_GAMEPAD_DPAD_LEFT     14
#define LINAGX_GAMEPAD_LEFT_TRIGGER  4
#define LINAGX_GAMEPAD_RIGHT_TRIGGER 5
#define LINAGX_GAMEPAD_LAST          LINAGX_GAMEPAD_RIGHT_TRIGGER

#define LINAGX_KEY_UNKNOWN -1
#define LINAGX_KEY_A       65
#define LINAGX_KEY_B       66
#define LINAGX_KEY_C       67
#define LINAGX_KEY_D       68
#define LINAGX_KEY_E       69
#define LINAGX_KEY_F       70
#define LINAGX_KEY_G       71
#define LINAGX_KEY_H       72
#define LINAGX_KEY_I       73
#define LINAGX_KEY_J       74
#define LINAGX_KEY_K       75
#define LINAGX_KEY_L       76
#define LINAGX_KEY_M       77
#define LINAGX_KEY_N       78
#define LINAGX_KEY_O       79
#define LINAGX_KEY_P       80
#define LINAGX_KEY_Q       81
#define LINAGX_KEY_R       82
#define LINAGX_KEY_S       83
#define LINAGX_KEY_T       84
#define LINAGX_KEY_U       85
#define LINAGX_KEY_V       86
#define LINAGX_KEY_W       87
#define LINAGX_KEY_X       88
#define LINAGX_KEY_Y       89
#define LINAGX_KEY_Z       90

#define LINAGX_KEY_0 48
#define LINAGX_KEY_1 49
#define LINAGX_KEY_2 50
#define LINAGX_KEY_3 51
#define LINAGX_KEY_4 52
#define LINAGX_KEY_5 53
#define LINAGX_KEY_6 54
#define LINAGX_KEY_7 55
#define LINAGX_KEY_8 56
#define LINAGX_KEY_9 57

#define LINAGX_KEY_RETURN    257
#define LINAGX_KEY_ESCAPE    256
#define LINAGX_KEY_BACKSPACE 259
#define LINAGX_KEY_TAB       258
#define LINAGX_KEY_SPACE     32

#define LINAGX_KEY_MINUS 45
#define LINAGX_KEY_GRAVE 96

#define LINAGX_KEY_COMMA    44
#define LINAGX_KEY_PERIOD   46
#define LINAGX_KEY_SLASH    47
#define LINAGX_KEY_CAPSLOCK 280

#define LINAGX_KEY_F1  290
#define LINAGX_KEY_F2  291
#define LINAGX_KEY_F3  292
#define LINAGX_KEY_F4  293
#define LINAGX_KEY_F5  294
#define LINAGX_KEY_F6  295
#define LINAGX_KEY_F7  296
#define LINAGX_KEY_F8  297
#define LINAGX_KEY_F9  298
#define LINAGX_KEY_F10 299
#define LINAGX_KEY_F11 300
#define LINAGX_KEY_F12 301

#define LINAGX_KEY_PRINTSCREEN LINAGX_KEY_UNKNOWN
#define LINAGX_KEY_SCROLLLOCK  281
#define LINAGX_KEY_PAUSE       284
#define LINAGX_KEY_INSERT      260

#define LINAGX_KEY_HOME         268
#define LINAGX_KEY_PAGEUP       266
#define LINAGX_KEY_DELETE       261
#define LINAGX_KEY_END          269
#define LINAGX_KEY_PAGEDOWN     267
#define LINAGX_KEY_RIGHT        262
#define LINAGX_KEY_LEFT         263
#define LINAGX_KEY_DOWN         264
#define LINAGX_KEY_UP           265
#define LINAGX_KEY_NUMLOCKCLEAR 282

#define LINAGX_KEY_KP_DECIMAL  330
#define LINAGX_KEY_KP_DIVIDE   331
#define LINAGX_KEY_KP_MULTIPLY 332
#define LINAGX_KEY_KP_MINUS    333
#define LINAGX_KEY_KP_PLUS     334
#define LINAGX_KEY_KP_ENTER    335
#define LINAGX_KEY_KP_1        321
#define LINAGX_KEY_KP_2        322
#define LINAGX_KEY_KP_3        323
#define LINAGX_KEY_KP_4        324
#define LINAGX_KEY_KP_5        325
#define LINAGX_KEY_KP_6        326
#define LINAGX_KEY_KP_7        327
#define LINAGX_KEY_KP_8        328
#define LINAGX_KEY_KP_9        329
#define LINAGX_KEY_KP_0        320

#define LINAGX_KEY_F13 302
#define LINAGX_KEY_F14 303
#define LINAGX_KEY_F15 304
#define LINAGX_KEY_F16 305
#define LINAGX_KEY_F17 306
#define LINAGX_KEY_F18 307
#define LINAGX_KEY_F19 308
#define LINAGX_KEY_F20 309
#define LINAGX_KEY_F21 310
#define LINAGX_KEY_F22 311
#define LINAGX_KEY_F23 312
#define LINAGX_KEY_F24 313

#define LINAGX_KEY_LCTRL  341
#define LINAGX_KEY_LSHIFT 340
#define LINAGX_KEY_LALT   342
#define LINAGX_KEY_LGUI   343
#define LINAGX_KEY_RCTRL  345
#define LINAGX_KEY_RSHIFT 344
#define LINAGX_KEY_RALT   346
#define LINAGX_KEY_RGUI   347
#endif

    enum InputCode
    {
        KeyUnknown        = LINAGX_KEY_UNKNOWN,
        KeyA              = LINAGX_KEY_A,
        KeyB              = LINAGX_KEY_B,
        KeyC              = LINAGX_KEY_C,
        KeyD              = LINAGX_KEY_D,
        KeyE              = LINAGX_KEY_E,
        KeyF              = LINAGX_KEY_F,
        KeyG              = LINAGX_KEY_G,
        KeyH              = LINAGX_KEY_H,
        KeyI              = LINAGX_KEY_I,
        KeyJ              = LINAGX_KEY_J,
        KeyK              = LINAGX_KEY_K,
        KeyL              = LINAGX_KEY_L,
        KeyM              = LINAGX_KEY_M,
        KeyN              = LINAGX_KEY_N,
        KeyO              = LINAGX_KEY_O,
        KeyP              = LINAGX_KEY_P,
        KeyQ              = LINAGX_KEY_Q,
        KeyR              = LINAGX_KEY_R,
        KeyS              = LINAGX_KEY_S,
        KeyT              = LINAGX_KEY_T,
        KeyU              = LINAGX_KEY_U,
        KeyV              = LINAGX_KEY_V,
        KeyW              = LINAGX_KEY_W,
        KeyX              = LINAGX_KEY_X,
        KeyY              = LINAGX_KEY_Y,
        KeyZ              = LINAGX_KEY_Z,
        KeyAlpha0         = LINAGX_KEY_0,
        KeyAlpha1         = LINAGX_KEY_1,
        KeyAlpha2         = LINAGX_KEY_2,
        KeyAlpha3         = LINAGX_KEY_3,
        KeyAlpha4         = LINAGX_KEY_4,
        KeyAlpha5         = LINAGX_KEY_5,
        KeyAlpha6         = LINAGX_KEY_6,
        KeyAlpha7         = LINAGX_KEY_7,
        KeyAlpha8         = LINAGX_KEY_8,
        KeyAlpha9         = LINAGX_KEY_9,
        KeyReturn         = LINAGX_KEY_RETURN,
        KeyEscape         = LINAGX_KEY_ESCAPE,
        KeyBackspace      = LINAGX_KEY_BACKSPACE,
        KeyTab            = LINAGX_KEY_TAB,
        KeySpace          = LINAGX_KEY_SPACE,
        KeyMinus          = LINAGX_KEY_MINUS,
        KeyGrave          = LINAGX_KEY_GRAVE,
        KeyComma          = LINAGX_KEY_COMMA,
        KeyPeriod         = LINAGX_KEY_PERIOD,
        KeySlash          = LINAGX_KEY_SLASH,
        KeyCapslock       = LINAGX_KEY_CAPSLOCK,
        KeyF1             = LINAGX_KEY_F1,
        KeyF2             = LINAGX_KEY_F2,
        KeyF3             = LINAGX_KEY_F3,
        KeyF4             = LINAGX_KEY_F4,
        KeyF5             = LINAGX_KEY_F5,
        KeyF6             = LINAGX_KEY_F6,
        KeyF7             = LINAGX_KEY_F7,
        KeyF8             = LINAGX_KEY_F8,
        KeyF9             = LINAGX_KEY_F9,
        KeyF10            = LINAGX_KEY_F10,
        KeyF11            = LINAGX_KEY_F11,
        KeyF12            = LINAGX_KEY_F12,
        KeyPrintscreen    = LINAGX_KEY_PRINTSCREEN,
        KeyScrolllock     = LINAGX_KEY_SCROLLLOCK,
        KeyPause          = LINAGX_KEY_PAUSE,
        KeyInsert         = LINAGX_KEY_INSERT,
        KeyHome           = LINAGX_KEY_HOME,
        KeyPageup         = LINAGX_KEY_PAGEUP,
        KeyDelete         = LINAGX_KEY_DELETE,
        KeyEnd            = LINAGX_KEY_END,
        KeyPagedown       = LINAGX_KEY_PAGEDOWN,
        KeyRight          = LINAGX_KEY_RIGHT,
        KeyLeft           = LINAGX_KEY_LEFT,
        KeyDown           = LINAGX_KEY_DOWN,
        KeyUp             = LINAGX_KEY_UP,
        KeyNumlockClear   = LINAGX_KEY_NUMLOCKCLEAR,
        KeyKeypadDecimal  = LINAGX_KEY_KP_DECIMAL,
        KeyKeypadDivide   = LINAGX_KEY_KP_DIVIDE,
        KeyKeypadMultiply = LINAGX_KEY_KP_MULTIPLY,
        KeyKeypadMinus    = LINAGX_KEY_KP_MINUS,
        KeyKeypadPlus     = LINAGX_KEY_KP_PLUS,
        KeyKeypadEnter    = LINAGX_KEY_KP_ENTER,
        KeyKeypad1        = LINAGX_KEY_KP_1,
        KeyKeypad2        = LINAGX_KEY_KP_2,
        KeyKeypad3        = LINAGX_KEY_KP_3,
        KeyKeypad4        = LINAGX_KEY_KP_4,
        KeyKeypad5        = LINAGX_KEY_KP_5,
        KeyKeypad6        = LINAGX_KEY_KP_6,
        KeyKeypad7        = LINAGX_KEY_KP_7,
        KeyKeypad8        = LINAGX_KEY_KP_8,
        KeyKeypad9        = LINAGX_KEY_KP_9,
        KeyKeypad0        = LINAGX_KEY_KP_0,
        KeyF13            = LINAGX_KEY_F13,
        KeyF14            = LINAGX_KEY_F14,
        KeyF15            = LINAGX_KEY_F15,
        KeyF16            = LINAGX_KEY_F16,
        KeyF17            = LINAGX_KEY_F17,
        KeyF18            = LINAGX_KEY_F18,
        KeyF19            = LINAGX_KEY_F19,
        KeyF20            = LINAGX_KEY_F20,
        KeyF21            = LINAGX_KEY_F21,
        KeyF22            = LINAGX_KEY_F22,
        KeyF23            = LINAGX_KEY_F23,
        KeyF24            = LINAGX_KEY_F24,
        KeyLCTRL          = LINAGX_KEY_LCTRL,
        KeyLSHIFT         = LINAGX_KEY_LSHIFT,
        KeyLALT           = LINAGX_KEY_LALT,
        KeyLGUI           = LINAGX_KEY_LGUI,
        KeyRCTRL          = LINAGX_KEY_RCTRL,
        KeyRSHIFT         = LINAGX_KEY_RSHIFT,
        KeyRALT           = LINAGX_KEY_RALT,
        KeyRGUI           = LINAGX_KEY_RGUI,

        MouseUnknown    = LINAGX_KEY_UNKNOWN,
        Mouse0          = LINAGX_MOUSE_0,
        Mouse1          = LINAGX_MOUSE_1,
        Mouse2          = LINAGX_MOUSE_2,
        Mouse3          = LINAGX_MOUSE_3,
        Mouse4          = LINAGX_MOUSE_4,
        Mouse5          = LINAGX_MOUSE_5,
        Mouse6          = LINAGX_MOUSE_6,
        Mouse7          = LINAGX_MOUSE_7,
        MouseLast       = LINAGX_MOUSE_LAST,
        MouseLeft       = LINAGX_MOUSE_LEFT,
        MouseRight      = LINAGX_MOUSE_RIGHT,
        MouseMiddle     = LINAGX_MOUSE_MIDDLE,
        JoystickUnknown = LINAGX_KEY_UNKNOWN,

        GamepadUnknown   = LINAGX_KEY_UNKNOWN,
        GamepadA         = LINAGX_GAMEPAD_A,
        GamepadB         = LINAGX_GAMEPAD_B,
        GamepadX         = LINAGX_GAMEPAD_X,
        GamepadY         = LINAGX_GAMEPAD_Y,
        GamepadLBumper   = LINAGX_GAMEPAD_LEFT_BUMPER,
        GamepadRBumper   = LINAGX_GAMEPAD_RIGHT_BUMPER,
        GamepadBack      = LINAGX_GAMEPAD_BACK,
        GamepadStart     = LINAGX_GAMEPAD_START,
        GamepadGuide     = LINAGX_GAMEPAD_GUIDE,
        GamepadLThumb    = LINAGX_GAMEPAD_LEFT_THUMB,
        GamepadRThumb    = LINAGX_GAMEPAD_RIGHT_THUMB,
        GamepadDpadUp    = LINAGX_GAMEPAD_DPAD_UP,
        GamepadDpadRight = LINAGX_GAMEPAD_DPAD_RIGHT,
        GamepadDpadDown  = LINAGX_GAMEPAD_DPAD_DOWN,
        GamepadDpadLeft  = LINAGX_GAMEPAD_DPAD_LEFT,
        GamepadDpadLast  = LINAGX_GAMEPAD_LAST,
        GamepadLTrigger  = LINAGX_GAMEPAD_LEFT_TRIGGER,
        GamepadRTrigger  = LINAGX_GAMEPAD_RIGHT_TRIGGER,
        GamepadLast      = LINAGX_GAMEPAD_LAST
    };

} // namespace LinaGX

#endif
