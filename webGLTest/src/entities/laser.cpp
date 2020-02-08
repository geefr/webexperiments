
#include "laser.h"
#include "engine/geometry/objmodel.h"

#include <glm/mat4x4.hpp>

Laser::Laser(Renderer& renderer) {
  mGeometry.reset( new ObjModel("data/models/primitives/cube/cube.obj") );
  mGeometry->shader( renderer.shaders()["laser"] );
  
  mGeometry->material().ambient = { 1.0f, 0.0f, 0.0f };
  mGeometry->material().diffuse = { 1.0f, 0.0f, 0.5f };
  mGeometry->material().specular = { 1.0f, 1.0f, 1.0f };
  mGeometry->material().shininess = 4.0f;
  
  renderer.geometry().push_back(mGeometry);
}

Laser::~Laser() {}

glm::vec3& Laser::position() { return mPosition; }
void Laser::positionDelta(glm::vec3 d) { mPositionDelta = d; }

void Laser::update(float delta) {
	mPosition += delta * mPositionDelta;
	if( mPosition.z < -10.0f ) mPosition.z = 0.0f;
	
	mRotation += delta * mRotationDelta;
	if( mRotation.x > 2 * M_PI ) mRotation.x = 0.0f;
	if( mRotation.y > 2 * M_PI ) mRotation.y = 0.0f;
	if( mRotation.z > 2 * M_PI ) mRotation.z = 0.0f;
	
	glm::mat4x4 m(1.0f);
	// translate
	m = glm::translate(m, mPosition);
	
	// rotate
	m = rotate(m, mRotation.x, glm::vec3(1.f,0.f,0.f));
	m = rotate(m, mRotation.y, glm::vec3(0.f,1.f,0.f));
	m = rotate(m, mRotation.z, glm::vec3(0.f,0.f,1.f));
	
	// scale
	m = glm::scale(m, glm::vec3(0.1f,0.1f, 0.5f));
	mGeometry->modelMatrix() = m;
}
