// 10.menu.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "10.menu.h"
#include "windowsx.h"

#undef IDM_EXIT

static const TCHAR MAIN_WND_CLASS_NAME[] = TEXT ("My Super window class");
static const TCHAR MAIN_WND_NAME[] = TEXT ("Menu example");

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL RegisterMainWindowClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);
HMENU CreateMainMenu();
int MessageLoop();

void OnDestroy(HWND /*hwnd*/);
void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/);

typedef enum
{
	IDM_NONE = 0xffff,
	IDM_EXIT = 1,
} MenuItem;

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
	HMENU hMainMenu = CreateMainMenu();

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
		hMainMenu,
		hInstance,
		NULL);
}

HMENU CreateMainMenu()
{
	HMENU menu = CreateMenu();

	HMENU fileMenu = CreateMenu();

	MENUITEMINFO item;
	item.cbSize = sizeof(item);
	item.fMask = MIIM_STRING | MIIM_ID;
	item.fType = MFT_STRING;
	item.fState = MFS_UNCHECKED;
	item.wID = IDM_EXIT;
	item.hSubMenu = NULL;
	item.hbmpChecked = NULL;
	item.hbmpUnchecked = NULL;
	item.dwItemData = 0;
	item.dwTypeData = (LPWSTR)L"Exit";
	item.cch = 0;

	InsertMenuItem(fileMenu, 0, FALSE, &item);
	AppendMenu(menu, MF_POPUP, (UINT_PTR)fileMenu, L"File");

	return menu;
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
	case IDM_EXIT:
		DestroyWindow(hwnd);
		break;
	}
}

void OnDestroy(HWND /*hwnd*/)
{
	PostQuitMessage(0);
}