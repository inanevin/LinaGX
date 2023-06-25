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
project(Common)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(COMMON_SOURCES 

src/WindowManager.cpp
src/App.cpp

)

set(COMMON_HEADERS

include/WindowManager.hpp
include/App.hpp
)


#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${COMMON_SOURCES} ${COMMON_HEADERS})
add_library(LinaGX::ExamplesCommon ALIAS ${PROJECT_NAME})

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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaGX/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Examples/_Vendor/glfw/include)

#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC Lina::GX
)

if (WIN32)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Examples/_Vendor/glfw/lib/Win64/$<CONFIGURATION>/glfw3.lib)
endif()
if(APPLE)
	#target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/Win64/$<CONFIGURATION>/${LIBNAME}.lib)
endif()


#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS COMMON_HEADERS COMMON_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${COMMON_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()