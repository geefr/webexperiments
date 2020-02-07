#ifndef SHADER_LIGHTINTERFACE_H
#define SHADER_LIGHTINTERFACE_H

#include "shader.h"
#include "light.h"

#include <vector>

/**
 * Shader interface extension, adds lighting uniforms
 * 
 * struct Light {
 *   vec3 position;
 *   vec3 colour;
 *   float falloff;
 *   float radius;
 * };
 * uniform int numLights;
 * uniform Light lights[10];
 */
class Shader_lightinterface : public Shader {
	public:
		virtual ~Shader_lightinterface();
				
		/// Compile the shader, make ready to be used etc.
		virtual void initialiseGLData() override;
		
		/**
		 * Bind the shader, set uniforms, make ready for use
		 */
		virtual void bind() override;
		
		std::vector<Light>& lights();
	protected:
		Shader_lightinterface();
		
	  // Uniforms
	  const GLuint mMaxLights = 10;
	  GLuint mShaderUniform_numLights = -1;
	  struct LightUniform {
			GLuint position;
			GLuint colour;
			GLuint falloff;
			GLuint radius;
		};
	  std::vector<LightUniform> mShaderUniform_lights;
	  std::vector<Light> mLights;
};

#endif
