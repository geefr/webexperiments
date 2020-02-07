#version 300 es
precision highp float;

// Matrices
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Material
uniform vec4 diffuseColour;

in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord;

out highp vec4 fragColour;

void main(void) {
  fragColour = diffuseColour;
}

