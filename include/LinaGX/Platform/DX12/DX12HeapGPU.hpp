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

#ifndef LINAGX_DX12HEAP_GPU_HPP
#define LINAGX_DX12HEAP_GPU_HPP

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Platform/DX12/DX12Common.hpp"

namespace LinaGX
{
    class DX12Backend;

    struct DX12AvailableHeapBlock
    {
        uint32 start = 0;
        uint32 count = 0;
    };
    class DX12HeapGPU
    {
    public:
        DX12HeapGPU(DX12Backend* backend, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors, bool isReferencedByShader);
        ~DX12HeapGPU();

        void             Reset();
        void             Reset(uint32 newStart);
        DescriptorHandle GetHeapHandleBlock(uint32 count);
        DescriptorHandle GetOffsetedHandle(uint32 count);
        void             RemoveDescriptorHandle(const DescriptorHandle& handle);

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

        D3D12_GPU_DESCRIPTOR_HANDLE GetHeapGPUStart() const
        {
            return m_gpuStart;
        }

        uint32 GetMaxDescriptors() const
        {
            return m_maxDescriptors;
        }

        uint32 GetDescriptorSize() const
        {
            return m_descriptorSize;
        }

        uint32 GetCurrentDescriptorIndex()
        {
            return m_currentDescriptorIndex;
        };

    private:
        ID3D12DescriptorHeap*              m_dx12Heap             = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE         m_heapType             = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        D3D12_CPU_DESCRIPTOR_HANDLE        m_cpuStart             = {};
        D3D12_GPU_DESCRIPTOR_HANDLE        m_gpuStart             = {};
        uint32                             m_maxDescriptors       = 0;
        uint32                             m_descriptorSize       = 0;
        bool                               m_isReferencedByShader = false;
        LINAGX_VEC<DX12AvailableHeapBlock> m_availableBlocks;

        uint32 m_currentDescriptorIndex = 0;
    };
} // namespace LinaGX

#endif
