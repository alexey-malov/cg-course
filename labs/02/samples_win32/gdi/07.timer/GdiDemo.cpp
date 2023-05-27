// GdiDemo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GdiDemo.h"

// Глобальные переменные
HINSTANCE g_hInstance;                                // current instance
const TCHAR g_title[] = TEXT("Gdi demo");             // Текст в заголовке окна
const TCHAR g_windowClass[] = TEXT("GdiDemoClass");   // Имя класса окна

const UINT_PTR DRAW_TIMER_ID = 1;
UINT_PTR g_drawTimerId = 0; // идентификатор таймера, который будет вызывать перерисовку нашего окна

// графические ресурсы
HBITMAP g_dogBitmap = NULL;
HBITMAP g_catBitmap = NULL;
HBITMAP g_catMask = NULL;
HBITMAP g_backBuffer = NULL; // Bitmap для виртуального экрана
HDC g_backBufferDC = NULL; // контекст виртуального экрана
HGDIOBJ g_oldBackBufferBitmap = NULL;
HBITMAP g_snailBitmap; // изображение улитки
HBITMAP g_snailMask; // маска улитки

int g_snailX = 10; // x - координата спрайта улитки
int g_snailY = 180; // y - координата спрайта улитки

int g_screenWidth; // ширина внеэкранного буффера
int g_screenHeight; // высота внеэкранного буффера

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
void OnTimer(HWND hWnd, UINT_PTR idTimer);

// Точка входа в приложение
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
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

void InitBackBuffer()
{
	// запрашиваем у системы размеры экрана
	g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// получаем DC экрана
	HDC screenDC = GetDC(NULL);

	// создаем совместимый с контекстом экрана Bitmap размером с экран
	g_backBuffer = CreateCompatibleBitmap(
		screenDC,
		g_screenWidth,
		g_screenHeight);

	// создаем контекст в памяти, совместимый с контекстом экрана
	g_backBufferDC = CreateCompatibleDC(screenDC);

	// и выбираем в него наш Bitmap нашего виртуального экрана
	g_oldBackBufferBitmap = SelectObject(g_backBufferDC, g_backBuffer);

	// контекст экрана нам больше не нужен, поэтому мы его освобождаем
	ReleaseDC(NULL, screenDC);
}

void DeleteBackBuffer()
{
	SelectObject(g_backBufferDC, g_oldBackBufferBitmap);
	DeleteObject(g_backBuffer);
	DeleteDC(g_backBufferDC);
}

void InitGraphicResources(HWND hWnd)
{
	// загружаем битмап с собачкой
	g_dogBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DOG));

	g_catBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_CAT));
	g_catMask = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_CAT_MASK));

	g_snailBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_SNAIL));
	g_snailMask = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_SNAIL_MASK));

	InitBackBuffer();
}

void FreeGraphicResources()
{
	DeleteObject(g_dogBitmap);

	DeleteObject(g_catBitmap);
	DeleteObject(g_catMask);

	DeleteObject(g_snailBitmap);
	DeleteObject(g_snailMask);

	DeleteBackBuffer();
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

void InitDrawTimer(HWND hWnd)
{
	// инициализируем таймер для окна
	g_drawTimerId = SetTimer(
		hWnd, // хэндл окна, которое будет получать сообщения от устанавливаемого таймера
		DRAW_TIMER_ID, // ненулевой идентификатор таймера
		50, // период (в миллисекундах) между событиями таймера
		NULL); // адрес функции-обработчика таймера (нам этот параметр не нужен)
}

void DeleteDrawTimer(HWND hWnd)
{
	// прибиваем таймер (нам он больше не нужен)
	KillTimer(hWnd, g_drawTimerId);
	g_drawTimerId = 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	// сообщение WM_CREATE приходит, когда окно только-только создано
	case WM_CREATE:
		// создаем ресурсы, необходимые нам для работы с графикой
		InitGraphicResources(hWnd);
		InitDrawTimer(hWnd);
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
	case WM_ERASEBKGND:
		return 1; // сигнализируем системе о том, что мы очистили фон окна
	case WM_TIMER: {
		UINT_PTR idTimer = wParam;
		OnTimer(hWnd, idTimer);
	}
	break;
	case WM_DESTROY:
		// освобождаем все ресурсы системы, созданные нами во время работы
		DeleteDrawTimer(hWnd);
		FreeGraphicResources();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawPicture(HDC hdc, HBITMAP picture, int x, int y)
{
	// создаем контекст, совместимый с контекстом окна, но располагающийся в памяти
	HDC dcBitmap = CreateCompatibleDC(hdc);
	// и выбираем в него картинку с собачкой
	HGDIOBJ oldBitmap = SelectObject(dcBitmap, picture);

	// получаем параметры картинки с собачкой
	BITMAP bmp{};
	GetObject(picture, sizeof(bmp), &bmp);

	int sourceX = 0;
	int sourceY = 0;
	int destinationX = x;
	int destinationY = y;
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

void DrawSpriteWithMask(HDC hdc, HBITMAP sprite, HBITMAP mask, int x, int y)
{
	// создаем контекст, совместимый с контекстом окна, но располагающийся в памяти
	HDC dcBitmap = CreateCompatibleDC(hdc);
	// и выбираем в него картинку с собачкой
	HGDIOBJ oldBitmap = SelectObject(dcBitmap, sprite);

	// получаем параметры картинки с кошкой
	BITMAP bmp{};
	GetObject(sprite, sizeof(bmp), &bmp);

	int sourceX = 0;
	int sourceY = 0;
	int destinationX = x;
	int destinationY = y;
	int destinationWidth = bmp.bmWidth;
	int destinationHeight = bmp.bmHeight;
	int maskX = 0;
	int maskY = 0;

	// Выполняем копирование пикселей (Bit block transfer)
	MaskBlt(
		hdc, // хэндл DC, НА который мы будет копировать пиксели
		destinationX, // координата X верхнего левого угла на области вывода
		destinationY, // координата Y верхнего левого угла на области вывода
		destinationWidth, // ширина выводимой области
		destinationHeight, // высота выводимой области
		dcBitmap, // хэндл DC, ИЗ которого мы будем копировать пиксели
		sourceX, // координата X на области-источнике пикселей
		sourceY, // координата Y на области-источнике пикселей
		mask, // монохромный битмап маски
		maskX, // координата X левого верхнего угла маски
		maskY, // координата Y левого верхнего угла маски
		MAKEROP4(SRCCOPY, SRCAND)); // Коды растровых операций для переднего и заднего фона.

	SelectObject(dcBitmap, oldBitmap);
	DeleteDC(dcBitmap);
}

void ClearBackBuffer()
{
	// просто рисуем прямоугольник размером с теневой буфер
	HBRUSH fillBrush = CreateSolidBrush(RGB(160, 20, 255));
	RECT rc{};
	rc.left = 0;
	rc.top = 0;
	rc.right = g_screenWidth;
	rc.bottom = g_screenHeight;

	FillRect(g_backBufferDC, &rc, fillBrush);
	DeleteObject(fillBrush);
}

void ShowBackBuffer(HDC hdc)
{
	// копируем содержимое контекста виртуального экрана на контекст окна
	BitBlt(
		hdc, // хэндл DC, НА который мы будет копировать пиксели
		0, // координата X верхнего левого угла на области вывода
		0, // координата Y верхнего левого угла на области вывода
		g_screenWidth, // ширина выводимой области
		g_screenHeight, // высота выводимой области
		g_backBufferDC, // хэндл DC, ИЗ которого мы будем копировать пиксели
		0, // координата X на области-источнике пикселей
		0, // координата Y на области-источнике пикселей
		SRCCOPY); // Код растровой операции (копирование с источника)
}

void DrawDog(HDC hdc)
{
	DrawPicture(hdc, g_dogBitmap, 0, 0);
}

void DrawCat(HDC hdc)
{
	DrawSpriteWithMask(hdc, g_catBitmap, g_catMask, 200, 170);
}

void DrawSnail(HDC hdc)
{
	DrawSpriteWithMask(hdc, g_snailBitmap, g_snailMask, g_snailX, g_snailY);
}

void OnPaint(HDC hdc)
{
	// очищаем виртуальный экран
	ClearBackBuffer();

	// отрисовываем зверюшек на виртуальном экран
	DrawDog(g_backBufferDC);
	DrawSnail(g_backBufferDC);
	DrawCat(g_backBufferDC);

	// копируем виртуальный экран на контекст окна
	ShowBackBuffer(hdc);
}

void OnTimer(HWND hWnd, UINT_PTR idTimer)
{
	// сообщаем системе о необходимости перерисовки нашего окна
	RECT rc;
	GetClientRect(hWnd, &rc);
	RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);

	// меняем координату улитки
	if (g_snailX < 500)
	{
		g_snailX += 1;
	}
}
