
#include "shader_lightinterface.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Shader_lightinterface::Shader_lightinterface() {}
Shader_lightinterface::~Shader_lightinterface() {}
		
/// Compile the shader, make ready to be used etc.
void Shader_lightinterface::initialiseGLData() {
	Shader::initialiseGLData();
  /**
	 * struct Light {
	 *   vec3 position;
	 *   vec3 colour;
	 *   float falloff;
	 *   float radius;
	 * };
	 * uniform int numLights;
	 * uniform Light lights[10];
	 */
	mShaderUniform_numLights = glGetUniformLocation(mShaderProgram, "numLights");
	
	// Yay! Uniform blocks without using uniform blocks
	mShaderUniform_lights.reserve(mMaxLights);
	for( auto i = 0; i < mMaxLights; ++i ) {
		mShaderUniform_lights[i].position = glGetUniformLocation(mShaderProgram, std::string("lights[" + std::to_string(i) + "].position").c_str());
		mShaderUniform_lights[i].colour = glGetUniformLocation(mShaderProgram, std::string("lights[" + std::to_string(i) + "].colour").c_str());
		mShaderUniform_lights[i].intensity = glGetUniformLocation(mShaderProgram, std::string("lights[" + std::to_string(i) + "].intensity").c_str());
		mShaderUniform_lights[i].falloff = glGetUniformLocation(mShaderProgram, std::string("lights[" + std::to_string(i) + "].falloff").c_str());
		mShaderUniform_lights[i].radius = glGetUniformLocation(mShaderProgram, std::string("lights[" + std::to_string(i) + "].radius").c_str());
	}
}

void Shader_lightinterface::bind() {
	Shader::bind();
	
	glUniform1i(mShaderUniform_numLights, mLights.size());
	
	for( auto i = 0; i < mLights.size(); ++i ) {
		glUniform3fv(mShaderUniform_lights[i].position, 1, glm::value_ptr(mLights[i].position));
		glUniform3fv(mShaderUniform_lights[i].colour, 1, glm::value_ptr(mLights[i].colour));
		glUniform3fv(mShaderUniform_lights[i].intensity, 1, glm::value_ptr(mLights[i].intensity));
		glUniform1f(mShaderUniform_lights[i].falloff, mLights[i].falloff);
		glUniform1f(mShaderUniform_lights[i].radius, mLights[i].radius);
	}
}

std::vector<Light>& Shader_lightinterface::lights() { return mLights; }
