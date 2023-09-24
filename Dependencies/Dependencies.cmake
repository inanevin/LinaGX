#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Author: Inan Evin
# www.inanevin.com
# 
# Copyright (C) 2022 Inan Evin
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
# and limitations under the License.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

if(WIN32)

# DX12
target_link_libraries(${PROJECT_NAME} 
	PRIVATE d3d12.lib
	PRIVATE dxgi.lib
	PRIVATE dxguid.lib
)

include(Dependencies/dxc/link.cmake)
include(Dependencies/nvapi/link.cmake)
link_dxc()
link_nvapi()

find_package(Vulkan REQUIRED FATAL_ERROR)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE Vulkan::Vulkan
)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC user32.lib
	PUBLIC Shcore.lib
)

endif()

add_subdirectory(Dependencies/easy_profiler)
target_link_libraries(${PROJECT_NAME} PUBLIC easy_profiler)  # STEP 2 ##########

if(APPLE)

target_link_libraries(${PROJECT_NAME}
PUBLIC "-framework Metal"
PUBLIC "-framework MetalKit"
PUBLIC "-framework AppKit"
PUBLIC "-framework Foundation"
PUBLIC "-framework QuartzCore"
)
endif()

add_subdirectory(Dependencies/glslang-12.2.0)
target_link_libraries(${PROJECT_NAME} PUBLIC GenericCodeGen)
target_link_libraries(${PROJECT_NAME} PUBLIC glslang-default-resource-limits)
target_link_libraries(${PROJECT_NAME} PUBLIC glslang)
target_link_libraries(${PROJECT_NAME} PUBLIC MachineIndependent)
target_link_libraries(${PROJECT_NAME} PUBLIC OGLCompiler)
target_link_libraries(${PROJECT_NAME} PUBLIC OSDependent)
target_link_libraries(${PROJECT_NAME} PUBLIC SPIRV)
message("LinaGX -> glslang has been linked.")


add_subdirectory(Dependencies/SPIRV-Cross)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-c)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-core)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-glsl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-hlsl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-msl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-reflect)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-util)
message("LinaGX -> spirv-cross has been linked.")

target_compile_definitions(GenericCodeGen PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(glslang-default-resource-limits PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(glslang PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(MachineIndependent PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(OGLCompiler PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(OSDependent PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(SPIRV PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})

target_compile_definitions(spirv-cross-c PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(spirv-cross-core PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(spirv-cross-glsl PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(spirv-cross-hlsl PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(spirv-cross-msl PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(spirv-cross-reflect PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})
target_compile_definitions(spirv-cross-util PUBLIC _ITERATOR_DEBUG_LEVEL=${LINAGX_ITERATOR_DEBUG_LEVEL})

