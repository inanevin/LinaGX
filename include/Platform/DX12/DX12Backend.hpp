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

#ifndef LINAGX_DX12Backend_HPP
#define LINAGX_DX12Backend_HPP

#include "Core/Backend.hpp"
#include "Platform/DX12/SDK/d3dx12.h"
#include "Platform/DX12/SDK/d3d12shader.h"
#include <dxc/dxcapi.h>
#include <dxgi1_6.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <stdexcept>

namespace D3D12MA
{
    class Allocator;
}

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

    struct DX12Swapchain
    {
        Microsoft::WRL::ComPtr<IDXGISwapChain3> ptr;
        bool                                    isValid = false;
    };

    class DX12Backend : public Backend
    {
    public:
        DX12Backend(){};
        virtual ~DX12Backend(){};

        virtual bool   Initialize(const InitInfo& initInfo) override;
        virtual void   Shutdown() override;
        virtual bool   CompileShader(ShaderStage stage, const LINAGX_STRING& source, CompiledShaderBlob& outBlob) override;
        virtual uint8  CreateSwapchain(const SwapchainDesc& desc) override;
        virtual void   DestroySwapchain(uint8 handle) override;
        virtual uint16 GenerateShader(const LINAGX_MAP<ShaderStage, CompiledShaderBlob>& stages, const ShaderDesc& shaderDesc) override;

    private:
        void DX12Exception(HrException e);

    private:
        D3D12MA::Allocator*                        m_dx12Allocator = nullptr;
        Microsoft::WRL::ComPtr<IDxcLibrary>        m_idxcLib;
        Microsoft::WRL::ComPtr<IDXGIAdapter1>      m_adapter = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Device>       m_device  = nullptr;
        Microsoft::WRL::ComPtr<IDXGIFactory4>      m_factory = nullptr;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queueDirect;
        bool                                       m_allowTearing = false;

        IDList<uint8, DX12Swapchain> m_swapchains = {10};
    };
} // namespace LinaGX

#endif
