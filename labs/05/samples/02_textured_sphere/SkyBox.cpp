#include "SkyBox.h"
#include "../libgl/TextureLoader.h"

/*
Режим оборачивания текстурных координат, при котором
при выходе текстурной координаты фрагмента за пределы диапазона 0-1,
используется значение пикселя на ближайшем ребре текстурного изображения.
Данные режим отсутствует в заголовочных файлах OpenGL 1.1, поставляемых с
Visual Studio, но доступен в виде расширения и поддерживается в настоящее время
всеми современными видеокартами
*/
#ifndef GL_CLAMP_TO_EDGE_EXT
#define GL_CLAMP_TO_EDGE_EXT 0x812F
#endif

CSkyBox::CSkyBox(
	std::wstring const& leftTexture,
	std::wstring const& rightTexture,
	std::wstring const& downTexture,
	std::wstring const& upTexture,
	std::wstring const& backTexture,
	std::wstring const& frontTexture)
	: m_initialized(false)
	, m_leftTexture(leftTexture)
	, m_rightTexture(rightTexture)
	, m_downTexture(downTexture)
	, m_upTexture(upTexture)
	, m_backTexture(backTexture)
	, m_frontTexture(frontTexture)
{
}

void CSkyBox::Draw() const
{
	if (!m_initialized)
	{
		CTextureLoader loader;
		// Устанавливаем режим "заворачиваиия" текстурных координат,
		// обеспечивающие минимальную видимость стыков на гранях куба
		loader.SetWrapMode(GL_CLAMP_TO_EDGE_EXT, GL_CLAMP_TO_EDGE_EXT);

		m_cube.SetTextures(
			loader.LoadTexture2D(m_leftTexture),
			loader.LoadTexture2D(m_rightTexture),
			loader.LoadTexture2D(m_downTexture),
			loader.LoadTexture2D(m_upTexture),
			loader.LoadTexture2D(m_backTexture),
			loader.LoadTexture2D(m_frontTexture));
		m_initialized = true;
	}
	// Устанавливаем необходимые режимы визуализации куба
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	// сохраняем текущую матрицу
	glPushMatrix();

	// Положение камеры располаагется в 4 столбце матрицы (элементы 12-14)
	// Если занести туда 0, то матрица станет матрицей вращения.
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	modelView[12] = modelView[13] = modelView[14] = 0;
	glLoadMatrixf(modelView);

	// рисуем куб
	m_cube.Draw();

	// восстанавливаем текущую матрицу
	glPopMatrix();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}
