#include "StdAfx.h"
#include "OpenGL.h"

COpenGL::COpenGL(HWND hwnd, bool needDepth, bool needStencil)
:m_dc(hwnd)
,m_rc(NULL)
,m_hwnd(hwnd)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));

	// Заполняем поля структуры PIXELFORMATDESCRIPTOR,
	// задающей желаемые параметры буфера кадра OpenGL
	pfd.nSize = sizeof(pfd);		// инициализируем размер структуры
	pfd.nVersion = 1;				// Windows поддерживает только версию 1
	pfd.dwFlags =
		PFD_SUPPORT_OPENGL |		// буфер поддерживает рисование OpenGL
		PFD_DRAW_TO_WINDOW |		// буфер поддерживает рисование в окне
		PFD_DOUBLEBUFFER |			// используется двойная буферизация
		PFD_GENERIC_ACCELERATED;	// и аппаратное ускорение
	pfd.iPixelType = PFD_TYPE_RGBA;	// формат пикселей - RGBA
	pfd.cColorBits = 32;			// 32 бита на хранение цвета
	pfd.cDepthBits = 
		needDepth ? 24 : 0;	// если нужно, испольузем 24-битный буфер глубины
	pfd.cStencilBits = 
		needStencil ? 8 : 0;	// если нужно, используем 8-битный буфер трафарета

	int pixelFormat = m_dc.ChoosePixelFormat(&pfd);

	if (!pixelFormat)
	{
		Destroy();
		return;
	}

	if (!m_dc.SetPixelFormat(pixelFormat, &pfd))
	{
		Destroy();
		return;
	}

	m_rc = wglCreateContext(m_dc);
	if (m_rc)
	{
		if (!wglMakeCurrent(m_dc, m_rc))
		{
			Destroy();
		}
	}
}

COpenGL::~COpenGL(void)
{
	Destroy();
}

// Освобождаем контекст устройства и ресурсы, связанные с OpenGL
void COpenGL::Destroy(void)
{
	if (m_rc)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_rc);
		m_rc = NULL;
	}

	if (m_dc)
	{
		::ReleaseDC(m_hwnd, m_dc.Detach());
	}
}

// Выполняем обмен лицевого и теневого буферов кадра
void COpenGL::SwapBuffers()
{
	if (m_dc && m_rc)
	{
		::SwapBuffers(m_dc);
	}
}
