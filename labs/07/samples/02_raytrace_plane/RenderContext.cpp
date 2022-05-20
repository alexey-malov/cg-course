#include "stdafx.h"
#include "RenderContext.h"
#include "Vector2.h"
#include "Ray.h"
#include "Plane.h"
#include "Intersection.h"
#include "Scene.h"
#include "VectorMath.h"

CRenderContext::CRenderContext(void)
{
}

std::uint32_t CRenderContext::CalculatePixelColor(CScene const& scene, int x, int y)const
{
	// Проверяем принадлежность точки видовому порту
	if (!m_viewPort.TestPoint(x, y))
	{
		// Точка за пределами видового порта - выходим
		return 0x000000;
	}

	// Вычисляем координаты центра пикселя в нормализованных координатах видового порта
	CVector2d pixelCenter = GetNormalizedViewportCoord(x + 0.5, y + 0.5);

	// Вычисляем начальную и конечную точки луча, проходящего через центр данного пикселя
	CVector3d rayStart = UnProject(pixelCenter.x, pixelCenter.y, 0);
	CVector3d rayEnd = UnProject(pixelCenter.x, pixelCenter.y, 1);

	// Направление трассируемого луча
	CVector3d rayDirection = rayEnd - rayStart;
	
	// Трассируем луч вглубь сцены, получая цвет объекта, с которым произошло столкновеине
	CVector4f color = scene.Shade(CRay(rayStart, rayDirection));
	
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
