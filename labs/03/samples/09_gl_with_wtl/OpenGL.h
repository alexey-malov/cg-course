#pragma once

class COpenGL
{
public:
	COpenGL(HWND hwnd, bool needDepth = true, bool needStencil = false);
	~COpenGL(void);
	void Destroy(void);
	void SwapBuffers();

private:
	HWND m_hwnd;
	HGLRC m_rc;
	CClientDC m_dc;
};
