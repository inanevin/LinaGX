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
