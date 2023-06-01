// gl.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "gl.h"
#include "../libgl/libgl.h"

// Global Variables:
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = TEXT("gl"); // window class name
const TCHAR WINDOW_TITLE[] = TEXT("Lighten sphere"); // The title bar text

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
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

	return (int) msg.wParam;
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
	wcex.lpszMenuName = (LPCWSTR)IDR_MAIN_MENU;
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

struct Vector3d
{
	float x, y, z; // координаты 3d вектора

	Vector3d()
	{
	}

	Vector3d(float newX, float newY, float newZ)
		: x(newX)
		, y(newY)
		, z(newZ)
	{
	}

	Vector3d(const Vector3d& v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{
	}

	Vector3d& operator=(const Vector3d& v)
	{
		if (this != &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}

	void DeclareOpenGLVertex() const
	{
		glVertex3f(x, y, z);
	}

	void DeclareOpenGLNormal() const
	{
		glNormal3f(x, y, z);
	}

	void Set(float newX, float newY, float newZ)
	{
		x = newX;
		y = newY;
		z = newZ;
	}

	void Clear()
	{
		x = y = z = 0;
	}

	Vector3d& operator-=(Vector3d const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3d& operator+=(Vector3d const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3d operator+(Vector3d const& v) const
	{
		return Vector3d(x + v.x, y + v.y, z + v.z);
	}

	Vector3d operator-(Vector3d const& v) const
	{
		return Vector3d(x - v.x, y - v.y, z - v.z);
	}

	void Normalize()
	{
		float one_div_len = 1.0f / sqrtf(x * x + y * y + z * z);
		x *= one_div_len;
		y *= one_div_len;
		z *= one_div_len;
	}

	float DotProduct(Vector3d const& v) const
	{
		return (x * v.x) + (y * v.y) + (z * v.z);
	}

	void CrossProduct(Vector3d const& vec1, Vector3d const& vec2)
	{
		const float x1 = vec1.x;
		const float y1 = vec1.y;
		const float z1 = vec1.z;
		const float x2 = vec2.x;
		const float y2 = vec2.y;
		const float z2 = vec2.z;
		/*
		|i  j  k |   |y1*z2 - y2*z1|
		|x1 y1 z1| = |z1*x2 - z2*x1|
		|x2 y2 z2|   |x1*y2 - x2*y1|

		*/

		x = y1 * z2 - y2 * z1;
		y = z1 * x2 - z2 * x1;
		z = x1 * y2 - x2 * y1;
	}
};

struct Color4d
{
	float r, g, b, a;
	void SetOpenGLColor() const
	{
		glColor4f(r, g, b, a);
	}
};

struct QuadFace
{
	int v0, v1, v2, v3; // номера вершин
	Color4d faceColor;
};

// вычисляем нормаль к треугольнику
void CalculateTriangleNormal(Vector3d const& a, Vector3d const& b, Vector3d const& c, Vector3d* normal)
{
	/*
		  A
		 / \
		/   \
	   /     \
	  /       \
	 /         \
	B-----------C
	Нормаль к треугольнику ABC - векторное произведение BC(x1, y1, z1) x BA(x2, y2, z2):
	|i  j  k |   |y1*z2 - y2*z1|
	|x1 y1 z1| = |z1*x2 - z2*x1|
	|x2 y2 z2|   |x1*y2 - x2*y1|
	где:
		x1 = (c-b).x
		y1 = (c-b).y
		z1 = (c-b).z
		x2 = (a-b).x
		y2 = (a-b).y
		z2 = (a-b).z
	*/
	// вычисляем координаты вектора BC
	Vector3d bc = c - b;
	// вычисляем координаты вектора BA
	Vector3d ba = a - b;
	// вычисляем их векторное произведение - нормаль к треугольнику
	normal->CrossProduct(bc, ba);
	// нормируем результат, т.к. вектор нормали должен быть единичной длины
	normal->Normalize();
}

void CalculateSpherePointColor(float alpha, float beta, Color4d* color)
{
	color->r = 0.2f * (cosf(5 * alpha) + cosf(6 * beta)) + 0.3f;
	color->g = 0.2f * (sinf(6 * alpha) + cosf(3 * beta)) + 0.3f;
	color->b = 0.2f * (sinf(5 * beta) + cosf(5 * alpha)) + 0.3f;
	color->a = 1;
}

void CalculateSpherePoint(float alpha, float beta, Vector3d* spherePoint)
{
	float rad = cosf(beta);
	spherePoint->Set(rad * cosf(alpha), rad * sinf(alpha), sinf(beta));
}

void DrawSphere()
{
	// количество параллелей
	static const int SPHERE_PARALLELS = 60;

	// количество меридианов
	static const int SPHERE_MERIDIANS = 90;

	float STEP_ALPHA = (float)(2 * M_PI / SPHERE_MERIDIANS);
	const float STEP_BETA = (float)(M_PI / SPHERE_PARALLELS);

	// координаты южного полюса
	Vector3d southPole(0, 0, -1);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLE_FAN);
	{
		Vector3d pnt;
		Color4d color;

		float alpha = 0;
		float beta = -M_PI / 2 + STEP_BETA;
		CalculateSpherePointColor(alpha, -M_PI / 2, &color);
		color.SetOpenGLColor();
		southPole.DeclareOpenGLNormal();
		southPole.DeclareOpenGLVertex();
		for (int i = 0; i < SPHERE_MERIDIANS; ++i, alpha -= STEP_ALPHA)
		{
			CalculateSpherePoint(alpha, beta, &pnt);
			CalculateSpherePointColor(alpha, beta, &color);
			color.SetOpenGLColor();
			pnt.DeclareOpenGLNormal();
			pnt.DeclareOpenGLVertex();
		}
		CalculateSpherePoint(0, beta, &pnt);
		CalculateSpherePointColor(alpha, beta, &color);
		color.SetOpenGLColor();
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
			Color4d color1;
			Color4d color2;
			Vector3d pnt2;
			float alpha = 0;
			for (int meridian = 0; meridian < SPHERE_MERIDIANS; ++meridian, alpha -= STEP_ALPHA)
			{
				CalculateSpherePoint(alpha, beta, &pnt1);
				CalculateSpherePointColor(alpha, beta, &color1);
				CalculateSpherePoint(alpha, beta1, &pnt2);
				CalculateSpherePointColor(alpha, beta1, &color2);

				color1.SetOpenGLColor();
				pnt1.DeclareOpenGLNormal();
				pnt1.DeclareOpenGLVertex();

				color2.SetOpenGLColor();
				pnt2.DeclareOpenGLNormal();
				pnt2.DeclareOpenGLVertex();
			}
			CalculateSpherePoint(0, beta, &pnt1);
			CalculateSpherePointColor(0, beta, &color1);
			CalculateSpherePoint(0, beta1, &pnt2);
			CalculateSpherePointColor(0, beta, &color2);
			color1.SetOpenGLColor();
			pnt1.DeclareOpenGLNormal();
			pnt1.DeclareOpenGLVertex();

			color2.SetOpenGLColor();
			pnt2.DeclareOpenGLNormal();
			pnt2.DeclareOpenGLVertex();
		}
		glEnd();
	}

	// координаты южного полюса
	Vector3d northPole(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	{
		Color4d color;
		CalculateSpherePointColor(0, M_PI / 2, &color);
		color.SetOpenGLColor();
		northPole.DeclareOpenGLNormal();
		northPole.DeclareOpenGLVertex();
		float alpha = 0;
		Vector3d pnt;
		for (int i = 0; i < SPHERE_MERIDIANS; ++i, alpha += STEP_ALPHA)
		{
			CalculateSpherePoint(alpha, beta, &pnt);
			CalculateSpherePointColor(alpha, beta, &color);
			color.SetOpenGLColor();
			pnt.DeclareOpenGLNormal();
			pnt.DeclareOpenGLVertex();
		}
		CalculateSpherePoint(0, beta, &pnt);
		CalculateSpherePointColor(0, beta, &color);
		color.SetOpenGLColor();
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
		0, 0, 2.2, // позиция наблюдателя
		0, 0, 0, // точка, на которую смотрим
		0, 1, 0); // направление вектора "верх" в 3д-пространстве
}

bool g_enableLighting = true;

void SetLights()
{
	if (g_enableLighting)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	const float ambientColor[4] = { 0.2f, 0.2f, 0.2f, 1 };
	const float diffuseColor[4] = { 1.0f, 1.0f, 1.0f, 1 };
	const float specularColor[4] = { 2.9f, 2.9f, 2.9f, 1 };
	const float lightPosition[4] = { 2.7f, 2.7f, 1.7f, 0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);
	glEnable(GL_LIGHT0);
}

void SetSphereMaterial()
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	float specular[4] = { 0.5f, 0.5f, 0.5f, 1 };
	float ambient[4] = { 0.2f, 0.2f, 0.2f, 1 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);
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

	angleX += 17 * time;
	if (angleX >= 360)
	{
		angleX -= 360;
	}

	angleY += 23 * time;
	if (angleY >= 360)
	{
		angleY -= 360;
	}

	angleZ += 29 * time;
	if (angleZ >= 360)
	{
		angleZ -= 360;
	}
}

GLuint g_sphereList = 0;

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

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

	if (g_sphereList == 0)
	{
		g_sphereList = glGenLists(1);
		glNewList(g_sphereList, GL_COMPILE);
		// рисуем сферу в список отображения
		DrawSphere();
		glEndList();
	}

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
	return InitOpenGL(hWnd);
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

void OnDestroy()
{
	if (g_sphereList != 0)
	{
		glDeleteLists(g_sphereList, 1);
		g_sphereList = 0;
	}
	DestroyOpenGL();
}

void OnCommand(WPARAM commandId)
{
	switch (commandId)
	{
	case ID_LIGHTING_OFF:
		g_enableLighting = false;
		break;
	case ID_LIGHTING_ON:
		g_enableLighting = true;
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
		OnDestroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
