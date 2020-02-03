#version 300 es
precision highp float;

// Matrices
struct Matrices {
  mat4 model;
  mat4 view;
  mat4 proj;
};
uniform Matrices matrices;

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

//uniform bool useDiffuseShader;
//uniform sampler2D diffuseTexture;
//uniform sampler2D specularTexture;

in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec3 fragViewPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

uniform highp vec4 diffuseColour;

out highp vec4 fragColour;

void main(void) {
  fragColour = diffuseColour;
}

