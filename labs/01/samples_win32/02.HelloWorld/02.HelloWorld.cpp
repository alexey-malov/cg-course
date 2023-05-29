// 02.HelloWorld.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "02.HelloWorld.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	MessageBox(NULL, L"Hello World", L"Hi", MB_OK | MB_ICONINFORMATION);
	return 0;
}
