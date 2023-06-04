#include "pch.h"
#include "Vector3d.h"

Vector3d::Vector3d()
	: x(0)
	, y(0)
	, z(0)
{
}

Vector3d::Vector3d(float newX, float newY, float newZ)
	: x(newX)
	, y(newY)
	, z(newZ)
{
}

Vector3d::Vector3d(const Vector3d& v)
	: x(v.x)
	, y(v.y)
	, z(v.z)
{
}

Vector3d& Vector3d::operator = (const Vector3d& v)
{
	if (this != &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	return *this;
}

void Vector3d::DeclareOpenGLVertex() const
{
	glVertex3f(x, y, z);
}

void Vector3d::DeclareOpenGLNormal() const
{
	glNormal3f(x, y, z);
}

void Vector3d::Set(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

void Vector3d::Clear()
{
	x = y = z = 0;
}

Vector3d& Vector3d::operator -= (Vector3d const& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3d& Vector3d::operator += (Vector3d const& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3d Vector3d::operator + (Vector3d const& v) const
{
	return Vector3d(x + v.x, y + v.y, z + v.z);
}

Vector3d Vector3d::operator - (Vector3d const& v) const
{
	return Vector3d(x - v.x, y - v.y, z - v.z);
}

void Vector3d::Normalize()
{
	float one_div_len = 1.0f / sqrtf(x * x + y * y + z * z);
	x *= one_div_len;
	y *= one_div_len;
	z *= one_div_len;
}

float Vector3d::DotProduct(Vector3d const& v) const
{
	return (x * v.x) + (y * v.y) + (z * v.z);
}

void Vector3d::CrossProduct(Vector3d const& vec1, Vector3d const& vec2)
{
	const float x1 = vec1.x;
	const float y1 = vec1.y;
	const float z1 = vec1.z;
	const float x2 = vec2.x;
	const float y2 = vec2.y;
	const float z2 = vec2.z;
	/*
	|i  j  k |   |y1*z2 - y2*z1|
	|x1 y1 z1| = |z1*x2 - z2*x1|
	|x2 y2 z2|   |x1*y2 - x2*y1|

	*/

	x = y1 * z2 - y2 * z1;
	y = z1 * x2 - z2 * x1;
	z = x1 * y2 - x2 * y1;
}