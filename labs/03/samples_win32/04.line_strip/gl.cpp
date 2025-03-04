#include "framework.h"
//
#include "gl.h"
#include "../libgl/libgl.h"

// Global Variables:
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = TEXT("gl"); // window class name
const TCHAR WINDOW_TITLE[] = TEXT("Sine curves with GL_LINE_STRIP"); // The title bar text

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
HWND InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// Initialize global strings
	MyRegisterClass(hInstance);

	// Perform application initialization:
	HWND windowHandle = InitInstance(hInstance, nCmdShow);
	if (!windowHandle)
	{
		return FALSE;
	}

	// Main message loop:
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			InvalidateRect(windowHandle, nullptr, TRUE);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(
		WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)
	{
		return nullptr;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

ULONGLONG g_lastTick = GetTickCount64();
float g_currentAngleX = 0;
float g_currentAngleY = 0;
#define CURVE_LENGTH 100
#define CURVE_STEP_X ((float)(-5.25 * M_PI / 180))
#define CURVE_STEP_Y ((float)(-8.3 * M_PI / 180))
#define SPEED_X 63.52f
#define SPEED_Y 97.52f
void DrawLineStrip()
{
	// вычисляем время с момента предыдущего кадра
	ULONGLONG currentTick = GetTickCount64();
	float delta = (currentTick - g_lastTick) * 0.001f;
	g_lastTick = currentTick;

	// Вычисляем начальную фазу колебаний по X
	g_currentAngleX = g_currentAngleX + SPEED_X * delta;
	if (g_currentAngleX >= 360)
	{
		g_currentAngleX -= 360;
	}

	// Вычисляем начальную фазу колебаний по Y
	g_currentAngleY = g_currentAngleX + SPEED_Y * delta;
	if (g_currentAngleY >= 360)
	{
		g_currentAngleY -= 360;
	}

	float angleX = (float)(g_currentAngleX * M_PI / 180);
	float angleY = (float)(g_currentAngleY * M_PI / 180);
	glLineWidth(3);

	// Рисуем синусоиду
	glBegin(GL_LINE_STRIP);
	for (int pnt = 0; pnt < CURVE_LENGTH; ++pnt, angleX += CURVE_STEP_X, angleY += CURVE_STEP_Y)
	{
		glColor3f(
			0.5f + 0.5f * sinf(angleX),
			0.5f + 0.5f * cosf(angleY),
			0.5f + 0.5f * sinf(angleX + angleY));
		glVertex2f(cosf(angleX), sinf(angleY));
	}
	glEnd();
}

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

	DrawLineStrip();

	// завершающие действия - дожидаемся конца рисования и
	// меняем внеэкранный и экранный буферы местами
	EndDrawing();
}

void OnPaint(HWND hWnd)
{
	ValidateRect(hWnd, nullptr);

	DrawScene();
}

bool OnCreate(HWND hWnd)
{
	// инициализируем OpenGL
	return InitOpenGL(hWnd);
}

void OnSize(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	glViewport(0, 0, rc.right - rc.left, rc.bottom - rc.top);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		if (OnCreate(hWnd))
		{
			return 0;
		}
		else
		{
			return -1;
		}
		break;
	case WM_SIZE:
		OnSize(hWnd);
		break;
	case WM_PAINT:
		OnPaint(hWnd);
		break;
	case WM_DESTROY:
		DestroyOpenGL();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
