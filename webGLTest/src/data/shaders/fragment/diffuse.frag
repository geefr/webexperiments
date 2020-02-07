#version 300 es
precision highp float;

// Matrices
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Material
uniform vec4 diffuseColour;

// Shaders
uniform bool diffuseTexturePresent;
uniform sampler2D diffuseTexture;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out highp vec4 fragColour;

void main(void) {
  fragColour = diffuseColour;
  // Yeah! If statements!
  if( diffuseTexturePresent ) {
		vec4 texColour = texture(diffuseTexture, fragTexCoord);
		fragColour = vec4(texColour.rgb, 1.0);
	}
}

