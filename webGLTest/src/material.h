#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm/vec4.hpp"

struct Material {
	glm::vec4 ambient = {1.0f,1.0f,1.0f,1.0f};
	glm::vec4 diffuse = {1.0f,1.0f,1.0f,1.0f};
	glm::vec4 specular = {1.0f,1.0f,1.0f,1.0f};
	float shininess = 32.0f;
};

#endif
