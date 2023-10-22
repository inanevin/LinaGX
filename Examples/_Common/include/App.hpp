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

#ifndef APP_HPP
#define APP_HPP

#include "LinaGX/Common/CommonGfx.hpp"
#include <iostream>
#include <cstdarg>
#include <chrono>
#include <mutex>

namespace LinaGX
{
    struct InitInfo;

    namespace Examples
    {
        extern std::mutex g_logMtx;

        inline void LogError(const char* err, ...)
        {
            std::lock_guard lg(g_logMtx);
            va_list args;
            va_start(args, err);

            std::cout << "\033[1;31m";
            std::cout << "LinaGX: ";
            vprintf(err, args);
            std::cout << "\033[0m" << std::endl;
            va_end(args);
        }

        inline void LogInfo(const char* info, ...)
        {
            std::lock_guard lg(g_logMtx);
            va_list args;
            va_start(args, info);
            std::cout << "LinaGX: ";
            vprintf(info, args);
            std::cout << std::endl;
            va_end(args);
        }

        class App
        {
        public:
            virtual void Initialize();
            virtual void Run();
            virtual void Shutdown();
            virtual void OnTick(){};
            virtual void OnRender(){};
            virtual void Tick();
            virtual void Quit();

        protected:
            bool                                  m_isRunning         = false;
            uint64                                m_deltaMicroseconds = 0;
            uint64                                m_framesPerSecond   = 0;
            float                                 m_deltaSeconds      = 0.0f;
            float                                 m_elapsedSeconds    = 0.0f;
            std::chrono::steady_clock::time_point m_prevTime;
            uint64                                m_lastFrames = 0;
        };

    } // namespace Examples
} // namespace LinaGX

#endif
