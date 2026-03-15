// 05_vertex_blend.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"

CMyApplication app("Vertex blend with vertex shader", 800, 600);

int _tmain(int argc, _TCHAR* argv[])
{
	glewInit();
	if (!GLEW_ARB_shader_objects)
	{
		return -1;
	}

	try
	{
		app.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what();
	}

	return 0;
}
