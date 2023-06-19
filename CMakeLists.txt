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

option(LINAGX_BUILD_EXAMPLES "Builds example projects." ON)

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
)


set(LinaGX_SOURCES

src/LinaGX.cpp

)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LinaGX_SOURCES} ${LinaGX_HEADERS})
add_library(Lina::GX ALIAS ${PROJECT_NAME})

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_MAJOR=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_MINOR=*)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_PATCH=0)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LinaGX_HEADERS LinaGX_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaVG_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()

if(LINAGX_BUILD_EXAMPLES)
	add_subdirectory(Examples/_Common)
	add_subdirectory(Examples/Introduction)
endif()



if(LINAGX_BUILD_EXAMPLES)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Introduction)
endif()

endif()