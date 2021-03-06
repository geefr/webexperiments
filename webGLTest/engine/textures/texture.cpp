
#include "texture.h"

Texture::Texture() {
	
}

Texture::~Texture() {
	glDeleteTextures(1, &mTextureID);
}

void Texture::initialiseGLData() {
	glGenTextures(1, &mTextureID);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind(GLenum texUnit) {
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
