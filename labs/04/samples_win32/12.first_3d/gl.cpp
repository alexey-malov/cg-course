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

void DrawTetrahedron()
{
	static const float A[3] = { -0, -0.3f, 0.5f };
	static const float B[3] = { 0.4f, -0.3f, -0.3f };
	static const float C[3] = { -0.4f, -0.3f, -0.3f };
	static const float D[3] = { 0, 0.5f, 0 };

	glBegin(GL_TRIANGLES);
	{
		// рисуем треугольник ABC
		glColor3f(1, 0, 0);
		glVertex3fv(A);
		glVertex3fv(B);
		glVertex3fv(C);

		// рисуем треугольник ACD
		glColor3f(0, 1, 0);
		glVertex3fv(A);
		glVertex3fv(C);
		glVertex3fv(D);

		// рисуем треугольник ABD
		glColor3f(0, 0, 1);
		glVertex3fv(A);
		glVertex3fv(B);
		glVertex3fv(D);

		// рисуем треугольник DBC
		glColor3f(1, 0, 1);
		glVertex3fv(D);
		glVertex3fv(B);
		glVertex3fv(C);
	}
	glEnd();
}

void SetProjection(double aspect)
{
	// устанавливаем матрицу проецирования (aspect - отношение ширины экрана к его высоте)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, aspect, 0.2, 3);
	glMatrixMode(GL_MODELVIEW);
}

void SetCamera()
{
	glLoadIdentity();
	gluLookAt(
		0, 1, 1.5, // позиция наблюдателя
		0, 0, 0, // точка, на которую смотрим
		0, 1, 0); // направление вектора "верх" в 3д-пространстве
}

float angleX = 0;
float angleY = 0;
float angleZ = 0;

ULONGLONG lastTick = GetTickCount64();

void RotateTetrahedron()
{

	glTranslatef(-0.5, 0, 0);

	glRotatef(angleX, 1, 0, 0);
	glRotatef(angleY, 0, 1, 0);
	glRotatef(angleZ, 0, 0, 1);

	// подсчитываем время, прошедшее с момента последнего нарисованного кадра
	ULONGLONG currentTick = GetTickCount64();
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

	// вращаем объект
	RotateTetrahedron();

	// включаем тест глубины для корректного отображение невидимых поверхностей
	glEnable(GL_DEPTH_TEST);

	// рисуем тетраэдр
	DrawTetrahedron();

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
