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
uniform mediump int numLights;
uniform Light lights[10];

// Shaders
uniform bool diffuseTexturePresent;
uniform sampler2D diffuseTexture;

// In view space
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

/// Calculate diffuse light component for lights[i]
vec3 diffuseLighting(int i) {
	Light l = lights[i];
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(l.position - fragPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 d = l.intensity.y * diff * l.colour;
	return d / float(numLights);
}

/// Calculate specular light component for lights[i]
vec3 specularLighting(int i) {
	Light l = lights[i];
	vec3 viewDir = normalize( - fragPosition );
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(l.position - fragPosition);
	vec3 reflectDir = reflect( - lightDir, norm );
	
	// TODO: Read shininess from material
	// explicit float casts here because WebGL
	float spec = pow( max( dot(viewDir, reflectDir), float(0.0)), float(32.0));
	vec3 s = l.intensity.z * spec * l.colour;
	return s / float(numLights);
}

void main(void) {
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	for( int i = 0; i < numLights; i++ ) {
		ambient += ambientLighting(i);
		diffuse += diffuseLighting(i);
		specular += specularLighting(i);
	}
	vec4 baseFragColour;
  // TODO: For now one or the other with an if. Maybe later these get split
  if( diffuseTexturePresent ) {
    //fragColour = vec4(1.0,0.0,1.0,1.0);
    baseFragColour = texture(diffuseTexture, fragTexCoord);
  } else {
		baseFragColour = diffuseColour;
  }
  fragColour = baseFragColour * vec4((ambient + diffuse + specular), 1.0);
}

