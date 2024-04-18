#pragma once
#include "IShader.h"
#include "SimpleMaterial.h"

/*
Простой шейдер, выполняющий расчет диффузной составляющей отраженного света
*/
class CSimpleDiffuseShader :
	public IShader
{
public:
	CSimpleDiffuseShader();

	/*
	В качестве параметра шейдера выступает класс CSimpleMaterial, хранящий в простейшем случае
	диффузный цвет материала поверхности объекта
	*/
	void SetMaterial(CSimpleMaterial const& material);

	/*
	Вычисление цвета с объекта
	*/
	virtual CVector4f Shade(CShadeContext const & shadeContext)const;
private:
	CSimpleMaterial m_material;
};
