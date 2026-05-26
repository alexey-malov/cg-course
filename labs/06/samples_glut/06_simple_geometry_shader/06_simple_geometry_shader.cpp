// 06_simple_geometry_shader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"
#include "../libgl/GDIPlusInitializer.h"

CMyApplication application("Geometry shader test", 800, 600);

int _tmain(int argc, _TCHAR* argv[])
{
	CGDIPlusInitializer gdiPlusInitializer;
	try
	{
		if (glewInit() != GLEW_OK)
		{
			throw std::runtime_error("Failed to initialize GLEW");
		}
		application.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what();
	}
	return 0;
}

