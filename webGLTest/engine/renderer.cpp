
#include "renderer.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#ifndef __EMSCRIPTEN__
// GLES 3.2 on desktop, for debug extensions and such
# include <GLES3/gl32.h>
#endif

Renderer::Renderer( GLfloat windowWidth, GLfloat windowHeight )
  : mWindowWidth(windowWidth), mWindowHeight(windowHeight) {

  SDL_CreateWindowAndRenderer(mWindowWidth, mWindowHeight, SDL_WINDOW_OPENGL, &mWindow, &mRenderer);

  mContext = SDL_GL_CreateContext(mWindow);
  
  auto glVer = glGetString(GL_VERSION);
  std::cout << "OpenGL initialised: " << glVer << std::endl;
  
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
}
  
Renderer::~Renderer() {}

bool Renderer::checkErrorCompileShader(GLuint shader) {
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

GLuint Renderer::loadShader(std::string shader) {
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

void Renderer::initialiseGLData() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	
  //std::cerr << "Renderer::initialising shaders" << std::endl;
  for( auto& s : mShaders ) {
    s.second->initialiseGLData();
  }
  //std::cerr << "Renderer::initialising textures" << std::endl;
	for( auto& t : mTextures ) {
		t.second->initialiseGLData();
	}
	//std::cerr << "Renderer::initialising geometry" << std::endl;
	for( auto& g : mGeometry ) {
		g->initialiseGLData();
	}
	
	mStartTime = std::chrono::high_resolution_clock::now();
}

void Renderer::render() {  
	auto now = std::chrono::high_resolution_clock::now();
	
	float renderDelta = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(now - mLastFrameTime).count()) / 1e6f;
	mLastFrameTime = now;
  
  glm::mat4 projMat = glm::perspective( mCamera.fov, static_cast<GLfloat>(mWindowWidth) / static_cast<GLfloat>(mWindowHeight), mCamera.near, mCamera.far );
  glm::mat4 viewMat = glm::lookAt( mCamera.position, mCamera.lookat, mCamera.up );

  SDL_GL_MakeCurrent(mWindow, mContext);
  glViewport(0,0,mWindowWidth,mWindowHeight);
	glClearColor( mClearColour.r, mClearColour.g, mClearColour.b, mClearColour.a );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  for( auto& g : mGeometry ) {
		g->render(this, renderDelta, projMat, viewMat);
	}
	
  SDL_GL_SwapWindow(mWindow);
}

#ifndef __EMSCRIPTEN__
void Renderer::openglCallbackFunction(GLenum source,
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

SDL_Window* Renderer::window() const { return mWindow; }
SDL_Surface* Renderer::surface() const { return mSurface; }
SDL_GLContext Renderer::context() const { return mContext; }
std::map<std::string, std::shared_ptr<Shader>>& Renderer::shaders() { return mShaders; }
std::map<std::string, std::shared_ptr<Texture>>& Renderer::textures() { return mTextures; }
std::vector<std::shared_ptr<Geometry>>& Renderer::geometry() { return mGeometry; }
glm::vec4& Renderer::clearColour() { return mClearColour; }
Camera& Renderer::camera() { return mCamera; }
