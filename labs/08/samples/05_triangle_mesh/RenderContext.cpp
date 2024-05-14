#include "stdafx.h"
#include "RenderContext.h"
#include "Vector2.h"
#include "Ray.h"
#include "Plane.h"
#include "Intersection.h"
#include "Scene.h"
#include "VectorMath.h"
#include "IColorBuffer.h"

CRenderContext::CRenderContext(void)
{
}

void CRenderContext::RenderRectangle(CScene const& scene, int left, int top, unsigned width, unsigned height, IColorBuffer & colorBuffer)const
{
	const int right = left + width;
	const int bottom = top + height;

	// Вычисляем направления лучей в левом верхнем, правом верхнем и левом нижнем углах прямоугольной области
	CRay leftTop = CalculateRay(left + 0.5, top + 0.5);
	CRay rightTop = CalculateRay(right + 0.5, top + 0.5);
	CRay leftBottom = CalculateRay(left + 0.5, bottom + 0.5);

	// Шаг, с которым изменяется начальная координата луча вдоль горизонтальной оси экрана
	CVector3d rayStartHorizontalStep = (rightTop.GetStart() - leftTop.GetStart()) / (double)width;
	// Шаг, с которым изменяется начальная координата луча вдоль вертикальной оси экрана
	CVector3d rayStartVerticalStep = (leftBottom.GetStart() - leftTop.GetStart()) / (double)height;

	// Шаг, с которым изменяется направление луча вдоль горизонтальной оси экрана
	CVector3d rayDirectionHorizontalStep = (rightTop.GetDirection() - leftTop.GetDirection()) / (double)width;
	// Шаг, с которым изменяется направление луча вдоль вертикальной оси экрана
	CVector3d rayDirectionVerticalStep = (leftBottom.GetDirection() - leftTop.GetDirection()) / (double)height;

	// Начало и направление луча в левой верхней точке рисуемой области
	CVector3d start0 = leftTop.GetStart();
	CVector3d direction0 = leftTop.GetDirection();

	for (int y = top; y < bottom;++y)
	{
		// Начало и направление луча в начале строки
		CVector3d start = start0;
		CVector3d direction = direction0;

		for (int x = left; x < right; ++x)
		{
			// Обрабатываемый пиксель с координатами (x, y) находится внутри видового порта?
			if (m_viewPort.TestPoint(x, y))
			{
				// Трассируем луч по сцене и получаем цвет объекта в точке ближайшего столкновения
				CVector4f color = scene.Shade(CRay(start, direction));

				// Записываем в буфер цвета квантованное значение цвета
				colorBuffer.SetPixelColor(x, y, QuantizeColor(color));
			}
			else
			{
				// Пиксель находится за пределами видового порта и имеет цвет 0
				colorBuffer.SetPixelColor(x, y, 0x000000);
			}

			// Вычисляем начальную точку и направление луча для следующего пикселя в данной строке
			start += rayStartHorizontalStep;
			direction += rayDirectionHorizontalStep;
		}
		// Вычисляем точку начала и направление трассировки луча для следующей рисуемой строки
		start0 += rayStartVerticalStep;
		direction0 += rayDirectionVerticalStep;
	}
}

CRay CRenderContext::CalculateRay(double screenX, double screenY)const
{
	// Вычисляем координаты центра пикселя в нормализованных координатах видового порта
	CVector2d pixelCenter = GetNormalizedViewportCoord(screenX + 0.5, screenY + 0.5);

	// Вычисляем начальную и конечную точки луча, проходящего через центр данного пикселя
	CVector3d rayStart = UnProject(pixelCenter.x, pixelCenter.y, 0);
	CVector3d rayEnd = UnProject(pixelCenter.x, pixelCenter.y, 1);

	// Направление трассируемого луча
	CVector3d rayDirection = rayEnd - rayStart;

	return CRay(rayStart, rayDirection);
}

std::uint32_t CRenderContext::CalculatePixelColor(CScene const& scene, int x, int y)const
{
	// Проверяем принадлежность точки видовому порту
	if (!m_viewPort.TestPoint(x, y))
	{
		// Точка за пределами видового порта - выходим
		return 0x000000; // (RGB 0 0 0)
	}

	double pixelCenterX = x + 0.5;
	double pixelCenterY = y + 0.5;

	// Трассируем луч вглубь сцены, получая цвет объекта, с которым произошло столкновение
	CVector4f color = scene.Shade(CalculateRay(pixelCenterX, pixelCenterY));
	
	return QuantizeColor(color);
}

std::uint32_t CRenderContext::QuantizeColor(CVector4f const& color)
{
	// Приводим компоненты цвета к диапазону 0 до 1
	CVector4f clampedColor = Clamp(color, 0.0f, 1.0f);

	// Возвращаем цвет в формате 0xAARRGGBB
	uint8_t r = static_cast<uint8_t>(clampedColor.x * 255);
	uint8_t g = static_cast<uint8_t>(clampedColor.y * 255);
	uint8_t b = static_cast<uint8_t>(clampedColor.z * 255);
	uint8_t a = static_cast<uint8_t>(clampedColor.w * 255);
	return (a << 24) | (r << 16) | (g << 8) | b;
}

/*
Установка видового порта
*/
void CRenderContext::SetViewPort(CViewPort const& viewPort)
{
	m_viewPort = viewPort;
}

/*
Установка матрицы проецирования
*/
void CRenderContext::SetProjectionMatrix(CMatrix4d const& projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;

	UpdateInverseModelViewProjectionMatrix();
}

void CRenderContext::UpdateInverseModelViewProjectionMatrix()
{
	m_inverseModelViewProjectionMatrix = (m_projectionMatrix * m_modelViewMatrix).GetInverseMatrix();
}

/*
Установка матрицы моделирования-вида
*/
void CRenderContext::SetModelViewMatrix(CMatrix4d const& modelViewMatrix)
{
	m_modelViewMatrix = modelViewMatrix;

	UpdateInverseModelViewProjectionMatrix();
}

/*
Вычисление нормализованных координат внутри видового порта
*/
CVector2d CRenderContext::GetNormalizedViewportCoord(double x, double y)const
{
	// Координаты центра видового порта
	double viewportCenterX = (m_viewPort.GetLeft() + m_viewPort.GetRight()) * 0.5;
	double viewportCenterY = (m_viewPort.GetTop() + m_viewPort.GetBottom()) * 0.5;

	// Нормализованная координата X внутри видового порта будет иметь координаты от -1 (слева) до +1 (справа)
	double xNormalized = 2 * (x - viewportCenterX) / m_viewPort.GetWidth();
	// Нормализованная координата Y внутри видового порта будет иметь координата от -1 (внизу) до +1 (сверху)
	double yNormalized = -2 * (y - viewportCenterY) / m_viewPort.GetHeight();

	// Возвращаем результат
	return CVector2d(xNormalized, yNormalized);
}

/*
Возвращаем обратную матрицу произведения матриц проецирования и моделирования вида
*/
CMatrix4d const& CRenderContext::GetInverseModelViewProjectionMatrix()const
{
	return m_inverseModelViewProjectionMatrix;
}

/*
Вычисляет координаты точки, имеющей нормализованные экранные координатами scceenX, screenY и глубину depth
в мировой системе координат.
Глубине=0 соответствует ближняя плоскость отсечения, задаваемая матрицей проецирования, а глубине=1 - дальняя
*/
CVector3d CRenderContext::UnProject(double normalizedX, double normalizedY, double depth)const
{
	// Для вычисления нужно преобразовать глубину точки в нормализованную координату z
	// и умножить точку в нормализованных координатах на обратную матрицу произведения матриц проецирования и моделирования-вида
	CMatrix4d const& invModelViewProj = GetInverseModelViewProjectionMatrix();

	/*
	Вычисляем нормализованную координату z (от -1 (ближняя плоскость отсечения) до +1 (дальняя)
	*/
	double normalizedZ = depth * 2 - 1;

	// Теперь просто трансформируем четырехмерную точку
	CVector4d pos = invModelViewProj * CVector4d(normalizedX, normalizedY, normalizedZ, 1);

	// И преобразовываетм ее в трехмерные координаты, выполняя перспективное деление
	return pos.Project();
}
