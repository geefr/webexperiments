#ifndef SHADER_H
#define SHADER_H

#include <GLES3/gl3.h>
#include <glm/mat4x4.hpp>

#include <list>
#include <string>

/**
 * Base shader program
 * renderer/geometry should use one of the child classes, which will select the correct shader set
 */
class Shader {
	public:
		virtual ~Shader();
				
		/** 
		 * Compile the shader, make ready to be used etc.
		 * When overriding child classes must call the base implementation first, then perform child-specific actions
		 */
		virtual void initialiseGLData();
		
		/**
		 * Bind the shader, set uniforms, make ready for use
		 */
		virtual void bind();
		
		/// The OpenGL shader ID
		GLuint id() const;
		
		/// Set matrix uniforms
		void modelMatrix( glm::mat4 m );
		void viewMatrix( glm::mat4 m );
		void projMatrix( glm::mat4 m );
	protected:
	  Shader();
		
	  // Source shader paths
	  std::list<std::string> mShaders;
	  
	  // The shader program itself
		GLuint mShaderProgram = 0;
		
		// As per Vertex definition, shader independent
		GLint mShaderAttribute_vertPosition = 0;
		GLint mShaderAttribute_vertNormal = 1;
		GLint mShaderAttribute_vertTexCoord = 2;
		
		// Common uniform layout, shader independent
		GLint mShaderUniformModelMatrix = -1;
		GLint mShaderUniformViewMatrix = -1;
		GLint mShaderUniformProjMatrix = -1;
		
		// Uniform values
		glm::mat4x4 mModelMatrix;
		glm::mat4x4 mViewMatrix;
		glm::mat4x4 mProjMatrix;
};

#endif
