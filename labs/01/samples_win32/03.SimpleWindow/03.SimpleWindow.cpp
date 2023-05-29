// 03.SimpleWindow.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "03.SimpleWindow.h"

static const TCHAR MAIN_WND_CLASS_NAME[] = TEXT("My Super window class");
static const TCHAR MAIN_WND_NAME[] = TEXT("This is my main window");

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL RegisterMainWindowClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);
int MessageLoop();

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	if (!RegisterMainWindowClass(hInstance))
	{
		return -1;
	}

	HWND hMainWindow = CreateMainWindow(hInstance);
	if (!hMainWindow)
	{
		return -1;
	}

	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);
	int result = MessageLoop();
	return result;

}

int MessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

HWND CreateMainWindow(HINSTANCE hInstance)
{
	return CreateWindowEx(
		0,
		MAIN_WND_CLASS_NAME,
		MAIN_WND_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);
}

BOOL RegisterMainWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = &MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = MAIN_WND_CLASS_NAME;
	wc.hIconSm = NULL;

	return RegisterClassEx(&wc) != 0;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}