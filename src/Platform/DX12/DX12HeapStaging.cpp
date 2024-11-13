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

#pragma once

#include "LinaGX/Platform/DX12/DX12HeapStaging.hpp"
#include "LinaGX/Platform/DX12/DX12Backend.hpp"
#include "LinaGX/Common/CommonConfig.hpp"

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
