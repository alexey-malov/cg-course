//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICS_H__4EF982B4_DE4E_4BCF_8CD2_080AA3646DF4__INCLUDED_)
#define AFX_GRAPHICS_H__4EF982B4_DE4E_4BCF_8CD2_080AA3646DF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Инициализация ресурсов, связанных с OpenGL
bool InitOpenGL(HWND hWnd, int colorBits = 32, int depthBits = 16, int stencilBits = 0);

// освобождаем ресурсы, связанные с OpenGL
void DestroyOpenGL();


void ClearBuffers(float r, float g, float b, float a);

void EndDrawing();


#endif // !defined(AFX_GRAPHICS_H__4EF982B4_DE4E_4BCF_8CD2_080AA3646DF4__INCLUDED_)
