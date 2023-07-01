macro(link_nvapi)

   set(DIRNAME "nvapi")
   set(LIBNAME "nvapi64")

   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/include)

   if(WIN32)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/Win64/$<CONFIGURATION>/${LIBNAME}.lib)
   endif(WIN32)

   if(APPLE)
   endif(APPLE)

   message("${PROJECT_NAME} -> ${LIBNAME} has been linked.")
endmacro()