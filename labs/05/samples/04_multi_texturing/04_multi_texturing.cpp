#include "../libgl/GdiPlusInitializer.h"
#include "MyApplication.h"
#include <iostream>
#include <tchar.h>

CMyApplication app("Multitexturing example", 800, 600);
CGdiPlusInitializer gdiplusInitializer;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	try
	{
		app.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
