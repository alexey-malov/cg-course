#pragma once

#include "GaussianBlurFilter.h"
#include "../libgl/Shaders.h"
#include "../libgl/Texture.h"
#include "../libgl/GLApplication.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char* title, int width, int height);

protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnSpecialKey(int key, int x, int y);

private:
	void LoadTextures();
	void LoadShaders();

	// Фильтр размытия
	CGaussianBlurFilter m_blurFilter;
	// Флаг, сигнализирующий о необходимости применения фильтра
	bool m_blur;
	// Дисперсия
	GLfloat m_sigma;
	// Радиус фильтра
	static const int BLUR_RADIUS;

	// Размеры текстурного изображения
	unsigned m_textureWidth;
	unsigned m_textureHeight;

	// Текстура
	CTexture2D m_texture;

	// Шейдерная программа, выполняющая размытие Гаусса
	CProgram m_blurProgram;
};
