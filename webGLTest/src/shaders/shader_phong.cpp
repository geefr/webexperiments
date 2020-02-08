
#include "shader_phong.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Shader_phong::Shader_phong() {
	mShaders = {
		"data/shaders/vertex/default.vert",
		"data/shaders/fragment/phong.frag",
	};
}
Shader_phong::~Shader_phong() {}
		
/// Compile the shader, make ready to be used etc.
void Shader_phong::initialiseGLData() {
	Shader_lightinterface::initialiseGLData();
	/**
	 * Uniform locations
	 * uniform vec4 diffuseColour;
	 * uniform bool diffuseTexturePresent;
   * uniform sampler2D diffuseTexture;
	 */
	mShaderUniformDiffuseColour = glGetUniformLocation(mShaderProgram, "diffuseColour");
	mShaderUniformDiffuseTexturePresent = glGetUniformLocation(mShaderProgram, "diffuseTexturePresent");
	mShaderUniformDiffuseTextureSampler = glGetUniformLocation(mShaderProgram, "diffuseTexture");
		
	if( mShaderUniformDiffuseColour == -1 ||
	    mShaderUniformDiffuseTexturePresent == -1 ||
	    mShaderUniformDiffuseTextureSampler == -1 ) {
		std::cerr << "Shader_diffuse::initialiseGLData: Failed to lookup a shader uniform" << std::endl;
	}
}

void Shader_phong::bind() {
	Shader_lightinterface::bind();
	
	if( mTextures.diffuse ) {
    //std::cerr << "Diffuse shader, with texture" << std::endl;
	  glUniform1i(mShaderUniformDiffuseTexturePresent, GL_TRUE);
    mTextures.diffuse->bind(GL_TEXTURE0);
    glUniform1i(mShaderUniformDiffuseTextureSampler, 0);
	} else {
    //std::cerr << "Diffuse shader, without texture" << std::endl;
		glUniform1i(mShaderUniformDiffuseTexturePresent, GL_FALSE);
		glUniform4fv(mShaderUniformDiffuseColour, 1, glm::value_ptr(mDiffuseColour));
	}
}

glm::vec4& Shader_phong::diffuseColour() { return mDiffuseColour; }
