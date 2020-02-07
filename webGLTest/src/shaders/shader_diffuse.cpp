
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

void Shader_diffuse::bind() {
	Shader::bind();
	
	if( mTextures.diffuse ) {
		mTextures.diffuse->bind(GL_TEXTURE0);
	  glUniform1i(mShaderUniformDiffuseTexturePresent, GL_TRUE);
	  glUniform1i(mShaderUniformDiffuseTextureSampler, 0);
	} else {
		glUniform1i(mShaderUniformDiffuseTexturePresent, GL_FALSE);
		glUniform4fv(mShaderUniformDiffuseColour, 1, glm::value_ptr(mDiffuseColour));
	}
}

void Shader_diffuse::diffuseColour( glm::vec4 c ) { mDiffuseColour = c; }
void Shader_diffuse::diffuseTexture( std::shared_ptr<Texture> tex ) { mTextures.diffuse = tex; }
