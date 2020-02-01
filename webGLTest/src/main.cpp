#include <stdio.h>
#include <SDL.h>
#include <GLES3/gl3.h>

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

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

SDL_Window* window = nullptr;
GLfloat windowWidth = 800;
GLfloat windowHeight = 600;
SDL_Surface *screen = nullptr;
static SDL_GLContext gl_context;

glm::vec4 diffuseColour(1.f,1.f,1.f,1.f);

glm::vec3 viewRot(1.0f);
glm::vec3 viewRotDelta(0.5f,0.7f,1.0f);

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;

struct FetchData {
	std::string data;
	bool ready = false;
};

#ifdef __EMSCRIPTEN__
  std::map<emscripten_fetch_t*, FetchData> currentFetches;
  
  // Function hooks to javascript
	void webgltest_jshook_slider_colour_red( int v ) { diffuseColour.r = static_cast<float>(v) / 255.f; }
	void webgltest_jshook_slider_colour_green( int v ) { diffuseColour.g = static_cast<float>(v) / 255.f; }
	void webgltest_jshook_slider_colour_blue( int v ) { diffuseColour.b = static_cast<float>(v) / 255.f; }
	void webgltest_jshook_slider_colour_alpha( int v ) { diffuseColour.a = static_cast<float>(v) / 255.f; }
	
  EMSCRIPTEN_BINDINGS(webGLTest) {
		emscripten::function("slider_colour_red", &webgltest_jshook_slider_colour_red);
		emscripten::function("slider_colour_green", &webgltest_jshook_slider_colour_green);
		emscripten::function("slider_colour_blue", &webgltest_jshook_slider_colour_blue);
		emscripten::function("slider_colour_alpha", &webgltest_jshook_slider_colour_alpha);
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

GLuint vertexBuffer, shaderProgram;
GLint shaderAttribute_vertCoord;
GLint shaderUniform_mvp;
GLint shaderUniform_diffuseColour;

struct Vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
std::vector<Vertex> vertices;

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

bool checkErrorCompileShader(GLint shader) {
	GLint compileSuccess = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
	if( compileSuccess == GL_FALSE ) {
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> log(static_cast<std::vector<GLchar>::size_type>(logLength));
		glGetShaderInfoLog(shader, logLength, &logLength, &log[0]);
		glDeleteShader(shader);

		std::cerr << "Failed to link shader: " << std::string(reinterpret_cast<const char*>(&log[0])) << std::endl;		
		return false;
	}
	return true;
}

/**
 * Load and compile a shader
 * @param program Shader program to attach source to
 * @param shader Path to shader source. Shader type will be determined by extension (.vert, .frag)
 * @return Shader ID, or zero on error
 */
GLuint loadShader(std::string shader) {
	std::filesystem::path p(shader);
	
#if defined(__EMSCRIPTEN__) && defined(TODO_MANAGED_TO_GET_EM_WGET_TO_WORK)
  // TODO: This concept works, but emscripten_wget doesn't - will need some more compile flags and might not be possible due to browser restrictions
  // If we don't have access to the file, try to load from the server hosting us
	if( !std::filesystem::exists(p) ) {
		auto hrefStr = js_getWindowHref();
		if( hrefStr ) {
			std::string shaderUrl = hrefStr;
			std::free(hrefStr);
						
			std::regex reg(R"(^(([^:\/?#]+):)?(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)", std::regex::extended);
			std::smatch sm;
			if( std::regex_search(shaderUrl, sm, reg) && sm.size() >= 5 ) {
				auto counter = 0;
				for (const auto& res : sm) {
					std::cout << counter++ << ": " << res << std::endl;
				}
				
				shaderUrl = std::string(sm[1]) + "//" + std::string(sm[4]) + "/";
			} else {
				std::cerr << "loadShader: Failed to determine server base url" << std::endl;
				return 0;
			}
			
			shaderUrl += "/" + shader;
			std::cerr << "loadShader: File not found, attempting to fetch from server: " << shaderUrl << " -> " << shader << std::endl;
			emscripten_wget(shaderUrl.c_str(), shader.c_str());
		} else {
			std::cerr << "loadShader: Unable to fetch shader, failed to determine server base url" << std::endl;
			return 0;
		}
	}
#endif	
	
	GLenum shaderType;
	if( p.extension().string() == ".vert" ) shaderType = GL_VERTEX_SHADER;
	else if( p.extension().string() == ".frag" ) shaderType = GL_FRAGMENT_SHADER;
	else { 
		std::cerr << "loadShader: Unknown shader type: " << p.extension().string() << std::endl; 
		return 0; 
	}
	
	// if( std::filesystem::exists(p) ) std::cerr << "loadShader: Shader seems to exist: " << shader << std::endl;
	std::ifstream shaderFile( shader );
	if( !shaderFile.is_open() ) { 
		std::cerr << "loadShader: Failed to find shader source: " << shader << std::endl;
		return 0; 
	}
	
	std::string buf( (std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	auto result = glCreateShader(shaderType);
	auto* bufP = buf.c_str();
	glShaderSource(result, 1, &bufP, nullptr);
	glCompileShader(result);
	
	if( !checkErrorCompileShader(result) ) {
		glDeleteShader(result);
		return 0;
	}
	
	return result;
}

void initialiseGLData() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	
	// Vertex data TODO: Placeholder
	{
		vertices = {			
			{-1.0f,-1.0f,-1.0f},
			{-1.0f,-1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f,-1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f, 1.0f},
			{-1.0f,-1.0f,-1.0f},
			{1.0f,-1.0f,-1.0f},
			{1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f,-1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f, 1.0f},
			{-1.0f,-1.0f, 1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f, 1.0f, 1.0f},
			{-1.0f,-1.0f, 1.0f},
			{1.0f,-1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f,-1.0f,-1.0f},
			{1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f,-1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f,-1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f,-1.0f},
			{-1.0f, 1.0f,-1.0f},
			{1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f,-1.0f},
			{-1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f},
			{1.0f,-1.0f, 1.0f},
		};
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
	}
	
	// Shader program
	{
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, loadShader("shaders/vertex/default.vert"));
		glAttachShader(shaderProgram, loadShader("shaders/fragment/default.frag"));
		glLinkProgram(shaderProgram);
		
		shaderAttribute_vertCoord = glGetAttribLocation(shaderProgram, "vertCoord");
		glEnableVertexAttribArray(shaderAttribute_vertCoord);
		glVertexAttribPointer(shaderAttribute_vertCoord, 3, GL_FLOAT, false, sizeof(Vertex), static_cast<const void*>(0));
    
    shaderUniform_mvp = glGetUniformLocation(shaderProgram, "mvp");
    shaderUniform_diffuseColour = glGetUniformLocation(shaderProgram, "diffuseColour");
	}
}

#ifndef __EMSCRIPTEN
void endApplication() {
  SDL_DestroyWindow(window);
  SDL_Quit();
  std::exit(0);
}
#endif

void render() {
	auto now = std::chrono::high_resolution_clock::now();
	
	float renderDelta = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(now - lastFrameTime).count()) / 1e6f;
	lastFrameTime = now;
	
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
  
  viewRot += renderDelta * viewRotDelta;
  if( viewRot.x > 2 * 3.14 ) viewRot.x = 0.0f;
  if( viewRot.y > 2 * 3.14 ) viewRot.y = 0.0f;
  if( viewRot.z > 2 * 3.14 ) viewRot.z = 0.0f;
  
  // glm::mat4 projMat = glm::ortho( -180.0f, 180.0f, -90.0f, 90.0f, 1.f, -1.f );
  glm::mat4 projMat = glm::perspective( 90.0f, static_cast<GLfloat>(windowWidth) / static_cast<GLfloat>(windowHeight), 0.1f, 100.0f );
  glm::mat4 viewMat = glm::lookAt( glm::vec3(0.0, 0.0, 2.0), glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0) );
  viewMat = glm::rotate( viewMat, viewRot.x, glm::vec3(1.0, 0.0, 0.0) );
  viewMat = glm::rotate( viewMat, viewRot.y, glm::vec3(0.0, 1.0, 0.0) );
  viewMat = glm::rotate( viewMat, viewRot.z, glm::vec3(0.0, 0.0, 1.0) );
  auto mvp = projMat * viewMat /* * modelMat */; 
  
  SDL_GL_MakeCurrent(window, gl_context);
  
  glViewport(0,0,windowWidth,windowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(shaderProgram);
  glUniformMatrix4fv(shaderUniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform4fv(shaderUniform_diffuseColour, 1, glm::value_ptr(diffuseColour));
  
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  
  SDL_GL_SwapWindow(window);

#ifdef __EMSCRIPTEN__
	auto it = currentFetches.begin();
	while( it != currentFetches.end() ) {
		auto& fetchOp = it->first;
		auto& fetchData = it->second;
		
		if( !fetchData.ready ) { ++it; continue; }
		/*
		// Specifies the readyState of the XHR request:
        // 0: UNSENT: request not sent yet
        // 1: OPENED: emscripten_fetch has been called.
        // 2: HEADERS_RECEIVED: emscripten_fetch has been called, and headers and status are available.
        // 3: LOADING: download in progress.
        // 4: DONE: download finished.
        // See https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
		if (fetchOp->readyState != 4) { ++it; continue; }
		*/
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
#endif
}


int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	
  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
#ifdef __EMSCRIPTEN__  
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
#else  
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif

  SDL_Renderer* renderer = nullptr;
  SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_OPENGL, &window, &renderer);

  gl_context = SDL_GL_CreateContext(window);
  
	auto glVer = glGetString(GL_VERSION);
	std::cout << "OpenGL initialised: " << glVer << std::endl;
  
  if( !shapes.empty() ) currentShape = *shapes.begin();

  initialiseGLData();

	startTime = std::chrono::high_resolution_clock::now();

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

