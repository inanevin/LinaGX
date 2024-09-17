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

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"
#include "LinaGX/Core/Window.hpp"


namespace LinaGX
{
    class OSXWindow : public Window
    {
        
    private:
        
        enum class MouseConfineStyle
        {
            None,
            Center,
            Window,
            Region
        };
        
    public:

        virtual void         Close() override;
        virtual void         SetStyle(WindowStyle style) override;
        virtual void         SetCursorType(CursorType type) override;
        virtual void         SetPosition(const LGXVector2i& pos) override;
        virtual void         SetSize(const LGXVector2ui& size) override;
        virtual void         CenterPositionToCurrentMonitor() override;
        virtual void         SetVisible(bool isVisible) override;
        virtual void         BringToFront() override;
        virtual void         SetAlpha(float alpha) override;
        virtual void         SetTitle(const LINAGX_STRING& str) override;
        virtual void         Maximize() override;
        virtual void         Minimize() override;
        virtual void         Restore() override;
        virtual bool         GetIsMaximized() override;
        virtual bool         GetIsMinimized() override;
        virtual void         ConfineMouse() override;
        virtual void         ConfineMouseToRegion(const LGXRectui& region) override;
        virtual void         ConfineMouseToCenter() override;
        virtual void         FreeMouse() override;
        virtual void         SetMouseVisible(bool visible) override;
        virtual void SetIsFloating(bool isFloating) override;
        virtual bool QueryVisibility() override;
        
        virtual void* GetWindowHandle() override
        {
            return m_nsWindow;
        }

        virtual void* GetOSHandle() override
        {
            return m_nsView;
        }

    protected:
        friend class WindowManager;
        OSXWindow(Input* input, WindowManager* wm)
            : Window(input, wm){};
        virtual ~OSXWindow() = default;

        virtual bool Create(LINAGX_STRINGID sid, const char* title, int32 x, int32 y, uint32 width, uint32 height, WindowStyle style, Window* parent) override;
        virtual void Destroy() override;
        virtual void Tick() override;
        
    private:
        
        float ConvertY(float desiredY);
        uint32 ConvertYInt(uint32 desiredY);
        void CalculateDPI();
        uint32 GetStyle(WindowStyle style);
        void CalculateMonitorInfo();
        
        void CallbackWindowClosed();
        void CallbackSetMain(bool isMain);
        void CallbackSetFocus(bool isFocused);

    private:
        bool         m_titleChangeRequested = false;
        OSXWindow*   m_parent               = nullptr;
        void* m_nsWindow = nullptr;
        void* m_nsView = nullptr;
        bool m_lmDownForDrag = false;
        LGXVector2 m_lmDragDelta = {};
        LGXRectui m_confineRegion = {};
        MouseConfineStyle m_confineStyle = MouseConfineStyle::None;
    };
} // namespace LinaGX

