#pragma once

class Vector3d
{
public:
	float x, y, z; // координаты 3d вектора

	Vector3d();

	Vector3d(float newX, float newY, float newZ);

	Vector3d(const Vector3d& v);

	Vector3d& operator=(const Vector3d& v);

	void DeclareOpenGLVertex() const;

	void DeclareOpenGLNormal() const;

	void Set(float newX, float newY, float newZ);

	void Clear();

	Vector3d& operator-=(Vector3d const& v);

	Vector3d& operator+=(Vector3d const& v);

	Vector3d operator+(Vector3d const& v) const;

	Vector3d operator-(Vector3d const& v) const;

	void Normalize();

	float DotProduct(Vector3d const& v) const;

	void CrossProduct(Vector3d const& vec1, Vector3d const& vec2);
};