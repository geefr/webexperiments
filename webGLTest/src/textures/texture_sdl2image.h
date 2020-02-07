#ifndef TEXTURE_SDL2IMAGE_H
#define TEXTURE_SDL2IMAGE_H

#include "texture.h"

#include <string>

class Texture_SDL2Image : public Texture {
	public:		
	  Texture_SDL2Image(std::string filename);
	  virtual ~Texture_SDL2Image();
	  
		virtual void initialiseGLData() override final;
		
	private:
	  std::string mFilename;
};

#endif
