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

- name your shader functions main

 - 16 byte alignment per element in each struct for SSBO and UBOs.

- Projection matrices should produce depth for range 0.0f and 1.0f.

- Only 1 type of PushConstants are supported.

- All matrices and their operations are column-major.

- Bindless is only supported for texture2D, sampler and sampler2D and of course SSBOs.

- If using bindless, need to: #extension GL_EXT_nonuniform_qualifier : enable

- If using bindless, it is forbidden to statically index the unbounded array in the glsl shader.

- UBO buffers need to be 256 byte aligned.

- Indirect: You need to always use IndexedIndirectCommand structure. You need to set LGX_DrawID in the structure to the index of the draw command in the buffer. You need to use gl_DrawID in Vertex Shader to access the current draw index, which you can
use to index into another buffer for per-draw-call parameters.

- Sets need to be starting from 0. (e.g. cant have set =1 when there is not set = 0)

- #version 460 required

- Align buffers 16 bytes always, if using constants buffers on top, align 256 for DX12

- Any submissions made containing writes to the swapchain image must be sent to the primary queue.

- Don't forget to set isMultithreaded flag if submitting from multiple threads.

- Refactor so that CMDComputeBarrier is CMDComputeIndirectBarrier or something
 
 - dynamic descriptor set stuff, offsetting into buffers take a look at that.
 
 - Updating descriptor sets after binding is not allowed.

- Multiple queues can not write to the same swapchain at the same time.
 
 - stencil not supported yet
 
 - subpasses not supported yet
 
 - take a look at the depth stuff in DX12, how do we not use the depth texture at all?
 - Also we always assume a depth texture with vulkan, what if no depth is enabled?
 
 - only 1 vertex buffer index/slot is supported atm.
 
 - single write mask only for psos.
 
 - Custom barriers man.
 
 - Confine and restrict mouse
 
 - 16 Byte or 32 byte alignment for constant buffers in METAL? Idk, check all alignment requirements properly and document.
 
 - no render pass load store selection
 
 - Metal 2 minimum, needs to support argument buffers.
 
 - rename isBindless to isUnbounded
 
 - makeResidency and binding e.g 10000 textures.
 
 - matrices will be unrolled to vectors in vertex input layouts, check that out.
 
 - In bindless model, take a look at binding descriptor 0 global once, not PER SHADER.
 
 - loading fero model got fucked
 
 - secondary command buffers
 
 - subpasses
 
 - Feature support needs to be better, bindless is not supported on all devices, need to inform users so they can do their own shaiba.
 
 - Always bind unbounded desciptors to the last element of the set. You can only bind unbounded data one after another given they are different types of data.
 - Take a look at the limitation above, see if there is a way to fix that for DX12, adding an extra space if same kind of data is already unbounded in the same space.
 - e.g. textures1 [] and textures2[] -> automatically move textures2[] to different space (do we want that?)
 
 - compute shaders don't use a seperate queue but use a resource barrier on the resource?
 
- metl index stuff is different uint16, uint32 in shader? take a look at that?
- descriptor names cant end with "_"
- Use unique names for descriptor. Descriptor named "GlobalData", if it's Set 0 Binding 0 in Vertex shader, gotta be the same layout in other stages.
- Can't name any descriptor ending with Smplr_ if targeting metal too for reflection purposes.
- blend logic operations are not supported on Metal.
- compare op NotEqual is not supported on Metal.
- format checking is not supported on metal.
- no geometry shaders in metal.
- no separate tesellation shaders?
 - dont bind resource if already bound

 
*/

#pragma once

#ifndef LinaGX_HPP
#define LinaGX_HPP

#include "Core/Instance.hpp"
#include "Core/CommandStream.hpp"
#include "Core/Commands.hpp"
#include "Core/Input.hpp"
#include "Common/Math.hpp"
#include "Utility/PlatformUtility.hpp"
#include "Utility/ImageUtility.hpp"
#include "Utility/ModelUtility.hpp"

namespace LinaGX
{

} // namespace LinaGX

#endif
