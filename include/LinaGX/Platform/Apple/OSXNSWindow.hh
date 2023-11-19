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

#ifndef LINAGX_OSXNSWINDOW_HH
#define LINAGX_OSXNSWINDOW_HH

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



#endif
