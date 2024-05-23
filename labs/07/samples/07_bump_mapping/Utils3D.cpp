#include "StdAfx.h"
#include "Utils3D.h"

CVector3f CUtils3D::CalculateTangent(
   CVector3f const& p1, CVector3f const& p2, CVector3f const& p3,
   CVector2f const& t1, CVector2f const& t2, CVector2f const& t3
	)
{
	float denominator = (t2.x - t1.x) * (t3.y - t1.y) - (t3.x - t1.x) * (t2.y - t1.y);
	CVector3f numerator = (p2 - p1) * (t3.y - t2.y) - (p3 - p1) * (t2.y - t1.y);
	CVector3f tangent = numerator * (1.0f / denominator);
	tangent.Normalize();
	return tangent;
}
