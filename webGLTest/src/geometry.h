#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vertex.h"

#include <GLES3/gl3.h>
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
#include <vector>

class Renderer;

class Geometry {
	public:
		Geometry();
		~Geometry();
		
		void initialiseGLData();
		
		void render( Renderer* renderer, glm::mat4 projMat, glm::mat4 viewMat );
		
		glm::vec4& diffuseColour();
	
	private:
		GLuint mVertexBuffer = 0;
		GLuint mShaderProgram = 0;
		GLint mShaderUniform_mvp = 0;
		GLint mShaderUniform_diffuseColour = 0;
		
		GLint mShaderAttribute_vertPosition = 0;
		GLint mShaderAttribute_vertNormal = 0;
		GLint mShaderAttribute_vertColour = 0;
		
		std::vector<Vertex> mVertices;
		GLuint mNumVertices = 0;
		
		glm::vec4 mDiffuseColour = {1.f,1.f,1.f,1.f};
};

#endif
