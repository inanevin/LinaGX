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
cmake_minimum_required (VERSION 3.8)
project(Common)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

if(WIN32)
set(SOURCES 
src/App.cpp
)

endif()

if(APPLE)
set(SOURCES 
src/App.mm
)
endif()

set(HEADERS

include/App.hpp
)


#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(LinaGX::ExamplesCommon ALIAS ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER LinaGXProject/Examples)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/ProjectSettings.cmake)

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

#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC Lina::GX
)

set(glm_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glm-0.9.9.8/cmake/glm/) 
find_package(glm REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)


if(APPLE)

target_link_libraries(${PROJECT_NAME}
PUBLIC "-framework AppKit"
)

endif()


#--------------------------------------------------------------------
# Folder Structuring 
#--------------------------------------------------------------------
include(${LINAGX_SOURCE_DIR}/CMake/ProjectStructure.cmake)

