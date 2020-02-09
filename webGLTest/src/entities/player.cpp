
#include "player.h"
#include "engine/geometry/objmodel.h"

#include <glm/mat4x4.hpp>

Player::Player(Renderer& renderer) {
  mGeometry.reset( new ObjModel("data/models/space/ships/fighter_blocky_01/fighter_blocky_01.obj") );
  mGeometry->shader( renderer.shaders()["laser"] );
  mGeometry->textures().diffuse = renderer.textures()["player.diffuse"];
  
  mGeometry->material().ambient = { 0.25f, 0.25f, 0.25f };
  mGeometry->material().diffuse = { 0.4f, 0.4f, 0.4f };
  mGeometry->material().specular = { 0.774597f, 0.774597f, 0.774597f };
  mGeometry->material().shininess = 0.6f;
 
  renderer.geometry().push_back(mGeometry);
}

Player::~Player() {}

glm::vec3& Player::position() { return mPosition; }

void Player::update(float delta) {
	mPositionDelta = {0.0f,0.0f,0.0f};
	for( auto& t : mThrusterState ) {
		if( t.second.fire ) {
			mPositionDelta += t.second.force;
		}
	}
	mPosition += delta * mPositionDelta;
	
	mRotation += delta * mRotationDelta;
	
	glm::mat4x4 m(1.0f);
	// translate
	m = glm::translate(m, mPosition);
	
	// rotate
	m = rotate(m, mRotation.x, glm::vec3(1.f,0.f,0.f));
	m = rotate(m, mRotation.y, glm::vec3(0.f,1.f,0.f));
	m = rotate(m, mRotation.z, glm::vec3(0.f,0.f,1.f));
	
	// scale
	m = glm::scale(m, glm::vec3(0.5f,0.5f,0.5f));
	mGeometry->modelMatrix() = m;
}

void Player::thrusterOn( Player::Thruster t ) { mThrusterState[t].fire = true; }
void Player::thrusterOff( Player::Thruster t ) { mThrusterState[t].fire = false; }
	
