
add_executable( webgl
  main.cpp
  entities/player.cpp
  entities/laser.cpp
)
target_include_directories( 
  webgl PUBLIC
  ${PROJECT_SOURCE_DIR}
)
target_link_libraries( webgl engine )

if( NOT EMSCRIPTEN )  
  target_link_libraries( webgl engine SDL2::Main SDL2::Image OpenGL::OpenGL )
  install(TARGETS webgl
	  RUNTIME DESTINATION ${INSTALL_BIN}
	  LIBRARY DESTINATION ${INSTALL_LIB}
	  ARCHIVE DESTINATION ${INSTALL_LIB}
  )
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/engine/data DESTINATION ${INSTALL_DATA})
else()
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/webgl.js DESTINATION ./)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/webgl.wasm DESTINATION ./)
  install(FILES ${PROJECT_SOURCE_DIR}/src/html/index.html DESTINATION ${INSTALL_DATA})
endif()
