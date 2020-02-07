#include "texture_sdl2image.h"

#include <SDL_image.h>

#include <iostream>

Texture_SDL2Image::Texture_SDL2Image(std::string filename)
: mFilename(filename) {

}

Texture_SDL2Image::~Texture_SDL2Image() {

}

void Texture_SDL2Image::initialiseGLData() {
	Texture::initialiseGLData();
	// Shader has been created, and bound
	SDL_Surface* img = IMG_Load(mFilename.c_str());
	if( !img ) {
		std::cerr << "Texture_SDL2Image::initialiseGLData: ERROR: Failed to load image: " << mFilename << std::endl;
		return;
	}
	
	GLint texFormat;
	switch(img->format->BytesPerPixel) {
		case 3:
		  texFormat = GL_RGB8;
		  break;
		case 4:
			texFormat = GL_RGBA8;
			break;
		default:
			std::cerr << "Texture_SDL2Image::initialiseGLData: ERROR: Unsupported image depth (" << img->format->BytesPerPixel << " bytes per pixel): " << mFilename << std::endl;
			SDL_FreeSurface(img);
			return;
	}

  glTexImage2D(GL_TEXTURE_2D, 0, texFormat, img->w, img->h, 0, texFormat, GL_UNSIGNED_BYTE, img->pixels);
	
	SDL_FreeSurface(img);
}
