//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Graphics.h"

static HWND		g_hWnd = NULL;
static HDC		g_DC = NULL;	// window device context
static HGLRC	g_RC = NULL;	// openGL render context

static int g_colorBufferBits;	// количество бит цвета на пиксель
static int g_depthBufferBits;	// количество бит в буффере глубины
static int g_stencilBufferBits;	// количество бит в буффере трафарета

// Инициализация ресурсов, связанных с OpenGL
bool InitOpenGL(HWND hWnd, int colorBits, int depthBits, int stencilBits)
{
	// получаем и запоминаем контекст устройства, связанного с окном
	g_hWnd = hWnd;

	g_colorBufferBits = colorBits;
	g_depthBufferBits = depthBits;
	g_stencilBufferBits = stencilBits;

	g_DC = GetDC(hWnd);
	if (g_DC == NULL)
	{
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// WORD  nSize;	размер структуры
		1,								// WORD  nVersion; версия OpenGL (должна быть равна 1)
		PFD_SUPPORT_OPENGL |			// флаги: буфер поддерживает рисование OpenGL
			PFD_GENERIC_ACCELERATED |	// формат пикселей поддерживается драйвером видеокарты - будет работать аппаратное ускорение
			PFD_DOUBLEBUFFER,			// используется двойная буферизация
		PFD_TYPE_RGBA,					// BYTE  iPixelType; тип пикселей - RGBA
		colorBits,						// BYTE  cColorBits; количество бит на пиксель
		0,								// BYTE  cRedBits;	пофиг
		0,								// BYTE  cRedShift;	пофиг
		0,								// BYTE  cGreenBits;	пофиг
		0,								// BYTE  cGreenShift;	пофиг
		0,								// BYTE  cBlueBits;	пофиг
		0,								// BYTE  cBlueShift;	пофиг
		0,								// BYTE  cAlphaBits;	пофиг
		0,								// BYTE  cAlphaShift;	пофиг
		0,								// BYTE  cAccumBits;	пофиг
		0,								// BYTE  cAccumRedBits;	пофиг
		0,								// BYTE  cAccumGreenBits;	пофиг
		0,								// BYTE  cAccumBlueBits;	пофиг
		0,								// BYTE  cAccumAlphaBits;	пофиг
		depthBits,						// BYTE  cDepthBits;	количество бит в буфере глубины. Обычно 16 или 24
		stencilBits,					// BYTE  cStencilBits;	количество бит в буфере трафарета. Обычно 0 или 8
		0,								// BYTE  cAuxBuffers; количество дополнительных буферов
		0,								// BYTE  iLayerType; не используется
		0,								// BYTE  bReserved; зарезервировано
		0,								// DWORD dwLayerMask; не используется
		0,								// DWORD dwVisibleMask; пусть будет 0
		0,								// DWORD dwDamageMask; не используется более
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

	// создаем контекст рендеринга для на данном контексте рисования
	g_RC = wglCreateContext(g_DC);
	if (g_RC == NULL)
	{
		return FALSE;
	}

	/*
		т.к. с данным контекстом устройства могут быть связаны 
		несколько контекстов рендеринга а работать мы можем в 
		один момент времени из данного потока выполнения только с одним из них, 
		то делаем этот контекст рендеринга текущим
	*/
	if (!wglMakeCurrent(g_DC, g_RC))
	{
		return false;
	}

	
	return true;
}

// освобождаем ресурсы, связанные с OpenGL
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
	// устанавливаем цвет очистки
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
