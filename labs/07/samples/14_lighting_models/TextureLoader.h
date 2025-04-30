#pragma once

#include <filesystem>
#include "Texture.h"

class TextureLoader
{
public:
	void LoadTexture2D(const std::filesystem::path& p, GLenum target = GL_TEXTURE_2D) const;
};

