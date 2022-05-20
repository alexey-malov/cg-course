#pragma once

#include "Matrix4.h"
#include "ViewPort.h"

class CScene;

/*
Класс CRenderContext - контекст визуализации
Выполняет вычисление цвета пикселя в заданных координатах внутри буфера кадра
методом трассировки лучей
Видовой порт задает прямоугольную область в буфере кадра, которая принадлежит контексту рисования
Матрицы моделирования-вида и проецирования аналогично соответствующим матрицам OpenGL
*/
class CRenderContext
{
public:
	CRenderContext(void);

	/*
	Трассирует путь луча по сцене, проходящего через пиксель с указанными координатами
	Возвращает цвет данного пикселя в формате 0xAARRBBGG
	*/
	std::uint32_t CalculatePixelColor(CScene const& scene, int x, int y)const;

	/*
	Задает параметры видового порта
	*/
	void SetViewPort(CViewPort const& viewPort);

	/*
	Устанавливает матрицу проецирования (перспективного, либо ортографического)
	*/
	void SetProjectionMatrix(CMatrix4d const& projectionMatrix);

	/*
	Устанавливает матрицу моделирования-вида, задающую положение и ориентацию камеры в пространстве
	*/
	void SetModelViewMatrix(CMatrix4d const& modelViewMatrix);

private:
	/*
	Преобразовывает экранные координаты пикселя в нормализованные экранные координаты
	В нормализованных координатах верхний левый угол видового порта имеет координаты (-1, +1), 
		а правый нижний: (+1, -1)
	*/
	CVector2d GetNormalizedViewportCoord(double x, double y)const;

	/*
	Возвращает матрицу, обратную произведению матриц проецирования и моделирования-вида
	*/
	CMatrix4d const& GetInverseModelViewProjectionMatrix()const;

	/*
	Вычисляет координаты точки, имеющей нормализованные экранные координатами scceenX, screenY и глубину depth
	в мировой системе координат.
	Глубине=0 соответствует ближняя плоскость отсечения, задаваемая матрицей проецирования, а глубине=1 - дальняя
	*/
	CVector3d UnProject(double normalizedX, double normalizedY, double depth)const;

	/*
	Вычисляет значение матрицы обратной произведению матриц проецирования и
	моделирования-вида
	*/
	void UpdateInverseModelViewProjectionMatrix();
private:
	// Матрица проецирования
	CMatrix4d	m_projectionMatrix;
	// Матрицам моделирования-вида
	CMatrix4d	m_modelViewMatrix;

	// Матрица обратная произведению матриц проецирования и моделирования-вида
	CMatrix4d	m_inverseModelViewProjectionMatrix;

	// Видовой порт
	CViewPort	m_viewPort;
};
