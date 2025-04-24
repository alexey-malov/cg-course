// gl.cpp : Defines the entry point for the application.
//

#include "resource.h"

#include "../libgl/GdiPlusInitializer.h"
#include "../libgl/TextureLoader.h"

#include "../libgl/Graphics.h"
#include "../libgl/Utils.h"
#include "../libgl/stdafx.h"

#define MAX_LOADSTRING 100
#define M_PI 3.1415927f

// Global Variables:

CGdiPlusInitializer gdi;
CTextureLoader textureLoader;
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = L"gl"; // window class name
const TCHAR WINDOW_TITLE[] = L"Color blending"; // The title bar text

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
HWND InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

GLuint g_earthTexture = 0;

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
	HWND hMainWnd = InitInstance(hInstance, nCmdShow);
	if (!hMainWnd)
	{
		return FALSE;
	}

	HACCEL accel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));
	if (!accel)
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
				if (!TranslateAccelerator(hMainWnd, accel, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
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
void CalculateSpherePointColor(float alpha, float beta, Color4d* color)
{
	color->r = 0.5f * cosf(5 * alpha) + 0.5f;
	color->g = 0.25f * (sinf(2 * alpha) + cosf(3 * beta)) + 0.5f;
	color->b = 0.25f * (sinf(5 * beta) + cosf(4 * alpha)) + 0.5f;
	color->a = 1;
}

void CalculateSpherePoint(float alpha, float beta, Vector3d* spherePoint)
{
	float rad = cosf(beta);
	spherePoint->Set(rad * cosf(alpha), rad * sinf(alpha), sinf(beta));
}

void CalculateSphereTextureCoordinate(float alpha, float beta, Vector2d* texCoord)
{
	texCoord->x = alpha / (2 * M_PI);
	texCoord->y = beta / (M_PI) + 0.5f; // 0.5f * sinf(beta) + 0.5f;
}

void DrawSphere()
{

	// количество параллелей
	static const int SPHERE_PARALLELS = 40;

	// количество меридианов
	static const int SPHERE_MERIDIANS = 60;

	float STEP_ALPHA = (float)(2 * M_PI / SPHERE_MERIDIANS);
	const float STEP_BETA = (float)(M_PI / SPHERE_PARALLELS);

	// координаты южного полюса
	Vector3d southPole(0, 0, -1);
	Vector2d southPoleTex = { 0.5f, 0 };
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLE_FAN);
	{
		Vector3d pnt;
		Vector2d tex;

		float alpha = 0;
		float beta = -M_PI / 2 + STEP_BETA;
		southPoleTex.DeclareTextureCoord();
		southPole.DeclareOpenGLNormal();
		southPole.DeclareOpenGLVertex();
		for (int i = 0; i < SPHERE_MERIDIANS; ++i, alpha -= STEP_ALPHA)
		{
			CalculateSphereTextureCoordinate(-alpha, beta, &tex);
			CalculateSpherePoint(alpha, beta, &pnt);
			pnt.DeclareOpenGLNormal();
			tex.DeclareTextureCoord();
			pnt.DeclareOpenGLVertex();
		}
		CalculateSphereTextureCoordinate(alpha, beta, &tex);
		CalculateSpherePoint(0, beta, &pnt);
		tex.DeclareTextureCoord();
		pnt.DeclareOpenGLNormal();
		pnt.DeclareOpenGLVertex();
	}
	glEnd();

	float beta = -M_PI / 2 + STEP_BETA;
	float beta1 = -M_PI / 2 + STEP_BETA * 2;
	for (int par = 1; par < (SPHERE_PARALLELS - 1); ++par, beta = beta1, beta1 += STEP_BETA)
	{
		glBegin(GL_QUAD_STRIP);
		{
			Vector3d pnt1;
			Vector3d pnt2;
			Vector2d tex1;
			Vector2d tex2;

			float alpha = 0;
			for (int meridian = 0; meridian < SPHERE_MERIDIANS; ++meridian, alpha -= STEP_ALPHA)
			{
				CalculateSpherePoint(alpha, beta, &pnt1);
				CalculateSpherePoint(alpha, beta1, &pnt2);
				CalculateSphereTextureCoordinate(-alpha, beta, &tex1);
				CalculateSphereTextureCoordinate(-alpha, beta1, &tex2);

				tex1.DeclareTextureCoord();
				pnt1.DeclareOpenGLNormal();
				pnt1.DeclareOpenGLVertex();

				tex2.DeclareTextureCoord();
				pnt2.DeclareOpenGLNormal();
				pnt2.DeclareOpenGLVertex();
			}
			CalculateSpherePoint(0, beta, &pnt1);
			CalculateSpherePoint(0, beta1, &pnt2);
			CalculateSphereTextureCoordinate(-alpha, beta, &tex1);
			CalculateSphereTextureCoordinate(-alpha, beta1, &tex2);

			tex1.DeclareTextureCoord();
			pnt1.DeclareOpenGLNormal();
			pnt1.DeclareOpenGLVertex();

			tex2.DeclareTextureCoord();
			pnt2.DeclareOpenGLNormal();
			pnt2.DeclareOpenGLVertex();
		}
		glEnd();
	}

	// координаты южного полюса
	Vector3d northPole(0, 0, 1);
	Vector2d northPoleTex = { 0.5f, 1 };
	glBegin(GL_TRIANGLE_FAN);
	{
		northPole.DeclareOpenGLNormal();
		northPoleTex.DeclareTextureCoord();
		northPole.DeclareOpenGLVertex();
		float alpha = 0;
		Vector3d pnt;
		Vector2d tex;
		for (int i = 0; i < SPHERE_MERIDIANS; ++i, alpha += STEP_ALPHA)
		{
			CalculateSphereTextureCoordinate(alpha, beta, &tex);
			CalculateSpherePoint(alpha, beta, &pnt);
			tex.DeclareTextureCoord();
			pnt.DeclareOpenGLNormal();
			pnt.DeclareOpenGLVertex();
		}
		CalculateSpherePoint(0, beta, &pnt);
		CalculateSpherePoint(alpha, beta, &pnt);
		tex.DeclareTextureCoord();
		pnt.DeclareOpenGLNormal();
		pnt.DeclareOpenGLVertex();
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
		0, 0, 6, // позиция наблюдателя
		0, 0, 0, // точка, на которую смотрим
		0, 1, 0); // направление вектора "верх" в 3д-пространстве
}

float angleX = 0;
float angleY = 0;
float angleZ = 0;

DWORD lastTick = GetTickCount();

bool g_animate = true;

void RotateSphere()
{
	//	glTranslatef(-0.5, 0, 0);

	glRotatef(angleX, 1, 0, 0);
	glRotatef(angleY, 0, 1, 0);
	glRotatef(angleZ, 0, 0, 1);

	// подсчитываем время, прошедшее с момента последнего нарисованного кадра
	DWORD currentTick = GetTickCount();
	float time = (currentTick - lastTick) * 0.001f;
	lastTick = currentTick;

	if (!g_animate)
	{
		time = 0;
	}

	angleX += 34 * time;
	if (angleX >= 360)
	{
		angleX -= 360;
	}

	angleY += 23 * time;
	if (angleY >= 360)
	{
		angleY -= 360;
	}

	angleZ += 37 * time;
	if (angleZ >= 360)
	{
		angleZ -= 360;
	}
}

Color4d g_bgColor = { 0, 0, 0, 1 };

GLuint g_sphereList = 0;

GLenum g_srcBlendFactor = GL_SRC_ALPHA;
GLenum g_dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;

bool g_drawFrontFaces = true;
bool g_drawBackFaces = true;

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(g_bgColor.r, g_bgColor.g, g_bgColor.b, g_bgColor.a);

	// ставим камеру в заданную точку
	SetCamera();

	// SetLights();
	// SetSphereMaterial();

	if (g_sphereList == 0)
	{

		g_sphereList = glGenLists(1);
		glNewList(g_sphereList, GL_COMPILE);
		DrawSphere();
		glEndList();
	}

	// устанавливаем параметры текстуры
	{
		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// включаем тест глубины для корректного отображение невидимых поверхностей

	{
		glColor4f(1, 1, 1, 1);

		// рисуем непрозрачную сферу
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// разрешаем запись в z-buffer
		glDepthMask(GL_TRUE);
		glCullFace(GL_BACK);

		// glPushMatrix();
		RotateSphere();
		glCallList(g_sphereList);

		// glPopMatrix();
		glTranslatef(3, 0, 0);
		RotateSphere();

		glEnable(GL_BLEND);
		glBlendFunc(g_srcBlendFactor, g_dstBlendFactor);

		// glColor4f(1, 1, 1, 0.7f);

		// запрещаем запись в z-buffer
		glDepthMask(GL_FALSE);

		if (g_drawBackFaces)
		{
			glCullFace(GL_FRONT);
			glCallList(g_sphereList);
		}
		if (g_drawFrontFaces)
		{
			glCullFace(GL_BACK);
			glCallList(g_sphereList);
		}
		glDisable(GL_BLEND);

		glDepthMask(GL_TRUE);
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
	if (InitOpenGL(hWnd))
	{
		if ((g_earthTexture = textureLoader.LoadTexture2D(L"earth.png")) != 0)
		{
			return true;
		}
	}
	return false;
}

void OnDestroy(HWND hWnd)
{
	if (g_earthTexture)
	{
		glDeleteTextures(1, &g_earthTexture);
	}
	if (g_sphereList)
	{
		glDeleteLists(g_sphereList, 1);
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
	// Младшие 16 бит хранят id команды. Старшие 16 бит - id уведомления или 1 (акселератор)
	switch (LOWORD(wParam))
	{
	case ID_WHITE_BACKGROUND:
		g_bgColor.r = 1;
		g_bgColor.g = 1;
		g_bgColor.b = 1;
		g_bgColor.a = 1;
		break;
	case ID_BLACK_BACKGROUND:
		g_bgColor.r = 0;
		g_bgColor.g = 0;
		g_bgColor.b = 0;
		g_bgColor.a = 1;
		break;
	case ID_SRCBLEND_GLZERO:
		g_srcBlendFactor = GL_ZERO;
		break;
	case ID_SRCBLEND_GLONE:
		g_srcBlendFactor = GL_ONE;
		break;
	case ID_SRCBLEND_GLDSTCOLOR:
		g_srcBlendFactor = GL_DST_COLOR; // Rd/kR,Gd/kG,Bd/kB,Ad/kA
		break;
	case ID_SRCBLEND_GLONEMINUSDSTCOLOR:
		g_srcBlendFactor = GL_ONE_MINUS_DST_COLOR;
		break;
	case ID_SRCBLEND_GLSRCALPHA:
		g_srcBlendFactor = GL_SRC_ALPHA;
		break;
	case ID_SRCBLEND_GLONEMINUSSRCALPHA:
		g_srcBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case ID_SRCBLEND_GLDSTALPHA:
		g_srcBlendFactor = GL_DST_ALPHA;
		break;
	case ID_SRCBLEND_GLONEMINUSDSTALPHA:
		g_srcBlendFactor = GL_ONE_MINUS_DST_ALPHA;
		break;
	case ID_SRCBLEND_GLSRCALPHASATURATE:
		g_srcBlendFactor = GL_SRC_ALPHA_SATURATE;
		break;
		// dst blend factors
	case ID_DSTBLEND_GLZERO:
		g_dstBlendFactor = GL_ZERO;
		break;
	case ID_DSTBLEND_GLONE:
		g_dstBlendFactor = GL_ONE;
		break;
	case ID_DSTBLEND_GLSRCCOLOR:
		g_dstBlendFactor = GL_SRC_COLOR;
		break;
	case ID_DSTBLEND_GLONEMINUSSRCCOLOR:
		g_dstBlendFactor = GL_ONE_MINUS_SRC_COLOR;
		break;
	case ID_DSTBLEND_GLSRCALPHA:
		g_dstBlendFactor = GL_SRC_ALPHA;
		break;
	case ID_DSTBLEND_GLONEMINUSSRCALPHA:
		g_dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case ID_DSTBLEND_GLDSTALPHA:
		g_dstBlendFactor = GL_DST_ALPHA;
		break;
	case ID_DSTBLEND_GLONEMINUSDSTALPHA:
		g_dstBlendFactor = GL_ONE_MINUS_DST_ALPHA;
		break;
		// transparency
	case ID_TRANSPARENCY_DRAWFRONTANDBACKFACES:
		g_drawFrontFaces = g_drawBackFaces = true;
		break;
	case ID_TRANSPARENCY_DRAWONLYBACKFACES:
		g_drawFrontFaces = false;
		g_drawBackFaces = true;
		break;
	case ID_TRANSPARENCY_DRAWONLYFRONTFACES:
		g_drawBackFaces = false;
		g_drawFrontFaces = true;
		break;
	case ID_ANIMATION_ON:
		g_animate = true;
		break;
	case ID_ANIMATION_OFF:
		g_animate = false;
		break;
	case ID_TOGGLE_ANIMATION:
		g_animate = !g_animate;
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
