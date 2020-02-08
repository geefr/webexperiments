#version 300 es

/**
 * The default vertex shader (hopefully) compatible with all fragment shaders and such
 */

// Matrices
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;

// In view space
out vec3 fragPosition;
out vec3 fragNormal;

out vec2 fragTexCoord;

void main(void) {
  mat4 modelViewMatrix = viewMatrix * modelMatrix;
  vec4 vertViewPosition = modelViewMatrix * vec4(vertPosition, 1.0);
  fragPosition = vec3(vertViewPosition);
  fragTexCoord = vertTexCoord;
  gl_Position = projMatrix * vertViewPosition;
  
  // TODO: Performance: Calculate normal matrix on cpu instead of per-vertex
  mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
  fragNormal = normalize(normalMatrix * vertNormal);
}

