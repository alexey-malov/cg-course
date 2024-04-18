#include "pch.h"
#include "GLFWInitializer.h"
#include "Window.h"


int main()
{
	try
	{
		GLFWInitializer initGLFW;
		Window window{ 800, 600, "3D Cube" };
		window.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
