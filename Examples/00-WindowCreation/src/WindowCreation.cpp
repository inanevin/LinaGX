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

#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include <iostream>
#include <cstdarg>
#include "WindowCreation.hpp"

namespace LinaGX::Examples
{

#define MAIN_WINDOW_ID 0

    LinaGX::Instance* _lgx = nullptr;
    Window*           _window   = nullptr;

    void Example::Initialize()
    {
        App::Initialize();

        //******************* CONFIGURATION & INITIALIZATION
        {
            LinaGX::Config.logLevel      = LogLevel::Verbose;
            LinaGX::Config.errorCallback = LogError;
            LinaGX::Config.infoCallback  = LogInfo;

            BackendAPI api = BackendAPI::DX12;

#ifdef LINAGX_PLATFORM_APPLE
            api = BackendAPI::Metal;
#endif

            LinaGX::InitInfo initInfo = InitInfo{
                .api               = api,
                .gpu               = PreferredGPUType::Integrated,
                .framesInFlight    = 2,
                .backbufferCount   = 2,
            };

            _lgx = new LinaGX::Instance();
            _lgx->Initialize(initInfo);
        }

        //*******************  WINDOW CREATION & CALLBACKS
        {
            _window = _lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX Window Creation", 0, 0, 800, 600, WindowStyle::Borderless);
            _window->SetCallbackClose([this]() { m_isRunning = false; });
        }
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        _lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        // Wait for queues to finish
        _lgx->Join();

        // Terminate renderer & shutdown app.
        delete _lgx;
        App::Shutdown();
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        _lgx->PollWindow();
    }

} // namespace LinaGX::Examples
