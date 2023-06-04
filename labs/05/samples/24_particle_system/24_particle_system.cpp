// gl.cpp : Defines the entry point for the application.
//




#include "resource.h"

#include "../libgl/TextureLoader.h"
#include "../libgl/GdiPlusInitializer.h"

#include "../libgl/stdafx.h"
#include "../libgl/Graphics.h"
#include "../libgl/Utils.h"

#include "ParticleSystem.h"


#define MAX_LOADSTRING 100
#define M_PI 3.1415927f

// Global Variables:

CGdiPlusInitializer gdi;
CTextureLoader textureLoader;
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = L"gl"; // window class name
const TCHAR WINDOW_TITLE[] = L"Particle system"; // The title bar text

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

GLuint g_particleTexture = 0;
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
			Sleep(5);
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
		0, 0, -2, // точка, на которую смотрим
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

ParticleSystem particleSystem(8000);

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 1);

	// ставим камеру в заданную точку
	SetCamera();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	particleSystem.Process(g_delta);

	ShiftTime();
	Rotate();

	glEnable(GL_TEXTURE_2D);
	// рисуем "пол"
	glBindTexture(GL_TEXTURE_2D, g_floorTexture);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	DrawFloor();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_2D, g_particleTexture);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	particleSystem.Draw();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	// DrawBillboard(Vector3d(0.5f, 0 , 0), 2, 1);
	// DrawBasis();

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
			(g_particleTexture = textureLoader.LoadTexture2D(L"particle.bmp"))
			&& (g_floorTexture = textureLoader.LoadTexture2D(L"floor.bmp")))
		{
			return true;
		}
	}
	return false;
}

void OnDestroy(HWND hWnd)
{
	if (g_particleTexture)
	{
		glDeleteTextures(1, &g_particleTexture);
	}
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
	/*
	switch(wParam)
	{

	default:
		break;
	}
	*/
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
