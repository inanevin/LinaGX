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

#include "LinaGX/Platform/DX12/DX12HeapStaging.hpp"
#include "LinaGX/Platform/DX12/DX12Backend.hpp"

namespace LinaGX
{
    DX12HeapStaging::DX12HeapStaging(DX12Backend* backend, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors)
    {
        m_heapType       = heapType;
        m_maxDescriptors = numDescriptors;

        try
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
            heapDesc.NumDescriptors = m_maxDescriptors;
            heapDesc.Type           = m_heapType;
            heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NodeMask       = 0;
            ThrowIfFailed(backend->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dx12Heap)));
        }
        catch (HrException e)
        {
            LOGE("Backend -> Exception when creating a descriptor heap! {0}", e.what());
            backend->DX12Exception(e);
        }

        m_dx12Heap->SetName(L"Descriptor Heap");
        m_cpuStart = m_dx12Heap->GetCPUDescriptorHandleForHeapStart();

        m_descriptorSize = backend->GetDevice()->GetDescriptorHandleIncrementSize(m_heapType);
    }

    DX12HeapStaging::~DX12HeapStaging()
    {
        m_dx12Heap->Release();
        m_dx12Heap = NULL;
    }

    DescriptorHandle DX12HeapStaging::GetNewHeapHandle()
    {
        uint32 newHandleID = 0;

        if (m_currentDescriptorIndex < m_maxDescriptors)
        {
            newHandleID = m_currentDescriptorIndex;
            m_currentDescriptorIndex++;
        }
        else if (m_freeDescriptors.size() > 0)
        {
            newHandleID = m_freeDescriptors.front();
            m_freeDescriptors.pop_front();
        }
        else
        {
            LOGA(false, "DX12Backend -> No more handles available in the descriptor heap.");
        }

        DescriptorHandle newHandle;
        size_t           cpuHandle = m_cpuStart.ptr;
        cpuHandle += newHandleID * m_descriptorSize;
        newHandle.SetCPUHandle(cpuHandle);
        newHandle.SetHeapIndex(newHandleID);
        m_activeHandleCount++;
        return newHandle;
    }

    void DX12HeapStaging::FreeHeapHandle(DescriptorHandle handle)
    {
        m_freeDescriptors.push_front(handle.GetHeapIndex());

        if (m_activeHandleCount == 0)
        {
            LOGA(false, "Freeing handles where there ain't none.");
        }
        m_activeHandleCount--;
    }
} // namespace LinaGX
