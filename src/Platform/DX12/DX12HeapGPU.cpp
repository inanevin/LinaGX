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

#include "LinaGX/Platform/DX12/DX12HeapGPU.hpp"
#include "LinaGX/Platform/DX12/DX12Backend.hpp"

namespace LinaGX
{
    DX12HeapGPU::DX12HeapGPU(DX12Backend* backend, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors, bool isReferencedByShader)
    {
        m_heapType             = heapType;
        m_maxDescriptors       = numDescriptors;
        m_isReferencedByShader = isReferencedByShader;

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
        uint32 newHandleID = 0;
        uint32 blockEnd    = m_currentDescriptorIndex + count;

        if (blockEnd < m_maxDescriptors)
        {
            newHandleID              = m_currentDescriptorIndex;
            m_currentDescriptorIndex = blockEnd;
        }
        else
        {
            LOGA(false, "DX12Backend -> Ran out of render pass descriptor heap handles, need to increase heap size.");
        }

        DescriptorHandle newHandle;
        size_t           cpuHandle = m_cpuStart.ptr;
        cpuHandle += newHandleID * m_descriptorSize;
        newHandle.SetCPUHandle(cpuHandle);

        uint64 gpuHandle = m_gpuStart.ptr;
        gpuHandle += newHandleID * m_descriptorSize;
        newHandle.SetGPUHandle(gpuHandle);
        newHandle.SetHeapIndex(newHandleID);
        return newHandle;
    }

    DescriptorHandle DX12HeapGPU::GetOffsetedHandle(uint32 count)
    {
        DescriptorHandle handle;
        handle.SetGPUHandle({GetHeapGPUStart().ptr + count * GetDescriptorSize()});
        handle.SetCPUHandle({GetHeapCPUStart().ptr + count * GetDescriptorSize()});
        return handle;
    }
} // namespace LinaGX
