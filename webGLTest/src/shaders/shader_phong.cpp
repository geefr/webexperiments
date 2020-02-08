
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
	Shader_materialinterface::initialiseGLData();

	mShaderUniformDiffuseTexturePresent = glGetUniformLocation(mShaderProgram, "diffuseTexturePresent");
	mShaderUniformDiffuseTextureSampler = glGetUniformLocation(mShaderProgram, "diffuseTexture");
}

void Shader_phong::bind() {
	Shader_materialinterface::bind();
	
	if( mTextures.diffuse ) {
		// A diffuse texture is present, flag to override the material
	  glUniform1i(mShaderUniformDiffuseTexturePresent, GL_TRUE);
    mTextures.diffuse->bind(GL_TEXTURE0);
    glUniform1i(mShaderUniformDiffuseTextureSampler, 0);
	} else {
    // A diffuse texture isn't present, just render the material
		glUniform1i(mShaderUniformDiffuseTexturePresent, GL_FALSE);
	}
}
