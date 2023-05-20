#include "../libgl/GdiPlusInitializer.h"
#include "MyApplication.h"
#include <tchar.h>

CMyApplication app("Fog example (press space to toggle fog)", 800, 600);
CGdiPlusInitializer gdiplusInitializer;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	app.MainLoop();
	return 0;
}
