#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

#include <GLES3/gl3.h>

#ifndef __EMSCRIPTEN__
// GLES 3.2 on desktop, for debug extensions and such
# include <GLES3/gl32.h>
#endif

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
# include <emscripten/fetch.h>
# include <emscripten/bind.h>
#endif

#include <iostream>
#include <fstream>
#include <future>
#include <thread>
#include <chrono>
#include <cmath>
#include <atomic>
#include <list>
#include <map>
#include <vector>
#include <regex>
#include <experimental/filesystem>
#include <memory>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "renderer.h"
#include "shaders/shader_phong.h"
#include "textures/texture_sdl2image.h"
#include "geometry/objmodel.h"

std::unique_ptr<Renderer> renderer;
std::shared_ptr<Geometry> geometry;
std::shared_ptr<Shader_phong> shaderPhong;
std::shared_ptr<Texture_SDL2Image> nullDiffuse;

struct FetchData {
	std::string data;
	bool ready = false;
};

#ifdef __EMSCRIPTEN__
  std::map<emscripten_fetch_t*, FetchData> currentFetches;
  
  // Function hooks to javascript
	void jshook_slider_colour_red( int v ) { geometry->material().diffuse.r = static_cast<float>(v) / 255.f; }
	void jshook_slider_colour_green( int v ) { geometry->material().diffuse.g = static_cast<float>(v) / 255.f; }
	void jshook_slider_colour_blue( int v ) { geometry->material().diffuse.b = static_cast<float>(v) / 255.f; }
	void jshook_slider_colour_alpha( int v ) { geometry->material().alpha = static_cast<float>(v) / 255.f; }
  void jshook_checkbox_use_texture( bool b ) { if( b ) { geometry->textures().diffuse = nullDiffuse; } else { geometry->textures().diffuse.reset(); } }
  void jshook_picker_colour_ambient( int r, int g, int b ) {
		geometry->material().ambient.r = (float)r / 255.f;
		geometry->material().ambient.g = (float)g / 255.f;
		geometry->material().ambient.b = (float)b / 255.f;
	}
  void jshook_picker_colour_diffuse( int r, int g, int b ) {
		geometry->material().diffuse.r = (float)r / 255.f;
		geometry->material().diffuse.g = (float)g / 255.f;
		geometry->material().diffuse.b = (float)b / 255.f;
	}
	void jshook_picker_colour_specular( int r, int g, int b ) {
		geometry->material().specular.r = (float)r / 255.f;
		geometry->material().specular.g = (float)g / 255.f;
		geometry->material().specular.b = (float)b / 255.f;
	}
	void jshook_slider_shininess( int s ) {
		geometry->material().shininess = s;
	}
	
  EMSCRIPTEN_BINDINGS(webGLTest) {
		emscripten::function("slider_colour_red", &jshook_slider_colour_red);
		emscripten::function("slider_colour_green", &jshook_slider_colour_green);
		emscripten::function("slider_colour_blue", &jshook_slider_colour_blue);
		emscripten::function("slider_colour_alpha", &jshook_slider_colour_alpha);
    emscripten::function("checkbox_use_texture", &jshook_checkbox_use_texture);
    
    emscripten::function("picker_colour_ambient", &jshook_picker_colour_ambient);
    emscripten::function("picker_colour_diffuse", &jshook_picker_colour_diffuse);
    emscripten::function("picker_colour_specular", &jshook_picker_colour_specular);
    
    emscripten::function("slider_shininess", &jshook_slider_shininess);
  }
  
  /**
   * Get the base url of the page. Used when loading resources from the server.
   * @return The base url of the page, utf8 encoded. The caller must call free() on the result.
   */
	EM_JS(char*, js_getWindowHref, (), {
		var jsString = window.location.href;
		if( jsString == null ) return null;
		// 'jsString.length' would return the length of the string as UTF-16
		// units, but Emscripten C strings operate as UTF-8.
		var lengthBytes = lengthBytesUTF8(jsString)+1;
		var stringOnWasmHeap = _malloc(lengthBytes);
		stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
		return stringOnWasmHeap;
	});
#endif

std::list<std::string> shapes;/* = {
  "110m_physical%2fne_110m_geography_regions_polys.shp",
  "110m_physical%2fne_110m_land.shp",
};*/

std::string currentShape;

#ifdef __EMSCRIPTEN__
void downloadSucceeded(emscripten_fetch_t* fetch) {
  std::cout << "downloadSucceeded in thread " << std::this_thread::get_id() << std::endl;
	std::cout << "Downloaded " << fetch->numBytes << " bytes from " << fetch->url << std::endl;
	// The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
	//fetchPayload.assign(static_cast<const char*>(fetch->data), fetch->numBytes);
	// Cleanup
	//emscripten_fetch_close(fetch);
	
	currentFetches[fetch].data.assign(static_cast<const char*>(fetch->data), fetch->numBytes);
	currentFetches[fetch].ready = true;
}

void downloadFailed(emscripten_fetch_t* fetch) {
	std::cout << "Download failed with code " << fetch->status << " " << fetch->url << std::endl;

	//try { throw std::runtime_error("Fetch timed out"); }
	//catch (...) { fetchPayload.set_exception(std::current_exception()); }
	// Cleanup
//	emscripten_fetch_close(fetch);
currentFetches[fetch].ready = true;
}

void downloadProgress(emscripten_fetch_t* fetch) {
	  if (fetch->totalBytes) {
    printf("Downloading %s.. %.2f%% complete.\n", fetch->url, fetch->dataOffset * 100.0 / fetch->totalBytes);
  } else {
    printf("Downloading %s.. %lld bytes complete.\n", fetch->url, fetch->dataOffset + fetch->numBytes);
  }
}
#endif

#ifndef __EMSCRIPTEN
void endApplication() {
  SDL_DestroyWindow(renderer->window());
  SDL_Quit();
  std::exit(0);
}
#endif

void render() {
#ifndef __EMSCRIPTEN__
  // Get the next event
  SDL_Event event;
  if(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      endApplication();
    }
  }
#endif
	renderer->render();

/*
#ifdef __EMSCRIPTEN__
	auto it = currentFetches.begin();
	while( it != currentFetches.end() ) {
		auto& fetchOp = it->first;
		auto& fetchData = it->second;
		
		if( !fetchData.ready ) { ++it; continue; }
		
		// Specifies the readyState of the XHR request:
        // 0: UNSENT: request not sent yet
        // 1: OPENED: emscripten_fetch has been called.
        // 2: HEADERS_RECEIVED: emscripten_fetch has been called, and headers and status are available.
        // 3: LOADING: download in progress.
        // 4: DONE: download finished.
        // See https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
		//if (fetchOp->readyState != 4) { ++it; continue; }
		
		if (fetchOp->status == 200) {
			printf("Finished downloading %llu bytes from URL %s.\n", fetchOp->numBytes, fetchOp->url);
			// The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
			
			std::string fetchPayload( fetchOp->data, fetchOp->numBytes );
					  
		  std::regex reg("\\s+");
		  std::sregex_token_iterator rit(fetchPayload.begin(), fetchPayload.end(), reg, -1);
		  std::sregex_token_iterator rend;
		  vertices.clear();
		  while( rit != rend ) {
        try {
			    GLfloat x = std::stof(*rit++);
          if( rit == rend ) continue;
			    GLfloat y = std::stof(*rit++);
          vertices.push_back( {x,y} );
        } catch ( std::logic_error& e ) {
          std::cout << "Vertex Parse Error: " << e.what() << " : " << fetchPayload << std::endl;
        }
		  }
		  
		  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
		  
		} else {
			printf("Downloading %s failed, HTTP failure status code: %d.\n", fetchOp->url, fetchOp->status);
		}

		emscripten_fetch_close(fetchOp);
		it = currentFetches.erase(it);
	}

  if( !currentFetches.empty() ) return;

	static uint32_t loopIter = 0;
	loopIter++;
	// TODO: Hacky testing of the fetch api, this should be some background thread
	// that grabs new data when needed or something
	if( loopIter < 100 ) return;
  
  if( !shapes.empty() ) {
		auto nextShapeToLoad = std::find(shapes.begin(), shapes.end(), currentShape);
		if( nextShapeToLoad == shapes.end() ) nextShapeToLoad = shapes.begin();
		else if( ++nextShapeToLoad == shapes.end() ) nextShapeToLoad = shapes.begin();
		currentShape = *nextShapeToLoad;
		loopIter = 0;
		
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		strcpy(attr.requestMethod, "GET");
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
		attr.onsuccess = downloadSucceeded;
		attr.onerror = downloadFailed;
		attr.onprogress = downloadProgress;
		std::string url = "http://localhost:3000/data/";
		url.append(currentShape);	
		currentFetches[ emscripten_fetch(&attr, url.c_str())] = {};
	}
#endif */
}

#ifndef __EMSCRIPTEN__
void openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam){
 
    std::cout << "---------------------opengl-callback-start------------" << std::endl;
    std::cout << "message: "<< message << std::endl;
    std::cout << "type: ";
    
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "OTHER";
        break;
    }
    std::cout << std::endl;
 
    std::cout << "id: " << id << std::endl;
    std::cout << "severity: ";
    switch (severity){
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "HIGH";
        break;
    }
    std::cout << std::endl;
    std::cout << "---------------------opengl-callback-end--------------" << std::endl;
}
#endif

int main(int argc, char** argv) {
  if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "Failed to initialise SDL Video" << std::endl;
		return EXIT_FAILURE;
	}

  if( !(IMG_Init(IMG_INIT_PNG) &
        (IMG_INIT_PNG) ) ) {
		std::cerr << "Failed to initialise SDL Image" << std::endl;
		return EXIT_FAILURE;
	}

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	
  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
#ifdef __EMSCRIPTEN__  
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
#else  
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  // TODO: Only in debug builds
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

  renderer.reset(new Renderer(800, 600));

#ifndef __EMSCRIPTEN__
  if(glDebugMessageCallback){
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            &unusedIds,
            true);
    }
    else
        std::cout << "glDebugMessageCallback not available" << std::endl;
#endif

  if( !shapes.empty() ) currentShape = *shapes.begin();

  shaderPhong.reset(new Shader_phong());
  Light testLight1;
  testLight1.position = {-3.0f,-3.0f,2.0f};
  testLight1.colour = {1.0f,0.0f,0.0f};
  testLight1.intensity = {0.2f,1.0f,0.8f};
  testLight1.falloff = 100.0f;
  testLight1.radius = 10.0f;
  shaderPhong->lights().emplace_back( testLight1 );
  
  Light testLight2;
  testLight2.position = {3.0f,-3.0f,2.0f};
  testLight2.colour = {0.0f,1.0f,0.0f};
  testLight2.intensity = {0.2f,1.0f,0.8f};
  testLight2.falloff = 100.0f;
  testLight2.radius = 10.0f;  
  shaderPhong->lights().emplace_back( testLight2 );

  Light testLight3;
  testLight3.position = {0.0f,3.0f,2.0f};
  testLight3.colour = {0.0f,0.0f,1.0f};
  testLight3.intensity = {0.2f,1.0f,0.8f};
  testLight3.falloff = 100.0f;
  testLight3.radius = 10.0f;  
  shaderPhong->lights().emplace_back( testLight3 );
  
  renderer->shaders()["phong"] = shaderPhong;
  
  nullDiffuse.reset(new Texture_SDL2Image("data/textures/diffuse/scrunched-paper.png"));
  renderer->textures()["null"] = nullDiffuse;
  
  //geometry.reset(new Geometry());
  geometry.reset(new ObjModel("data/models/primitives/cube/cube.obj"));
  geometry->shader(shaderPhong);
  geometry->textures().diffuse = nullDiffuse;
  renderer->geometry().push_back(geometry);

  // Upload data to gpu, init etc
  // - compile/link shaders
  // - upload textures in renderer->textures
  // - upload geometry in renderer->geometry
  renderer->initialiseGLData();
  
// Fire the main loop. Won't quit this until SDL does in some form
#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
  emscripten_set_main_loop(render, 60, 1);
#else
  while( true ) {
	  render();
	  // TODO: Framerate throttling/vsync
  }
#endif  

  // TODO: All the cleanup and stuff
  // SDL_Quit();
  
  return 0;
}

