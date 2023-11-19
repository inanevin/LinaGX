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

/*

Example: Window Creation

Most basic example using LinaGX. It demonstrates how to create an empty app window. Nothing is rendered on the window.
Note: Since nothing is rendered, we do not use StartFrame() and EndFrame() from LinaGX API.

*/

#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include "WindowCreation.hpp"
#include <iostream>
#include <cstdarg>

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID 0

    LinaGX::Instance* _lgx     = nullptr;
    Window*           _window  = nullptr;
    uint32            _windowX = 0;
    uint32            _windowY = 0;

    void Example::Initialize()
    {
        App::Initialize();

        //******************* CONFIGURATION & INITIALIZATION
        {
            BackendAPI api = BackendAPI::Vulkan;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::Config.api             = api;
            LinaGX::Config.gpu             = PreferredGPUType::Discrete;
            LinaGX::Config.framesInFlight  = 2;
            LinaGX::Config.backbufferCount = 2;
            LinaGX::Config.gpuLimits       = {};
            LinaGX::Config.logLevel        = LogLevel::Verbose;
            LinaGX::Config.errorCallback   = LogError;
            LinaGX::Config.infoCallback    = LogInfo;

            _lgx = new LinaGX::Instance();
            _lgx->Initialize();
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Window Creation", 0, 0, 800, 600, WindowStyle::WindowedApplication);
            App::RegisterWindowCallbacks(_window);
            _windowX = _window->GetSize().x;
            _windowY = _window->GetSize().y;
        }
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Get rid of LGX.
        delete _lgx;
    }

    void Example::OnTick()
    {
        // Update window & input systems.
        // Windows require frequent ticks to handle custom resizing & drag operations (if window is not using native style) as well as confinement operations.
        // Also input system requires ticks to mutate input state.
        _lgx->TickWindowSystem();
    }

} // namespace LinaGX::Examples
