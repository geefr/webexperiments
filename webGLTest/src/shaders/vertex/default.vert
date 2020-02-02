#version 300 es

uniform mat4 mvp;

in vec3 vertPosition;
in vec3 vertNormal;
in vec4 vertColour;

out vec4 fragColour;

void main(void) {
  gl_Position = mvp * vec4(vertPosition, 1.0);
  fragColour = vertColour;
}

