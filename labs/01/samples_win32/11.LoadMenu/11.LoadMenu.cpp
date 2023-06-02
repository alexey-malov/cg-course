// 11.LoadMenu.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "11.LoadMenu.h"
#include "windowsx.h"
#include "resource.h"

#define IDR_MAIN_MENU 101
#define IDR_ACCELERATORS 102
#define ID_FILE_EXIT 40001
#define ID_HELP_ABOUT 40002

static const TCHAR MAIN_WND_CLASS_NAME[] = TEXT ("My Super window class");
static const TCHAR MAIN_WND_NAME[] = TEXT ("Menu example");

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL RegisterMainWindowClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);
HMENU CreateMainMenu(HINSTANCE hInstance);
int MessageLoop();

void OnDestroy(HWND /*hwnd*/);
void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/);

HACCEL g_mainMenuAccelerators;

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

	g_mainMenuAccelerators = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));

	ShowWindow(hMainWindow, nCmdShow);

	int result = MessageLoop();

	return result;
}

int MessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, g_mainMenuAccelerators, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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
		CreateMainMenu(hInstance),
		hInstance,
		NULL);
}

HMENU CreateMainMenu(HINSTANCE hInstance)
{
	return LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));
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
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/)
{
	switch (id)
	{
	case ID_FILE_EXIT:
		DestroyWindow(hwnd);
		break;
	case ID_HELP_ABOUT:
		MessageBox(hwnd, L"Menu example. Created by Aleksey Malov aka Vivid.", L"About", MB_OK);
		break;
	}
}

void OnDestroy(HWND /*hwnd*/)
{
	PostQuitMessage(0);
}