#ifndef PLAYER_H
#define PLAYER_H

#include "engine/geometry/geometry.h"
#include "engine/renderer.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/**
 * The player, in some kind of ship
 * Rendered as regular geometry, nothing special
 * 
 * When constructing ensure that
 * - A shader is present on the Renderer with the name 'ship'
 */
class Player {
public:
  /// Thruster directions to activate
  enum class Thruster {
		Front,
		Back,
		Left,
		Right,
		Top,
		Bottom,
	};

	Player(Renderer& renderer);
	~Player();
	
	glm::vec3& position();
	
	void update(float delta);
	
	void thrusterOn( Player::Thruster t );
	void thrusterOff( Player::Thruster t );
	
	glm::vec3& rotationDelta() { return mRotationDelta; }
	
protected:
  void axisRotate(float angle, glm::vec3 axis);
  
  std::shared_ptr<Geometry> mGeometry;
  glm::vec3 mPosition = {0.0f,0.0f,0.0f};
  glm::vec3 mPositionDelta = {0.0f,0.0f,0.0f};
  glm::vec3 mRotation = {0.0f,0.0f,0.0f};
  glm::vec3 mRotationDelta = {0.0f,0.0f,0.0f};
  glm::vec3 mForward = {0.0f,0.0f,-1.0f};
  glm::vec3 mUp = {0.0f,1.0f,0.0f};
  glm::vec3 mRight = {1.0f,0.0f,0.0f};
  glm::mat4 mRotMatrix = glm::mat4(1.0f);
  
  /// Thruster id, enabled, force direction (points inwards)
  struct ThrusterState {
		bool fire;
		glm::vec3 force;
	};
  std::map<Player::Thruster, ThrusterState> mThrusterState = {
		{Thruster::Front, {false, glm::vec3(0.0f,0.0f,2.0f)}},
		{Thruster::Back, {false, glm::vec3(0.0f,0.0f,-2.0f)}},
		{Thruster::Left, {false, glm::vec3(1.0f,0.0f,0.0f)}},
		{Thruster::Right, {false, glm::vec3(-1.0f,0.0f,0.0f)}},
		{Thruster::Top, {false, glm::vec3(0.0f,-1.0f,0.0f)}},
		{Thruster::Bottom, {false, glm::vec3(0.0f,1.0f,0.0f)}},
	};
};

#endif
