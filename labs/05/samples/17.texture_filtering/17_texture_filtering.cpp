// gl.cpp : Defines the entry point for the application.
//

#include "resource.h"

#include "../libgl/GdiPlusInitializer.h"
#include "../libgl/TextureLoader.h"

#include "../libgl/Graphics.h"
#include "../libgl/Utils.h"
#include "../libgl/stdafx.h"

#define MAX_LOADSTRING 100
#define M_PI 3.1415927

// Global Variables:

CGdiPlusInitializer gdi;
CTextureLoader textureLoader;
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = L"gl"; // window class name
const TCHAR WINDOW_TITLE[] = L"Texture filtering modes"; // The title bar text

GLuint g_sheepTexture = 0;

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

GLint g_minFilter = GL_NEAREST;
GLint g_magFilter = GL_NEAREST;

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

	static const QuadFace cubeFaces[6] = {
		{ 2, 3, 1, 0, { 1, 0, 0, 1 } },
		{ 7, 3, 2, 6, { 0, 1, 0, 1 } },
		{ 5, 1, 3, 7, { 0, 0, 1, 1 } },
		{ 4, 0, 1, 5, { 1, 1, 0, 1 } },
		{ 6, 2, 0, 4, { 0, 1, 1, 1 } },
		{ 5, 7, 6, 4, { 1, 0, 1, 1 } },
	};

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_sheepTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_magFilter);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3f(1, 1, 1);

	static const int numFaces = sizeof(cubeFaces) / sizeof(QuadFace);
	glBegin(GL_QUADS);
	{
		for (int faceIndex = 0; faceIndex < numFaces; ++faceIndex)
		{
			QuadFace const& face = cubeFaces[faceIndex];
			// face.faceColor.SetOpenGLColor();
			Vector3d const& v0 = cubeVertices[face.v0];
			Vector3d const& v1 = cubeVertices[face.v1];
			Vector3d const& v2 = cubeVertices[face.v2];
			Vector3d const& v3 = cubeVertices[face.v3];
			Vector3d normal;
			CalculateTriangleNormal(v0, v1, v2, &normal);
			normal.DeclareOpenGLNormal();

			glTexCoord2f(1, 1);
			v0.DeclareOpenGLVertex();

			glTexCoord2f(1, 0);
			v1.DeclareOpenGLVertex();

			glTexCoord2f(0, 0);
			v2.DeclareOpenGLVertex();

			glTexCoord2f(0, 1);
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

	const float diffuseColor[4] = { 0.8f, 0.8f, 0.8f, 1 };
	const float ambientColor[4] = { 0.2f, 0.2f, 0.2f, 1 };
	const float lightDirection[4] = { 0.0f, 0.0f, 1.0f, 0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
	glEnable(GL_LIGHT0);
}

void SetCubeMaterial()
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
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

	angleX += 23 * time;
	if (angleX >= 360)
	{
		angleX -= 360;
	}

	angleY += 17 * time;
	if (angleY >= 360)
	{
		angleY -= 360;
	}

	angleZ += 19 * time;
	if (angleZ >= 360)
	{
		angleZ -= 360;
	}
}

void DrawBasis()
{
	glDisable(GL_LIGHTING);
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

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

	// ставим камеру в заданную точку
	SetCamera();

	SetLights();
	SetCubeMaterial();

	// вращаем объект
	RotateCube();

	// включаем тест глубины для корректного отображение невидимых поверхностей
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// рисуем куб
	DrawCube();

	DrawBasis();

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

bool LoadSheepTexture()
{
	if (g_sheepTexture != 0)
	{
		glDeleteTextures(1, &g_sheepTexture);
		g_sheepTexture = 0;
	}

	glGenTextures(1, &g_sheepTexture);
	glBindTexture(GL_TEXTURE_2D, g_sheepTexture);

	textureLoader.BuildMipmaps(false);
	textureLoader.SetMagFilter(g_magFilter);
	textureLoader.SetMinFilter(g_minFilter);
	textureLoader.LoadTexture2D(L"sheep0.bmp", 1, 0);
	textureLoader.LoadTexture2D(L"sheep1.bmp", 1, 1);
	textureLoader.LoadTexture2D(L"sheep2.bmp", 1, 2);
	textureLoader.LoadTexture2D(L"sheep3.bmp", 1, 3);
	textureLoader.LoadTexture2D(L"sheep4.bmp", 1, 4);
	textureLoader.LoadTexture2D(L"sheep5.bmp", 1, 5);
	textureLoader.LoadTexture2D(L"sheep6.bmp", 1, 6);
	textureLoader.LoadTexture2D(L"sheep7.bmp", 1, 7);
	textureLoader.LoadTexture2D(L"sheep8.bmp", 1, 8);

	return true;
}

bool OnCreate(HWND hWnd)
{
	// инициализируем OpenGL
	if (InitOpenGL(hWnd))
	{
		if (LoadSheepTexture())
		{
			return true;
		}
	}
	return false;
}

void OnDestroy()
{
	glDeleteTextures(1, &g_sheepTexture);
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

void OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_LINEAR:
		g_magFilter = GL_LINEAR;
		LoadSheepTexture();
		break;
	case ID_NEAREST:
		g_magFilter = GL_NEAREST;
		LoadSheepTexture();
		break;
	case ID_MIN_NEAREST:
		g_minFilter = GL_NEAREST;
		LoadSheepTexture();
		break;
	case ID_MIN_LINEAR:
		g_minFilter = GL_LINEAR;
		LoadSheepTexture();
		break;
	case ID_MIN_NEAREST_MIPMAP_NEAREST:
		g_minFilter = GL_NEAREST_MIPMAP_NEAREST;
		LoadSheepTexture();
		break;
	case ID_MIN_LINEAR_MIPMAP_NEAREST:
		g_minFilter = GL_LINEAR_MIPMAP_NEAREST;
		LoadSheepTexture();
		break;
	case ID_MIN_NEAREST_MIPMAP_LINEAR:
		g_minFilter = GL_NEAREST_MIPMAP_LINEAR;
		LoadSheepTexture();
		break;
	case ID_MIN_LINEAR_MIPMAP_LINEAR:
		g_minFilter = GL_LINEAR_MIPMAP_LINEAR;
		LoadSheepTexture();
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
		OnCommand(wParam, lParam);
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
		OnDestroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
