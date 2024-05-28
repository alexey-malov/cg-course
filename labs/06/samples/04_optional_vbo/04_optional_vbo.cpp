// 04_optional_vbo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"

CMyApplication app("The optional use of Vertex Buffer Object", 800, 600);

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	glewInit();
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

