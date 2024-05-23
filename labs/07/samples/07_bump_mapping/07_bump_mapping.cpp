// 04_bump_mapping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GDIPlusInitializer.h"
#include "MyApplication.h"

CMyApplication application("Bump mapping", 800, 600);
CGDIPlusInitializer gdiplusInit;

int _tmain(int argc, _TCHAR* argv[])
{
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