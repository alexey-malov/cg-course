#pragma once
#include "IShader.h"
#include "Matrix4.h"
/*
Шейдер шахматной доски
*/
class CCheckerShader :
	public IShader
{
public:
	// Инициализация шейдера матрицей преобразования текстурных координат
	CCheckerShader(CMatrix4d const& textureTransform = CMatrix4d());

	void SetTextureTransform(CMatrix4d const& textureTransform);

	virtual CVector4f Shade(CShadeContext const & shadeContext)const;
private:
	CMatrix4d m_textureTransform;
};
