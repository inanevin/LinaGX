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

#include <functional>
#include "LinaGX/Common/CommonGfx.hpp"
#import <Cocoa/Cocoa.h>

namespace LinaGX
{
    class Input;
    class OSXWindow;
}

@interface CustomView : NSView
- (void)setMouseMovedCallback:(std::function<void(unsigned int, unsigned int)>)callback;
- (void)setMouseCallback:(std::function<void(int, LinaGX::InputAction action)>)callback;
@end

@interface CustomWindowDelegate : NSObject <NSWindowDelegate>
- (void)setWindowMovedCallback:(std::function<void(int, int)>)callback;
- (void)setWindowResizedCallback:(std::function<void(unsigned int, unsigned int)>)callback;
- (void)setFocusCallback:(std::function<void(bool)>)callback;
- (void)setAppActivateCallback:(std::function<void(bool)>)callback;
- (void)setScreenChangedCallback:(std::function<void()>)callback;
- (void)setWindowClosedCallback:(std::function<void()>)callback;
@end



@interface CustomNSWindow : NSWindow
@property (assign) LinaGX::Input* input;
@property (assign) LinaGX::OSXWindow* window;
- (void)setKeyCallback:(std::function<void(int, LinaGX::InputAction action)>)callback;
- (void)setMouseEnteredCallback:(std::function<void()>)callback;
- (void)setMouseExitedCallback:(std::function<void()>)callback;
- (void)setMouseWheelCallback:(std::function<void(int)>)callback;
- (void)setMouseDraggedCallback:(std::function<void()>)callback;

@end
