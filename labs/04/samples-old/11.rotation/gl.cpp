// gl.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "gl.h"
#include "Graphics.h"

#define MAX_LOADSTRING 100
#define M_PI 3.1415927

// Global Variables:
HINSTANCE hInst;                                               // current instance
const TCHAR WINDOW_CLASS_NAME[] = TEXT("gl");                  // window class name
const TCHAR WINDOW_TITLE[] = TEXT("Rotation");     // The title bar text

// Foward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    // TODO: Place code here.
    MSG msg;

    // Initialize global strings
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
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
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void DrawTriangle()
{
	glBegin(GL_TRIANGLES);
	{
		glColor3ub(255, 0, 0);
		glVertex3d(-0.4, -0.3, 0);

		glColor3ub(0, 255, 0);
		glVertex3d(0.4, -0.3, 0);

		glColor3ub(0, 0, 255);
		glVertex3d(0, 0.4, 0);
	}
	glEnd();
}

float angleX = 0;
float angleY = 0;
float angleZ = 0;

DWORD lastTick = GetTickCount();

void DrawObjects()
{
	// вращаем треугольник вокруг оси X
	{
		glLoadIdentity();
		glTranslatef(-0.6f, 0.6f, 0);
		glRotatef(angleX, 1, 0, 0);
		DrawTriangle();
	}

	// вращаем треугольник вокруг оси Y
	{
		glLoadIdentity();
		glTranslatef(0.6f, 0.6f, 0);
		glRotatef(angleY, 0, 1, 0);
		DrawTriangle();
	}

	// вращаем треугольник вокруг оси Z
	{
		glLoadIdentity();
		glTranslatef(0, -0.3f, 0);
		glRotatef(angleZ, 0, 0, 1);
		DrawTriangle();
	}

	// подсчитываем время, прошедшее с момента последнего нарисованного кадра
	DWORD currentTick = GetTickCount();
	float time = (currentTick - lastTick) * 0.001f;
	lastTick = currentTick;

	// вращаем первый треугольник со скоростью 60 градусов в секунду
	angleX += 60 * time;
	if (angleX >= 360)
	{
		angleX -= 360;
	}

	// вращаем второй треугольник со скоростью 30 градусов в секунду
	angleY += 30 * time;
	if (angleY >= 360)
	{
		angleY -= 360;
	}

	// вращаем третий треугольник со скоростью 70 градусов в секунду
	angleZ += 70 * time;
	if (angleZ >= 360)
	{
		angleZ -= 360;
	}
}

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

	DrawObjects();

	// завершающие действия - дожидаемся конца рисования и
	// меняем внеэкранный и экранный буферы местами
	EndDrawing();
}

void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	// Рисуем сцену - пока это будет просто закрашенная красным цветом клиенсткая область окна
	DrawScene();

	EndPaint(hWnd, &ps);
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
	case WM_ERASEBKGND:
		return 1; // делаем вид, что очистили фон окна
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