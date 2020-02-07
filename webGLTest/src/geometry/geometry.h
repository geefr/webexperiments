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
		virtual ~Geometry();
		
		void initialiseGLData();
		
		void render( Renderer* renderer, float renderDelta, glm::mat4 projMat, glm::mat4 viewMat );
		
		glm::vec4& diffuseColour();
		TextureSet& textures();
    void shader( std::shared_ptr<Shader> s );
	
	protected:
		Geometry();
		
		glm::mat4x4 mModelMat;
		glm::vec3 mModelRot = {0.0f,0.0f,0.0f};
		glm::vec3 mModelRotDelta = {0.25f,0.35f,0.5f};
		
	  std::shared_ptr<Shader> mShaderProgram;
	
		GLuint mVertexBuffer = 0;
		GLuint mVAO = 0;
		
		std::vector<Vertex> mVertices;
		GLuint mNumVertices = 0;
		
		TextureSet mTextures;
};

#endif
