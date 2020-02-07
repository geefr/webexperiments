#ifndef SHADER_LIGHTINTERFACE_H
#define SHADER_LIGHTINTERFACE_H

#include "shader.h"

#include <list>

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
		struct Light {
			vec3 position;
			vec3 colour;
			float falloff;
			float radius;
		};
	
		virtual ~Shader_lightinterface();
				
		/// Compile the shader, make ready to be used etc.
		virtual void initialiseGLData() override;
		
		std::list<Shader_lightinterface::Light>& lights();
		
		/// Set matrix uniforms
		void diffuseColour( glm::vec4 c );
	protected:
		Shader_lightinterface();
		
	  // Uniforms
	  blablablabla need to do a bunch of tricky stuff here to get the uniforms/arrays working somehow. Probably GLES is weirder than normal so this'll be hard..
	  
	  std::list<Shader_lightinterface::Light> mLights;
};

#endif
