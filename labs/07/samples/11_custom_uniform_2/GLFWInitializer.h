#pragma once

class GLFWInitializer final
{
public:
	GLFWInitializer();

	GLFWInitializer(const GLFWInitializer&) = delete;
	GLFWInitializer& operator=(const GLFWInitializer&) = delete;

	~GLFWInitializer();
};