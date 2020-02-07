#ifndef RENDERER_H
#define RENDERER_H

#include "geometry/geometry.h"
#include "shaders/shader.h"
#include "textures/textureset.h"

#include <SDL.h>
#include <GLES3/gl3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <vector>
#include <map>
#include <memory>

class Renderer {
	public:
	  Renderer( GLfloat windowWidth, GLfloat windowHeight );
		~Renderer();

	  /**
	   * Load and compile a shader
	   * @param program Shader program to attach source to
	   * @param shader Path to shader source. Shader type will be determined by extension (.vert, .frag)
	   * @return Shader ID, or zero on error
	   */
		static GLuint loadShader(std::string shader);
    static bool checkErrorCompileShader(GLuint shader);
    
    void initialiseGLData();
    
    void render();
    
    SDL_Window* window() const;
    SDL_Surface* surface() const;
    SDL_GLContext context() const;
    
    std::map<std::string, std::shared_ptr<Shader>>& shaders();
    std::map<std::string, std::shared_ptr<Texture>>& textures();
    std::vector<std::shared_ptr<Geometry>>& geometry();
    
	private:
		SDL_Window* mWindow = nullptr;
		SDL_Surface* mSurface = nullptr;
		SDL_Renderer* mRenderer;
		SDL_GLContext mContext;
		GLfloat mWindowWidth = 0;
		GLfloat mWindowHeight;
		
		glm::vec4 mDiffuseColour = {1.f,1.f,1.f,1.f};
		glm::vec3 mViewRot = {0.f,0.f,0.f};
		glm::vec3 mViewPos = {0.f,0.f,2.f};
		
		glm::vec3 mViewRotDelta = {0.5f,0.7f,1.0f};
		//glm::vec3 mViewRotDelta = {0.0f,0.0f,0.0f};
		glm::vec3 mViewPosDelta = {0.0f,0.0f,0.0f};
		
		std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;
		
    std::map<std::string, std::shared_ptr<Shader>> mShaders;
		std::map<std::string, std::shared_ptr<Texture>> mTextures;
		std::vector<std::shared_ptr<Geometry>> mGeometry;
};

#endif
