// 02_vertex_array.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"

CMyApplication app("Rendering using Vertex Arrays", 800, 600);

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	try
	{
		app.MainLoop();
	}
	catch (std::runtime_error const& e)
	{
		std::cout << e.what();
	}
	return 0;
}

