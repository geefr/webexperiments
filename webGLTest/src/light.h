#ifndef LIGHT_H
#define LIGHT_H

#include "glm/vec3.hpp"

struct Light {
	glm::vec3 position;
	glm::vec3 colour;
	float falloff;
  float radius;
};

#endif
