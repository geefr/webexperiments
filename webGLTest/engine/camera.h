#ifndef CAMERA_H
#define CAMERA_H

/**
 * Camera parameters
 */
struct Camera {
	float fov = 90.0f;
	float near = 0.1f;
	float far = 100.0f;
	glm::vec3 position = {0.0f,0.0f,0.0f};
	glm::vec3 lookat = {0.0f,0.0f,-1.0f};
	glm::vec3 up = {0.0f,1.0f,0.0f};
};

#endif
