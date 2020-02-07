#include "geometry.h"

#include "renderer.h"

#include <string>

std::vector<Vertex> cube = {
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
	
std::vector<Vertex> triangle = {
	{{-1.0f,-1.0f, 1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
	{{ 1.0f,-1.0f, 1.0f}, {0.0f,0.0f,1.0f}, {1.0f,0.0f}},
	{{ 0.0f, 1.0f, 1.0f}, {0.0f,0.0f,1.0f}, {0.5f,1.0f}},
};

Geometry::Geometry() {
	// TODO: Placeholder goemetry
	mVertices = triangle;
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
}

void Geometry::render( Renderer* renderer, glm::mat4 projMat, glm::mat4 viewMat ) {
  glm::mat4 modelMat(1.0f);
  
  glUseProgram(mShaderProgram->id());
  
  // TODO: Not the right way to do things, texture slots should be up to the shader to determine
  if( mTextures.diffuse ) mTextures.diffuse->bind(GL_TEXTURE0);
  if( mTextures.specular ) mTextures.specular->bind(GL_TEXTURE1);
  if( mTextures.normal ) mTextures.normal->bind(GL_TEXTURE2);
  
  mShaderProgram->modelMatrix( modelMat );
  mShaderProgram->viewMatrix( viewMat );
  mShaderProgram->projMatrix( projMat );
  
  mShaderProgram->diffuseColour( mDiffuseColour );

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
}

glm::vec4& Geometry::diffuseColour() { return mDiffuseColour; }
TextureSet& Geometry::textures() { return mTextures; }
