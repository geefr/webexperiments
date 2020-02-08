
#include "laser.h"
#include "engine/geometry/objmodel.h"

#include <glm/mat4x4.hpp>

Laser::Laser(Renderer& renderer) {
  mGeometry.reset( new ObjModel("data/models/primitives/cube/cube.obj") );
  mGeometry->shader( renderer.shaders()["laser"] );
  
  mGeometry->material().ambient = { 0.0f, 0.0f, 0.0f };
  mGeometry->material().diffuse = { 1.0f, 0.0f, 0.5f };
  mGeometry->material().specular = { 1.0f, 1.0f, 1.0f };
  mGeometry->material().shininess = 4.0f;
  
  renderer.geometry().push_back(mGeometry);
}

Laser::~Laser() {}

glm::vec3& Laser::position() { return mPosition; }
void Laser::positionDelta(glm::vec3 d) { mPositionDelta = d; }

void Laser::update(float delta) {
	glm::mat4x4 m(1.0f);
	m *= glm::vec4(0.1f,0.1f, 2.0f, 1.0f);
	m = glm::translate(m, mPosition);
	mGeometry->modelMatrix() = m;
}
