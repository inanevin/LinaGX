macro(link_spirv)

   set(DIRNAME "spirv-cross")
   set(LIBNAME "spirv-cross")
   
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/include)

   if(WIN32)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-cd.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-cored.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-glsld.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-hlsld.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-msld.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-reflectd.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-utild.lib)

      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-c.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-core.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-glsl.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-hlsl.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-msl.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-reflect.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/spirv-cross-util.lib)
      
   endif(WIN32)

   if(APPLE)
   endif(APPLE)

   message("${PROJECT_NAME} -> ${DIRNAME} has been linked.")

endmacro()

