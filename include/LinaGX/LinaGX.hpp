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

/*

THINGS TO RESEARCH:

- gl_InstanceIndex and alike and their support.

INDIRECT NOTES:

- Need to use IndexedIndirectCommand or IndirectCommand structure.
- In GLSL, use gl_DrawID to index into any custom buffers per-draw.
- In Vulkan, gl_DrawID will always be 0 if multiDraw feature is not supported. In DX12 and Metal, it will be set to the index of the active draw.
- In cases where gl_DrawID is not supported in Vulkan or not desired, need to call DrawIndirect commands one by one while setting your custom buffer bindings.

NOTES TO DOCUMENT:

 - Metal no runtime texture format support: https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
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
