#include <windows.h>
#include <tchar.h>

int WINAPI WinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int /*nCmdShow*/)
{
	MessageBox(NULL, _T("Hello world"), _T("My first windows program"), MB_OK);
	return 0;
}
