#include <stdio.h>
#include <SDL.h>
#include <GLES3/gl3.h>

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
# include <emscripten/fetch.h>
# include <emscripten/bind.h>
#endif

#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <cmath>
#include <atomic>
#include <list>
#include <map>
#include <vector>
#include <regex>

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
glm::vec3 viewRotDelta(1.0f,1.0f,1.0f);

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

std::string shaderSource_Vertex = 
"#version 300 es\n"
"uniform mat4 mvp;\n"
"in vec3 vertCoord;\n"
"void main(void) { \n"
"gl_Position = mvp * vec4(vertCoord, 1.0);\n"
"}\n"
"";

std::string shaderSource_Fragment =
"#version 300 es\n"
"uniform mediump vec4 diffuseColour;\n"
"out mediump vec4 fragColour;\n"
"void main(void) {\n"
"fragColour = diffuseColour;\n"
"}\n"
"";

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

void checkErrorCompileShader(GLint shader) {
	GLint compileSuccess = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
	if( compileSuccess == GL_FALSE ) {
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> log(static_cast<std::vector<GLchar>::size_type>(logLength));
		glGetShaderInfoLog(shader, logLength, &logLength, &log[0]);
		glDeleteShader(shader);

		std::cout << "Failed to link shader: " << std::string(reinterpret_cast<const char*>(&log[0])) << std::endl;		
	}
}

void initialiseGLData() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	
	// Vertex data TODO: Placeholder triangle
	{
		vertices = {
			// bottom
			/*{-1.0,-1.0,-1.0},
			{1.0,-1.0,-1.0},
			{-1.0,-1.0,1.0},
			{-1.0,-1.0,1.0},
			{1.0,-1.0,-1.0},
			{1.0,-1.0,1.0},*/
			// top
			{-1.0,1.0,1.0},
			{1.0,1.0,1.0},
			{-1.0,1.0,-1.0},
			{-1.0,1.0,-1.0},
			{1.0,1.0,1.0},
			{1.0,1.0,-1.0},
			// left
			{-1.0,1.0,-1.0},
			{-1.0,-1.0,-1.0},
			{-1.0,1.0,1.0},
			{-1.0,1.0,1.0},
			{-1.0,-1.0,-1.0},
			{-1.0,-1.0,1.0},
			// right
			
			
			
		};
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
	}
	
	// Shader program
	{
		std::cout << "Compiling vertex shader" << std::endl;
		auto vertShader = glCreateShader(GL_VERTEX_SHADER);
		const char* vsource = shaderSource_Vertex.c_str();
		glShaderSource(vertShader, 1, &vsource, nullptr);
		glCompileShader(vertShader);
		checkErrorCompileShader(vertShader);
		
		std::cout << "Compiling fragment shader" << std::endl;
		auto fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fsource = shaderSource_Fragment.c_str();
		glShaderSource(fragShader, 1, &fsource, nullptr);
		glCompileShader(fragShader);
		checkErrorCompileShader(fragShader);
		
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertShader);
		glAttachShader(shaderProgram, fragShader);
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
	
	//auto secsSinceStart = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count()) / 1000.0f;
	//auto renderDelta = secsSinceStart - lastFrameTime;
	//std::cout << "GAH!: " << renderDelta << std::endl;
	//lastFrameTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count());
	
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
			// std::cout << "Triangle coords: " << fetchPayload << std::endl;
					  
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
	std::cout << "OpenGL Version: " << glVer << std::endl;
  
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
  }
#endif  


  // TODO: All the cleanup and stuff
  // SDL_Quit();
  
  return 0;
}

