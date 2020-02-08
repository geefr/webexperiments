#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm/vec4.hpp"

struct Material {
	glm::vec3 ambient = {1.0f,1.0f,1.0f};
	glm::vec3 diffuse = {1.0f,1.0f,1.0f};
	glm::vec3 specular = {1.0f,1.0f,1.0f};
	float shininess = 32.0f;
	float alpha = 1.0f;
};

#endif
