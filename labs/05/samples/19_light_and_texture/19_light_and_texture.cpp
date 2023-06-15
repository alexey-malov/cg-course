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
const TCHAR WINDOW_TITLE[] = L"Texture + lighting "; // The title bar text

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
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

GLint g_textureEnvMode = 1;

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
		0, 0, 3, // позиция наблюдателя
		0, 0, 0, // точка, на которую смотрим
		0, 1, 0); // направление вектора "верх" в 3д-пространстве
}

struct LightDescriptor
{
	float ambient[4];
	float diffuse[4];
	float specular[4];

	float position[4];
	float spotDirection[3];

	float spotExponent;
	float spotCutOff;

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

const LightDescriptor lights[] = {
	// light 0
	{
		{ 0, 0, 0, 1 }, // ambient
		{ 3, 3, 3, 1 }, // diffuse
		{ 0, 0, 0, 1 }, // specular

		{ 1, 1, 1, 0 }, // position
		{ 0, 0, -1 }, // direction

		0, // spot exponent
		180, // spot cutoff

		1, // constant attenuation
		0, // linear attenuation
		0, // quadratic attenuation
	},

	// light 1
	{
		{ 1, 1, 1, 1 }, // ambient
		{ 2, 0, 2, 1 }, // diffuse
		{ 0, 3, 4, 1 }, // specular

		{ 1, 1, 1, 0 }, // position
		{ 0, 0, -1 }, // direction

		5, // spot exponent
		180, // spot cutoff

		1, // constant attenuation
		0, // linear attenuation
		0, // quadratic attenuation
	},

	// light 2
	{
		{ 1, 1, 1, 1 }, // ambient
		{ 2, 3, 4, 1 }, // diffuse
		{ 2, 2, 3, 1 }, // specular

		{ 1, 1, 3, 1 }, // position
		{ 0, 0, -1 }, // direction

		90, // spot exponent
		180, // spot cutoff

		1, // constant attenuation
		0.5f, // linear attenuation
		0.5f, // quadratic attenuation
	},

	// light 3
	{
		{ 1, 1, 1, 1 }, // ambient
		{ 2, 1, 2, 1 }, // diffuse
		{ 5, 3, 3, 1 }, // specular

		{ 0, 0, 3, 1 }, // position
		{ 0.7f, 0.6f, -3 }, // direction

		40, // spot exponent
		80, // spot cutoff

		1, // constant attenuation
		0.2f, // linear attenuation
		0.05f, // quadratic attenuation
	},

};

static const int numLights = sizeof(lights) / sizeof(LightDescriptor);

int g_currentLight = 0;
void SetLights()
{
	if ((g_currentLight < numLights) && (g_currentLight >= 0))
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		const LightDescriptor* pLight = &lights[g_currentLight];
		glLightfv(GL_LIGHT0, GL_AMBIENT, pLight->ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, pLight->diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, pLight->specular);

		glLightfv(GL_LIGHT0, GL_POSITION, pLight->position);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pLight->spotDirection);

		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, pLight->spotExponent);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, pLight->spotCutOff);

		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, pLight->constantAttenuation);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, pLight->linearAttenuation);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, pLight->quadraticAttenuation);
	}
}

struct MaterialDecriptor
{
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emission[4];

	float shininess;
};

const MaterialDecriptor materials[] = {
	// material 0
	{
		{ 0.6f, 0.6f, 0.6f, 1 }, // ambient
		{ 0.8f, 0.8f, 0.8f, 1 }, // diffuse
		{ 0, 0, 0, 1 }, // specular
		{ 0, 0, 0, 1 }, // emission
		0 },

	// material 1
	{
		{ 0.2f, 0.1f, 0.5f, 1 }, // ambient
		{ 0.9f, 0.9f, 0.9f, 1 }, // diffuse
		{ 0.5f, 1, 1, 1 }, // specular
		{ 0, 0, 0, 1 }, // emission
		10 },

	// material 2
	{
		{ 0.2f, 0.7f, 0.7f, 1 }, // ambient
		{ 0.8f, 0.7f, 0.4f, 1 }, // diffuse
		{ 1, 1, 1, 1 }, // specular
		{ 0, 0.1f, 0, 1 }, // emission
		50 },

	// material 3
	{
		{ 0.2f, 0.2f, 0.9f, 1 }, // ambient
		{ 1.5f, 1.7f, 1.3f, 1 }, // diffuse
		{ 0.8f, 1.1f, 1, 1 }, // specular
		{ 0, 0.0f, 0, 1 }, // emission
		100 },
};
static const int numMaterials = sizeof(materials) / sizeof(MaterialDecriptor);

int g_currentMaterial = 0;
void SetSphereMaterial()
{
	if ((g_currentMaterial >= 0) && (g_currentMaterial < numMaterials))
	{
		MaterialDecriptor const* pMaterial = &materials[g_currentMaterial];
		glMaterialfv(GL_FRONT, GL_AMBIENT, pMaterial->ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, pMaterial->diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, pMaterial->specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, pMaterial->emission);
		glMaterialf(GL_FRONT, GL_SHININESS, pMaterial->shininess);
	}
}

float angleX = 0;
float angleY = 0;
float angleZ = 0;

DWORD lastTick = GetTickCount();

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

Color4d g_bgColor = { 0, 0, 0, 1 };

GLenum g_polygonMode = GL_FILL;

GLuint g_sphereList = 0;

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(g_bgColor.r, g_bgColor.g, g_bgColor.b, g_bgColor.a);

	// ставим камеру в заданную точку
	SetCamera();

	SetLights();
	SetSphereMaterial();

	// вращаем объект
	RotateSphere();

	// включаем тест глубины для корректного отображение невидимых поверхностей
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, g_polygonMode);
	glPointSize(2);
	glLineWidth(2);

	if (g_sphereList == 0)
	{
		g_sphereList = glGenLists(1);
		glNewList(g_sphereList, GL_COMPILE);
		DrawSphere();
		glEndList();
	}

	if (g_textureEnvMode != 0)
	{
		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, g_textureEnvMode);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	// рисуем сферу
	glCallList(g_sphereList);

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
		if ((g_earthTexture = textureLoader.LoadTexture2D(L"earth.bmp")) != 0)
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
	switch (wParam)
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
	case ID_LIGHT0:
		g_currentLight = 0;
		break;
	case ID_LIGHT1:
		g_currentLight = 1;
		break;
	case ID_LIGHT2:
		g_currentLight = 2;
		break;
	case ID_LIGHT3:
		g_currentLight = 3;
		break;
	case ID_MATERIAL0:
		g_currentMaterial = 0;
		break;
	case ID_MATERIAL1:
		g_currentMaterial = 1;
		break;
	case ID_MATERIAL2:
		g_currentMaterial = 2;
		break;
	case ID_MATERIAL3:
		g_currentMaterial = 3;
		break;
	case ID_FILL_MODE:
		g_polygonMode = GL_FILL;
		break;
	case ID_WIREFRAME:
		g_polygonMode = GL_LINE;
		break;
	case ID_POINTS_MODE:
		g_polygonMode = GL_POINT;
		break;
	case ID_NO_TEXTURE:
		g_textureEnvMode = 0;
		break;
	case ID_GL_DECAL:
		g_textureEnvMode = GL_DECAL;
		break;
	case ID_GL_REPLACE:
		g_textureEnvMode = GL_REPLACE;
		break;
	case ID_GL_ADD:
		g_textureEnvMode = GL_ADD;
		break;
	case ID_GL_MODULATE:
		g_textureEnvMode = GL_MODULATE;
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
