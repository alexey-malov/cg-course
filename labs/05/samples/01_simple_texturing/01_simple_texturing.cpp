#include "../libgl/GdiPlusInitializer.h"
#include "common_inc.h"
#include "MyApplication.h"

CMyApplication app("Texture mapping example", 800, 600);
CGdiPlusInitializer gdiPlusInitializer;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	app.MainLoop();
	return 0;
}
