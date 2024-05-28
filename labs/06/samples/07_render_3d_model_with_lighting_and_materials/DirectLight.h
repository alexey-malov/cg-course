#pragma once

#include "Vector3.h"
#include "Light.h"

class CDirectLight : public CLight
{
public:
	CDirectLight(CVector3f const& lightDirection = CVector3f(0, 0, 1));

	void SetDirection(CVector3f const& direction);

	void SetLight(GLenum light)const;
private:
	CVector3f m_direction;
};
