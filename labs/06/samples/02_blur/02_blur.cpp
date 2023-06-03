#include "pch.h"
#include "../libgl/GDIPlusInitializer.h"
#include "MyApplication.h"

CMyApplication application("Blur filter example (click to toggle, press up/down keys to change dispersion)", 800, 600);
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
