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
  // TODO: For now one or the other with an if. Maybe later these get split
  if( diffuseTexturePresent ) {
    //fragColour = vec4(1.0,0.0,1.0,1.0);
    fragColour = texture(diffuseTexture, fragTexCoord);
  } else {
    fragColour = diffuseColour;
  }
}

