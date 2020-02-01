#version 300 es

uniform mat4 mvp;

in vec3 vertCoord;

void main(void) {
  gl_Position = mvp * vec4(vertCoord, 1.0);
}

