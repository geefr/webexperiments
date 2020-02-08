
#include "player.h"
#include "engine/geometry/objmodel.h"

#include <glm/mat4x4.hpp>

Player::Player(Renderer& renderer) {
  mGeometry.reset( new ObjModel("data/models/primitives/cube/cube.obj") );
  mGeometry->shader( renderer.shaders()["laser"] );
  
  mGeometry->material().ambient = { 0.25f, 0.25f, 0.25f };
  mGeometry->material().diffuse = { 0.4f, 0.4f, 0.4f };
  mGeometry->material().specular = { 0.774597f, 0.774597f, 0.774597f };
  mGeometry->material().shininess = 0.6f;

  renderer.geometry().push_back(mGeometry);
}

Player::~Player() {}

glm::vec3& Player::position() { return mPosition; }

void Player::update(float delta) {
	glm::mat4x4 m(1.0f);
	m = glm::translate(m, mPosition);
	mGeometry->modelMatrix() = m;
}
