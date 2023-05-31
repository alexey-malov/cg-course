// gl.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"
#include "Graphics.h"
#include "Utils.h"

#include "resource.h"

#ifndef GL_MIRRORED_REPEAT_ARB
#define GL_MIRRORED_REPEAT_ARB 0x8370
#endif

#ifndef GL_CLAMP_TO_BORDER
#define GL_CLAMP_TO_BORDER 0x812d
#endif

#ifndef GL_CLAMP_TO_EDGE_EXT
#define GL_CLAMP_TO_EDGE_EXT 0x812F
#endif

#define MAX_LOADSTRING 100
#define M_PI 3.1415927f

// Global Variables:
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = L"gl"; // window class name
const TCHAR WINDOW_TITLE[] = L"Texture wrapping"; // The title bar text

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

GLuint g_floorTexture = 0;

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
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
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				break;
			}
		}
		else
		{
			DrawScene();
			Sleep(3);
		}
	}

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_GL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = L"Menu";
	wcex.lpszClassName = WINDOW_CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
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

void SetProjection(double aspect)
{
	// устанавливаем матрицу проецирования (aspect - отношение ширины экрана к его высоте)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, aspect, 0.2, 20);
	glMatrixMode(GL_MODELVIEW);
}

void SetCamera()
{
	glLoadIdentity();
	gluLookAt(
		0, 3, 3, // позиция наблюдателя
		0, 0, 0, // точка, на которую смотрим
		0, 1, 0); // направление вектора "верх" в 3д-пространстве
}

float angleX = 0;
float angleY = 0;
float angleZ = 0;

DWORD lastTick = GetTickCount();
float g_delta = 0;

void ShiftTime()
{
	// подсчитываем время, прошедшее с момента последнего нарисованного кадра
	DWORD currentTick = GetTickCount();
	g_delta = (currentTick - lastTick) * 0.001f;
	lastTick = currentTick;
}

void Rotate()
{
	//	glTranslatef(-0.5, 0, 0);
	//	ShiftTime();

	float time = g_delta;

	glRotatef(angleX, 1, 0, 0);
	glRotatef(angleY, 0, 1, 0);
	glRotatef(angleZ, 0, 0, 1);

	/*
		angleX += 60 * time;
		if (angleX >= 360)
		{
			angleX -= 360;
		}
	*/
	angleY += 46 * time;
	if (angleY >= 360)
	{
		angleY -= 360;
	}
	/*
		angleZ += 53 * time;
		if (angleZ >= 360)
		{
			angleZ -= 360;
		}
	*/
}

void DrawFloor()
{
	Vector2d t0(0, 0);
	Vector2d t1(0, 4);
	Vector2d t2(4, 4);
	Vector2d t3(4, 0);

	Vector3d v0(-2, 0, -2);
	Vector3d v1(-2, 0, 2);
	Vector3d v2(2, 0, 2);
	Vector3d v3(2, 0, -2);

	glBegin(GL_QUADS);
	{
		t0.DeclareTextureCoord();
		v0.DeclareOpenGLVertex();

		t1.DeclareTextureCoord();
		v1.DeclareOpenGLVertex();

		t2.DeclareTextureCoord();
		v2.DeclareOpenGLVertex();

		t3.DeclareTextureCoord();
		v3.DeclareOpenGLVertex();
	}
	glEnd();
}

void DrawBasis()
{
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(2, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 2, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 2);
	}
	glEnd();
}

GLint g_textureWrapS = GL_REPEAT;
GLint g_textureWrapT = GL_REPEAT;

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 1);

	// ставим камеру в заданную точку
	SetCamera();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	ShiftTime();
	Rotate();

	glEnable(GL_TEXTURE_2D);
	// рисуем "пол"
	glBindTexture(GL_TEXTURE_2D, g_floorTexture);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, g_textureWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, g_textureWrapT);
	}
	DrawFloor();

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
	if (InitOpenGL(hWnd))
	{

		if (
			(g_floorTexture = LoadTextureFromRgbBitmapFile("texture.bmp")))
		{
			static const float borderColor[4] = { 1, 0, 0, 0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			return true;
		}
	}
	return false;
}

void OnDestroy(HWND hWnd)
{
	if (g_floorTexture)
	{
		glDeleteTextures(1, &g_floorTexture);
	}
	DestroyOpenGL();
}

void OnSize(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	glViewport(0, 0, width, height);
	SetProjection((double)width / (double)height);
}

void OnCommandMessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_GLTEXTUREWRAPS_GLREPEAT:
		g_textureWrapS = GL_REPEAT;
		break;
	case ID_GLTEXTUREWRAPS_GLCLAMP:
		g_textureWrapS = GL_CLAMP;
		break;
	case ID_GLTEXTUREWRAPT_GLREPEAT:
		g_textureWrapT = GL_REPEAT;
		break;
	case ID_GLTEXTUREWRAPT_GLCLAMP:
		g_textureWrapT = GL_CLAMP;
		break;
	case ID_GLTEXTUREWRAPS_GLMIRROREDREPEATARB:
		g_textureWrapS = GL_MIRRORED_REPEAT_ARB;
		break;
	case ID_GLTEXTUREWRAPT_GLMIRROREDREPEATARB:
		g_textureWrapT = GL_MIRRORED_REPEAT_ARB;
		break;
	case ID_GLTEXTUREWRAPS_GLCLAMPTOBORDERARB:
		g_textureWrapS = GL_CLAMP_TO_BORDER;
		break;
	case ID_GLTEXTUREWRAPT_GLCLAMPTOBORDERARB:
		g_textureWrapT = GL_CLAMP_TO_BORDER;
		break;
	case ID_GLTEXTUREWRAPS_GLCLAMPTOEDGEEXT:
		g_textureWrapS = GL_CLAMP_TO_EDGE_EXT;
		break;
	case ID_GLTEXTUREWRAPT_GLCLAMPTOEDGEEXT:
		g_textureWrapT = GL_CLAMP_TO_EDGE_EXT;
		break;
	default:
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		OnCommandMessage(wParam, lParam);
		break;
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
		OnDestroy(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
