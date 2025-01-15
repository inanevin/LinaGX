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
