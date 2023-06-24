
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Examples/${PROJECT_NAME}/Resources/" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Resources/")

## IDE directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Examples/${PROJECT_NAME}/Resources/" "${CMAKE_BINARY_DIR}/Examples/${PROJECT_NAME}/Resources/")

## Copy windows dlls
if(WIN32)

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/_Vendor/bin/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>")

endif()