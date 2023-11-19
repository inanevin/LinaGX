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

#ifndef APP_HPP
#define APP_HPP

#include "LinaGX/Common/CommonGfx.hpp"
#include <iostream>
#include <cstdarg>
#include <chrono>
#include <atomic>

namespace LinaGX
{
    struct InitInfo;
    class Window;

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
            virtual void Initialize() {};
            virtual void Run();
            virtual void Shutdown() {};
            virtual void OnTick() = 0;
            virtual void OnRender() = 0;
            void Tick();

        protected:
            
            void RegisterWindowCallbacks(LinaGX::Window* wnd);
            virtual void OnWindowResized(uint32 newWidth, uint32 newHeight) {};

        protected:
            std::atomic<bool>                     m_isRunning         = false;
            std::mutex                            m_sizeMtx;
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
