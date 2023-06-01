// gl.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "gl.h"
#include "../libgl/libgl.h"

// Global Variables:
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = TEXT("gl"); // window class name
const TCHAR WINDOW_TITLE[] = TEXT("Lighten cube"); // The title bar text

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
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_GL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCWSTR)NULL;
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

bool g_EnableLighting = true;
void SetLights()
{
	if (g_EnableLighting)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}
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

void OnCommand(WPARAM wParam)
{
	switch (wParam)
	{
	case ID_LIGHTING_ON:
		g_EnableLighting = true;
		break;
	case ID_LIGHTING_OFF:
		g_EnableLighting = false;
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
