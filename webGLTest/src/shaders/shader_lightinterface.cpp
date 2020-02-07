
#include "shader_diffuse.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Shader_diffuse::Shader_diffuse() {
	mShaders = {
		"data/shaders/vertex/default.vert",
		"data/shaders/fragment/diffuse.frag",
	};
}
Shader_diffuse::~Shader_diffuse() {}
		
/// Compile the shader, make ready to be used etc.
void Shader_diffuse::initialiseGLData() {
	Shader::initialiseGLData();
	/**
	 * Uniform locations
	 * uniform vec4 diffuseColour;
	 */
	mShaderUniformDiffuseColour = glGetUniformLocation(mShaderProgram, "diffuseColour");
	if( mShaderUniformDiffuseColour == -1 ) {
		std::cerr << "Shader_diffuse::initialiseGLData: Failed to lookup a shader uniform" << std::endl;
	}
}

/// Set matrix uniforms
void Shader_diffuse::diffuseColour( glm::vec4 c ) { glUniform4fv(mShaderUniformDiffuseColour, 1, glm::value_ptr(c)); }
