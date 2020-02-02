#ifndef VERTEX_H
#define VERTEX_H

#include <GLES3/gl3.h>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

// Just floats please to make the packing simpler
struct Vertex {
	glm::vec3 p; // Position
	glm::vec3 n; // Normal
	glm::vec4 c; // Colour
};

#endif
