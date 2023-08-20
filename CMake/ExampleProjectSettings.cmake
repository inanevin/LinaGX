set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER LinaGXProject/Examples)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/$<CONFIGURATION>/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources/" "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/$<CONFIGURATION>/Resources/")

## Copy windows dlls
if(WIN32)
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../../_Vendor/bin/" "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/$<CONFIGURATION>/")

endif()