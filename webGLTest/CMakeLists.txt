cmake_minimum_required(VERSION 3.10)
project( webGLTest )

set( CMAKE_CXX_STANDARD 17 ) 

if( CMAKE_COMPILER_IS_GNUCC )
  if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8 )
    # Can't get std::filesystem working on gcc 7, so just require 8
		message(FATAL_ERROR "Compiling under GCC requires gcc 8 or higher")
	else()
	  # Compiling with gcc, so need a library to get std::filesystem
	  set( EXTRA_SYSTEM_LIBS "stdc++fs" )
	endif()
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Emscripten" )
  set( EMSCRIPTEN TRUE )
endif()

set( INSTALL_BIN "./" )
set( INSTALL_LIB "./" )
set( INSTALL_DATA "./" )

if( EMSCRIPTEN )  
  # Fetch options
  # enable fetches, enable waitable fetches on all threads
  # PTHREADS support requires SharedArrayBuffer support in browser. Disabled in many as of ~Sep 2019 due to Spectre vulns
  # -s USE_PTHREADS=1
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FETCH=1" )

  # OpenGL options
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1 -s USE_SDL=2" )

  # Language and optimisation options
  # Exception catches will be disabled at higher optimisation levels by default, turn them back on but try not to abuse them due to inherent performance issues
  # ALLOW_MEMORY_GROWTH allows the heap to grow if needed, if not present we'll abort on large allocations (obviously need to optimise/test/limit caches and such)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++14 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy" )

  # Binding options
  # See https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#embind
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind" )
  
  # Embedded Data
  # Syntax is --embed-file /path/to/file@/destination/path/at/runtime
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --embed-file ${PROJECT_SOURCE_DIR}/src/shaders@/shaders" )

  # emscripten doesn't provide glm, fortunately it's header-only so we can just include the system version
  # TODO: This is hardcoded for now and points to another repo entirely, yay hacks!
  include_directories( "/home/gareth/source/junk/js-render-stuff/common/glm/" )
else()
  find_package( SDL2 REQUIRED )
  set( OpenGL_GL_PREFERENCE GLVND )
  find_package( OpenGL REQUIRED )
  find_package( glm REQUIRED )
  include_directories( ${SDL2_INCLUDE_DIRS} )
endif()  

add_executable( ${PROJECT_NAME}
	src/main.cpp
	src/geometry.cpp
	src/renderer.cpp
	src/shader.cpp
	src/shader_diffuse.cpp
	src/shader_phong.cpp
	)
target_link_libraries( ${PROJECT_NAME} ${EXTRA_SYSTEM_LIBS} )

if( NOT EMSCRIPTEN )
  target_link_libraries( ${PROJECT_NAME} ${SDL2_LIBRARIES} OpenGL::OpenGL )
  install(TARGETS ${PROJECT_NAME}
	  RUNTIME DESTINATION ${INSTALL_BIN}
	  LIBRARY DESTINATION ${INSTALL_LIB}
	  ARCHIVE DESTINATION ${INSTALL_LIB}
  )
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/shaders DESTINATION ${INSTALL_DATA})
else()
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js DESTINATION ./)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wasm DESTINATION ./)
  install(FILES ${PROJECT_SOURCE_DIR}/src/html/index.html DESTINATION ${INSTALL_DATA})
endif()


