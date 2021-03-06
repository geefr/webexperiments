#ifndef RENDERER_H
#define RENDERER_H

#include "geometry/geometry.h"
#include "shaders/shader.h"
#include "textures/textureset.h"
#include "camera.h"

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
#ifndef __EMSCRIPTEN__    
    static void openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam);
#endif
    
    void initialiseGLData();
    
    /// Trigger an update
    void update();
    /// Query the time in seconds since the last update (frame/physics delta)
    float updateDelta();
    /// Render the scene
    void render();
    
    SDL_Window* window() const;
    SDL_Surface* surface() const;
    SDL_GLContext context() const;
    
    glm::vec4& clearColour();
    Camera& camera();
    
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
		
		std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;
		float mRenderDelta = 0.0f;
		
    std::map<std::string, std::shared_ptr<Shader>> mShaders;
		std::map<std::string, std::shared_ptr<Texture>> mTextures;
		std::vector<std::shared_ptr<Geometry>> mGeometry;
		
		glm::vec4 mClearColour = {0.0f,0.0f,0.0f,1.0f};
		Camera mCamera;
};

#endif
