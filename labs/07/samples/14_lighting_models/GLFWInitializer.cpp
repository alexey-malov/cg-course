#include "GLFWInitializer.h"
#include "gl.h"
#include <stdexcept>

GLFWInitializer::GLFWInitializer()
{
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
}

GLFWInitializer::~GLFWInitializer()
{
	glfwTerminate();
}
