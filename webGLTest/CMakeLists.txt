cmake_minimum_required(VERSION 3.10)
project( webGLTest )

set( CMAKE_CXX_STANDARD 17 )

if( CMAKE_SYSTEM_NAME MATCHES "Emscripten" )
  set( EMSCRIPTEN TRUE )
endif()

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
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++14 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1" )

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
	)

if( NOT EMSCRIPTEN )
  target_link_libraries( ${PROJECT_NAME} ${SDL2_LIBRARIES} OpenGL::OpenGL )
endif()

