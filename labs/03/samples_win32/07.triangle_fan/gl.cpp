// gl.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "gl.h"
#include "../libgl/libgl.h"

// Global Variables:
HINSTANCE hInst;                                               // current instance
const TCHAR WINDOW_CLASS_NAME[] = TEXT("gl");                  // window class name
const TCHAR WINDOW_TITLE[] = TEXT("Triangle fan");     // The title bar text

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

void DrawTriangleFan()
{
	// включаем проволочный режим рисования полигонов для лицевых и нелицевых граней
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_TRIANGLE_FAN);
	{
		glColor3d(1, 1, 1);
		// центральна¤ вершина "веера"
		glVertex3d(0, 0.5, 0);

		// теперь объявляем вершины веера, расположенные по его периметру
		for (int angle = 0; angle < 320; angle += 13)
		{
			double rad = M_PI * angle / 180;
			double x = 0.4 * cos(rad);
			double y = 0.4 * sin(rad);
			glColor3d(sin(rad) / 2 + 0.5, cos(rad) / 2 + 0.5, 0.5 - sin(rad) / 2);
			glVertex3d(x, y + 0.5, 0);
		}
	}
	glEnd();

	// включаем режим рисования полигонов "с заполнением" для лицевых и нелицевых граней
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_TRIANGLE_FAN);
	{
		glColor3d(1, 1, 1);
		glVertex3d(0, -0.5, 0);
		for (int angle = 0; angle < 320; angle += 13)
		{
			double rad = M_PI * angle / 180;
			double x = 0.4 * cos(rad);
			double y = 0.4 * sin(rad);
			glColor3d(sin(rad) / 2 + 0.5, cos(rad) / 2 + 0.5, 0.5 - sin(rad) / 2);
			glVertex3d(x, y - 0.5, 0);
		}
	}
	glEnd();
}

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

	DrawTriangleFan();

	// завершающие действи¤ - дожидаемс¤ конца рисовани¤ и
	// мен¤ем внеэкранный и экранный буферы местами
	EndDrawing();
}

void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

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
