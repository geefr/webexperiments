
#include "player.h"
#include "engine/geometry/objmodel.h"

#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

Player::Player(Renderer& renderer) {
  mGeometry.reset( new ObjModel("data/models/space/ships/fighter_blocky_01/fighter_blocky_01.obj") );
  mGeometry->shader( renderer.shaders()["laser"] );
  mGeometry->textures().diffuse = renderer.textures()["player.diffuse"];
  
  mGeometry->material().ambient = { 0.25f, 0.25f, 0.25f };
  mGeometry->material().diffuse = { 0.4f, 0.4f, 0.4f };
  mGeometry->material().specular = { 0.774597f, 0.774597f, 0.774597f };
  mGeometry->material().shininess = 0.6f;
 
  renderer.geometry().push_back(mGeometry);
  
  mQuatOrientation = glm::yawPitchRoll(0.0f,0.0f,0.0f);
}

Player::~Player() {}

glm::vec3& Player::position() { return mPosition; }

void Player::update(float delta) {
  // Update deltas
  // TODO: Physics
	mPositionDelta = {0.0f,0.0f,0.0f};
	for( auto& t : mThrusterState ) {
		if( t.second.fire ) {
			mPositionDelta += t.second.force;
		}
	}
	mPosition += delta * mPositionDelta;
  
  // Build the model matrix for the next render
  glm::mat4x4 m(1.0f);
  
	// translate
	m = glm::translate(m, mPosition);
	
  /*
   * Rotation
   * - mRotationDelta is specified as Euler angles, in radians per second
   * - mQuatOrientation stores the orientation of the ship, will be updated over time
   * - Each update doesn't use anything from the previous frame (to avoid error accumulation)
   */
  // Translate this update's rotation change into a quaternion
  glm::quat rotChange = glm::toQuat( glm::yawPitchRoll( 
    glm::degrees(delta * mRotationDelta.y),
    glm::degrees(delta * mRotationDelta.x),
    glm::degrees(delta * mRotationDelta.z)
  ));
  
  // Normalise the change, we only care about rotation in this case
  rotChange = glm::normalize(rotChange);
  // Merge into the orientation of the model
  mQuatOrientation = mQuatOrientation * rotChange;
  // Translate the orientation to a rotation matrix, add to the model's transform
  m = m * mat4_cast(mQuatOrientation);
  
	// scale
  // TODO: Distinguish between the constant scale for model loading and dynamic stuff at runtime
	m = glm::scale(m, glm::vec3(0.5f,0.5f,0.5f));
	mGeometry->modelMatrix() = m;
}

void Player::thrusterOn( Player::Thruster t ) { mThrusterState[t].fire = true; }
void Player::thrusterOff( Player::Thruster t ) { mThrusterState[t].fire = false; }
void Player::addRotation( glm::vec3 r ) { mRotationDelta += r; }
