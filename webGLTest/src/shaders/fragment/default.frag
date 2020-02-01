#version 300 es

uniform highp vec4 diffuseColour;

out highp vec4 fragColour;

void main(void) {
  fragColour = diffuseColour;
}

