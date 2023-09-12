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

#include "ThreadPool.hpp"

namespace LinaGX::Examples
{
    ThreadPool::ThreadPool(size_t numThreads)
    {
        m_numThreads = numThreads;
    }

    ThreadPool::~ThreadPool()
    {
    }

    void ThreadPool::AddTask(const std::function<void()>& task)
    {
        m_tasks.push(task);
    }

    void ThreadPool::Run()
    {
        for (size_t i = 0; i < m_numThreads; ++i)
            m_workers.emplace_back(&ThreadPool::Worker, this);
    }

    void ThreadPool::Join()
    {
        for (auto& worker : m_workers)
        {
            if (worker.joinable())
                worker.join();
        }
    }

    void ThreadPool::Worker()
    {
        while (!m_tasks.empty())
        {
            std::function<void()> task;

            m_queueMutex.lock();

            if (!m_tasks.empty())
            {
                task = m_tasks.front();
                m_tasks.pop();
            }

            m_queueMutex.unlock();

            if (task)
                task();
        }
    }
} // namespace LinaGX::Examples
