/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

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
#include <chrono>
#include <Windows.h>

namespace LinaGX::Examples
{
    void App::Initialize()
    {
    }

    void App::Run()
    {
        m_isRunning = true;
        m_prevTime  = std::chrono::high_resolution_clock::now();

        while (m_isRunning)
        {
            Tick();
        }

        Shutdown();
    }

    void App::Tick()
    {
        auto now            = std::chrono::high_resolution_clock::now();
        auto duration       = std::chrono::duration_cast<std::chrono::microseconds>(now - m_prevTime);
        m_prevTime          = now;
        m_deltaMicroseconds = duration.count();
        m_deltaSeconds      = static_cast<float>(m_deltaMicroseconds) / 1000000.0f;
        m_elapsedSeconds += m_deltaSeconds;

        MSG msg    = {0};
        msg.wParam = 0;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        OnTick();

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

    void App::Quit()
    {
        m_isRunning = false;
    }

    void App::Shutdown()
    {
    }

} // namespace LinaGX::Examples
