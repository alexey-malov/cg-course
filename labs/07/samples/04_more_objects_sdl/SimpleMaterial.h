#pragma once

#include "Vector4.h"

/*
Простейший материал, для которого задан только лишь диффузный цвет
*/
class CSimpleMaterial
{
public:
	CSimpleMaterial(void)
	{}

	CVector4f const& GetDiffuseColor()const
	{
		return m_diffuseColor;
	}

	void SetDiffuseColor(CVector4f const& diffuseColor)
	{
		m_diffuseColor = diffuseColor;
	}

private:
	CVector4f m_diffuseColor;
};
