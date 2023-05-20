#include "../libgl/GdiPlusInitializer.h"
#include "MyApplication.h"
#include "common_inc.h"

CMyApplication app("Texture mapping example", 800, 600);
CGdiPlusInitializer gdiplusInitializer;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	app.MainLoop();
	return 0;
}
