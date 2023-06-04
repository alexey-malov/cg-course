#include "pch.h"
#include "../libgl/GDIPlusInitializer.h"
#include "MyApplication.h"

CMyApplication app("Blur filter example (click to toggle, press up/down keys to change dispersion)", 800, 600);
CGDIPlusInitializer gdiplusInit;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		if (glewInit() != GLEW_OK)
		{
			throw std::runtime_error("Failed to initialize GLEW");
		}

		if (!GLEW_EXT_framebuffer_object)
		{
			throw std::runtime_error("GL_EXT_framebuffer_object extension is not available");
		}

		app.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}
