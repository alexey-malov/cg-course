// GdiDemo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GdiDemo.h"

// Глобальные переменные
HINSTANCE g_hInstance;                                                       // current instance
const TCHAR g_title[] = TEXT("Работа со шрифтами и их свойствамиo");         // Текст в заголовке окна
const TCHAR g_windowClass[] = TEXT("GdiDemoClass");                          // Имя класса окна
const TCHAR g_message[] = TEXT("Работа со шрифтами и их свойствами!");       // строка, для вывода на поверхность окна
HFONT g_messageFont = NULL;

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

void InitGraphicResources(HWND hWnd)
{
	g_messageFont = CreateFont(
		50, // Высота шрифта
		0, // Средняя ширина шрифта
		0, // Угол наклона вертикальной оси шрифта в десятых долях градуса
		0, // угол наклона горизонтальной оси шрифта
		FW_BOLD, // font weight
		FALSE, // Курсив
		FALSE, // Подчеркивание
		FALSE, // Перечеркивание
		DEFAULT_CHARSET, // Набор символов
		OUT_DEFAULT_PRECIS, // Точность вывода
		CLIP_DEFAULT_PRECIS, // Точность отсечения символов, выходящих за область отсвечения
		DEFAULT_QUALITY, // Качество вывода
		FF_DONTCARE | DEFAULT_PITCH, // Расстояние между символами (Pitch) и семейство шрифтов (FontFamily)
		TEXT("Comic Sans MS") // Имя шрифта
	);
}

void FreeGraphicResources()
{
	DeleteObject(g_messageFont);
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

void OnPaint(HDC hdc)
{
	// сохраняем предыдущий фонт контекста устройства и выбираем созданный фонт
	HGDIOBJ oldFont = SelectObject(hdc, g_messageFont);

	SetTextColor(hdc, RGB(70, 120, 35));
	// выводим текст на экран новым цветом и шрифтом
	TextOut(hdc, 0, 0, g_message, (int)_tcslen(g_message));

	// восстанавливаем предыдущий фонт контекста устройства
	SelectObject(hdc, oldFont);
}
