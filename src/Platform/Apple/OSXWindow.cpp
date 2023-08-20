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

namespace LinaGX
{
    
} // namespace LinaGX

using namespace LinaGX;

bool OSXWindow::Create(uint32_t sid, const char *title, LinaGX::int32 x, LinaGX::int32 y, LinaGX::uint32 width, LinaGX::uint32 height, LinaGX::WindowStyle style, LinaGX::Window *parent) { 
        ;
}

void OSXWindow::Destroy() { 
        ;
}

void OSXWindow::PreTick() { 
        ;
}

void OSXWindow::Tick() { 
        ;
}

using namespace LinaGX;

void OSXWindow::SetStyle(LinaGX::WindowStyle style) { 
        ;
}

void OSXWindow::SetCursorType(LinaGX::CursorType type) { 
        ;
}

void OSXWindow::SetPosition(const LinaGX::LGXVector2i &pos) { 
        ;
}

void OSXWindow::SetSize(const LinaGX::LGXVector2ui &size) { 
        ;
}

void OSXWindow::CenterPositionToCurrentMonitor() { 
        ;
}

void OSXWindow::SetVisible(bool isVisible) { 
        ;
}

void OSXWindow::BringToFront() { 
        ;
}

void OSXWindow::SetAlpha(float alpha) { 
        ;
}

void OSXWindow::SetTitle(const std::string &str) { 
            ;
}

void OSXWindow::SetForceIsDragged(bool isDragged, const LinaGX::LGXVector2ui &offset) { 
            ;
}

void OSXWindow::SetIcon(const std::string &name) { 
            ;
}

void OSXWindow::Maximize() { 
        ;
}

void OSXWindow::Minimize() { 
        ;
}

void OSXWindow::Restore() { 
        ;
}

LinaGX::MonitorInfo OSXWindow::GetMonitorInfoFromWindow() { 
        ;
}

LinaGX::LGXVector2ui OSXWindow::GetMonitorWorkArea() { 
        ;
}

LinaGX::LGXVector2ui OSXWindow::GetMonitorSize() { 
        ;
}

bool OSXWindow::GetIsMaximized() { 
    return false;
}

bool OSXWindow::GetIsMinimized() { 
    return false;
}

void OSXWindow::Close() { 
}
