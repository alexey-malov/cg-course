﻿// gl.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "gl.h"
#include "../libgl/libgl.h"

// Global Variables:
HINSTANCE hInst;                                               // current instance
const TCHAR WINDOW_CLASS_NAME[] = TEXT("gl");                  // window class name
const TCHAR WINDOW_TITLE[] = TEXT("Quad Strip");     // The title bar text

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

void DrawQuadStip()
{
	// включаем проволочный режим рисования полигонов для лицевых и нелицевых граней
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUAD_STRIP);
	{
		for (double x = -0.8; x <= 0.8; x += 0.2)
		{
			glColor3d((x + 1) * 0.5, 1 - (x + 1) * 0.5, 1);
			glVertex3d(x, 0.9, 0);

			glColor3d((x + 1) * 0.5, 1 - (x + 1) * 0.5, 0.5);
			glVertex3d(x, 0.3, 0);
		}
	}
	glEnd();

	// включаем режим рисования полигонов "с заполнением" для лицевых и нелицевых граней
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_QUAD_STRIP);
	{
		for (double x = -0.8; x <= 0.8; x += 0.2)
		{
			glColor3d((x + 1) * 0.5, 1 - (x + 1) * 0.5, 1);
			glVertex3d(x, -0.3, 0);

			glColor3d((x + 1) * 0.5, 1 - (x + 1) * 0.5, 0.5);
			glVertex3d(x, -0.9, 0);
		}
	}
	glEnd();
}

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

	DrawQuadStip();

	// завершающие действия - дожидаемся конца рисования и
	// меняем внеэкранный и экранный буферы местами
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
