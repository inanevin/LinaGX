macro(link_dxc)

   set(DIRNAME "dxc")
   set(LIBNAME "dxcompiler")

   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include)

   if(WIN32)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/Win64/$<CONFIGURATION>/${LIBNAME}.lib)
   endif(WIN32)

   if(APPLE)
   endif(APPLE)

   message("${PROJECT_NAME} -> ${DIRNAME} has been linked.")
endmacro()