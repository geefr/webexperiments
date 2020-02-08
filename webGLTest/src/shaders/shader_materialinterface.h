#ifndef SHADER_MATERIALINTERFACE_H
#define SHADER_MATERIALINTERFACE_H

#include "shader_lightinterface.h"
#include "material.h"

#include <vector>

/**
 * Shader interface extension, adds material uniforms
 * 
 * struct Material {
 * 	vec3 ambient;
 * 	vec3 diffuse;
 * 	vec3 specular;
 * 	float shininess;
 * };
 * uniform Material material;
 */
class Shader_materialinterface : public Shader_lightinterface {
	public:
		virtual ~Shader_materialinterface();
				
		/// Compile the shader, make ready to be used etc.
		virtual void initialiseGLData() override;
		
		/**
		 * Bind the shader, set uniforms, make ready for use
		 */
		virtual void bind() override;
		
		Material& material();
	protected:
		Shader_materialinterface();
		
	  // Uniforms
	  struct MaterialUniform {
			GLint ambient = -1;
			GLint diffuse = -1;
			GLint specular = -1;
			float shininess = -1;
		};
		MaterialUniform mShaderUniform_material;
		Material mMaterial;
};

#endif
