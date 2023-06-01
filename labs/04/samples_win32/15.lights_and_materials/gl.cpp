// gl.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "gl.h"
#include "../libgl/libgl.h"

// Global Variables:
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = TEXT("gl"); // window class name
const TCHAR WINDOW_TITLE[] = TEXT("First 3D object"); // The title bar text

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
	normal->CrossProduct(ba, bc);
	// нормируем результат, т.к. вектор нормали должен быть единичной длины
	normal->Normalize();
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

		float alpha = 0;
		float beta = -M_PI / 2 + STEP_BETA;
		southPole.DeclareOpenGLNormal();
		southPole.DeclareOpenGLVertex();
		for (int i = 0; i < SPHERE_MERIDIANS; ++i, alpha -= STEP_ALPHA)
		{
			CalculateSpherePoint(alpha, beta, &pnt);
			pnt.DeclareOpenGLNormal();
			pnt.DeclareOpenGLVertex();
		}
		CalculateSpherePoint(0, beta, &pnt);
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
			float alpha = 0;
			for (int meridian = 0; meridian < SPHERE_MERIDIANS; ++meridian, alpha -= STEP_ALPHA)
			{
				CalculateSpherePoint(alpha, beta, &pnt1);
				CalculateSpherePoint(alpha, beta1, &pnt2);

				pnt1.DeclareOpenGLNormal();
				pnt1.DeclareOpenGLVertex();

				pnt2.DeclareOpenGLNormal();
				pnt2.DeclareOpenGLVertex();
			}
			CalculateSpherePoint(0, beta, &pnt1);
			CalculateSpherePoint(0, beta1, &pnt2);
			pnt1.DeclareOpenGLNormal();
			pnt1.DeclareOpenGLVertex();

			pnt2.DeclareOpenGLNormal();
			pnt2.DeclareOpenGLVertex();
		}
		glEnd();
	}

	// координаты южного полюса
	Vector3d northPole(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	{
		northPole.DeclareOpenGLNormal();
		northPole.DeclareOpenGLVertex();
		float alpha = 0;
		Vector3d pnt;
		for (int i = 0; i < SPHERE_MERIDIANS; ++i, alpha += STEP_ALPHA)
		{
			CalculateSpherePoint(alpha, beta, &pnt);
			pnt.DeclareOpenGLNormal();
			pnt.DeclareOpenGLVertex();
		}
		CalculateSpherePoint(0, beta, &pnt);
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
		{ 1, 1, 1, 1 }, // diffuse
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
		{ 1, 0, 1, 1 }, // diffuse
		{ 0, 1, 1, 1 }, // specular

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
		{ 8, 8, 8, 1 }, // diffuse
		{ 8, 8, 8, 1 }, // specular

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
		{ 0.4f, 0.4f, 0.4f, 1 }, // ambient
		{ 0.6f, 0.6f, 0.6f, 1 }, // diffuse
		{ 0, 0, 0, 1 }, // specular
		{ 0, 0, 0, 1 }, // emission
		0 // shininess
	},

	// material 1
	{
		{ 0.2f, 0.1f, 0.5f, 1 }, // ambientf
		{ 0.6f, 0.8f, 0.9f, 1 }, // diffuse
		{ 0.5f, 1, 1, 1 }, // specular
		{ 0, 0, 0, 1 }, // emission
		10 // shininess
	},

	// material 2
	{
		{ 0.2f, 0.7f, 0.7f, 1 }, // ambient
		{ 0.2f, 0.7f, 0.4f, 1 }, // diffuse
		{ 1, 1, 1, 1 }, // specular
		{ 0, 0.1f, 0, 1 }, // emission
		50 // shininess
	},

	// material 3
	{
		{ 0.2f, 0.2f, 0.9f, 1 }, // ambient
		{ 0.5f, 0.7f, 0.3f, 1 }, // diffuse
		{ 0.8f, 1.1f, 1, 1 }, // specular
		{ 0, 0.0f, 0, 1 }, // emission
		100 // shininess
	},
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

	angleX += 23 * time;
	if (angleX >= 360)
	{
		angleX -= 360;
	}

	angleY += 31 * time;
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
		// рисуем сферу в список отображения
		DrawSphere();
		glEndList();
	}

	// вызываем то, что нарисовали
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
	default:
		break;
	}
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
		OnDestroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
