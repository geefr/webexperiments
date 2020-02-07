#version 300 es
precision highp float;

// Matrices
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Material
uniform vec4 diffuseColour;

// Lights
struct Light {
  vec3 position;
  vec3 colour;
  vec3 intensity; // ambient, diffuse, specular
  float falloff;
  float radius;
};
uniform int numLights;
uniform Light lights[10];

// Shaders
uniform bool diffuseTexturePresent;
uniform sampler2D diffuseTexture;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out highp vec4 fragColour;

/// Calculate ambient light component for lights[i]
vec3 ambientLighting(int i) {
	Light l = lights[i];
	vec3 a = l.intensity.x * l.colour;
	return a / float(numLights);
}

void main(void) {
	vec3 ambient = vec3(0.0);
	for( int i = 0; i < numLights; i++ ) {
		ambient += ambientLighting(i);
	}
	
  // TODO: For now one or the other with an if. Maybe later these get split
  if( diffuseTexturePresent ) {
    //fragColour = vec4(1.0,0.0,1.0,1.0);
    fragColour = texture(diffuseTexture, fragTexCoord);
  } else {
    fragColour = diffuseColour * vec4(ambient, 1.0);
  }
}

