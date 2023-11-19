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

# Following modifications are made to the dependency libraries CMake projects:
# SPIRV-CROSS -> remove enable_testing() and disable if(SPIRV_CROSS_ENABLE_TESTS) (AND FALSE)
# SPIRV-CROSS -> CMake version bumped to 3.6.
# glm -> CMake version bumped to 3.6. (main CMakeLists.txt, glm/CMakeLists.txt, CMake/glmconfig.cmake)
# Trust me, keeping this log is easier than managing fancy submodules.

if(WIN32)

# DX12
target_link_libraries(${PROJECT_NAME} 
	PRIVATE d3d12.lib
	PRIVATE dxgi.lib
	PRIVATE dxguid.lib
)

include(Dependencies/dxc/link.cmake)
include(Dependencies/nvapi/link.cmake)
include(Dependencies/pix/link.cmake)
link_dxc()
link_nvapi()
link_pix()


find_package(Vulkan REQUIRED FATAL_ERROR)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE Vulkan::Vulkan
)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC user32.lib
	PUBLIC Shcore.lib
)

endif()

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
set_property(TARGET GenericCodeGen PROPERTY FOLDER LinaGXProject/Dependencies/glslang)
set_property(TARGET glslang-default-resource-limits PROPERTY FOLDER LinaGXProject/Dependencies/glslang)
set_property(TARGET glslang PROPERTY FOLDER LinaGXProject/Dependencies/glslang)
set_property(TARGET MachineIndependent PROPERTY FOLDER LinaGXProject/Dependencies/glslang)
set_property(TARGET OGLCompiler PROPERTY FOLDER LinaGXProject/Dependencies/glslang)
set_property(TARGET OSDependent PROPERTY FOLDER LinaGXProject/Dependencies/glslang)
set_property(TARGET SPIRV PROPERTY FOLDER LinaGXProject/Dependencies/glslang)
set_property(TARGET HLSL PROPERTY FOLDER LinaGXProject/Dependencies/glslang)

message("LinaGX -> glslang has been linked.")


add_subdirectory(Dependencies/SPIRV-Cross)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-c)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-core)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-glsl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-hlsl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-msl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-reflect)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-util)
set_property(TARGET spirv-cross PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-c PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-core PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-glsl PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-hlsl PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-msl PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-reflect PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-util PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
set_property(TARGET spirv-cross-cpp PROPERTY FOLDER LinaGXProject/Dependencies/spvc)
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

