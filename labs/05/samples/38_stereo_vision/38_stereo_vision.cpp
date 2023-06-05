// gl.cpp : Defines the entry point for the application.
//
#include "resource.h"

#include "../libgl/GdiPlusInitializer.h"
#include "../libgl/TextureLoader.h"

#include "../libgl/stdafx.h"
#include "../libgl/Camera.h"
#include "../libgl/Graphics.h"
#include "../libgl/MapTile.h"
#include "../libgl/Utils.h"

#define MAX_LOADSTRING 100
#define M_PI 3.1415927f

// Global Variables:
CGdiPlusInitializer gdi;
CTextureLoader textureLoader;
HINSTANCE hInst; // current instance
const TCHAR WINDOW_CLASS_NAME[] = L"gl"; // window class name
const TCHAR WINDOW_TITLE[] = L"Terrain - part1"; // The title bar text

enum GameAction
{
	HK_SPEED_UP = 0,
	HK_SPEED_DOWN,
	HK_ROLL_LEFT,
	HK_ROLL_RIGHT,
	HK_YAW_LEFT,
	HK_YAW_RIGHT,
	HK_PITCH_UP,
	HK_PITCH_DOWN,
	HK_STRAFE_LEFT,
	HK_STRAFE_RIGHT,
	HK_STOP,

	HK_NUM_KEYS,
};

bool g_controlState[HK_NUM_KEYS] = {
	false, // HK_SPEED_UP
	false, // HK_SPEED_DOWN
	false, // HK_ROLL_LEFT
	false, // HK_ROLL_RIGHT
	false, // HK_YAW_LEFT
	false, // HK_YAW_RIGHT
	false, // HK_PITCH_LEFT
	false, // HK_PITCH_RIGHT
	false, // HK_STRAFE_LEFT
	false, // HK_STRAFE_RIGHT
	false, // HK_STOP
};

struct KeyMap
{
	WPARAM keyCode;
	GameAction gameAction;
};

const KeyMap g_keyMap[] = {
	{ 'W', HK_SPEED_UP },
	{ 'S', HK_SPEED_DOWN },
	{ 'A', HK_YAW_LEFT },
	{ 'D', HK_YAW_RIGHT },
	{ 'Q', HK_STRAFE_LEFT },
	{ 'E', HK_STRAFE_RIGHT },
	{ VK_LEFT, HK_ROLL_LEFT },
	{ VK_RIGHT, HK_ROLL_RIGHT },
	{ VK_UP, HK_PITCH_DOWN },
	{ VK_DOWN, HK_PITCH_UP },
	{ VK_SPACE, HK_STOP },
};

// количество кнопок
const int NUM_KEYS = sizeof(g_keyMap) / sizeof(*g_keyMap);

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

MapTile g_tile;

GLuint g_skyTexture;
GLuint g_skyList;
GLint g_fogMode = 0;

bool g_enableStereo = false;

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
			Sleep(7);
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
	gluPerspective(50, aspect, 1, 500);
	glMatrixMode(GL_MODELVIEW);
}

DWORD lastTick = GetTickCount();
float g_delta = 0;

void ShiftTime()
{
	// подсчитываем время, прошедшее с момента последнего нарисованного кадра
	DWORD currentTick = GetTickCount();
	g_delta = (currentTick - lastTick) * 0.001f;
	lastTick = currentTick;
}

Vector3d cameraPosition;
Vector3d cameraDirection;

float ToRadians(float angle)
{
	return angle * M_PI / 180.0f;
}

const double MAX_CAMERA_SPEED = 15;
const double CAMERA_ACCELERATION = 15;
const double CAMERA_DECELERATION = 0.3;

const double MAX_CAMERA_YAW = 50;
const double YAW_ACCELERATION = 50;
const double YAW_DECELERATION = 0.99;

const double MAX_CAMERA_PITCH = 50;
const double PITCH_ACCELERATION = 50;
const double PITCH_DECELERATION = 0.99;

const double MAX_CAMERA_ROLL = 50;
const double ROLL_ACCELERATION = 50;
const double ROLL_DECELERATION = 0.99;

CCamera g_camera;
doubleVector3 g_cameraSpeed(0, 0, 0);
double g_yawSpeed = 0;
double g_pitchSpeed = 0;
double g_rollSpeed = 0;

void CalculateCameraMovement()
{
	doubleVector3 cameraAcc(0, 0, 0);

	if (g_controlState[HK_SPEED_UP])
	{
		cameraAcc.z = CAMERA_ACCELERATION;
	}
	if (g_controlState[HK_SPEED_DOWN])
	{
		cameraAcc.z = -CAMERA_ACCELERATION;
	}

	if (g_controlState[HK_STRAFE_LEFT])
	{
		cameraAcc.x = CAMERA_ACCELERATION;
	}
	if (g_controlState[HK_STRAFE_RIGHT])
	{
		cameraAcc.x = -CAMERA_ACCELERATION;
	}

	{
		g_cameraSpeed -= g_cameraSpeed * CAMERA_DECELERATION * g_delta;
		g_cameraSpeed += cameraAcc * g_delta;
		double absSpeed = g_cameraSpeed.length();
		if (absSpeed > MAX_CAMERA_SPEED)
		{
			g_cameraSpeed *= MAX_CAMERA_SPEED / absSpeed;
		}
	}
}

void CalculateCameraYaw()
{
	double yawAcc = 0;
	if (g_controlState[HK_YAW_LEFT])
	{
		yawAcc = YAW_ACCELERATION;
	}
	if (g_controlState[HK_YAW_RIGHT])
	{
		yawAcc = -YAW_ACCELERATION;
	}

	g_yawSpeed -= g_yawSpeed * YAW_DECELERATION * g_delta;
	g_yawSpeed += yawAcc * g_delta;
	if (g_yawSpeed > MAX_CAMERA_YAW)
	{
		g_yawSpeed = MAX_CAMERA_YAW;
	}
	if (g_yawSpeed < -MAX_CAMERA_YAW)
	{
		g_yawSpeed = -MAX_CAMERA_YAW;
	}
}

void CalculateCameraPitch()
{
	double pitchAcc = 0;
	if (g_controlState[HK_PITCH_UP])
	{
		pitchAcc = PITCH_ACCELERATION;
	}
	if (g_controlState[HK_PITCH_DOWN])
	{
		pitchAcc = -PITCH_ACCELERATION;
	}

	g_pitchSpeed -= g_pitchSpeed * PITCH_DECELERATION * g_delta;
	g_pitchSpeed += pitchAcc * g_delta;
	if (g_pitchSpeed > MAX_CAMERA_PITCH)
	{
		g_pitchSpeed = MAX_CAMERA_PITCH;
	}
	if (g_pitchSpeed < -MAX_CAMERA_PITCH)
	{
		g_pitchSpeed = -MAX_CAMERA_PITCH;
	}
}

void CalculateCameraRoll()
{
	double rollAcc = 0;
	if (g_controlState[HK_ROLL_RIGHT])
	{
		rollAcc = -ROLL_ACCELERATION;
	}
	if (g_controlState[HK_ROLL_LEFT])
	{
		rollAcc = ROLL_ACCELERATION;
	}

	g_rollSpeed -= g_rollSpeed * ROLL_DECELERATION * g_delta;
	g_rollSpeed += rollAcc * g_delta;
	if (g_rollSpeed > MAX_CAMERA_ROLL)
	{
		g_rollSpeed = MAX_CAMERA_ROLL;
	}
	if (g_rollSpeed < -MAX_CAMERA_ROLL)
	{
		g_rollSpeed = -MAX_CAMERA_ROLL;
	}
}

void CalculateCameraBrake()
{
	if (g_controlState[HK_STOP])
	{
		g_cameraSpeed.set(0, 0, 0);
		g_rollSpeed = g_yawSpeed = g_pitchSpeed = 0;
	}
}

void MoveCamera()
{
	CalculateCameraMovement();
	CalculateCameraYaw();
	CalculateCameraPitch();
	CalculateCameraRoll();
	CalculateCameraBrake();

	g_camera.Yaw(g_yawSpeed * g_delta * M_PI / 180.0f);
	g_camera.Pitch(g_pitchSpeed * g_delta * M_PI / 180.0f);
	g_camera.Roll(g_rollSpeed * g_delta * M_PI / 180.0f);
	doubleVector3 cameraOffset = g_cameraSpeed * g_delta;
	g_camera.Move(cameraOffset.x, cameraOffset.y, cameraOffset.z);
	g_camera.Normalize();
}

void SetCamera()
{
	g_camera.LoadToOpenGL();
}

GLenum g_polygonMode = GL_FILL;
bool g_textureEnabled = true;
bool g_detailTextureEnabled = true;

void DrawSky(float eyeOffset)
{
	{
		// выключаем мультитекстурирование при рисовании неба
		if (multiTextureSupported)
		{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
		glBindTexture(GL_TEXTURE_2D, g_skyTexture);
		glEnable(GL_TEXTURE_2D);
	}

	glPushMatrix();
	{
		float matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, &matrix[0]);
		matrix[12] = matrix[13] = matrix[14] = 0;
		glLoadIdentity();
		glTranslatef(eyeOffset, 0, -20);
		glMultMatrixf(matrix);

		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			glCallList(g_skyList);

			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
		}
	}
	glPopMatrix();
}

void DrawTerrain()
{
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT, g_polygonMode);

	if (multiTextureSupported)
	{
		if (g_detailTextureEnabled)
		{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
		else
		{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
	}

	if (g_textureEnabled)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	g_tile.Draw();
}

void DrawEyeView(float dx)
{
	CCamera eyeView(g_camera);

	eyeView.Move(dx, 0, 0);
	eyeView.LoadToOpenGL();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if (g_fogMode)
	{
		glEnable(GL_FOG);
		float fogDensity = 0.008f;
		float fogStart = 100.0f;
		float fogEnd = 230;
		Color4d fogColor = { 0.8f, 0.8f, 0.8f, 1 };

		glFogi(GL_FOG_MODE, g_fogMode);
		glFogf(GL_FOG_DENSITY, fogDensity);
		glFogf(GL_FOG_START, fogStart);
		glFogf(GL_FOG_END, fogEnd);
		glFogfv(GL_FOG_COLOR, &fogColor.r);
	}
	else
	{
		glDisable(GL_FOG);
	}

	// рисуем небо
	DrawSky(dx);

	DrawTerrain();
}

void DrawScene()
{
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// просто очищаем буфер рисования
	ClearBuffers(1, 1, 1, 1);

	glPointSize(3);

	ShiftTime();
	MoveCamera();

	if (g_enableStereo)
	{
		float EYE_DISTANCE = 0.1f;
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		DrawEyeView(-EYE_DISTANCE);

		glClear(GL_DEPTH_BUFFER_BIT);

		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		DrawEyeView(+EYE_DISTANCE);
	}
	else
	{
		DrawEyeView(0);
	}

	// завершающие действия - дожидаемся конца рисования и
	// меняем внеэкранный и экранный буферы местами
	EndDrawing();
}

void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	DrawScene();

	EndPaint(hWnd, &ps);
}

bool InitMapTile()
{
	GLfloat heightMap[VERTEX_TILE_SIZE * VERTEX_TILE_SIZE];
	BYTE heightFile[VERTEX_TILE_SIZE * VERTEX_TILE_SIZE];
	FILE* pFile;
	fopen_s(&pFile, "height.raw", "rb");
	if (!pFile)
	{
		return false;
	}
	fread(&heightFile, 1, sizeof(heightFile), pFile);
	fclose(pFile);

	static const float HEIGHT_SCALE = 100 / 255.0f;
	static const float HEIGHT_OFFSET = HEIGHT_SCALE / 2;

	for (int i = 0; i < sizeof(heightFile); ++i)
	{
		heightMap[i] = heightFile[i] * HEIGHT_SCALE + HEIGHT_OFFSET;
	}

	GLuint tex = textureLoader.LoadTexture2D(L"tex.bmp");
	if (!tex)
	{
		return false;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint texDetail = textureLoader.LoadTexture2D(L"detail.bmp");
	if (!texDetail)
	{
		return false;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	g_tile.Load(-VERTEX_TILE_SIZE * 2, -VERTEX_TILE_SIZE * 2, VERTEX_TILE_SIZE, VERTEX_TILE_SIZE, 4, &heightMap[0], tex, texDetail);

	return true;
}

void DeleteTileMap()
{
	g_tile.Destroy();
}

bool InitSkySphere()
{
	g_skyTexture = textureLoader.LoadTexture2D(L"sky.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	g_skyList = glGenLists(1);
	glNewList(g_skyList, GL_COMPILE);
	{
		const int SKY_SEGMENTS = 20;
		const int SKY_SECTORS = 40;
		const float SKY_SPHERE_RADIUS_Z = 200;
		const float SKY_SPHERE_RADIUS_XY = 200;

		const float dv = 1.0f / SKY_SEGMENTS;
		float v = 0.01f;
		for (int segment = 0; segment < SKY_SEGMENTS - 1; ++segment, v += dv)
		{
			float z0 = SKY_SPHERE_RADIUS_Z * cosf(segment * M_PI / (SKY_SEGMENTS * 2)) - 60;
			float z1 = SKY_SPHERE_RADIUS_Z * cosf((segment + 1) * M_PI / (SKY_SEGMENTS * 2)) - 60;

			float r0 = SKY_SPHERE_RADIUS_XY * sinf(segment * M_PI / (SKY_SEGMENTS * 2));
			float r1 = SKY_SPHERE_RADIUS_XY * sinf((segment + 1) * M_PI / (SKY_SEGMENTS * 2));

			float u = 0;
			const float du = 1.0f / SKY_SECTORS;

			glBegin(GL_TRIANGLE_STRIP);
			for (int sector = 0; sector <= SKY_SECTORS; ++sector, u += du)
			{
				int sector3d = (sector == SKY_SECTORS) ? 0 : sector;
				float x0 = r0 * cosf(sector3d * M_PI / (SKY_SECTORS * 0.5f));
				float y0 = r0 * sinf(sector3d * M_PI / (SKY_SECTORS * 0.5f));
				float x1 = r1 * cosf(sector3d * M_PI / (SKY_SECTORS * 0.5f));
				float y1 = r1 * sinf(sector3d * M_PI / (SKY_SECTORS * 0.5f));

				glTexCoord2f(u, v + dv);
				glVertex3f(x1, y1, z1);

				glTexCoord2f(u, v);
				glVertex3f(x0, y0, z0);
			}
			glEnd();
		}
	}
	glEndList();

	return true;
}

bool OnCreate(HWND hWnd)
{
	g_camera.LookAt(
		0, 0, 50,
		0, 1, 50,
		0, 0, 1);
	// инициализируем OpenGL
	if (InitOpenGL(hWnd))
	{
		if (InitMapTile() && InitSkySphere())
		{
			return true;
		}
	}
	return false;
}

void OnDestroy(HWND hWnd)
{
	DeleteTileMap();
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
	case ID_POLYGONMODE_LINES:
		g_polygonMode = GL_LINE;
		break;
	case ID_POLYGONMODE_POINTS:
		g_polygonMode = GL_POINT;
		break;
	case ID_POLYGONMODE_SOLID:
		g_polygonMode = GL_FILL;
		break;
	case ID_TEXTURE_ENABLE:
		g_textureEnabled = true;
		break;
	case ID_TEXTURE_DISABLE:
		g_textureEnabled = false;
		break;
	case ID_DETAILTEXTURE_ENABLED:
		g_detailTextureEnabled = true;
		break;
	case ID_DETAILTEXTURE_DISABLED:
		g_detailTextureEnabled = false;
		break;
	case ID_FOG_NONE:
		g_fogMode = 0;
		break;
	case ID_FOG_LINEAR:
		g_fogMode = GL_LINEAR;
		break;
	case ID_FOG_EXPONENT:
		g_fogMode = GL_EXP;
		break;
	case ID_FOG_EXPONENT2:
		g_fogMode = GL_EXP2;
		break;
	case ID_STEREO_ENABLE:
		g_enableStereo = true;
		break;
	case ID_STEREO_DISABLE:
		g_enableStereo = false;
		break;
	default:
		break;
	}
}

void OnKeyDown(WPARAM keyCode, LPARAM flags)
{
	for (int k = 0; k < NUM_KEYS; ++k)
	{
		if (keyCode == g_keyMap[k].keyCode)
		{
			g_controlState[g_keyMap[k].gameAction] = true;
			return;
		}
	}
}

void OnKeyUp(WPARAM keyCode, LPARAM flags)
{
	for (int k = 0; k < NUM_KEYS; ++k)
	{
		if (keyCode == g_keyMap[k].keyCode)
		{
			g_controlState[g_keyMap[k].gameAction] = false;
			return;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		break;
	case WM_KEYUP:
		OnKeyUp(wParam, lParam);
		break;
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
