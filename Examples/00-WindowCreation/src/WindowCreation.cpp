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
