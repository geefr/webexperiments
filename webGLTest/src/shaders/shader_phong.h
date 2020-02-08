#ifndef SHADER_PHONG_H
#define SHADER_PHONG_H

#include "shader_lightinterface.h"

#include "textures/textureset.h"

/**
 * A basic shader set with phong lighting
 */
class Shader_phong : public Shader_lightinterface {
	public:
		Shader_phong();
		virtual ~Shader_phong();
				
		/// Compile the shader, make ready to be used etc.
		virtual void initialiseGLData() override final;
		
		/// Bind the shader, set uniforms
		virtual void bind() override final;
		
		/// Access uniform values
    glm::vec4& diffuseColour();
		
	private:
	  // Uniforms
	  GLint mShaderUniformDiffuseColour = -1;
	  
	  GLint mShaderUniformDiffuseTexturePresent = -1;
	  GLint mShaderUniformDiffuseTextureSampler = -1;
	  
	  // Uniform values/state
	  glm::vec4 mDiffuseColour = {1.0f,1.0f,1.0f,1.0f};
};

#endif
