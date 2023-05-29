// 08.Timer.cpp : Определяет точку входа для приложения.
//
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "windowsx.h"
#include "cstdio"
#include "08.Timer.h"

static const TCHAR MAIN_WND_CLASS_NAME[] = TEXT ("My Super window class");
static const TCHAR MAIN_WND_NAME[] = TEXT ("This is a timer example");

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL RegisterMainWindowClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);
int MessageLoop();

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT /*lpCreateStruct*/);
void OnDestroy(HWND /*hwnd*/);
void OnTimer(HWND hwnd, UINT id);

static const UINT MAIN_TIMER_ID = 1;

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
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_TIMER, OnTimer);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void OnTimer(HWND hwnd, UINT id)
{
	if (id == MAIN_TIMER_ID)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);

		// I AM REALLY SURE 255 characters is enough for fitting the string below:
		wchar_t buffer[255]{};
		_swprintf(buffer, L"Current time is: %d.%.2d.%.4d - %.2d:%.2d:%.2d", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond);

		SetWindowText(hwnd, (LPCWSTR)buffer);
	}
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT /*lpCreateStruct*/)
{
	// create timer
	SetTimer(
		hwnd, // window that will receive WM_TIMER notification
		MAIN_TIMER_ID, // ID of the timer
		500, // timer interval in milliseconds
		NULL // timer procedure address (NULL means that window procedure will be called)
	);
	return TRUE;
}

void OnDestroy(HWND hwnd)
{
	KillTimer(hwnd, MAIN_TIMER_ID);
	PostQuitMessage(0);
}