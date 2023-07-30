macro(link_glslang)
   set(DIRNAME "glslang")
   set(LIBNAME "glslang")
   
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/include)

   if(WIN32)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/GenericCodeGend.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/glslangd.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/glslang-default-resource-limitsd.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/MachineIndependentd.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/OGLCompilerd.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/SPIRVd.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC debug ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/OSDependentd.lib)

      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/GenericCodeGen.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/glslang.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/glslang-default-resource-limits.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/MachineIndependent.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/OGLCompiler.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/SPIRV.lib)
      target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${CMAKE_CURRENT_SOURCE_DIR}/_Vendor/${DIRNAME}/lib/Win64/$<CONFIGURATION>/OSDependent.lib)
      
   endif(WIN32)

   if(APPLE)
   endif(APPLE)

   message("${PROJECT_NAME} -> ${DIRNAME} has been linked.")

endmacro()

