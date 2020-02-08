
file( GLOB_RECURSE DATA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/* )

add_library( engine STATIC
	geometry/geometry.cpp
	geometry/objmodel.cpp
	renderer.cpp
	shaders/shader.cpp
	shaders/shader_phong.cpp
	shaders/shader_lightinterface.cpp
	shaders/shader_materialinterface.cpp
	textures/texture.cpp
	textures/texture_sdl2image.cpp
)
target_include_directories( 
  engine PUBLIC
  ${PROJECT_SOURCE_DIR}
)
target_link_libraries( engine ${EXTRA_SYSTEM_LIBS} )

if( NOT EMSCRIPTEN )
  target_link_libraries( engine SDL2::Main SDL2::Image OpenGL::OpenGL )
endif()
