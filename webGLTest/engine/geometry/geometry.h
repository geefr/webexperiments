#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "engine/vertex.h"
#include "engine/textures/textureset.h"
#include "engine/material.h"
#include "engine/shaders/shader.h"

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
		
		Material& material();
		TextureSet& textures();
    void shader( std::shared_ptr<Shader> s );
	
	  glm::mat4x4& modelMatrix();
	
	protected:
		Geometry();
		
		glm::mat4x4 mModelMat;
		
		GLuint mVertexBuffer = 0;
		GLuint mVAO = 0;
		
		std::vector<Vertex> mVertices;
		GLuint mNumVertices = 0;
		
		std::shared_ptr<Shader> mShaderProgram;
		TextureSet mTextures;
		Material mMaterial;
};

#endif
