// GdiDemo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GdiDemo.h"

// Глобальные переменные
HINSTANCE g_hInstance;                                  // current instance
const TCHAR g_title[] = TEXT("Работа с изображениями"); // Текст в заголовке окна
const TCHAR g_windowClass[] = TEXT("GdiDemoClass");     // Имя класса окна

// графические ресурсы
HBITMAP g_dogBitmap = NULL;
HBITMAP g_catBitmap = NULL;

// инициализация приложения, обеспечение его минимальной работоспособности
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int MessageLoop();

// иницилизация / деинициализация ресурсов системы
void InitGraphicResources(HWND hWnd);
void FreeGraphicResources();

// обработчики сообщений
void OnPaint(HDC hdc);

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

// Цикл обработки сообщений
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

void InitGraphicResources(HWND hWnd)
{
	// загружаем битмап с собачкой
	g_dogBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DOG));
	g_catBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_CAT));
}

void FreeGraphicResources()
{
	DeleteObject(g_dogBitmap);
	DeleteObject(g_catBitmap);
}

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	// сообщение WM_CREATE приходит, когда окно только-только создано
	case WM_CREATE:
		// создаем ресурсы, необходимые нам для работы с графикой
		InitGraphicResources(hWnd);
		break;
	case WM_PAINT: {
		HDC hdc;
		PAINTSTRUCT ps;

		// в обработчике WM_PAINT для получения графического контекста необходимо обязательно
		// вызвать BeginPaint() перед рисование и EndPaint() в конце рисования
		hdc = BeginPaint(hWnd, &ps);

		OnPaint(hdc);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		// освобождаем все ресурсы системы, созданные нами во время работы
		FreeGraphicResources();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawDog(HDC hdc)
{
	// создаем контекст, совместимый с контекстом окна, но располагающийся в памяти
	HDC dcBitmap = CreateCompatibleDC(hdc);
	// и выбираем в него картинку с собачкой
	HGDIOBJ oldBitmap = SelectObject(dcBitmap, g_dogBitmap);

	// получаем параметры картинки с собачкой
	BITMAP bmp;
	GetObject(g_dogBitmap, sizeof(bmp), &bmp);

	int sourceX = 0;
	int sourceY = 0;
	int destinationX = 0;
	int destinationY = 0;
	int destinationWidth = bmp.bmWidth;
	int destinationHeight = bmp.bmHeight;

	// Выполняем копирование пикселей (Bit block transfer)
	BitBlt(
		hdc, // хэндл DC, НА который мы будет копировать пиксели
		destinationX, // координата X верхнего левого угла на области вывода
		destinationY, // координата Y верхнего левого угла на области вывода
		destinationWidth, // ширина выводимой области
		destinationHeight, // высота выводимой области
		dcBitmap, // хэндл DC, ИЗ которого мы будем копировать пиксели
		sourceX, // координата X на области-источнике пикселей
		sourceY, // координата Y на области-источнике пикселей
		SRCCOPY); // Код растровой операции (копирование с источника)

	SelectObject(dcBitmap, oldBitmap);
	DeleteDC(dcBitmap);
}

void DrawCat(HDC hdc)
{
	// создаем контекст, совместимый с контекстом окна, но располагающийся в памяти
	HDC dcBitmap = CreateCompatibleDC(hdc);
	// и выбираем в него картинку с собачкой
	HGDIOBJ oldBitmap = SelectObject(dcBitmap, g_catBitmap);

	// получаем параметры картинки с кошкой
	BITMAP bmp{};
	GetObject(g_catBitmap, sizeof(bmp), &bmp);

	int sourceX = 0;
	int sourceY = 0;
	int destinationX = 360;
	int destinationY = 250;
	int destinationWidth = bmp.bmWidth;
	int destinationHeight = bmp.bmHeight;

	// Выполняем копирование пикселей (Bit block transfer)
	BitBlt(
		hdc, // хэндл DC, НА который мы будет копировать пиксели
		destinationX, // координата X верхнего левого угла на области вывода
		destinationY, // координата Y верхнего левого угла на области вывода
		destinationWidth, // ширина выводимой области
		destinationHeight, // высота выводимой области
		dcBitmap, // хэндл DC, ИЗ которого мы будем копировать пиксели
		sourceX, // координата X на области-источнике пикселей
		sourceY, // координата Y на области-источнике пикселей
		SRCCOPY); // Код растровой операции (копирование с источника)

	SelectObject(dcBitmap, oldBitmap);
	DeleteDC(dcBitmap);
}

void OnPaint(HDC hdc)
{
	DrawDog(hdc);
	DrawCat(hdc);
}
