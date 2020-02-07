#version 300 es

// Matrices
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Material
uniform vec4 diffuseColour;

/*
// Material
struct Material {
  vec3 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
uniform Material material;

struct Light {
  vec3 position;
  vec3 colour;
  float falloff;
  float radius;
};
uniform int numLights;
uniform Light lights[10];
*/

//uniform bool useDiffuseShader;
//uniform sampler2D diffuseTexture;
//uniform sampler2D specularTexture;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;

out vec3 fragViewPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main(void) {
  mat4 modelViewMatrix = viewMatrix * modelMatrix;
  vec4 vertViewPosition = modelViewMatrix * vec4(vertPosition, 1.0);
  fragViewPosition = vertViewPosition.xyz;
  fragTexCoord = vertTexCoord;
  gl_Position = projMatrix * vertViewPosition;
  
  mat3 normalMatrix  = transpose(inverse(mat3(modelViewMatrix)));
  fragNormal = normalize(normalMatrix * vertNormal);
}
