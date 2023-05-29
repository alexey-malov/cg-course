// 07.ChildWindows.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "windowsx.h"
#include "07.ChildWindows.h" 

static const TCHAR MAIN_WND_CLASS_NAME[] = TEXT("My Super window class");
static const TCHAR MAIN_WND_NAME[] = TEXT("Child windows");

typedef enum
{
	ID_NONE = 0xffff,
	ID_PUSHME_BUTTON = 1,
	ID_DONTPUSHME_BUTTON,
	ID_YOURNAME_EDITBOX,
} ChildId;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL RegisterMainWindowClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);
int MessageLoop();
HINSTANCE g_appInstance = NULL;

HWND CreateButton(HWND parent, const TCHAR* caption, HMENU id, int x, int y, int width, int height);
HWND CreateStaticText(HWND parent, const TCHAR* text, HMENU id, int x, int y, int width, int height);
HWND CreateEditBox(HWND parent, const TCHAR* text, HMENU id, int x, int y, int width, int height);

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	g_appInstance = hInstance;

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
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
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
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/)
{
	switch (id)
	{
	case ID_PUSHME_BUTTON:
		MessageBox(hwnd, L"Thank you!", L"PushMe button pressed", MB_OK | MB_ICONINFORMATION);
		return;
	case ID_DONTPUSHME_BUTTON:
		MessageBox(hwnd, L"I told you don't push me!!!", L"Grrrrr!!!", MB_OK | MB_ICONSTOP);
		return;
	}
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT /*lpCreateStruct*/)
{
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);

	CreateButton(hwnd, L"Push Me", (HMENU)ID_PUSHME_BUTTON, rcClient.right - 75, rcClient.bottom - 30, 70, 25);
	CreateButton(hwnd, L"Don't Push Me", (HMENU)ID_DONTPUSHME_BUTTON, 5, rcClient.bottom - 30, 120, 25);
	CreateButton(hwnd, L"I don't care", (HMENU)ID_NONE, (rcClient.right - 120) / 2, rcClient.bottom - 30, 120, 25);

	CreateEditBox(hwnd, L"...", (HMENU)ID_YOURNAME_EDITBOX, 130, 5, 200, 25);

	CreateStaticText(hwnd, L"Enter your name: ", (HMENU)ID_NONE, 5, 5, 120, 25);

	return TRUE; // successful creation
}

void OnDestroy(HWND /*hwnd*/)
{
	PostQuitMessage(0);
}

HWND CreateButton(HWND parent, const TCHAR* caption, HMENU id, int x, int y, int width, int height)
{
	HWND hButtonWindow = CreateWindowEx(
		0,
		L"BUTTON",
		caption,
		WS_BORDER | WS_CHILDWINDOW | WS_VISIBLE,
		x, y,
		width,
		height,
		parent,
		(HMENU)id,
		g_appInstance,
		NULL);

	return hButtonWindow;
}

HWND CreateEditBox(HWND parent, const TCHAR* text, HMENU id, int x, int y, int width, int height)
{
	HWND hEditBox = CreateWindowEx(
		0,
		L"EDIT",
		text,
		WS_BORDER | WS_CHILDWINDOW | WS_VISIBLE,
		x, y,
		width,
		height,
		parent,
		(HMENU)id,
		g_appInstance,
		NULL);

	return hEditBox;
}

HWND CreateStaticText(HWND parent, const TCHAR* text, HMENU id, int x, int y, int width, int height)
{
	HWND hStaticText = CreateWindowEx(
		0, // extended style
		L"STATIC", // static window class name
		text, // text
		WS_CHILDWINDOW | WS_VISIBLE,
		x, y,
		width,
		height,
		parent,
		(HMENU)id,
		g_appInstance,
		NULL);

	return hStaticText;
}

