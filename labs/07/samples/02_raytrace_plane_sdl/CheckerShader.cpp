#include "stdafx.h"
#include "CheckerShader.h"
#include "VectorMath.h"
#include "ShadeContext.h"

CCheckerShader::CCheckerShader(CMatrix4d const& textureTransform)
:m_textureTransform(textureTransform)
{
}

void CCheckerShader::SetTextureTransform(CMatrix4d const& textureTransform)
{
	m_textureTransform = textureTransform;
}

CVector4f CCheckerShader::Shade(CShadeContext const & shadeContext)const
{
	/*
	Шейдер шахматной доски подвергает точку, заданную в системе координат объекта,
	преобразованию, заданному матрицей трансформации.
	У полученной точки определяется принадлежность к черной или белой клетке трехмерного шахматного пространства
	*/

	// Представление точки в виде 4-мерного вектора
	CVector4d pt(shadeContext.GetSurfacePointInObjectSpace(), 1.0);
	// Трансформируем координаты матрицей трансформации текстурных координат
	CVector3d transformedPoint = (m_textureTransform * pt).Project();
	/*
	Вычисляем дробную часть координат точки в систем координат объекта
	*/
	CVector3d fract = Fract(transformedPoint);
	/*
	Координаты, превышающие 0.5, будут иметь значение 1, а не превышающие - 0
	*/
	CVector3d s = Step(0.5, fract);

	/*
	Применяем операцию XOR для определения принадлежности точки либо к черному, либо к белому кубу
	*/
	if (int(s.x) ^ int(s.y) ^ int(s.z))
	{
		return CVector4f(1, 1, 1, 1);
	}
	else
	{
		return CVector4f(0, 0, 0, 1);
	}

}