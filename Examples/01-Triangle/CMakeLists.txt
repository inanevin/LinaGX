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
project(01-Triangle)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(01-TRIANGLE_SOURCES 
src/Main.cpp
src/Example.cpp
)

set(01-TRIANGLE_HEADERS
include/Main.hpp
include/Example.hpp
Resources/Shaders/triangle_frag.glsl
Resources/Shaders/triangle_vert.glsl
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${01-TRIANGLE_SOURCES} ${01-TRIANGLE_HEADERS})

include(${CMAKE_SOURCE_DIR}/CMake/ProjectSettings.cmake)

set_target_properties(
    ${PROJECT_NAME}
      PROPERTIES 
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
)

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Examples/_Common/include)


#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC LinaGX::ExamplesCommon
)
include(${CMAKE_SOURCE_DIR}/CMake/CopyResources.cmake)


#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS 01-TRIANGLE_HEADERS 01-TRIANGLE_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${01-TRIANGLE_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()