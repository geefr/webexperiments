#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vertex.h"
#include "textures/textureset.h"
#include "shaders/shader_diffuse.h"

#include <GLES3/gl3.h>
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
#include <vector>
#include <memory>

class Renderer;

class Geometry {
	public:
		Geometry();
		~Geometry();
		
		void initialiseGLData();
		
		void render( Renderer* renderer, glm::mat4 projMat, glm::mat4 viewMat );
		
		glm::vec4& diffuseColour();
		TextureSet& textures();
	
	private:
	  std::unique_ptr<Shader_diffuse> mShaderProgram;
	
		GLuint mVertexBuffer = 0;
		
		std::vector<Vertex> mVertices;
		GLuint mNumVertices = 0;
		
		glm::vec4 mDiffuseColour = {1.f,1.f,1.f,1.f};
		
		TextureSet mTextures;
};

#endif
