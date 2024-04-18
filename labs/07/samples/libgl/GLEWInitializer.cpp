#include "pch.h"
#include "GLEWInitializer.h"

CGLEWInitializer::CGLEWInitializer()
{
	if (glewInit() != GLEW_OK)
	{
		throw std::runtime_error("Failed to initialize GLEW");
	}

	if (!GLEW_EXT_framebuffer_object)
	{
		throw std::runtime_error("GL_EXT_framebuffer_object extension is not available");
	}
}
