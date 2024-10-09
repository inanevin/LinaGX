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

#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include <chrono>
#include <Windows.h>

namespace LinaGX::Examples
{
    std::mutex g_logMtx;

    void App::Run()
    {
        m_isRunning = true;
        m_prevTime  = std::chrono::steady_clock::now();

        while (m_isRunning)
        {
            Tick();
        }

        Shutdown();
    }

    void App::Tick()
    {
        MSG msg    = {0};
        msg.wParam = 0;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto now               = std::chrono::steady_clock::now();
        auto deltaMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(now - m_prevTime);
        m_prevTime             = now;
        m_deltaMicroseconds    = deltaMicroSeconds.count();
        m_deltaSeconds         = static_cast<float>(m_deltaMicroseconds) / 1000000.0f;
        m_elapsedSeconds += m_deltaSeconds;

        OnTick();
        OnRender();

        static uint64 fpsCounter = 0;
        fpsCounter += m_deltaMicroseconds;

        if (fpsCounter > 2000000)
        {
            fpsCounter        = 0;
            m_framesPerSecond = (PerformanceStats.totalFrames - m_lastFrames) / 2;
            m_lastFrames      = PerformanceStats.totalFrames;
            std::cout << "FPS: " << m_framesPerSecond << std::endl;
        }
    }

    void App::RegisterWindowCallbacks(LinaGX::Window* wnd)
    {
        m_window = wnd;
        m_window->AddListener(this);
    }

    void App::OnWindowClose(LinaGX::Window* window)
    {
        m_isRunning = false;
    }

    void App::OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui& newSize)
    {
         OnWindowResized(newSize.x, newSize.y); 
    }
} // namespace LinaGX::Examples
