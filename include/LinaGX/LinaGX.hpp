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
THINGS TO ADD ALL BACKENDS/FIX:

 - Proper feature support. Find all features we are using, categorize them, make sure users are able to know if particular thing is supported or not.
 - Rename isBindless to isUnbounded.
 - Fero model won't load.
 - Local thread group size limit on metal.
 - PreTick() :D.
 - App utilities / common main.
 - Fero'da format/hdr problemi.
 - Metal debug.
 
THINGS TO RESEARCH:
 
- Proper alignment. Alignment between struct elements in SSBO and UBO in different platforms. Also alignment between lets say array ubos. Or alignment between different bindings. UBO alignment 256 for example?
- What was the reason for 226 in compute group?
- gl_InstanceIndex and alike and their support.
- Minimium #version we require?
- Tesellation shaders across supported APIs.
- Find a proper solution for bindless and multi-draw-indirect issue. If using gl_DrawID it makes sense only if multi-draw-indirect is supported. Otherwise need to use push-constant.
 
FEATURES TO IMPLEMENT
 
- PSO write mask. (+Metal)
- Shader multiple attachments. (+Metal)
- Render pass multiple attachments, load/store actions per attachment. (+Metal)
- Stencil buffer. (+Metal)
- Finalize/test depth stuff. (+Metal)
- Array textures and sampler2Darray and alike (+Metal). (Used MipMaps example)
- Dynamic descriptor set UBO and SSBO bindings.
- Allow for binding without shader.
- Secondary command buffers.
- Proper custom barriers.
- Compute barrier.
- Mouse confinement and restriction.
- DrawIndirect (without indexed)
- Indirect rendering count buffer.
- DispatchIndirect for computes.
 
THINGS TO TEST:
 
 - Matrix as a vertex input.
 - Different vertex buffer slots.
 - Binding without shader test.
 
INDIRECT NOTES:

- Indirect: You need to always use IndexedIndirectCommand structure. You need to set LGX_DrawID in the structure to the index of the draw command in the buffer. You need to use gl_DrawID in Vertex Shader to access the current draw index, which you can use to index into another buffer for per-draw-call parameters.
 - fuuuu gl_DrawID will always be 0 for cases where there is no multi-draw-indirect support.
 - Should we enforce users to create a constant buffer?
 - also should we just use a CBV for per-draw data?
 - vulkan users need to check if multi draw indirect is supported.
 - if not, glDrawID won't be of any use. So they should make sure push constants or use vertex data for accessing draw-specific parameters.
 - Change vulkan implementation so that it only calls 1 time when multiDrawIndirect is not supported.
 - So users can make the call N times pushing different constants on each. (give warning if count != 1 && !multiDrawIndirectSupported)
 
POSSIBLE EPIC DEMO:
 
- Some sort of alley, semi outdoors.
- PBR shaders.
- Proper IBL.
- Sun light + point lights.
- Shadows.
- HDR + tonemapping.
- Deferred rendering.
- Forward pass transparency.
- GPU driven occlusion culling.
- Instanced particles maybe.
- Some AA.
- SSBO.
- Fast + performant, bindless.
- Resolution selection.
 
OR INSTEAD OF AN EPIC DEMO, maybe make a smaller one :).
 
- Fox GLTF using PBR, light and a shadow map, a mirror on the side.
 
NOTES TO DOCUMENT:
 
 - All entry points to your shaders must be named main.
 - Only single push_constants can be used.
 - Don't do bindless SSBO its pointless. You can do binless texture2D, sampler, sampler2D and UBO.
 - If dynamically indexing into unsized array, don't forget the nonuniform extension: #extension GL_EXT_nonuniform_qualifier : enable
 - If you statically index a unsized array, it will be converted to a sized one.
 - Set numbers must be properly increasing. Can't have set 1 if the shader does not define a set 0. Of course this is across all stages. (e.g. vertex have Set 1, fragment has Set 0, its valid.)
 - If you are writing to a swapchain, make sure you submit that draw to a primary queue.
 - Don't forget to set isMultithreaded flag if submitting queues from multiple threads (Vulkan and Metal).
 - And of course don't write to the same texture/swapchain from multiple queues.
 - Always bind unbounded desciptors to the last element of the set. You can only bind unbounded data one after another given they are different types of data.
 - Make sure you do not end descriptor names with "_". Can't end with "Smplr_" if targeting Metal.
 - Geometry shaders are not supported on Metal.
 - In Metal, you need to check Format support from Metal docs, along with CPU visible GPU memory considerations. (its gonna be 1 if device has unified memory, thats it).
 - Bindings need to be in order while creating descriptor sets.
 
 
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
