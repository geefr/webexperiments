#ifndef LASER_H
#define LASER_H

#include "engine/geometry/geometry.h"
#include "engine/renderer.h"

#include <glm/vec3.hpp>

/**
 * A laser bolt
 * Rendered as regular geometry, with a light positioned at the front
 * 
 * When constructing ensure that
 * - A shader is present on the Renderer with the name 'laser'
 */
class Laser {
public:
	Laser(Renderer& renderer);
	~Laser();
	
	glm::vec3& position();
	void positionDelta(glm::vec3 d);
	
	void update(float delta);
	
protected:
  std::shared_ptr<Geometry> mGeometry;
  glm::vec3 mPosition = {0.0f,0.0f,0.0f};
  glm::vec3 mPositionDelta = {0.0f,0.0f,-1.0f}; // m/s
  
  glm::vec3 mRotation = {0.0f,0.0f,0.0f};
  glm::vec3 mRotationDelta = {0.0f,0.0f,2 * M_PI}; // rads/s
};

#endif
