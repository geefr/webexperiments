#include "geometry.h"

#include "renderer.h"

#include <string>

Geometry::Geometry() {
	// TODO: Placeholder goemetry
	mVertices = {
		// bottom
		{{-1.0f,-1.0f,-1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,0.0f}},
		
		// top
		{{1.0f,-1.0f, 1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f,-1.0f,-1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f,-1.0f,-1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f}},
		
		// left
		{{-1.0f,-1.0f,-1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f,-1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f,-1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		
		// right
		{{-1.0f, 1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f,-1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f,-1.0f,-1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},
		
		// front
		{{1.0f,-1.0f,-1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
		
		// back
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f}, {0.0f,0.0f,-1.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f,0.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f,0.0f}},
	};
	mNumVertices = mVertices.size();
}

Geometry::~Geometry() {
	
}

void Geometry::initialiseGLData() {
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mVertices.size() * sizeof(Vertex)), mVertices.data(), GL_STATIC_DRAW);
  // Ditch the cpu-side buffer
  mVertices.clear();
  
  mShaderProgram.reset(new Shader_diffuse());
  mShaderProgram->initialiseGLData();
  
	mTextures = {
		Renderer::defaultTextureDiffuse(),
		Renderer::defaultTextureNormal(),
		Renderer::defaultTextureSpecular(),
	};
}

void Geometry::render( Renderer* renderer, glm::mat4 projMat, glm::mat4 viewMat ) {
  glm::mat4 modelMat(1.0f);
  
  glUseProgram(mShaderProgram->id());
  
  mShaderProgram->modelMatrix( modelMat );
  mShaderProgram->viewMatrix( viewMat );
  mShaderProgram->projMatrix( projMat );
  
  mShaderProgram->diffuseColour( mDiffuseColour );

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
}

glm::vec4& Geometry::diffuseColour() { return mDiffuseColour; }

