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
cmake_minimum_required (VERSION 3.6)
project(LinaGX)
set(CMAKE_CXX_STANDARD 17)

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

option(LINAGX_BUILD_EXAMPLES "Builds example projects." OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------


set(LinaGX_HEADERS
include/LinaGX.hpp
include/LinaGXExports.hpp
include/Common/Common.hpp
include/Core/Backend.hpp
include/Core/Renderer.hpp
include/Core/CommandStream.hpp
include/Core/Commands.hpp
include/Core/WindowManager.hpp
include/Utility/SPIRVUtility.hpp
)

set(LinaGX_SOURCES
src/Common/Common.cpp
src/Core/Backend.cpp
src/Core/Renderer.cpp
src/Core/CommandStream.cpp
src/Core/WindowManager.cpp
src/Utility/SPIRVUtility.cpp
)


if(WIN32)

set(LinaGX_PLATFORM_HEADERS

# Windows
include/Platform/Windows/Win32Window.hpp

# Vulkan
include/Platform/Vulkan/VKBackend.hpp
include/Platform/Vulkan/SDK/VkBootstrap.h
include/Platform/Vulkan/SDK/VkBootstrapDispatch.h
include/Platform/Vulkan/SDK/vk_mem_alloc.h

## DX12 
include/Platform/DX12/DX12Backend.hpp
include/Platform/DX12/DX12HeapStaging.hpp
include/Platform/DX12/DX12HeapGPU.hpp
include/Platform/DX12/DX12Common.hpp
include/Platform/DX12/SDK/ID3DIncludeInterface.hpp
include/Platform/DX12/SDK/d3d12.h
include/Platform/DX12/SDK/d3d12compatibility.h
include/Platform/DX12/SDK/d3d12sdklayers.h
include/Platform/DX12/SDK/d3d12shader.h
include/Platform/DX12/SDK/d3d12video.h
include/Platform/DX12/SDK/d3dcommon.h
include/Platform/DX12/SDK/d3dx12.h
include/Platform/DX12/SDK/d3dx12_barriers.h
include/Platform/DX12/SDK/d3dx12_check_feature_support.h
include/Platform/DX12/SDK/d3dx12_core.h
include/Platform/DX12/SDK/d3dx12_default.h
include/Platform/DX12/SDK/d3dx12_pipeline_state_stream.h
include/Platform/DX12/SDK/d3dx12_property_format_table.h
include/Platform/DX12/SDK/d3dx12_render_pass.h
include/Platform/DX12/SDK/d3dx12_resource_helpers.h
include/Platform/DX12/SDK/d3dx12_root_signature.h
include/Platform/DX12/SDK/d3dx12_state_object.h
include/Platform/DX12/SDK/dxcore.h
include/Platform/DX12/SDK/dxcore_interface.h
include/Platform/DX12/SDK/dxgicommon.h
include/Platform/DX12/SDK/dxgiformat.h
include/Platform/DX12/SDK/Residency/d3dx12Residency.h
include/Platform/DX12/SDK/D3D12MemAlloc.h

)

set(LinaGX_PLATFORM_SOURCES

# Windows
src/Platform/Windows/Win32Window.cpp

# Vulkan
src/Platform/Vulkan/VKBackend.cpp
src/Platform/Vulkan/SDK/VkBootstrap.cpp

# DX12
src/Platform/DX12/DX12Backend.cpp
src/Platform/DX12/DX12HeapStaging.cpp
src/Platform/DX12/DX12HeapGPU.cpp
src/Platform/DX12/SDK/D3D12MemAlloc.cpp
src/Platform/DX12/SDK/ID3DIncludeInterface.cpp
)

endif()

if(APPLE)

set(LinaGX_PLATFORM_HEADERS
include/Platform/Metal/MTLBackend.hpp
include/Platform/MacOS/MacOSWindow.hpp
)

set(LinaGX_PLATFORM_SOURCES
src/Platform/Metal/MTLBackend.cpp
src/Platform/MacOS/MacOSWindow.cpp
)

endif()

#--------------------------------------------------------------------
# Create project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LinaGX_SOURCES} ${LinaGX_HEADERS} ${LinaGX_PLATFORM_SOURCES} ${LinaGX_PLATFORM_HEADERS})
add_library(Lina::GX ALIAS ${PROJECT_NAME})
include(${CMAKE_SOURCE_DIR}/CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_MAJOR=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_MINOR=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_PATCH=0)

if(WIN32)

# DX12
target_link_libraries(${PROJECT_NAME} 
	PRIVATE d3d12.lib
	PRIVATE dxgi.lib
	PRIVATE dxguid.lib
)

include(_Vendor/dxc/link.cmake)
link_dxc()

find_package(Vulkan REQUIRED FATAL_ERROR)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE Vulkan::Vulkan
)

endif()

include(_Vendor/glslang/link.cmake)
include(_Vendor/spirv-cross/link.cmake)
link_glslang()
link_spirv()

if(LINAGX_BUILD_EXAMPLES)
	add_subdirectory(Examples/_Common)
	add_subdirectory(Examples/Introduction)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Introduction)
endif()

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LinaGX_HEADERS LinaGX_SOURCES LinaGX_PLATFORM_HEADERS LinaGX_PLATFORM_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaVG_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()

endif()

