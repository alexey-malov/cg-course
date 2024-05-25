// 01_shadow_volumes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"

CMyApplication app("Stencil shadow volumes", 800, 600);


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		if (glewInit() != GLEW_OK)
		{
			throw std::runtime_error("GLEW initialization failed");
		}
		app.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what();
	}
	return 0;
}

