#include "pch.h"
#include "../libgl/GDIPlusInitializer.h"
#include "../libgl/GLEWInitializer.h"
#include "MyApplication.h"

CMyApplication app("Blur filter example (click to toggle, press up/down keys to change dispersion)", 800, 600);
CGDIPlusInitializer gdiplusInit;
CGLEWInitializer glewInitializer;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		app.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}
