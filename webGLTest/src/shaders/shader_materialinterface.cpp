
#include "shader_materialinterface.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Shader_materialinterface::Shader_materialinterface() {}
Shader_materialinterface::~Shader_materialinterface() {}
		
/// Compile the shader, make ready to be used etc.
void Shader_materialinterface::initialiseGLData() {
	Shader_lightinterface::initialiseGLData();

	mShaderUniform_material.ambient = glGetUniformLocation(mShaderProgram, "material.ambient");
	mShaderUniform_material.diffuse = glGetUniformLocation(mShaderProgram, "material.diffuse");
	mShaderUniform_material.specular = glGetUniformLocation(mShaderProgram, "material.specular");
	mShaderUniform_material.shininess = glGetUniformLocation(mShaderProgram, "material.shininess");
}

void Shader_materialinterface::bind() {
	Shader_lightinterface::bind();
	
	glUniform4fv(mShaderUniform_material.ambient, 1, glm::value_ptr(mMaterial.ambient));
	glUniform4fv(mShaderUniform_material.diffuse, 1, glm::value_ptr(mMaterial.diffuse));
	glUniform4fv(mShaderUniform_material.specular, 1, glm::value_ptr(mMaterial.specular));
	glUniform1f(mShaderUniform_material.shininess, mMaterial.shininess);
}

Material& Shader_materialinterface::material() { return mMaterial; }