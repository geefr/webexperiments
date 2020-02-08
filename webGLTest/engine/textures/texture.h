#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES3/gl3.h>

class Texture {
	public:		
	  virtual ~Texture();
	  
		virtual void initialiseGLData();
		
		virtual void bind(GLenum texUnit);
		
	protected:
		Texture();
		
	private:
		GLuint mTextureID = 0;
};

#endif
