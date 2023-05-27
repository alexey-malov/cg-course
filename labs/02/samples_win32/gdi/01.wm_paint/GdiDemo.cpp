// GdiDemo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GdiDemo.h"

// Глобальные переменные
HINSTANCE g_hInstance;                                // current instance
const TCHAR g_title[] = TEXT("Gdi demo");             // Текст в заголовке окна
const TCHAR g_windowClass[] = TEXT("GdiDemoClass");   // Имя класса окна

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnPaint(HWND hWnd);
int MessageLoop();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
	// Регистрируем класс окна
	MyRegisterClass(hInstance);

	// Инициализируем приложение
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	return MessageLoop();
}

// Цикл обработки сообщений сообщений
int MessageLoop()
{
	MSG msg;
	// выбираем сообщения из очереди до тех пор, пока приложение не завершит свою работу - т.е. не вызовет PostQuitMessage
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	// Инициализируем структуру, задающую класс нашего окна
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_GDIDEMO);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCWSTR)IDC_GDIDEMO;
	wcex.lpszClassName = g_windowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	// Регистрируем класс нашего окна
	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	g_hInstance = hInstance; // Store instance handle in our global variable

	// создаем окно
	hWnd = CreateWindow(g_windowClass, g_title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	// показываем окно
	ShowWindow(hWnd, nCmdShow);

	// обновляем окно, вызывая его перерисовку
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	// сообщение WM_PAINT приходит, когда окно, или его часть требует перерисовки
	case WM_PAINT:
		OnPaint(hWnd);
		break;
	// Сообщение WM_DESTROY приходит перед уничтожением окна
	case WM_DESTROY:
		// функция PostQuitMessage сообщает системе, о том, что программный поток делает запрос на завершение
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	// get Paint DC
	HDC dc = BeginPaint(hwnd, &ps);

	RECT rc;
	rc.left = 10;
	rc.top = 50;
	rc.right = rc.left + 100;
	rc.bottom = rc.top + 75;

	HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
	FillRect(dc, &rc, redBrush);
	DeleteBrush(redBrush);

	EndPaint(hwnd, &ps);
}
