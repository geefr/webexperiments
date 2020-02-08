#ifndef PLAYER_H
#define PLAYER_H

#include "engine/geometry/geometry.h"
#include "engine/renderer.h"

#include <glm/vec3.hpp>

/**
 * The player, in some kind of ship
 * Rendered as regular geometry, nothing special
 * 
 * When constructing ensure that
 * - A shader is present on the Renderer with the name 'ship'
 */
class Player {
public:
	Player(Renderer& renderer);
	~Player();
	
	glm::vec3& position();
	
	void update(float delta);
	
protected:
  std::shared_ptr<Geometry> mGeometry;
  glm::vec3 mPosition;
};

#endif
