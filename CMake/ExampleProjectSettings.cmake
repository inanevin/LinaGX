set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER LinaGXProject/Examples)

if(WIN32)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/$<CONFIGURATION>/")

endif()

if(APPLE)

set_target_properties(${PROJECT_NAME} PROPERTIES 
                      XCODE_GENERATE_SCHEME TRUE
                      XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/$<CONFIGURATION>"
                      XCODE_ATTRIBUTE_IOS_DEPLOYMENT_TARGET "11.0"
                      XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"
                      )

endif()


add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources/" "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/$<CONFIGURATION>/Resources/")

## Copy windows dlls
if(WIN32)
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../../Dependencies/bin/" "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/$<CONFIGURATION>/")

endif()

if(APPLE)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${LINAGX_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist
)

endif()