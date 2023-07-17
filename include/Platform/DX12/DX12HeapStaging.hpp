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

#pragma once

#ifndef LINAGX_DX12HEAP_STAGING_HPP
#define LINAGX_DX12HEAP_STAGING_HPP

#include "Common/CommonGfx.hpp"
#include "Platform/DX12/DX12Common.hpp"

namespace LinaGX
{
    class DX12Backend;

    class DX12HeapStaging
    {
    public:
        DX12HeapStaging(DX12Backend* backend, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors);
        ~DX12HeapStaging();

        DescriptorHandle GetNewHeapHandle();
        void             FreeHeapHandle(DescriptorHandle handle);

        ID3D12DescriptorHeap* GetHeap()
        {
            return m_dx12Heap;
        }

        D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const
        {
            return m_heapType;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetHeapCPUStart() const
        {
            return m_cpuStart;
        }

        uint32 GetMaxDescriptors() const
        {
            return m_maxDescriptors;
        }

        uint32 GetDescriptorSize() const
        {
            return m_descriptorSize;
        }

        virtual uint32 GetCurrentDescriptorIndex()
        {
            return 0;
        };

    private:
        ID3D12DescriptorHeap*       m_dx12Heap       = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE  m_heapType       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart       = {};
        uint32                      m_maxDescriptors = 0;
        uint32                      m_descriptorSize = 0;

        LINAGX_DEQUE<uint32> m_freeDescriptors;
        uint32               m_currentDescriptorIndex = 0;
        uint32               m_activeHandleCount      = 0;
    };
} // namespace LinaGX

#endif
