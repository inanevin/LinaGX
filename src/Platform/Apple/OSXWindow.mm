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


#include "LinaGX/Platform/Apple/OSXWindow.hpp"
#include "LinaGX/Core/Input.hpp"
#include "LinaGX/Platform/Apple/OSXNSWindow.hh"

namespace LinaGX
{

bool OSXWindow::Create(LINAGX_STRINGID sid, const char *title, LinaGX::int32 x, LinaGX::int32 y, LinaGX::uint32 width, LinaGX::uint32 height, LinaGX::WindowStyle style, LinaGX::Window *parent) { 
    
        m_size = LGXVector2ui{width, height};
    m_position = LGXVector2i{x,y};
    m_sid = sid;
    m_title = title;
    m_style = style;
    
    if(parent != nullptr)
        m_parent = static_cast<OSXWindow*>(parent);
    
    NSUInteger nsStyle = GetStyle(m_style);  // Define your styles mapping from WindowStyle to NSUInteger
    NSRect frame = NSMakeRect(x, ConvertY(y), width, height);

    CustomNSWindow* wnd = [[CustomNSWindow alloc] initWithContentRect:frame
                                               styleMask:nsStyle
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];

    [wnd retain];
    
    if (!wnd) {
        LOGE("Window -> Failed creating the window!");
        return false;
    }
    
    m_nsWindow = static_cast<void*>(wnd);

    [wnd setInput:m_input];
    [wnd setWindow:this];

    std::function<void(int, LinaGX::InputAction action)> keyCallback = [this](int keyCode, LinaGX::InputAction action) {
        m_input->WindowFeedKey(keyCode, keyCode, action, this);
        
        if(m_cbKey)
            m_cbKey(keyCode, keyCode, action, this);
        
    };
    
    std::function<void(int, LinaGX::InputAction action)> mouseCallback = [this](int keyCode, LinaGX::InputAction action) {
        
        
        if(m_style != LinaGX::WindowStyle::WindowedApplication && keyCode == 0)
        {
            if(action == LinaGX::InputAction::Pressed && m_dragRect.IsPointInside(m_mousePosition))
            {
                m_lmDownForDrag = true;
                m_lmDragDelta = m_mousePosition;
            }
            else if(action == LinaGX::InputAction::Released)
                m_lmDownForDrag = false;
        }
        
        m_input->WindowFeedMouseButton(keyCode, action);
        if(m_cbMouse)
            m_cbMouse(keyCode, action);
     
    };
    
    std::function<void(unsigned int, unsigned int)> mouseMovedCallback = [this](unsigned int x, unsigned int y) {
        m_mousePosition = {x, m_size.y - y};
        m_input->WindowFeedMousePosition(m_mousePosition);
        if(m_cbMouseMove)
            m_cbMouseMove(m_mousePosition);
    };
    
    std::function<void()> mouseEnteredCallback = [this]() {
        m_isHovered = true;
        if(m_cbHoverBegin)
            m_cbHoverBegin();
    };
    
    
    std::function<void()> mouseExitedCallback = [this]() {
        m_isHovered = false;
        
        if(m_cbHoverEnd)
            m_cbHoverEnd();
    };
    
    std::function<void(int)> mouseWheelCallback = [this](int f) {

        if(m_cbMouseWheel)
            m_cbMouseWheel(f);
    };
    
    std::function<void(int, int)> windowMovedCallback = [this](int x, int y) {
        m_position.x = x;
        m_position.y = ConvertYInt(y);

        if(m_cbPosChanged)
            m_cbPosChanged(m_position);
    };
    
    std::function<void(unsigned int, unsigned int)> windowResizedCallback = [this](unsigned int x, unsigned int y) {
        
        m_size.x = x;
        m_size.y = y;

        if(m_cbSizeChanged)
            m_cbSizeChanged(m_size);
    };
    
    std::function<void(bool)> setFocusCallback = [this](bool cond) {
        
        if(!cond)
            m_isHovered = false;

        m_hasFocus = cond;
        if(m_cbFocus)
            m_cbFocus(cond);
    };
    
    std::function<void(bool)> appActivateCallback = [this](bool cond) {
        if(!cond)
            m_isHovered = false;

        m_input->WindowFeedMouseWheel(cond);
    };
    
    std::function<void()> screenChangedCallback = [this]() {
        CalculateDPI();
    };
    
    std::function<void()> windowClosedCallback = [this]() {
        if(m_cbClose)
            m_cbClose();
    };
    
    std::function<void()> mouseDraggedCallback = [this]() {
       
        if(m_lmDownForDrag)
        {
            const LGXVector2i absMouse = m_input->GetMousePositionAbs();
            const LGXVector2i p = {absMouse.x - static_cast<int32>(m_lmDragDelta.x), absMouse.y - static_cast<int32>(m_lmDragDelta.y)};
           SetPosition(p);
        }
    };
    
    // Winndow delegate events.
    CustomWindowDelegate * wndDelegate = [[CustomWindowDelegate alloc] init];
    [wndDelegate setWindowMovedCallback:windowMovedCallback];
    [wndDelegate setWindowResizedCallback:windowResizedCallback];
    [wndDelegate setFocusCallback:setFocusCallback];
    [wndDelegate setAppActivateCallback:appActivateCallback];
    [wndDelegate setScreenChangedCallback:screenChangedCallback];
    [wndDelegate setWindowClosedCallback:windowClosedCallback];
    [wnd setDelegate:wndDelegate];
    
    CustomView* wndContent = [[CustomView alloc] init];
    [wndContent retain];
    m_nsView = static_cast<void*>(wndContent);
    [wndContent setWantsLayer:YES];
    [wndContent setMouseMovedCallback:mouseMovedCallback];
    [wndContent setMouseCallback:mouseCallback];
    [wnd setContentView:wndContent];
    
    // Window events.
    [wnd setKeyCallback:keyCallback];
    [wnd setMouseEnteredCallback:mouseEnteredCallback];
    [wnd setMouseExitedCallback:mouseExitedCallback];
    [wnd setMouseWheelCallback:mouseWheelCallback];
    [wnd setMouseDraggedCallback:mouseDraggedCallback];
    [wnd setAcceptsMouseMovedEvents:YES];
    
    CalculateDPI();
    BringToFront();
    SetVisible(true);
    SetTitle(m_title);
    
    return (m_nsWindow != nil);
}

void OSXWindow::Destroy() {
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    CustomView* view = static_cast<CustomView*>(m_nsView);
    [view release];
    [wnd close];
    [wnd release];
    m_nsWindow = nil;
}

void OSXWindow::Tick() {

    if(!m_sizeRequests.empty())
    {
        auto size = m_sizeRequests[m_sizeRequests.size() - 1];
        SetSize(size);
        m_sizeRequests.clear();
    }

    if(m_titleChangeRequested)
    {
        SetTitle(m_title.c_str());
        m_titleChangeRequested = false;
    }
    
    // Confine mouse.
     if(m_hasFocus)
     {
         if(m_confineStyle == MouseConfineStyle::Window || m_confineStyle == MouseConfineStyle::Region)
         {
             const LGXRectui fullRect = {{0, 0}, m_size};
             const LGXRectui region = m_confineStyle == MouseConfineStyle::Window ? fullRect : m_confineRegion;
    
             const auto& abs = m_input->GetMousePositionAbs();
             const LGXVector2i regionStartAbs = {static_cast<int32>(region.pos.x) + m_position.x, static_cast<int32>(region.pos.y) + m_position.y};
             const LGXVector2i regionEndAbs = {regionStartAbs.x + static_cast<int32>(region.size.x), regionStartAbs.y + static_cast<int32>(region.size.y)};
    
             if(abs.x > regionEndAbs.x || abs.x < regionStartAbs.x || abs.y > regionEndAbs.y || abs.y < regionStartAbs.y)
             {
                 LGXVector2i closestBorderPoint = {std::clamp(abs.x, regionStartAbs.x, regionEndAbs.x), std::clamp(abs.y, regionStartAbs.y, regionEndAbs.y)};
                 CGPoint point;
                 point.x = closestBorderPoint.x;
                 point.y = closestBorderPoint.y;
                 CGWarpMouseCursorPosition(point);
             }
         }
         else if(m_confineStyle == MouseConfineStyle::Center)
         {
             CGPoint point;
             point.x = m_position.x + m_size.x / 2;
             point.y = m_position.y + m_size.y / 2;
             CGWarpMouseCursorPosition(point);
         }
    
     }
}

void OSXWindow::SetStyle(LinaGX::WindowStyle style) { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    m_style = style;
    [wnd setStyleMask:GetStyle(m_style)];
}


void OSXWindow::SetCursorType(LinaGX::CursorType type) {
    
    if(type == m_cursorType)
        return;
    
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    m_cursorType = type;
    
    switch (type) {
        case LinaGX::CursorType::None:
        case LinaGX::CursorType::Default:
            // Set to the standard arrow cursor
            [[NSCursor arrowCursor] set];
            break;
        case LinaGX::CursorType::SizeHorizontal:
            // Set to the horizontal resize cursor
            [[NSCursor resizeLeftRightCursor] set];
            break;
        case LinaGX::CursorType::SizeVertical:
            // Set to the vertical resize cursor
            [[NSCursor resizeUpDownCursor] set];
            break;
        case LinaGX::CursorType::Caret:
            // Set to the I-Beam cursor (usually used for text)
            [[NSCursor IBeamCursor] set];
            break;
        default:
            // Set to the standard arrow cursor as a fallback
            [[NSCursor arrowCursor] set];
            break;
    }
}

void OSXWindow::SetPosition(const LinaGX::LGXVector2i &pos) {
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    const LGXVector2i convertedPos = LGXVector2i{pos.x, static_cast<int32>(ConvertY(pos.y))};
    NSPoint p;
    p.x = convertedPos.x;
    p.y = convertedPos.y;
    [wnd setFrameOrigin:p];
    m_position = pos;
}

void OSXWindow::SetSize(const LinaGX::LGXVector2ui &size) {
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    NSSize sz;
    sz.width = static_cast<CGFloat>(size.x);
    sz.height = static_cast<CGFloat>(size.y);
    [wnd setContentSize:sz];
    m_size = size;
}

void OSXWindow::CenterPositionToCurrentMonitor() { 
    const auto& mi = GetMonitorInfoFromWindow();
    const int32 x = mi.workTopLeft.x + mi.size.x - m_size.x / 2;
    const int32 y = mi.workTopLeft.y + mi.size.y - m_size.y / 2;
    const LGXVector2i newPos = {x, y};
    SetPosition(newPos);
}

void OSXWindow::SetVisible(bool isVisible) { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    m_isVisible = isVisible;
    [wnd setIsVisible:isVisible];
}

void OSXWindow::BringToFront() { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    [wnd makeKeyAndOrderFront:nil];
}

void OSXWindow::SetAlpha(float alpha) { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    [wnd setAlphaValue:alpha];
    m_alpha = alpha;
    m_isTransparent = alpha < 0.99f;
}

void OSXWindow::SetTitle(const std::string &str) { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    NSString *title = [NSString stringWithUTF8String:str.c_str()];
    [wnd setTitle:title];
    m_title = str;
}


void OSXWindow::Maximize() { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    [wnd zoom:nil];
}

void OSXWindow::Minimize() { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    [wnd miniaturize:nil];
}

void OSXWindow::Restore() { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    if ([wnd isMiniaturized]) {
        [wnd deminiaturize:nil];
    } else if ([wnd isZoomed]) {
        [wnd zoom:nil];  // Toggle zoom to restore to previous size
    }
}

MonitorInfo OSXWindow::GetMonitorInfoFromWindow() {
    NSWindow* window = static_cast<NSWindow*>(m_nsWindow);

    // Initialize your MonitorInfo struct
    MonitorInfo info;

    // Get the screen that the window is on
    NSScreen *screen = [window screen];

    // 1. Monitor Size
    NSRect screenFrame = [screen frame];
    info.size.x = screenFrame.size.width;
    info.size.y = screenFrame.size.height;

    // 2. Monitor Work Area Size
    // On macOS, the work area is generally the full screen size minus menu bar.
    NSRect visibleFrame = [screen visibleFrame];
    info.workArea.x = visibleFrame.size.width;
    info.workArea.y = visibleFrame.size.height;

    // 3. Monitor Work Area Top Left
    info.workTopLeft.x = visibleFrame.origin.x;
    info.workTopLeft.y = visibleFrame.origin.y;

    // 4. DPI and 5. DPI Scale
    // This assumes the screen is retina. You might have to add more logic for non-retina screens.
    CGFloat dpi = 72.0 * [screen backingScaleFactor];
    info.dpi = dpi;
    info.dpiScale = [screen backingScaleFactor];

    // 6. Is Monitor Primary
    // Assuming the primary screen is the first one in the array
    info.isPrimary = (screen == [[NSScreen screens] objectAtIndex:0]);

    return info;
}

LGXVector2ui OSXWindow::GetMonitorWorkArea() {
    return GetMonitorInfoFromWindow().workArea;
}

LGXVector2ui OSXWindow::GetMonitorSize() {
    return GetMonitorInfoFromWindow().size;
}

bool OSXWindow::GetIsMaximized() { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    return [wnd isZoomed];
}

bool OSXWindow::GetIsMinimized() { 
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    return [wnd isMiniaturized];
}

void OSXWindow::Close() {
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    [wnd close];
    
    if(m_cbClose)
        m_cbClose();
}

float OSXWindow::ConvertY(float targetY)
{
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    NSScreen *currentScreen = [wnd screen];
    if (currentScreen == nil) {
        currentScreen = [NSScreen mainScreen];
    }

    NSRect screenRect = [currentScreen frame];

    // Convert the y-coordinate
    // Substract the window's height so that we're setting the y-coordinate for the top-left corner
    return screenRect.size.height - targetY - static_cast<float>(m_size.y);
}

uint32 OSXWindow::ConvertYInt(uint32 targetY)
{
    NSWindow* wnd = static_cast<NSWindow*>(m_nsWindow);
    NSScreen *currentScreen = [wnd screen];
    if (currentScreen == nil) {
        currentScreen = [NSScreen mainScreen];
    }

    NSRect screenRect = [currentScreen frame];

    // Convert the y-coordinate
    // Substract the window's height so that we're setting the y-coordinate for the top-left corner
    return static_cast<uint32>(screenRect.size.height) - targetY - m_size.y;
}

void OSXWindow::CalculateDPI()
{
    NSWindow* window = static_cast<NSWindow*>(m_nsWindow);

    NSScreen *currentScreen = [window screen];
    if (currentScreen == nil) {
        currentScreen = [NSScreen mainScreen];
    }
    
    NSDictionary *description = [currentScreen deviceDescription];
    NSNumber *screenID = [description objectForKey:@"NSScreenNumber"];
    
    CGDirectDisplayID displayID = (CGDirectDisplayID)[screenID longValue];
    CGSize displayPhysicalSize = CGDisplayScreenSize(displayID); // in millimeters
    
    CGFloat scaleFactor = [currentScreen backingScaleFactor];
    CGRect screenFrame = [currentScreen frame];
    
    CGFloat dpi = ceil((screenFrame.size.width * scaleFactor) / (displayPhysicalSize.width / 25.4)); // convert mm to inches
    m_dpi = dpi * scaleFactor;
    m_dpiScale = scaleFactor;
       
}

uint32 OSXWindow::GetStyle(WindowStyle style)
{
    NSUInteger nsStyle = 0;
    
    if(style == WindowStyle::WindowedApplication)
    {
        nsStyle = NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
    }
    else if(style == WindowStyle::Borderless || style == WindowStyle::BorderlessAlpha || style == WindowStyle::BorderlessApplication)
    {
        nsStyle = NSWindowStyleMaskBorderless;
    }
    else if(style == WindowStyle::BorderlessFullscreen)
    {
        nsStyle = NSWindowStyleMaskBorderless | NSWindowStyleMaskFullScreen;
    }
    else if(style == WindowStyle::Fullscreen)
    {
        nsStyle = NSWindowStyleMaskFullScreen;
    }
    
    return static_cast<uint32>(nsStyle);
}

void OSXWindow::ConfineMouse() {
    m_confineStyle = MouseConfineStyle::Window;
};

void OSXWindow::ConfineMouseToRegion(const LGXRectui& region) {
    m_confineStyle = MouseConfineStyle::Region;
    m_confineRegion = region;
};

void OSXWindow::ConfineMouseToCenter()
{
    m_confineStyle = MouseConfineStyle::Center;
}

void OSXWindow::FreeMouse()
{
    m_confineStyle = MouseConfineStyle::None;
}

void OSXWindow::SetMouseVisible(bool visible)
{
    if(visible)
        [NSCursor unhide];
    else
        [NSCursor hide];
};

} // namespace LinaGX
