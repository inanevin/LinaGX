#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# This file is a part of: LinaGX
# https://github.com/inanevin/LinaGX
# 
# Author: Inan Evin
# http://www.inanevin.com
# 
# The 2-Clause BSD License
# 
# Copyright (c) [2023-] Inan Evin
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
#    1. Redistributions of source code must retain the above copyright notice, this
#       list of conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright notice,
#       this list of conditions and the following disclaimer in the documentation
#       and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# Following modifications are made to the dependency libraries CMake projects:
# SPIRV-CROSS -> remove enable_testing() and disable if(SPIRV_CROSS_ENABLE_TESTS) (AND FALSE)
# SPIRV-CROSS -> CMake version bumped to 3.6.
# SPIRV-CROSS -> Comment out options CPP and CLI and TESTS
# glm -> CMake version bumped to 3.6. (main CMakeLists.txt, glm/CMakeLists.txt, CMake/glmconfig.cmake)
# Trust me, keeping this log is easier than managing fancy submodules.

if(WIN32)

	# DX12
	if(NOT LINAGX_DISABLE_DX12)
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
	endif()

	if(NOT LINAGX_DISABLE_VK)
		find_package(Vulkan REQUIRED FATAL_ERROR)

		target_link_libraries(${PROJECT_NAME} 
			PRIVATE Vulkan::Vulkan
		)
	endif()

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
		PUBLIC "-framework Carbon"
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
set_property(TARGET GenericCodeGen PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)
set_property(TARGET glslang-default-resource-limits PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)
set_property(TARGET glslang PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)
set_property(TARGET MachineIndependent PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)
set_property(TARGET OGLCompiler PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)
set_property(TARGET OSDependent PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)
set_property(TARGET SPIRV PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)
set_property(TARGET HLSL PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/glslang)

message("LinaGX -> glslang has been linked.")


add_subdirectory(Dependencies/SPIRV-Cross)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-c)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-core)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-glsl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-hlsl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-msl)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-reflect)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-util)
set_property(TARGET spirv-cross-c PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/spirv-cross)
set_property(TARGET spirv-cross-core PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/spirv-cross)
set_property(TARGET spirv-cross-glsl PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/spirv-cross)
set_property(TARGET spirv-cross-hlsl PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/spirv-cross)
set_property(TARGET spirv-cross-msl PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/spirv-cross)
set_property(TARGET spirv-cross-reflect PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/spirv-cross)
set_property(TARGET spirv-cross-util PROPERTY FOLDER ${LINAGX_FOLDER_BASE}/Dependencies/spirv-cross)
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

if(MSVC)
target_compile_options(SPIRV PRIVATE /W0)
else()
target_compile_options(SPIRV PRIVATE -w)
endif()