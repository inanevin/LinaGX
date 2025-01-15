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

#include "LinaGX/Platform/DX12/SDK/ID3DIncludeInterface.hpp"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace Lina
{
    HRESULT __stdcall ID3DIncludeInterface::LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource)
    {
        // ComPtr<IDxcUtils> utils;
        // HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
        //
        // if (FAILED(hr))
        // {
        // 	LINA_CRITICAL("[Shader Compile] -> Failed to create DXC utils");
        // }
        //
        // char*		 fl	   = FileSystem::WCharToChar(pFilename);
        // const String base  = "Resources/Core/Shaders/Common/";
        // const String incl  = base + fl;
        // wchar_t*	 filew = FileSystem::CharToWChar(incl.c_str());
        //
        // ComPtr<IDxcBlobEncoding> pEncoding;
        // if (SUCCEEDED(utils->LoadFile(filew, nullptr, pEncoding.GetAddressOf())))
        // {
        // 	*ppIncludeSource = pEncoding.Detach();
        // }
        // else
        // {
        // 	LINA_ERR("[ID3DIncludeInterface] -> Could not load the include file! {0}", incl);
        // }
        //
        // delete[] filew;
        // delete[] fl;
        return S_OK;
    }

    HRESULT __stdcall ID3DIncludeInterface::QueryInterface(REFIID riid, void** ppvObject)
    {
        return m_defaultIncludeHandler->QueryInterface(riid, ppvObject);
    }
    ULONG __stdcall ID3DIncludeInterface::AddRef(void)
    {
        return 0;
    }
    ULONG __stdcall ID3DIncludeInterface::Release(void)
    {
        return 0;
    }
} // namespace Lina
