// 06.SpyTheMouse.cpp : Определяет точку входа для приложения.
//
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "windowsx.h"
#include "cstdio"
#include "06.SpyTheMouse.h"

static const TCHAR MAIN_WND_CLASS_NAME[] = TEXT("My Super window class");
static const TCHAR MAIN_WND_NAME[] = TEXT("Press left mouse button to spy the mouse");

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL RegisterMainWindowClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);
int MessageLoop();

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
void OnDestroy(HWND hwnd);
void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);

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
	/*
		These HANDLE_MSG macros are so-called 'Message crackers' declared in WindowsX.h header file
		They make message handline simpler
	*/
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void SpyTheMouse(HWND hwnd, const wchar_t* format, int x, int y)
{
	// I am REALLY sure this buffer is long enough to store this string
	wchar_t buffer[255]{};

	_swprintf(buffer, format, x, y);
	SetWindowText(hwnd, (LPCWSTR)buffer);
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT /*keyFlags*/)
{
	SpyTheMouse(hwnd, L"Spying finished at x: %d, y: %d", x, y);

	ReleaseCapture();
}

void OnLButtonDown(HWND hwnd, BOOL /*fDoubleClick*/, int x, int y, UINT /*keyFlags*/)
{
	SpyTheMouse(hwnd, L"Spying started at x: %d, y: %d", x, y);

	// now our window will capture mouse movements even when mouse is not over our window
	SetCapture(hwnd);
}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if ((keyFlags & MK_LBUTTON) != 0)
	{
		SpyTheMouse(hwnd, L"Mouse moved to x: %d, y: %d", x, y);
	}
}

void OnDestroy(HWND /*hwnd*/)
{
	PostQuitMessage(0);
}
