#include "pch.h"
#include "MyApplication.h"
#include "../libgl/GDIPlusInitializer.h"

CMyApplication app("FrameBuffer example", 800, 600);
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
		std::cout << e.what();
		return 1;
	}

	return 0;
}
