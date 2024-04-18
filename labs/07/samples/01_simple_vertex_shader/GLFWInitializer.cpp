#include "pch.h"
#include "GLFWInitializer.h"

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
