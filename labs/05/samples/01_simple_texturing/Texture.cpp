#include "pch.h"
#include "Texture.h"

void ThrowIfFailed(const char* msg)
{
	if (glGetError() != GL_NO_ERROR)
	{
		throw std::invalid_argument(msg);
	}
}
