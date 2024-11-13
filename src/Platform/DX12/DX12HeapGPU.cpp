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

#include "LinaGX/Platform/DX12/DX12HeapGPU.hpp"
#include "LinaGX/Platform/DX12/DX12Backend.hpp"
#include "LinaGX/Common/CommonConfig.hpp"

namespace LinaGX
{
    DX12HeapGPU::DX12HeapGPU(DX12Backend* backend, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors, bool isReferencedByShader)
    {
        m_heapType             = heapType;
        m_maxDescriptors       = numDescriptors;
        m_isReferencedByShader = isReferencedByShader;
        m_availableBlocks.reserve(numDescriptors / 2);

        try
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
            heapDesc.NumDescriptors = m_maxDescriptors;
            heapDesc.Type           = m_heapType;
            heapDesc.Flags          = m_isReferencedByShader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NodeMask       = 0;
            ThrowIfFailed(backend->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dx12Heap)));
        }
        catch (HrException e)
        {
            LOGE("[Instance] -> Exception when creating a descriptor heap! {0}", e.what());
            backend->DX12Exception(e);
        }

        m_dx12Heap->SetName(L"Descriptor Heap");
        m_cpuStart = m_dx12Heap->GetCPUDescriptorHandleForHeapStart();

        if (m_isReferencedByShader)
            m_gpuStart = m_dx12Heap->GetGPUDescriptorHandleForHeapStart();

        m_descriptorSize = backend->GetDevice()->GetDescriptorHandleIncrementSize(m_heapType);
    }

    DX12HeapGPU::~DX12HeapGPU()
    {
        m_dx12Heap->Release();
        m_dx12Heap = NULL;
    }

    void DX12HeapGPU::Reset()
    {
        m_currentDescriptorIndex = 0;
    }

    void DX12HeapGPU::Reset(uint32 newStart)
    {
        m_currentDescriptorIndex = newStart;
    }

    DescriptorHandle DX12HeapGPU::GetHeapHandleBlock(uint32 count)
    {
        for (auto it = m_availableBlocks.begin(); it != m_availableBlocks.end(); ++it)
        {
            auto& block = *it;

            if (block.count >= count)
            {
                DescriptorHandle handle;
                handle.SetCPUHandle(m_cpuStart.ptr + block.start * m_descriptorSize);
                handle.SetGPUHandle(m_gpuStart.ptr + block.start * m_descriptorSize);
                handle.SetDescriptorCount(count);
                handle.SetHeapIndex(block.start);
                block.start += count;
                block.count -= count;

                if (block.count == 0)
                    m_availableBlocks.erase(it);

                return handle;
            }
        }

        uint32 newHandleID = 0;
        uint32 blockEnd    = m_currentDescriptorIndex + count;

        if (blockEnd < m_maxDescriptors)
        {
            newHandleID              = m_currentDescriptorIndex;
            m_currentDescriptorIndex = blockEnd;
        }
        else
        {
            LOGA(false, "DX12Backend -> Ran out of descriptor heap handles, need to increase heap size.");
        }

        DescriptorHandle newHandle;
        size_t           cpuHandle = m_cpuStart.ptr;
        cpuHandle += newHandleID * m_descriptorSize;
        newHandle.SetCPUHandle(cpuHandle);

        uint64 gpuHandle = m_gpuStart.ptr;
        gpuHandle += newHandleID * m_descriptorSize;
        newHandle.SetGPUHandle(gpuHandle);
        newHandle.SetHeapIndex(newHandleID);
        newHandle.SetDescriptorCount(count);

        return newHandle;
    }

    DescriptorHandle DX12HeapGPU::GetOffsetedHandle(uint32 count)
    {
        DescriptorHandle handle;
        handle.SetGPUHandle({GetHeapGPUStart().ptr + count * GetDescriptorSize()});
        handle.SetCPUHandle({GetHeapCPUStart().ptr + count * GetDescriptorSize()});
        return handle;
    }

    void DX12HeapGPU::RemoveDescriptorHandle(const DescriptorHandle& handle)
    {
        const auto             start = handle.GetHeapIndex();
        DX12AvailableHeapBlock block = {handle.GetHeapIndex(), handle.GetDescriptorCount()};
        m_availableBlocks.push_back(block);
    }
} // namespace LinaGX
