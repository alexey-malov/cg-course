#pragma once

#include "Vector3.h"
#include "Vector2.h"

class CUtils3D
{
public:
	static CVector3f CalculateTangent(
		CVector3f const& p1, CVector3f const& p2, CVector3f const& p3,
		CVector2f const& t1, CVector2f const& t2, CVector2f const& t3
		);
};
