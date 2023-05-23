//////////////////////////////////////////////////////////////////////

#include "framework.h"
#include "Graphics.h"

static HWND		g_hWnd = NULL;
static HDC		g_DC = NULL;	// window device context
static HGLRC	g_RC = NULL;	// openGL render context

static int g_colorBufferBits;	// ���������� ��� ����� �� �������
static int g_depthBufferBits;	// ���������� ��� � ������� �������
static int g_stencilBufferBits;	// ���������� ��� � ������� ���������

// ������������� ��������, ��������� � OpenGL
bool InitOpenGL(HWND hWnd, BYTE colorBits, BYTE depthBits, BYTE stencilBits)
{
	// �������� � ���������� �������� ����������, ���������� � �����
	g_hWnd = hWnd;

	g_colorBufferBits = colorBits;
	g_depthBufferBits = depthBits;
	g_stencilBufferBits = stencilBits;

	g_DC = GetDC(hWnd);
	if (g_DC == NULL)
	{
		return false;
	}

	// �������������� ���� ��������� PIXELFORMATDESCRIPTOR ������������ ����������
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// WORD  nSize;	������ ���������
		1,								// WORD  nVersion; ������ OpenGL (������ ���� ����� 1)
		PFD_SUPPORT_OPENGL |			// �����: ����� ������������ ��������� OpenGL
			PFD_GENERIC_ACCELERATED |	// ������ �������� �������������� ��������� ���������� - ����� �������� ���������� ���������
			PFD_DOUBLEBUFFER,			// ������������ ������� �����������
		PFD_TYPE_RGBA,					// BYTE  iPixelType; ��� �������� - RGBA
		colorBits,						// BYTE  cColorBits; ���������� ��� �� �������
		0,								// BYTE  cRedBits;	�����
		0,								// BYTE  cRedShift;	�����
		0,								// BYTE  cGreenBits;	�����
		0,								// BYTE  cGreenShift;	�����
		0,								// BYTE  cBlueBits;	�����
		0,								// BYTE  cBlueShift;	�����
		0,								// BYTE  cAlphaBits;	�����
		0,								// BYTE  cAlphaShift;	�����
		0,								// BYTE  cAccumBits;	�����
		0,								// BYTE  cAccumRedBits;	�����
		0,								// BYTE  cAccumGreenBits;	�����
		0,								// BYTE  cAccumBlueBits;	�����
		0,								// BYTE  cAccumAlphaBits;	�����
		depthBits,						// BYTE  cDepthBits;	���������� ��� � ������ �������. ������ 16 ��� 24
		stencilBits,					// BYTE  cStencilBits;	���������� ��� � ������ ���������. ������ 0 ��� 8
		0,								// BYTE  cAuxBuffers; ���������� �������������� �������
		0,								// BYTE  iLayerType; �� ������������
		0,								// BYTE  bReserved; ���������������
		0,								// DWORD dwLayerMask; �� ������������
		0,								// DWORD dwVisibleMask; ����� ����� 0
		0,								// DWORD dwDamageMask; �� ������������ �����
	};

	int pixelFormat = ChoosePixelFormat(g_DC, &pfd);
	if (!pixelFormat)
	{
		return false;
	}

	if (!SetPixelFormat(g_DC, pixelFormat, &pfd))
	{
		return false;
	}

	// ������� �������� ���������� ��� �� ������ ��������� ����������
	g_RC = wglCreateContext(g_DC);
	if (g_RC == NULL)
	{
		return FALSE;
	}

	/*
		�.�. � ������ ���������� ���������� ����� ���� �������
		��������� ���������� ����������, � �������� �� ����� �
		���� ������ ������� �� ������� ������ ���������� ������ � ����� �� ���,
		�� ������ ���� �������� ���������� �������
	*/
	if (!wglMakeCurrent(g_DC, g_RC))
	{
		return false;
	}


	return true;
}

// ����������� �������, ��������� � OpenGL
void DestroyOpenGL()
{
	if (g_RC != NULL)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_RC);
		g_RC = NULL;
	}

	if (g_DC != NULL)
	{
		ReleaseDC(g_hWnd, g_DC);
		g_DC = NULL;
		g_hWnd = NULL;
	}
}

void ClearBuffers(float r, float g, float b, float a)
{
	// ������������� ���� �������
	glClearColor(r, g, b, a);

	GLbitfield bufferMask = GL_COLOR_BUFFER_BIT;
	if (g_depthBufferBits != 0)
	{
		bufferMask |= GL_DEPTH_BUFFER_BIT;
	}
	if (g_stencilBufferBits != 0)
	{
		bufferMask |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(bufferMask);
}

void EndDrawing()
{
	glFinish();
	SwapBuffers(g_DC);
}
