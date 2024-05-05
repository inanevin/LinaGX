macro(link_nvapi)

   set(DIRNAME "nvapi")
   set(LIBNAME "nvapi64")
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/${DIRNAME}/include)
   target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/${DIRNAME}/Win64/Debug/${LIBNAME}.lib optimized  ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/${DIRNAME}/Win64/Release/${LIBNAME}.lib)
   message("${PROJECT_NAME} -> ${LIBNAME} has been linked.")
endmacro()