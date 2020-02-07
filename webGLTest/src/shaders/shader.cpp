#include "shader.h"

#include "renderer.h"

#include <iostream>

Shader::Shader() {}
Shader::Shader( std::list<std::string> shaders ) : mShaders(shaders) {}

Shader::~Shader() {}

void Shader::initialiseGLData() {
	// Compile/link the shader
	mShaderProgram = glCreateProgram();
	for( auto& s : mShaders ) {
		glAttachShader(mShaderProgram, Renderer::loadShader(s));
	}
	glLinkProgram(mShaderProgram);
	if( glGetError() != GL_NO_ERROR ) { std::cerr << "Shader::initialiseGLData: Shader link failed" << std::endl; }

  /**
   * TODO: Should document the shader API somewhere, maybe formalise on the shadertoy api even for giggles
   * 
   * layout(location = 0) in vec3 vertPosition;
   * layout(location = 1) in vec3 vertNormal;
   * layout(location = 2) in vec2 vertTexCoord;
   */
	glEnableVertexAttribArray(mShaderAttribute_vertPosition);
	glVertexAttribPointer(mShaderAttribute_vertPosition, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex,p)));
		
	glEnableVertexAttribArray(mShaderAttribute_vertNormal);
	glVertexAttribPointer(mShaderAttribute_vertNormal, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex,n)));
		
	glEnableVertexAttribArray(mShaderAttribute_vertTexCoord);
	glVertexAttribPointer(mShaderAttribute_vertTexCoord, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex,t)));
	
	/**
	 * Uniform locations
	 * Could use layout specifier in shader, but that requires GL_ARB_explicit_uniform_location. Keep it simple for now to avoid any incompatibilities
	 * uniform mat4 modelMatrix;
   * uniform mat4 viewMatrix;
   * uniform mat4 projMatrix;
	 */
	mShaderUniformModelMatrix = glGetUniformLocation(mShaderProgram, "modelMatrix");
	mShaderUniformViewMatrix = glGetUniformLocation(mShaderProgram, "viewMatrix");
	mShaderUniformProjMatrix = glGetUniformLocation(mShaderProgram, "projMatrix");
	if( mShaderUniformModelMatrix == -1 ||
	    mShaderUniformViewMatrix == -1 ||
	    mShaderUniformProjMatrix == -1 ) {
		std::cerr << "Shader::initialiseGLData: Failed to lookup a shader uniform" << std::endl;
	}
}

GLuint Shader::id() const { return mShaderProgram; }

void Shader::modelMatrix( glm::mat4 m ) { glUniformMatrix4fv( mShaderUniformModelMatrix, 1, GL_FALSE, glm::value_ptr(m) ); }
void Shader::viewMatrix( glm::mat4 m ) { glUniformMatrix4fv( mShaderUniformViewMatrix, 1, GL_FALSE, glm::value_ptr(m) ); }
void Shader::projMatrix( glm::mat4 m ) { glUniformMatrix4fv( mShaderUniformProjMatrix, 1, GL_FALSE, glm::value_ptr(m) ); }
