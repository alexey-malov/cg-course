// gl.cpp : Defines the entry point for the application.
//
#include "Resource.h"

#include "../libgl/GdiPlusInitializer.h"

#include "../libgl/StdAfx.h"
#include "../libgl/Graphics.h"
#include "../libgl/Utils.h"

#define MAX_LOADSTRING 100
#define M_PI 3.1415927

// Global Variables:
CGdiPlusInitializer gdi;
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = L"gl"; // window class name
const TCHAR WINDOW_TITLE[] = L"Stencil buffer"; // The title bar text

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

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
			Sleep(10);
		}
	}

	return static_cast<int>(msg.wParam);
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
	wcex.lpszMenuName = (LPCTSTR)IDR_MAIN_MENU;
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

void DrawCube()
{
	/*
		|Y
		|
		4--------5
	   /|       /|
	  / |      / |
	 /  |     /  |
	6--------7   |
	|   |    |   |
	|   0----|---1----X
	|  /     |  /
	| /      | /
	|/       |/
	2--------3
   /
  /
 Z
	*/
	static const Vector3d cubeVertices[8] = {
		Vector3d(-1, -1, -1), // 0
		Vector3d(-1, -1, +1), // 1
		Vector3d(+1, -1, -1), // 2
		Vector3d(+1, -1, +1), // 3
		Vector3d(-1, +1, -1), // 4
		Vector3d(-1, +1, +1), // 5
		Vector3d(+1, +1, -1), // 6
		Vector3d(+1, +1, +1), // 7
	};

	/*
	если расскомментировать эти строки, то куб будет цветным
	static const QuadFace cubeFaces[6] =
	{
		{2, 3, 1, 0, {1, 0, 0, 1}},
		{7, 3, 2, 6, {0, 1, 0, 1}},
		{5, 1, 3, 7, {0, 0, 1, 1}},
		{4, 0, 1, 5, {1, 1, 0, 1}},
		{6, 2, 0, 4, {0, 1, 1, 1}},
		{5, 7, 6, 4, {1, 0, 1, 1}},
	};
	*/

	static const QuadFace cubeFaces[6] = {
		{ 2, 3, 1, 0, { 0.4f, 0.4f, 0.4f, 1 } },
		{ 7, 3, 2, 6, { 0.4f, 0.4f, 0.4f, 1 } },
		{ 5, 1, 3, 7, { 0.4f, 0.4f, 0.4f, 1 } },
		{ 4, 0, 1, 5, { 0.4f, 0.4f, 0.4f, 1 } },
		{ 6, 2, 0, 4, { 0.4f, 0.4f, 0.4f, 1 } },
		{ 5, 7, 6, 4, { 0.4f, 0.4f, 0.4f, 1 } },
	};

	static const int numFaces = sizeof(cubeFaces) / sizeof(QuadFace);
	glBegin(GL_QUADS);
	{
		for (int faceIndex = 0; faceIndex < numFaces; ++faceIndex)
		{
			QuadFace const& face = cubeFaces[faceIndex];
			face.faceColor.SetOpenGLColor();
			Vector3d const& v0 = cubeVertices[face.v0];
			Vector3d const& v1 = cubeVertices[face.v1];
			Vector3d const& v2 = cubeVertices[face.v2];
			Vector3d const& v3 = cubeVertices[face.v3];
			Vector3d normal;
			CalculateTriangleNormal(v0, v1, v2, &normal);
			normal.DeclareOpenGLNormal();
			v0.DeclareOpenGLVertex();
			v1.DeclareOpenGLVertex();
			v2.DeclareOpenGLVertex();
			v3.DeclareOpenGLVertex();
		}
	}
	glEnd();
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
		0, 0, 4, // позиция наблюдателя
		0, 0, 0, // точка, на которую смотрим
		0, 1, 0); // направление вектора "верх" в 3д-пространстве
}

void SetLights()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	const float diffuseColor[4] = { 0.8f, 0.8f, 0.8f, 1 };
	const float ambientColor[4] = { 0.8f, 0.8f, 0.8f, 1 };
	const float specularColor[4] = { 1.2f, 1.2f, 1.2f, 1 };
	const float lightDirection[4] = { 0.0f, 0.0f, 1.0f, 0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
}

void SetCubeMaterial()
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	float specular[4] = { 0.7f, 0.7f, 0.7f, 1 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 10);
}

float angleX = 0;
float angleY = 0;
float angleZ = 0;

DWORD lastTick = GetTickCount();

void RotateCube()
{
	//	glTranslatef(-0.5, 0, 0);

	glRotatef(angleX, 1, 0, 0);
	glRotatef(angleY, 0, 1, 0);
	glRotatef(angleZ, 0, 0, 1);

	// подсчитываем время, прошедшее с момента последнего нарисованного кадра
	DWORD currentTick = GetTickCount();
	float time = (currentTick - lastTick) * 0.001f;
	lastTick = currentTick;

	angleX += 60 * time;
	if (angleX >= 360)
	{
		angleX -= 360;
	}

	angleY += 46 * time;
	if (angleY >= 360)
	{
		angleY -= 360;
	}

	angleZ += 53 * time;
	if (angleZ >= 360)
	{
		angleZ -= 360;
	}
}

bool g_stencilEnabled = true;
bool g_stencilVisible = true;
void DrawStencil()
{
	// glPushMatrix();

	glDisable(GL_LIGHTING);
	// glLoadIdentity();
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		if (g_stencilEnabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		if (!g_stencilVisible)
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		}
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 255);
	}
	glColor4f(0, 0, 1, 1);
	glBegin(GL_POLYGON);
	{
		for (int alpha = 0; alpha < 360; alpha += 5)
		{
			glVertex3d(1.5 * cos(M_PI * alpha / 180), 1.5 * sin(M_PI * alpha / 180), 0);
		}
	}
	glEnd();

	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
	}

	// glPopMatrix();
}

void DrawScene()
{
	glClearStencil(0);
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

	// ставим камеру в заданную точку
	SetCamera();

	SetLights();
	SetCubeMaterial();

	// включаем тест глубины для корректного отображение невидимых поверхностей
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_LIGHTING);
	DrawStencil();

	// вращаем объект
	RotateCube();

	{
		if (g_stencilEnabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		glStencilFunc(GL_EQUAL, 1, 255);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// рисуем куб
		glEnable(GL_LIGHTING);
		DrawCube();
		glDisable(GL_STENCIL_TEST);
	}

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
	return InitOpenGL(hWnd, 32, 16, 8);
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

void OnCommand(WPARAM wParam)
{
	switch (wParam)
	{
	case ID_STENCIL_OFF:
		g_stencilEnabled = false;
		break;
	case ID_STENCIL_ON:
		g_stencilEnabled = true;
		break;
	case ID_STENCILVISIBILITY_OFF:
		g_stencilVisible = false;
		break;
	case ID_STENCILVISIBILITY_ON:
		g_stencilVisible = true;
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
		OnCommand(wParam);
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
		DestroyOpenGL();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
