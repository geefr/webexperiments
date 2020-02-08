#version 300 es
precision highp float;

// Matrices
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Material
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float alpha;
};
uniform Material material;

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

// Textures
uniform bool diffuseTexturePresent;
uniform sampler2D diffuseTexture;

// In view space
in vec3 fragPosition;
in vec3 fragNormal;

in vec2 fragTexCoord;

out highp vec4 fragColour;

void main(void) {
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	// Determine source of material info
	vec3 baseAmbient = material.ambient;
	vec3 baseDiffuse;
  // TODO: For now one or the other with an if. Maybe later these get split
  if( diffuseTexturePresent ) {
    baseDiffuse = vec3(texture(diffuseTexture, fragTexCoord));
  } else {
		baseDiffuse = material.diffuse;
  }
  vec3 baseSpecular = material.specular;
  
	// Lighting calculations
	for( int i = 0; i < numLights; i++ ) {
		Light l = lights[i];
		vec3 viewDir = normalize( - fragPosition );
		vec3 norm = normalize(fragNormal);
		vec3 lightDir = normalize(l.position - fragPosition);
		vec3 reflectDir = reflect( - lightDir, norm );
		
		// Ambient
		ambient += (l.intensity.x * l.colour * baseAmbient) / float(numLights);
		
		// Diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		ambient += (l.intensity.y * l.colour * (diff * baseDiffuse)) / float(numLights);
	
		// Specular
		// TODO: Read shininess from material
	  // explicit float casts here because WebGL
		float spec = pow( max( dot(viewDir, reflectDir), float(0.0)), float(32.0));
		specular += (l.intensity.z * l.colour * (spec * baseSpecular)) / float(numLights);
	}
	
  fragColour = vec4((ambient + diffuse + specular), material.alpha);
}

