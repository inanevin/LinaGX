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

#include "LinaGX/Platform/Apple/OSXNSWindow.hh"
#include "LinaGX/Core/Input.hpp"
#import <CoreGraphics/CoreGraphics.h>


@implementation CustomView
std::function<void(float, float)> mouseMovedCallback;
std::function<void(int, LinaGX::InputAction)> mouseCallback;

- (void)updateTrackingAreas {
    [super updateTrackingAreas];
    
    NSTrackingAreaOptions options = (NSTrackingActiveAlways |NSTrackingMouseEnteredAndExited | NSTrackingInVisibleRect | NSTrackingMouseMoved);
    
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                                 options:options
                                                                   owner:self
                                                                userInfo:nil];
    
    
    [self addTrackingArea:trackingArea];
}


- (void)setMouseMovedCallback:(std::function<void(float, float)>)callback {
   mouseMovedCallback = callback;
}

- (void)setMouseCallback:(std::function<void(int, LinaGX::InputAction action)>)callback {
   mouseCallback = callback;
}

- (void)mouseMoved:(NSEvent *)event
{
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    mouseMovedCallback(loc.x, loc.y);
    [super mouseMoved:event];
}

- (void)mouseDragged:(NSEvent *)event
{
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    mouseMovedCallback(loc.x, loc.y);
    [super mouseDragged:event];
}

- (void)otherMouseDragged:(NSEvent *)event
{
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    mouseMovedCallback(loc.x, loc.y);
    [super otherMouseDragged:event];
}

- (void)rightMouseDragged:(NSEvent *)event
{
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    mouseMovedCallback(loc.x, loc.y);
    [super rightMouseDragged:event];
}

- (void)mouseDown:(NSEvent *)event
{
    mouseCallback(static_cast<int>(event.buttonNumber), event.clickCount > 1 ? LinaGX::InputAction::Repeated : LinaGX::InputAction::Pressed);
    [super otherMouseDown:event];
}

- (void)mouseUp:(NSEvent *)theEvent {
    mouseCallback(static_cast<int>(theEvent.buttonNumber), LinaGX::InputAction::Released);
    [super mouseUp:theEvent];
}

- (void)rightMouseDown:(NSEvent *)event
{
    mouseCallback(static_cast<int>(event.buttonNumber), event.clickCount > 1 ? LinaGX::InputAction::Repeated : LinaGX::InputAction::Pressed);
    [super otherMouseDown:event];
}

- (void)rightMouseUp:(NSEvent *)theEvent {
    mouseCallback(static_cast<int>(theEvent.buttonNumber), LinaGX::InputAction::Released);
    [super mouseUp:theEvent];
}

- (void)otherMouseDown:(NSEvent *)event
{
    mouseCallback(static_cast<int>(event.buttonNumber), event.clickCount > 1 ? LinaGX::InputAction::Repeated : LinaGX::InputAction::Pressed);
    [super otherMouseDown:event];
}

- (void)otherMouseUp:(NSEvent *)theEvent {
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
    keyCallback(theEvent.keyCode, theEvent.isARepeat ? LinaGX::InputAction::Repeated : LinaGX::InputAction::Pressed);
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

- (BOOL)canBecomeMainWindow {
    return YES;
}

- (BOOL)canBecomeKeyWindow {
    return YES;
}

@end

