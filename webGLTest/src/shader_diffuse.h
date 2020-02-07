#ifndef SHADER_DIFFUSE_H
#define SHADER_DIFFUSE_H

#include "shader.h"

/**
 * A basic shader with diffuse colour only for rendering
 */
class Shader_diffuse : public Shader {
	public:
		Shader_diffuse();
		virtual ~Shader_diffuse();
				
		/// Compile the shader, make ready to be used etc.
		virtual void initialiseGLData() override;
		
		/// Set matrix uniforms
		void diffuseColour( glm::vec4 c );
	private:
	  // Uniforms
	  GLint mShaderUniformDiffuseColour = -1;
};

#endif
