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

/*
NOTES:

- 16 byte alignment per element in each struct for SSBO and UBOs.

- Projection matrices should produce depth for range 0.0f and 1.0f.

- Only 1 type of PushConstants are supported.

- All matrices and their operations are column-major.

- Bindless is only supported for texture2D, sampler and sampler2D and of course SSBOs.

- If using bindless, need to: #extension GL_EXT_nonuniform_qualifier : enable

- If using bindless, it is forbidden to statically index the unbounded array in the glsl shader.

- UBO buffers need to be 256 byte aligned.

*/

#pragma once

#ifndef LinaGX_HPP
#define LinaGX_HPP

#include "Core/Renderer.hpp"
#include "Core/CommandStream.hpp"
#include "Core/Commands.hpp"
#include "Common/Math.hpp"
#include "Utility/PlatformUtility.hpp"
#include "Utility/ImageUtility.hpp"
#include "Utility/ModelUtility.hpp"

namespace LinaGX
{

} // namespace LinaGX

#endif
