
#include "player.h"
#include "engine/geometry/objmodel.h"

#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

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
  
  std::cerr << "Rotation Delta: " << mRotationDelta.x << "," << mRotationDelta.y << "," << mRotationDelta.z << std::endl;
  
	mRotation += delta * mRotationDelta;
  if( mRotation.x > 2.0f * M_PI ) mRotation.x = 0.0f;
  if( mRotation.y > 2.0f * M_PI ) mRotation.y = 0.0f;
  if( mRotation.z > 2.0f * M_PI ) mRotation.z = 0.0f;
	
  glm::mat4x4 m(1.0f);
	// translate
	// m = glm::translate(m, mPosition);
	
  // Rotate

  // mRotation contains the rotation (euler) angles around each axis
  // First define the rotation axis, these will be modified as we go along as rotations are specified relative to the model
  //mForward = {0.0f,0.0f,-1.0f};
  //mUp = {0.0f,1.0f,0.0f};
  //mRight = {1.0f,0.0f,0.0f};

  glm::mat4 xRot(1.0f);
  glm::mat4 yRot(1.0f);
  glm::mat4 zRot(1.0f);

  std::cerr << "forward1: " << mForward.x << "," << mForward.y << "," << mForward.z << std::endl;
  std::cerr << "up1: " << mUp.x << "," << mUp.y << "," << mUp.z << std::endl;
  std::cerr << "right1: " << mRight.x << "," << mRight.y << "," << mRight.z << std::endl;
    
  xRot = glm::rotate(xRot, glm::degrees(delta * mRotationDelta.x), mRight); // THIS IS IN DEGREES!!!
  mUp = glm::normalize(glm::rotate(mUp, delta * mRotationDelta.x, mRight)); // THIS IS IN RADIANS!!!
  mForward = glm::normalize(glm::rotate(mForward, delta * mRotationDelta.x, mRight)); // THIS IS IN RADIANS!!!

  std::cerr << "forward2: " << mForward.x << "," << mForward.y << "," << mForward.z << std::endl;
  std::cerr << "up2: " << mUp.x << "," << mUp.y << "," << mUp.z << std::endl;
  std::cerr << "right2: " << mRight.x << "," << mRight.y << "," << mRight.z << std::endl;
  
  yRot = glm::rotate(yRot, glm::degrees(delta * mRotationDelta.y), mUp); // THIS IS IN DEGREES!!!
  mRight = glm::normalize(glm::rotate(mRight, delta * mRotationDelta.y, mUp)); // THIS IS IN RADIANS!!!
  mForward = glm::normalize(glm::rotate(mForward, delta * mRotationDelta.y, mUp)); // THIS IS IN RADIANS!!!

  zRot = glm::rotate(zRot, glm::degrees(delta * mRotationDelta.z), mForward); // THIS IS IN DEGREES!!!
  mUp = glm::normalize(glm::rotate(mUp, delta * mRotationDelta.z, mForward)); // THIS IS IN RADIANS!!!
  mRight = glm::normalize(glm::rotate(mRight, delta * mRotationDelta.z, mForward)); // THIS IS IN RADIANS!!!

  // Add the rotations to the model matrix
  // Rotations need to apply in the order listed above
  // (so listed in reverse here due to operator ordering)
  mRotMatrix = mRotMatrix * zRot * yRot * xRot;
  m = m * mRotMatrix;
  
	// rotate
  //axisRotate(delta * mRotationDelta.x, mRight);
  //axisRotate(delta * mRotationDelta.y, mUp);
  //axisRotate(delta * mRotationDelta.z, mForward);

  // Update the rotation matrix for this frame
  // TODO: Even after fixing the model this seems to be wrong somehow, is this function the wrong handedness???
  //glm::mat4x4 rotMat = glm::orientation( - mForward, mUp);
  //m = m * rotMat;
  
  // m = rotate(m, mRotation.z, glm::vec3(0.f,0.f,1.f));
  // m = rotate(m, mRotation.y, glm::vec3(0.f,1.f,0.f));
  // m = rotate(m, mRotation.x, glm::vec3(1.f,0.f,0.f));
  
	// scale
	m = glm::scale(m, glm::vec3(0.5f,0.5f,0.5f));
	mGeometry->modelMatrix() = m;
}

void Player::thrusterOn( Player::Thruster t ) { mThrusterState[t].fire = true; }
void Player::thrusterOff( Player::Thruster t ) { mThrusterState[t].fire = false; }

void Player::axisRotate(float angle, glm::vec3 axis) {
  if( axis != mUp ) mUp = glm::rotate(mUp, angle, axis);
  if( axis != mRight ) mRight = glm::rotate(mRight, angle, axis);
  if( axis != mForward ) mForward = glm::rotate(mForward, angle, axis);
  
  std::cerr << "forward: " << mForward.x << "," << mForward.y << "," << mForward.z << std::endl;
  std::cerr << "up: " << mUp.x << "," << mUp.y << "," << mUp.z << std::endl;
  std::cerr << "right: " << mRight.x << "," << mRight.y << "," << mRight.z << std::endl;
}
