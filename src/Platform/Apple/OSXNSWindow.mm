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


#include "LinaGX/Platform/Apple/OSXNSWindow.hh"
#include "LinaGX/Core/Input.hpp"
#import <CoreGraphics/CoreGraphics.h>


@implementation CustomView
std::function<void(int, int)> mouseMovedCallback;
std::function<void(int, LinaGX::InputAction)> mouseCallback;

- (void)updateTrackingAreas {
    [super updateTrackingAreas];
    
    NSTrackingAreaOptions options = (NSTrackingMouseEnteredAndExited | NSTrackingInVisibleRect | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow);
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                                 options:options
                                                                   owner:self
                                                                userInfo:nil];
    [self addTrackingArea:trackingArea];
}

- (void)setMouseMovedCallback:(std::function<void(unsigned int, unsigned int)>)callback {
   mouseMovedCallback = callback;
}

- (void)setMouseCallback:(std::function<void(int, LinaGX::InputAction action)>)callback {
   mouseCallback = callback;
}

- (void)mouseMoved:(NSEvent *)event
{
    NSPoint loc = event.locationInWindow;
    mouseMovedCallback(loc.x, loc.y);
    [super mouseMoved:event];
}

- (void)mouseDragged:(NSEvent *)event
{
    NSPoint loc = event.locationInWindow;
    mouseMovedCallback(loc.x, loc.y);
    [super mouseDragged:event];
}

- (void)otherMouseDragged:(NSEvent *)event
{
    NSPoint loc = event.locationInWindow;
    mouseMovedCallback(loc.x, loc.y);
    [super otherMouseDragged:event];
}

- (void)rightMouseDragged:(NSEvent *)event
{
    NSPoint loc = event.locationInWindow;
    mouseMovedCallback(loc.x, loc.y);
    [super rightMouseDragged:event];
}

- (void)mouseDown:(NSEvent *)event
{
    mouseCallback(static_cast<int>(event.buttonNumber), LinaGX::InputAction::Pressed);
    [super otherMouseDown:event];
}

- (void)mouseUp:(NSEvent *)theEvent {
    mouseCallback(static_cast<int>(theEvent.buttonNumber), LinaGX::InputAction::Released);
    [super mouseUp:theEvent];
}

@end


@implementation CustomWindowDelegate
std::function<void(int, int)> windowMovedCallback;
std::function<void(unsigned int, unsigned int)> windowResizedCallback;
std::function<void(bool)> setFocusCallback;
std::function<void(bool)> appActivateCallback;
std::function<void()> screenChangedCallback;
std::function<void()> windowClosedCallback;

- (void)setWindowMovedCallback:(std::function<void(int, int)>)callback {
    windowMovedCallback = callback;
}

- (void)setWindowResizedCallback:(std::function<void(unsigned int, unsigned int)>)callback {
    windowResizedCallback = callback;
}

- (void)setFocusCallback:(std::function<void(bool focus)>)callback {
    setFocusCallback = callback;
}

- (void)setAppActivateCallback:(std::function<void(bool focus)>)callback {
    appActivateCallback = callback;
}

- (void)setScreenChangedCallback:(std::function<void()>)callback {
   screenChangedCallback = callback;
}

- (void)setWindowClosedCallback:(std::function<void()>)callback {
   windowClosedCallback = callback;
}

- (void)windowDidMove:(NSNotification *)notification {
    NSWindow *window = (NSWindow *)[notification object];
    NSRect frame = [window frame];
    windowMovedCallback(static_cast<int>(frame.origin.x), static_cast<int>(frame.origin.y));
}

- (void)windowDidResize:(NSNotification *)notification {
    NSWindow *window = (NSWindow *)[notification object];
    NSRect frame = [window frame];
    windowResizedCallback(static_cast<unsigned int>(frame.size.width), static_cast<unsigned int>(frame.size.height));
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    setFocusCallback(true);
    
}

- (void)windowDidResignKey:(NSNotification *)notification {
    setFocusCallback(false);
}

- (void)applicationDidBecomeActive:(NSNotification *)notification {
    appActivateCallback(true);
}

- (void)applicationWillResignActive:(NSNotification *)notification {
    appActivateCallback(false);
}

- (void)windowDidChangeScreen:(NSNotification *)notification
{
    screenChangedCallback();
}

- (void)windowWillClose:(NSNotification *)notification
{
    windowClosedCallback();
}

@end


@implementation CustomNSWindow

std::function<void(int, LinaGX::InputAction)> keyCallback;
std::function<void()> mouseEnteredCallback;
std::function<void()> mouseExitedCallback;
std::function<void(int)> mouseWheelCallback;
std::function<void()> mouseDraggedCallback;

- (void)setKeyCallback:(std::function<void(int, LinaGX::InputAction action)>)callback {
    keyCallback = callback;
}

- (void)setMouseEnteredCallback:(std::function<void()>)callback {
   mouseEnteredCallback = callback;
}

- (void)setMouseExitedCallback:(std::function<void()>)callback {
   mouseExitedCallback = callback;
}

- (void)setMouseWheelCallback:(std::function<void(int)>)callback {
   mouseWheelCallback = callback;
}

- (void)setMouseDraggedCallback:(std::function<void()>)callback {
   mouseDraggedCallback = callback;
}

- (void)mouseDragged:(NSEvent *)event
{
    mouseDraggedCallback();
    [super mouseDragged:event];
}

- (void)keyDown:(NSEvent *)theEvent {
    keyCallback(theEvent.keyCode, LinaGX::InputAction::Pressed);
}

- (void)keyUp:(NSEvent *)theEvent {
    keyCallback(theEvent.keyCode, LinaGX::InputAction::Released);
}

- (void)mouseEntered:(NSEvent *)event{
    mouseEnteredCallback();
    [super mouseEntered:event];
}

- (void)mouseExited:(NSEvent *)event
{
    mouseExitedCallback();
    [super mouseExited:event];
}

- (void)scrollWheel:(NSEvent *)event
{
    mouseWheelCallback(static_cast<int>(event.deltaY));
    [super scrollWheel:event];
}

@end

