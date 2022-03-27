#include "pch.h"
#include "Frame.h"
#include "GLFWInitializer.h"
#include "Window.h"

int main()
{
	GLFWInitializer initGLFW;
	Window window{ 800, 600, "Two rectangles" };
	window.Run();
}
