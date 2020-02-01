#include <stdio.h>
#include <SDL.h>
#include <GLES2/gl2.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
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

struct FetchData {
	std::string data;
	bool ready = false;
};

#ifdef __EMSCRIPTEN__
  std::map<emscripten_fetch_t*, FetchData> currentFetches;
#endif

GLuint vertexBuffer, shaderProgram;
GLint shaderAttribute_vertCoord;
GLint shaderUniform_mvp;

struct Vertex {
	GLfloat x;
	GLfloat y;
};
std::vector<Vertex> vertices;

std::string shaderSource_Vertex = 
"#version 100\n"
"uniform mat4 mvp;\n"
"attribute vec2 vertCoord;\n"
"void main(void) { \n"
"gl_Position = mvp * vec4(vertCoord.x, vertCoord.y, 0.5, 1.0);\n"
"}\n"
"";

std::string shaderSource_Fragment =
"#version 100\n"
"void main(void) {\n"
"gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n"
"}\n"
"";

std::list<std::string> shapes = {
  "110m_physical%2fne_110m_geography_regions_polys.shp",
  "110m_physical%2fne_110m_land.shp",
};

std::string currentShape;

float viewRot(0.0);

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

void initialiseGLData() {
	glEnable(GL_DEPTH_TEST);
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	
	// Vertex data TODO: Placeholder triangle
	{
		vertices = {
			{-0.8f,-0.8f},
			{ 0.8f,-0.8f},
			{ 0.8f, 0.8f},
		};
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
	}
	
	// Shader program
	{
		auto vertShader = glCreateShader(GL_VERTEX_SHADER);
		const char* vsource = shaderSource_Vertex.c_str();
		glShaderSource(vertShader, 1, &vsource, nullptr);
		glCompileShader(vertShader);
		
		auto fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fsource = shaderSource_Fragment.c_str();
		glShaderSource(fragShader, 1, &fsource, nullptr);
		glCompileShader(fragShader);
		
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertShader);
		glAttachShader(shaderProgram, fragShader);
		glLinkProgram(shaderProgram);
		
		shaderAttribute_vertCoord = glGetAttribLocation(shaderProgram, "vertCoord");
		glEnableVertexAttribArray(shaderAttribute_vertCoord);
		glVertexAttribPointer(shaderAttribute_vertCoord, 2, GL_FLOAT, false, sizeof(Vertex), static_cast<const void*>(0));
    
    shaderUniform_mvp = glGetUniformLocation(shaderProgram, "mvp");
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


  // viewRot += 0.01;

  if( viewRot > 2 * 3.14 ) viewRot = 0.0;
  
  // glm::mat4 projMat = glm::ortho( -180.0f, 180.0f, -90.0f, 90.0f, 1.f, -1.f );
  glm::mat4 projMat = glm::perspective( 90.0f, static_cast<GLfloat>(windowWidth) / static_cast<GLfloat>(windowHeight), 0.1f, 100.0f );
  glm::mat4 viewMat = glm::lookAt( glm::vec3(0.0, 0.0, 2.0), glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0) );
  viewMat = glm::rotate( viewMat, viewRot, glm::vec3(0.0, 0.0, 1.0) );
  auto mvp = projMat * viewMat /* * modelMat */; 
  
  SDL_GL_MakeCurrent(window, gl_context);
  
  glViewport(0,0,windowWidth,windowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(shaderProgram);
  glUniformMatrix4fv(shaderUniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  
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
#endif
}


int main(int argc, char** argv) {
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Renderer* renderer = nullptr;
  SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_OPENGL, &window, &renderer);

  // TODO: Context flags
  gl_context = SDL_GL_CreateContext(window);
  
  currentShape = *shapes.begin();

  initialiseGLData();

// Fire the main loop. Won't quit this until SDL does in some form
#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
  emscripten_set_main_loop(render, 30, 1);
#else
  while( true ) {
	  render();
  }
#endif  


  // TODO: All the cleanup and stuff
  // SDL_Quit();
  
  return 0;
}

