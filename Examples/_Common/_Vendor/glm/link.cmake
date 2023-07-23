macro(link_glm)

   set(DIRNAME "glm")
   set(LIBNAME "glm_static")

   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Examples/_Common/_Vendor/${DIRNAME}/include)

   if(WIN32)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Examples/_Common/_Vendor/${DIRNAME}/Win64/$<CONFIGURATION>/${LIBNAME}.lib)
   endif(WIN32)

   if(APPLE)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Examples/_Common/_Vendor/${DIRNAME}/Apple/$<CONFIGURATION>/${LIBNAME}.a)
   endif(APPLE)

   message("${PROJECT_NAME} -> ${LIBNAME} has been linked.")
endmacro()