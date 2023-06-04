// sample02_simplest_winapi_application.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "sample02_simplest_winapi_application.h"

TCHAR const CLASS_NAME[] = _T("MainWndClass");
TCHAR const WINDOW_TITLE[] = _T("My OpenGL Application");

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int MainLoop();
bool RegisterWndClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);
bool InitOpenGL(HWND hwnd, bool needDepth = true, bool needStencil = false);
void DrawScene();

// контекст устройства главного окна приложения для OpenGL
HDC g_glDC = NULL;
// контекст рендеринга OpenGL
HGLRC g_glRC = NULL;

HDC CreateDeviceContext(HWND hwnd, bool depthBuffer, bool stencilBuffer)
{
	HDC hdc = GetDC(hwnd);
	if (hdc == NULL)
	{
		return NULL;
	}

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));

	// Заполняем поля структуры PIXELFORMATDESCRIPTOR,
	// задающей желаемые параметры буфера кадра OpenGL
	pfd.nSize = sizeof(pfd); // инициализируем размер структуры
	pfd.nVersion = 1; // Windows поддерживает только версию 1
	pfd.dwFlags = PFD_SUPPORT_OPENGL | // буфер поддерживает рисование OpenGL
		PFD_DRAW_TO_WINDOW | // буфер поддерживает рисование в окне
		PFD_DOUBLEBUFFER | // используется двойная буферизация
		PFD_GENERIC_ACCELERATED; // и аппаратное ускорение
	pfd.iPixelType = PFD_TYPE_RGBA; // формат пикселей - RGBA
	pfd.cColorBits = 32; // 32 бита на хранение цвета
	pfd.cDepthBits = depthBuffer ? 24 : 0; // если нужно, испольузем 24-битный буфер глубины
	pfd.cStencilBits = stencilBuffer ? 8 : 0; // если нужно, используем 8-битный буфер трафарета

	// Выбираем наиболее подходящий формат пикселей,
	// поддерживаемый контекстом устройства
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);

	// Пытаемся установить данный формат пикселей в контексте устройства
	if (
		(pixelFormat == 0) || !SetPixelFormat(hdc, pixelFormat, &pfd))
	{
		// Если не получилось, то освобождаем контекст устройства и выходим
		ReleaseDC(hwnd, hdc);
		return NULL;
	}

	// Если все хорошо, то возвращаем настроенный контекст устройства
	return hdc;
}

HGLRC CreateRenderContext(HDC hdc)
{
	// Создаем контекст рендеринга OpenGL
	HGLRC hrc = wglCreateContext(hdc);
	if (!hrc)
	{
		return NULL;
	}

	// Пытаемся сделать данный контекст рендеринга текущим
	// в контексте устройства
	if (!wglMakeCurrent(hdc, hrc))
	{
		wglDeleteContext(hrc);
		return NULL;
	}

	// Возвращаем контекст рендеринга в случае успеха
	return hrc;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT /*lpCreateStruct*/)
{
	// Инициализируем OpenGL и возвращаем результат
	return InitOpenGL(hwnd);
}

bool InitOpenGL(HWND hwnd, bool needDepth, bool needStencil)
{
	// Настраиваем контекст устройства для рендеринга внутри окна
	HDC hdc = CreateDeviceContext(hwnd, needDepth, needStencil);
	if (!hdc)
	{
		return false;
	}

	HGLRC hglRC = CreateRenderContext(hdc);
	if (!hglRC)
	{
		ReleaseDC(hwnd, hdc);
		return false;
	}

	// Сохраняем дескриптоы в глобальных переменных
	g_glDC = hdc;
	g_glRC = hglRC;

	return TRUE;
}

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(hwnd, &ps);
	EndPaint(hwnd, &ps);
	DrawScene();
}

// Рисуем контур эллипса
void DrawEllipse(float xCenter, float yCenter, float rx, float ry, int points = 360)
{
	const float step = (float)(2 * M_PI / points);

	// Эллипс представлен в виде замкнутой ломаной линии, соединяющей
	// points точек на его границе с шагом 2*PI/points
	glBegin(GL_LINE_LOOP);
	for (float angle = 0; angle < 2 * M_PI; angle += step)
	{
		const float dx = rx * cosf(angle);
		const float dy = ry * sinf(angle);
		glVertex2f(dx + xCenter, dy + yCenter);
	}
	glEnd();
}

void DrawScene()
{
	/*
	Задаем цвет очистки буфера кадра (желтый)
	*/
	glClearColor(1, 1, 0, 1);

	// Очищаем буфер цвета и буфер глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Делаем текущий цвет вершины синим
	glColor3f(0, 0, 1);

	// Рисуем эллипс (как ломаную линию)
	DrawEllipse(300, 200, 150, 120);

	// Обмениваем лицевой и нелицевой буферы кадра местами
	SwapBuffers(g_glDC);
}

void OnSize(HWND /*hwnd*/, UINT /*state*/, int width, int height)
{
	// Настраиваем порт просмотра
	glViewport(0, 0, width, height);

	// Делаем текущей матрицу проецирования и настраиваем ее параметры
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);

	// Делаем текущей матрицей матрицу моделирования-вида
	glMatrixMode(GL_MODELVIEW);
}

// Разрушаем контекст OpenGL и освобождаем контекст устройства
void DestroyOpenGL(HWND hwnd)
{
	if (g_glRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_glRC);
		g_glRC = NULL;
	}

	if (g_glDC)
	{
		ReleaseDC(hwnd, g_glDC);
		g_glDC = NULL;
	}
}

void OnDestroy(HWND hWnd)
{
	// Разрушаем OpenGL
	DestroyOpenGL(hWnd);
	PostQuitMessage(0);
}

BOOL OnEraseBkgnd(HWND /*hwnd*/, HDC /*hdc*/)
{
	// Сообщаем Windows, что стирание заднего фона выполнено
	return TRUE;
}

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_SIZE, OnSize);
		HANDLE_MSG(hwnd, WM_ERASEBKGND, OnEraseBkgnd);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wndClass = {
		sizeof(wndClass), // UINT cbSize;
		CS_HREDRAW | CS_VREDRAW, // UINT style;
		&WindowProc, // WNDPROC lpfnWndProc;
		0, // int cbClsExtra;
		0, // int cbWndExtra;
		hInstance, // HINSTANCE hInstance;
		NULL, // HICON hIcon;
		LoadCursor(NULL, IDC_ARROW), // HCURSOR hCursor;
		(HBRUSH)(COLOR_BTNFACE + 1), // HBRUSH hbrBackground;
		NULL, // LPCTSTR lpszMenuName;
		CLASS_NAME, // LPCTSTR lpszClassName;
		NULL, // HICON hIconSm;
	};

	return RegisterClassEx(&wndClass) != FALSE;
}

// Создаем главное окно приложения
HWND CreateMainWindow(HINSTANCE hInstance)
{
	HWND hMainWindow = CreateWindowEx(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | // Windows-реализация OpenGL требует,
			WS_CLIPSIBLINGS, // чтобы данные флаги окна были установлены
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	return hMainWindow;
}

int MainLoop()
{
	MSG msg;
	BOOL res;
	while ((res = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (res == -1)
		{
			// произошла ошибка - нужно обработать ее и, вероятно,
			// завершить работу приложения
		}
		else
		{
			// Если это сообщение о нажатии виртуальной клавиши,
			// то добавляем в очередь сообщений сообщения, несущие информацию о
			// коде вводимого пользователем символа
			TranslateMessage(&msg);
			// передаем сообщение в соответствующую оконную процедуру
			DispatchMessage(&msg);
		}
	}

	// сюда мы попадем только в том случае извлечения сообщения WM_QUIT
	// msg.wParam содержит код возврата, помещенный при помощи функции PostQuitMessage()
	return (int)msg.wParam;
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// Регистрируем класс главного окна
	if (!RegisterWndClass(hInstance))
	{
		return 1;
	}

	// Создаем главное окно приложения
	HWND hMainWindow = CreateMainWindow(hInstance);
	if (hMainWindow == NULL)
	{
		return 1;
	}

	// Показываем главное окно приложения
	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	// Запускаем цикл выборки сообщений, пока не получим
	// сигнал о завершении приложения
	return MainLoop();
}