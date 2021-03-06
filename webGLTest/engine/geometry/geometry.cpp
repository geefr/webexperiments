#include "geometry.h"

#include "engine/renderer.h"
#include "engine/shaders/shader_materialinterface.h"

#include <string>
#include <iostream>

Geometry::Geometry() : mModelMat(1.0f) {}

Geometry::~Geometry() {
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVertexBuffer);
}

void Geometry::initialiseGLData() {
  // Initialise geometry state
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	
  // Upload the vertex data
  mNumVertices = mVertices.size();
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mVertices.size() * sizeof(Vertex)), mVertices.data(), GL_STATIC_DRAW);
  // Ditch the cpu-side buffer
  mVertices.clear();
  
  // Register the shader attributes for this VAO
  mShaderProgram->initialiseVertexAttribs();
}

void Geometry::render( Renderer* renderer, float renderDelta, glm::mat4 projMat, glm::mat4 viewMat ) {
	glBindVertexArray(mVAO);
	
	mShaderProgram->modelMatrix( mModelMat );
  mShaderProgram->viewMatrix( viewMat );
  mShaderProgram->projMatrix( projMat );
  
  mShaderProgram->textureSet() = mTextures;
  if( auto matShader = dynamic_cast<Shader_materialinterface*>(mShaderProgram.get()) ) {
    matShader->material() = mMaterial;
  }
  
  // Everything is ready, bind the shader and set the uniforms
  mShaderProgram->bind();

  // Full steam ahead cap'n, time to render
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
}

TextureSet& Geometry::textures() { return mTextures; }
void Geometry::shader( std::shared_ptr<Shader> s ) { mShaderProgram = s; }
Material& Geometry::material() { return mMaterial; }

glm::mat4x4& Geometry::modelMatrix() { return mModelMat; }
