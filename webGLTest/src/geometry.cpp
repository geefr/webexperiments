#include "geometry.h"

#include "renderer.h"

#include <string>

Geometry::Geometry() {
	// TODO: Placeholder goemetry
	mVertices = {
		{{-1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f,-1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}},
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
  
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, Renderer::loadShader("shaders/vertex/default.vert"));
	glAttachShader(mShaderProgram, Renderer::loadShader("shaders/fragment/default.frag"));
	glLinkProgram(mShaderProgram);

	struct Vertex {
		glm::vec3 p; // Position
		glm::vec3 n; // Normal
		glm::vec4 c; // Colour
	};
	
	mShaderAttribute_vertPosition = glGetAttribLocation(mShaderProgram, "vertPosition");
	glEnableVertexAttribArray(mShaderAttribute_vertPosition);
	glVertexAttribPointer(mShaderAttribute_vertPosition, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex,p)));
	
	mShaderAttribute_vertNormal = glGetAttribLocation(mShaderProgram, "vertNormal");
	glEnableVertexAttribArray(mShaderAttribute_vertNormal);
	glVertexAttribPointer(mShaderAttribute_vertNormal, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex,n)));
	
	mShaderAttribute_vertColour = glGetAttribLocation(mShaderProgram, "vertColour");
	glEnableVertexAttribArray(mShaderAttribute_vertColour);
	glVertexAttribPointer(mShaderAttribute_vertColour, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex,c)));

	mShaderUniform_mvp = glGetUniformLocation(mShaderProgram, "mvp");
	mShaderUniform_diffuseColour = glGetUniformLocation(mShaderProgram, "diffuseColour");
}

void Geometry::render( Renderer* renderer, glm::mat4 projMat, glm::mat4 viewMat ) {
  auto mvp = projMat * viewMat /* * modelMat */; 

  glUseProgram(mShaderProgram);
  glUniformMatrix4fv(mShaderUniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform4fv(mShaderUniform_diffuseColour, 1, glm::value_ptr(mDiffuseColour));

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
}

glm::vec4& Geometry::diffuseColour() { return mDiffuseColour; }

