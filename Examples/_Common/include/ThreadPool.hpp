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

#ifndef LINAGX_EXAMPLE_THREADPOOL_HPP
#define LINAGX_EXAMPLE_THREADPOOL_HPP

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace LinaGX
{
    namespace Examples
    {
        class ThreadPool
        {
        public:
            ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
            ~ThreadPool();

            void AddTask(const std::function<void()>& task);
            void Run();
            void Join();

        private:
            void Worker();

        private:
            std::vector<std::thread>          m_workers;
            std::queue<std::function<void()>> m_tasks;
            std::mutex                        m_queueMutex;
            size_t                            m_numThreads = 0;
        };

    } // namespace Examples
} // namespace LinaGX

#endif