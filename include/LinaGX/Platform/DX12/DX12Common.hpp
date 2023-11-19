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
#include "LinaGX/Platform/DX12/SDK/d3dx12.h"
#include "LinaGX/Platform/DX12/SDK/d3d12shader.h"
#include <dxc/dxcapi.h>
#include <dxgi1_6.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <stdexcept>

namespace LinaGX
{
    inline LINAGX_STRING HrToString(HRESULT hr)
    {
        char s_str[64] = {};
        sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
        return LINAGX_STRING(s_str);
    }

    class HrException : public std::runtime_error
    {
    public:
        HrException(HRESULT hr)
            : std::runtime_error(HrToString(hr).c_str()), m_hr(hr)
        {
        }
        HRESULT Error() const
        {
            return m_hr;
        }

    private:
        const HRESULT m_hr;
    };

    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw HrException(hr);
        }
    }

    class DescriptorHandle
    {
    public:
        DescriptorHandle()
        {
            m_cpuHandle       = NULL;
            m_gpuHandle       = NULL;
            m_heapIndex       = 0;
            m_descriptorCount = 0;
        }

        inline size_t GetCPUHandle() const
        {
            return m_cpuHandle;
        }

        inline uint64 GetGPUHandle() const
        {
            return m_gpuHandle;
        }

        inline uint32 GetHeapIndex() const
        {
            return m_heapIndex;
        }

        inline uint32 GetDescriptorCount() const
        {
            return m_descriptorCount;
        }

        void SetCPUHandle(size_t cpuHandle)
        {
            m_cpuHandle = cpuHandle;
        }

        void SetGPUHandle(uint64 gpuHandle)
        {
            m_gpuHandle = gpuHandle;
        }

        void SetHeapIndex(uint32 heapIndex)
        {
            m_heapIndex = heapIndex;
        }

        void SetDescriptorCount(uint32 count)
        {
            m_descriptorCount = count;
        }

        bool IsValid() const
        {
            return m_cpuHandle != NULL;
        }

        bool IsReferencedByShader() const
        {
            return m_gpuHandle != NULL;
        }

    private:
        size_t m_cpuHandle       = NULL;
        uint64 m_gpuHandle       = NULL;
        uint32 m_heapIndex       = 0;
        uint32 m_descriptorCount = 0;
    };
} // namespace LinaGX

