//////////////////////////////////////////////////////////////////////


#if !defined(AFX_UTILS_H__E853DC92_4248_4C69_A444_CDA5510C9091__INCLUDED_)
#define AFX_UTILS_H__E853DC92_4248_4C69_A444_CDA5510C9091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



inline float GetRandomFloat(float lowBound, float highBound)
{
	return (rand() * (highBound - lowBound) / RAND_MAX) + lowBound;
}

struct Vector2d
{
	float x, y;

	Vector2d() = default;

	Vector2d(float newX, float newY)
		:x(newX)
		,y(newY)
	{
	}
	
	void DeclareTextureCoord()
	{
		glTexCoord2f(x, y);
	}
};


struct Vector3d
{
	float x, y, z;	// координаты 3d вектора

	Vector3d()
	{
	}

	Vector3d(float newX, float newY, float newZ)
		:x(newX)
		,y(newY)
		,z(newZ)
	{
	}

	Vector3d(const Vector3d& v)
		:x(v.x)
		,y(v.y)
		,z(v.z)
	{
	}
	
	Vector3d& operator=(const Vector3d& v)
	{
		if (this != &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}

	void DeclareOpenGLVertex()const
	{
		glVertex3f(x, y, z);
	}

	void DeclareOpenGLNormal()const
	{
		glNormal3f(x, y, z);
	}

	void Set(float newX, float newY, float newZ)
	{
		x = newX;
		y = newY;
		z = newZ;
	}

	void Clear()
	{
		x = y = z = 0;
	}

	Vector3d& operator-=(Vector3d const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3d& operator+=(Vector3d const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3d operator*(float scale)const
	{
		return Vector3d(x * scale, y*scale, z*scale);
	}

	Vector3d operator+(Vector3d const& v)const
	{
		return Vector3d(x + v.x, y + v.y, z + v.z);
	}

	Vector3d operator-(Vector3d const& v)const
	{
		return Vector3d(x - v.x, y - v.y, z - v.z);
	}

	void Normalize()
	{
		float one_div_len = 1.0f/sqrtf(x*x + y*y + z*z);
		x *= one_div_len;
		y *= one_div_len;
		z *= one_div_len;
	}

	float DotProduct(Vector3d const& v)const
	{
		return (x * v.x) + (y * v.y) + (z * v.z);
	}
	
	void CrossProduct(Vector3d const& vec1, Vector3d const& vec2)
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
		
		x = y1*z2 - y2*z1;
		y = z1*x2 - z2*x1;
		z = x1*y2 - x2*y1;
	}
};

struct Color4d
{
	float r, g, b, a;
	void SetOpenGLColor()const
	{
		glColor4f(r, g, b, a);
	}
};

struct QuadFace
{
	int v0, v1, v2, v3;	// номера вершин
	Color4d faceColor;
};

// вычисляем нормаль к треугольнику
void CalculateTriangleNormal(Vector3d const& a, Vector3d const& b, Vector3d const& c, Vector3d *normal);

GLuint LoadTextureFromRgbBitmapFile(const char fileName[]);

#endif // !defined(AFX_UTILS_H__E853DC92_4248_4C69_A444_CDA5510C9091__INCLUDED_)
