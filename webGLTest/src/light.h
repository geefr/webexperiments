#ifndef LIGHT_H
#define LIGHT_H

#include "glm/vec3.hpp"

struct Light {
	glm::vec3 position = {0.0f,0.0f,0.0f};
	glm::vec3 colour = {1.0f,1.0f,1.0f};
	glm::vec3 intensity = {0.1f,1.0f,0.5f}; // ambient, diffuse, specular
	float falloff = 100.0f;
  float radius = 10.0f;
};

#endif
