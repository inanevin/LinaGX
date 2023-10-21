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

 - Fero model won't load.
 - Fero'da format/hdr problemi.
 - Document & format all.
 - Metal obj release problem.

THINGS TO RESEARCH:

- gl_InstanceIndex and alike and their support.
- allocate N sets & pool them.

FEATURES TO IMPLEMENT

- Camera data transfer, investigate slowness on integrated card.
- metal: resize and exit bugs.

INDIRECT NOTES:

- Need to use IndexedIndirectCommand or IndirectCommand structure.
- In GLSL, use gl_DrawID to index into any custom buffers per-draw.
- In Vulkan, gl_DrawID will always be 0 if multiDraw feature is not supported. In DX12 and Metal, it will be set to the index of the active draw.
- In cases where gl_DrawID is not supported in Vulkan or not desired, need to call DrawIndirect commands one by one while setting your custom buffer bindings.

NOTES TO DOCUMENT:

 - Always use alignment rules for your target graphics backend.
 - Vertex, Fragment and Compute are supported. Geometry is not supported on Metal. Geometry and Tesellation stages are not battle tested, might be problematic.
 - Minimum Vulkan 1.3 is required and GLSL version #450 is required. On Vulkan, devices need to support 1.2. Timeline Semaphores and 1.3. Dynamic Rendering features.
 - Bindless rendering requires: shaderSampledImageArrayDynamicIndexing, shaderUniformBufferArrayDynamicIndexing, runtimeDescriptorArray, descriptorBindingPartiallyBound
 - Update after bind requires: descriptorBindingSampledImageUpdateAfterBind & descriptorBindingUniformBufferUpdateAfterBind and only supports those 2.
 - DX12 - D3D_FEATURE_LEVEL_11_0 is minimum support. HLSL shader model SM6_0 is required.
 - All entry points to your shaders must be named main.
 - Only single push_constants can be used.
 - Bindless is supported on sampler2D, texture2D, sampler and UBOs.
 - If dynamically indexing into unsized array, don't forget the nonuniform extension: #extension GL_EXT_nonuniform_qualifier : enable
 - If you statically index a unsized array, it will be converted to a sized one.
 - Set numbers must be properly increasing if you are using auto-generated shader layouts. Can't have set 1 if the shader does not define a set 0. Of course this is across all stages. (e.g. vertex have Set 1, fragment has Set 0, its valid.)
 - If you are writing to a swapchain, make sure you submit that draw to a primary queue.
 - Don't forget to set isMultithreaded flag if submitting queues from multiple threads (Vulkan and Metal).
 - And of course don't write to the same texture/swapchain from multiple queues.
 - Always bind unbounded desciptors to the last element of the set. You can only bind unbounded data one after another given they are different types of data.
 - Make sure you do not end descriptor names with "_". Can't end with "Smplr_" if targeting Metal.
 - Geometry shaders are not supported on Metal.
 - In Metal, you need to check Format support from Metal docs, along with CPU visible GPU memory considerations. (its gonna be 1 if device has unified memory, thats it).
 - Bindings need to be in order while creating descriptor sets.
 - Dynamic offsets need to adhere to buffer alignment requirements especially on DX12.
 - If you are using automatic shader pipeline layouts, in vulkan unsized arrays will use max available descriptors per stage. So you can't have the same type of descriptor anywhere in your shader.

UNSUPPORTED ON FIRST RELEASE:

 - Subpasses.
 - Multiple vertex buffer slot bindings.
 - Pipeline caching.
 - Indirect rendering count buffer.

*/

#pragma once

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
