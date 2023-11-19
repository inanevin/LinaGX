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

