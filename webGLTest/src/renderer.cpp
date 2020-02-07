
#include "renderer.h"

#include <iostream>
#include <fstream>
#include <filesystem>

Renderer::Renderer( GLfloat windowWidth, GLfloat windowHeight )
  : mWindowWidth(windowWidth), mWindowHeight(windowHeight) {

  SDL_CreateWindowAndRenderer(mWindowWidth, mWindowHeight, SDL_WINDOW_OPENGL, &mWindow, &mRenderer);

  mContext = SDL_GL_CreateContext(mWindow);
  
  auto glVer = glGetString(GL_VERSION);
  std::cout << "OpenGL initialised: " << glVer << std::endl;
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

	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	
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
	  
  mViewRot += renderDelta * mViewRotDelta;
  if( mViewRot.x > 2 * 3.14 ) mViewRot.x = 0.0f;
  if( mViewRot.y > 2 * 3.14 ) mViewRot.y = 0.0f;
  if( mViewRot.z > 2 * 3.14 ) mViewRot.z = 0.0f;
  
  mViewPos += renderDelta * mViewPosDelta;
  if( mViewPos.z <= 1.f ) mViewPos.z = 2.f;
  
  glm::mat4 projMat = glm::perspective( 90.0f, static_cast<GLfloat>(mWindowWidth) / static_cast<GLfloat>(mWindowHeight), 0.1f, 100.0f );
  glm::mat4 viewMat = glm::lookAt( mViewPos, glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0) );
  
  viewMat = glm::rotate( viewMat, mViewRot.x, glm::vec3(1.0, 0.0, 0.0) );
  viewMat = glm::rotate( viewMat, mViewRot.y, glm::vec3(0.0, 1.0, 0.0) );
  viewMat = glm::rotate( viewMat, mViewRot.z, glm::vec3(0.0, 0.0, 1.0) );

  SDL_GL_MakeCurrent(mWindow, mContext);
  glViewport(0,0,mWindowWidth,mWindowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  for( auto& g : mGeometry ) {
		g->render(this, projMat, viewMat);
	}
	
  SDL_GL_SwapWindow(mWindow);
}

SDL_Window* Renderer::window() const { return mWindow; }
SDL_Surface* Renderer::surface() const { return mSurface; }
SDL_GLContext Renderer::context() const { return mContext; }
std::map<std::string, std::shared_ptr<Shader>>& Renderer::shaders() { return mShaders; }
std::map<std::string, std::shared_ptr<Texture>>& Renderer::textures() { return mTextures; }
std::vector<std::shared_ptr<Geometry>>& Renderer::geometry() { return mGeometry; }

