#include <windows.h>
#include <tchar.h>

int WINAPI WinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int /*nCmdShow*/)
{
	MessageBoxW(NULL, L"Hello world", L"My first windows program", MB_OK);
	return 0;
}
