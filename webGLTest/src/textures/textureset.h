#ifndef TEXTURESET_H
#define TEXTURESET_H

#include "texture.h"
#include <memory>

struct TextureSet {
	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> specular;
	std::shared_ptr<Texture> normal;
};

#endif
