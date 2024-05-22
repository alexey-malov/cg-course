// Math.h: interface for the Math class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATH_H__B85D40A6_A4D9_45D7_8E67_29EE897C2F18__INCLUDED_)
#define AFX_MATH_H__B85D40A6_A4D9_45D7_8E67_29EE897C2F18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>


template <class TYPE>
class vector2
{
public:
	union
	{
		struct 
		{
			TYPE x,y;
		};
		struct 
		{
			TYPE s,t;
		};
		TYPE vec_array[2];
	};
	
	
	// default constructor
	vector2(){};
	
	vector2(const TYPE* array)
	{
		memcpy(vec_array, array, sizeof(vec_array));
	}
	
	vector2(TYPE x0, TYPE y0):
	x(x0), y(y0){}
	
	vector2(const vector2<TYPE>& other):
	x(other.x), y(other.y){}
	
	void set(TYPE x0, TYPE y0)
	{
		x = x0;
		y = y0;
	}
	
	// assignment
	vector2<TYPE>& operator=(const vector2<TYPE>& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}
	
	// addition with assignment
	vector2<TYPE>& operator+=(const vector2<TYPE>& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	
	// subtraction with assignment
	vector2<TYPE>& operator-=(const vector2<TYPE>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}
	
	// uniform scaling
	vector2<TYPE>& operator*=(const TYPE scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}
	
	// non-uniform scaling
	vector2<TYPE>& operator*=(const vector2<TYPE>& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}
	
	// addition
	vector2<TYPE> operator+(const vector2<TYPE>& other) const
	{
		return vector2<TYPE>(x+other.x, y+other.y);
	}
	
	// subtraction
	vector2<TYPE> operator-(const vector2<TYPE>& other) const
	{
		return vector2<TYPE>(x-other.x, y-other.y);
	}
	
	// non-uniform scaling
	vector2<TYPE> operator*(const vector2<TYPE>& other) const
	{
		return vector2<TYPE>(x*other.x, y*other.y);
	}
	
	// uniform scaling
	vector2<TYPE> operator*(const TYPE scale) const
	{
		return vector2<TYPE>(x*scale, y*scale);
	}
	
	friend vector2<TYPE>operator*(const TYPE scale, const vector2<TYPE>& v)
	{
		return vector2<TYPE>(v.x*scale, v.y*scale);;
	}
	
	// calculate vector length
	TYPE length()const
	{
		return (TYPE)sqrt(x*x + y*y);
	}
	
	// calculate fast vector length
	TYPE lengthFast()const
	{
		return (TYPE)fastsqrt(x*x + y*y);
	}
	
	// calculate length2
	TYPE length2()const
	{
		return (TYPE)(x*x + y*y);
	}
	
	vector2& normalize()
	{
		TYPE inv_length = 1/length();
		
		x *= inv_length;
		y *= inv_length;
		return *this;
	}
	
	// normalize vector length
	vector2 normalizeFast()
	{
		float inv_length;
		float len = lengthFast();
		inv_length = 1.f / len;

		
		x *= inv_length;
		y *= inv_length;
	}
};



template <class TYPE>
class vector3
{
public:
	union
	{
		struct 
		{
			TYPE x,y,z;
		};
		struct 
		{
			TYPE s,t,r;
		};
		TYPE vec_array[3];
	};
	
	
	// default constructor
	vector3(){};
	
	
	vector3(const TYPE* array)
	{
		memcpy(vec_array, array, sizeof(vec_array));
	}
	
	vector3(TYPE x0, TYPE y0, TYPE z0):
	x(x0), y(y0), z(z0){}
	
	vector3(const vector3<TYPE>& other):
	x(other.x), y(other.y), z(other.z){}
	
	void set(TYPE x0, TYPE y0, TYPE z0)
	{
		x = x0;
		y = y0;
		z = z0;
	}
	
	
	// assignment
	vector3<TYPE>& operator=(const vector3<TYPE>& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
	
	// addition with assignment
	vector3<TYPE>& operator+=(const vector3<TYPE>& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	
	// subtraction with assignment
	vector3<TYPE>& operator-=(const vector3<TYPE>& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	
	// uniform scaling
	vector3<TYPE>& operator*=(const TYPE scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}
	
	// non-uniform scaling
	vector3<TYPE>& operator*=(const vector3<TYPE>& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}
	
	// addition
	vector3<TYPE> operator+(const vector3<TYPE>& other) const
	{
		return vector3<TYPE>(x+other.x, y+other.y, z+other.z);
	}
	
	// subtraction
	vector3<TYPE> operator-(const vector3<TYPE>& other) const
	{
		return vector3<TYPE>(x-other.x, y-other.y, z-other.z);
	}
	
	// non-uniform scaling
	vector3<TYPE> operator*(const vector3<TYPE>& other) const
	{
		return vector3<TYPE>(x*other.x, y*other.y, z*other.z);
	}
	
	// uniform scaling
	vector3<TYPE> operator*(const TYPE scale) const
	{
		return vector3<TYPE>(x*scale, y*scale, z*scale);
	}
	
	friend vector3<TYPE>operator*(const TYPE scale, const vector3<TYPE>& v)
	{
		return vector3<TYPE>(v.x*scale, v.y*scale, v.z*scale);;
	}
	
	// calculate vector length
	TYPE length()const
	{
		return (TYPE)sqrt(x*x + y*y + z*z);
	}
	
	// calculate fast vector length
	TYPE lengthFast()const
	{
		return (TYPE)fastsqrt(x*x + y*y + z*z);
	}
	
	// calculate length2
	TYPE length2()const
	{
		return (TYPE)(x*x + y*y + z*z);
	}
	
	vector3& normalize()
	{
		TYPE inv_length = 1/length();
		
		x *= inv_length;
		y *= inv_length;
		z *= inv_length;
		return *this;
	}
	
	// normalize vector length
	vector3 normalizeFast()
	{
		float inv_length;
		float len = lengthFast();
		
		inv_length = 1.f / len;
		
		x *= inv_length;
		y *= inv_length;
		z *= inv_length;
	}
};

// calculate dot3 product between 2 vectors
template <class TYPE>
TYPE dot3(const vector3<TYPE>& a, const vector3<TYPE>& b)
{
	return (TYPE)(a.x*b.x + a.y*b.y + a.z*b.z);
}

// calculate cross product between 2 vectors
template <class TYPE>
vector3<TYPE>& cross(vector3<TYPE>& result, const vector3<TYPE>& a, const vector3<TYPE>& b)
{
/*
*	i	j	k
*	ax	ay	az
*	bx	by	bz
	*/
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return result;
}

// compute the reflected vector R of L w.r.t N - vectors need to be 
// normalized
//
//                R     N     L
//                  _       _
//                 |\   ^   /|
//                   \  |  /
//                    \ | /
//                     \|/
//                      +
template <class TYPE>
vector3<TYPE>& reflect(vector3<TYPE>& r, const vector3<TYPE>& n, const vector3<TYPE>& l)
{
	r = n*dot3(n,l);
	r += r - l;
	return r;
}


template <class TYPE>
class vector4
{
public:
	union
	{
		struct 
		{
			TYPE x,y,z,w;
		};
		struct 
		{
			TYPE s,t,r,q;
		};
		struct
		{
			TYPE red, green, blue, alpha;
		};
		TYPE vec_array[4];
	};
	
	
	// default constructor
	vector4(){};
	
	// constructor
	vector4(const TYPE* array)
	{
		set(array);
	}
	
	vector4(TYPE x0, TYPE y0, TYPE z0, TYPE w0):
	x(x0), y(y0), z(z0), w(w0){}
	
	vector4(const vector4<TYPE>& other):
	x(other.x), y(other.y), z(other.z), w(other.w){}
	
	vector4<TYPE>& set(const TYPE* array)
	{
		memcpy(vec_array, array, sizeof(vec_array));
		return *this;
	}
	
	vector4<TYPE>& set(TYPE x0, TYPE y0, TYPE z0, TYPE w0)
	{
		x = x0;
		y = y0;
		z = z0;
		w = w0;
		return *this;
	}
	
	// assignment
	vector4<TYPE>& operator=(const vector4<TYPE>& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}
	
	// addition with assignment
	vector4<TYPE>& operator+=(const vector4<TYPE>& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}
	
	// subtraction with assignment
	vector4<TYPE>& operator-=(const vector4<TYPE>& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}
	
	// uniform scaling
	vector4<TYPE>& operator*=(const TYPE scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return *this;
	}
	
	// non-uniform scaling
	vector4<TYPE>& operator*=(const vector4<TYPE>& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}
	
	// equivalence
	bool operator == (const vector4<TYPE> & other) const
	{
		return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
	}
	
	// difference
	bool operator != (const vector4<TYPE> & other) const
	{
		return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
	}
	
	// addition
	vector4<TYPE> operator+(const vector4<TYPE>& other) const
	{
		return vector4<TYPE>(x+other.x, y+other.y, z+other.z, w+other.w);
	}
	
	// subtraction
	vector4<TYPE> operator-(const vector4<TYPE>& other) const
	{
		return vector4<TYPE>(x-other.x, y-other.y, z-other.z, w-other.w);
	}
	
	// non-uniform scaling
	vector4<TYPE> operator*(const vector4<TYPE>& other) const
	{
		return vector4<TYPE>(x*other.x, y*other.y, z*other.z, z*other.w);
	}
	
	// uniform scaling
	vector4<TYPE> operator*(const TYPE scale) const
	{
		return vector4<TYPE>(x*scale, y*scale, z*scale, w*scale);
	}
	
	TYPE& operator[](unsigned int index)
	{
		return vec_array[index];
	}
	
	const TYPE& operator[](unsigned int index)const
	{
		return vec_array[index];
	}
	
	
	// set all coords to zero
	vector4<TYPE>& setZero()
	{
		static const TYPE v[] = {0,0,0,0};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set all coords to one
	vector4<TYPE>& setOne()
	{
		static const TYPE v[] = {1,1,1,1};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set x axis
	vector4<TYPE>& setAxisX()
	{
		static const TYPE v[] = {1,0,0,0};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set y axis
	vector4<TYPE>& setAxisY()
	{
		static const TYPE v[] = {0,1,0,0};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set z axis
	vector4<TYPE>& setAxisZ()
	{
		static const TYPE v[] = {0,0,1,0};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set w axis
	vector4<TYPE>& setAxisW()
	{
		static const TYPE v[] = {0,0,0,1};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set point (1,0,0)
	vector4<TYPE>& setPointX()
	{
		static const TYPE v[] = {1,0,0,1};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set point (0,1,0)
	vector4<TYPE>& setPointY()
	{
		static const TYPE v[] = {0,1,0,1};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// set point (0,0,1)
	vector4<TYPE>& setPointZ()
	{
		static const TYPE v[] = {0,0,1,1};
		memcpy(vec_array, v, sizeof(vec_array));
		return *this;
	}
	
	// multiply scalar*vector4 operator
	friend vector4<TYPE>operator*(const TYPE scale, const vector4<TYPE>& v)
	{
		return vector4<TYPE>(v.x*scale, v.y*scale, v.z*scale, v.w*scale);
	}
	
	// calculate vector length
	TYPE length()const
	{
		return (TYPE)sqrt(x*x + y*y + z*z + w*w);
	}
	
	// calculate fast vector length
	TYPE lengthFast()const
	{
		return (TYPE)fastsqrt(x*x + y*y + z*z + w*w);
	}
	
	// calculate length2
	TYPE length2()const
	{
		return (TYPE)(x*x + y*y + z*z, + w*w);
	}
	
	// normalize vector length
	vector4<TYPE>& normalizeFast()
	{
		float inv_length;
		float len = lengthFast();
		inv_length = 1.f / len;

		
		x *= inv_length;
		y *= inv_length;
		z *= inv_length;
		w *= inv_length;
		
		return *this;
	}
	
	vector4<TYPE>& blend(const vector4<TYPE>& v1, const vector4<TYPE>& v2, TYPE k1, TYPE k2)
	{
		return (*this) = v1 * k1 + v2 * k2;
	}
};


template <class TYPE>
class matrix3
{
public:
	matrix3(){};
	matrix3(const TYPE* array)
	{
		memcpy(mat_array, array, sizeof(mat_array));
	};
	
	matrix3(const matrix3<TYPE>& m)
	{
		memcpy(mat_array, m.mat_array, sizeof(mat_array));
	}
	
	void setColumn(int i, const vector3<TYPE>& v)
	{
		mat_array[i] = v.x;
		mat_array[i+3] = v.y;
		mat_array[i+6] = v.z;
	}
	
	void setRow(int i, const vector3<TYPE>& v)
	{
		mat_array[i*3 + 0] = v.x;
		mat_array[i*3 + 1] = v.y;
		mat_array[i*3 + 2] = v.z;
	}

	vector3<TYPE>& getRow(int i, vector3<TYPE>& v)const
	{
		v.x = mat_array[i * 3 + 0];
		v.y = mat_array[i * 3 + 1];
		v.z = mat_array[i * 3 + 2];
		return v;
	}
	
	matrix3<TYPE>& loadIdentity()
	{
		static const TYPE array[] = 
		{
			1,	0,	0,
				0,	1,	0,
				0,	0,	1
		};
		memcpy(mat_array, array, sizeof(mat_array));
		return *this;
	}
	
	
	
	union 
	{
		struct 
		{
			TYPE a00, a10, a20;
			TYPE a01, a11, a21;
			TYPE a02, a12, a22;
		};
		TYPE mat_array[9];
	};
};


template <class TYPE>
class matrix4
{
public:
	matrix4(){};
	matrix4(const TYPE* array)
	{
		memcpy(mat_array, array, sizeof(mat_array));
	};
	
	matrix4(const matrix4<TYPE>& m)
	{
		memcpy(mat_array, m.mat_array, sizeof(mat_array));
	}
	
	matrix4& setColumn(int i, const vector4<TYPE>& v)
	{
		mat_array[i+0]	= v.x;
		mat_array[i+4]	= v.y;
		mat_array[i+8]	= v.z;
		mat_array[i+12]	= v.w;
		return *this;
	}
	
	matrix4& setColumn(int i, const vector3<TYPE>& v)
	{
		mat_array[i+0]	= v.x;
		mat_array[i+4]	= v.y;
		mat_array[i+8]	= v.z;
		return *this;
	}
	
	matrix4& setRow(int i, const vector4<TYPE>& v)
	{
		int pos = i * 4;
		mat_array[pos + 0] = v.x;
		mat_array[pos + 1] = v.y;
		mat_array[pos + 2] = v.z;
		mat_array[pos + 4] = v.w;
		return *this;
	}

	matrix4& setRow(int i, const vector3<TYPE>& v)
	{
		int pos = i * 4;
		mat_array[pos + 0] = v.x;
		mat_array[pos + 1] = v.y;
		mat_array[pos + 2] = v.z;
		return *this;
	}
	
	vector4<TYPE>& getRow(int i, vector4<TYPE>& v)const
	{
		int pos = i * 4;
		v.x = mat_array[pos + 0];
		v.y = mat_array[pos + 1];
		v.z = mat_array[pos + 2];
		v.w = mat_array[pos + 4];
		return v;
	}
	
	vector3<TYPE>& getRow(int i, vector3<TYPE>& v)const
	{
		int pos = i * 4;
		v.x = mat_array[pos + 0];
		v.y = mat_array[pos + 1];
		v.z = mat_array[pos + 2];
		return v;
	}
	
	matrix4& clear()
	{
		static const TYPE array[] = 
		{
			0,	0,	0,	0,
				0,	0,	0,	0,
				0,	0,	0,	0,
				0,	0,	0,	0
		};
		memcpy(mat_array, array, sizeof(mat_array));
		return *this;
	}
	
	matrix4& loadIdentity()
	{
		static const TYPE array[] = 
		{
			1,	0,	0,	0,
				0,	1,	0,	0,
				0,	0,	1,	0,
				0,	0,	0,	1
		};
		memcpy(mat_array, array, sizeof(mat_array));
		return *this;
	}
	
	matrix4& loadPerspectiveFovLH(TYPE fovY, TYPE aspect, TYPE zNear, TYPE zFar)
	{
		TYPE top = (TYPE)(zNear * tan(fovY * (TYPE)0.5));
		TYPE bott = -top;
		TYPE right = top * aspect;
		TYPE left = -right;
		
		clear();
		
		TYPE znzn = zNear + zNear;
		TYPE inv_w = (TYPE)1 / (right - left);
		TYPE inv_h = (TYPE)1 / (top - bott);
		TYPE inv_depth = (TYPE)1 / (zFar - zNear);
		
		a00 = znzn * inv_w;
		a11 = znzn * inv_h;
		a22 = (zFar + zNear) * inv_depth;
		a23 = znzn * zFar * inv_depth;
		a32 = 1;
		
		
		return *this;
	}
	
	matrix4& loadPerspectiveFovRH(TYPE fovY, TYPE aspect, TYPE zNear, TYPE zFar)
	{
		TYPE top = (TYPE)(zNear * tan(fovY * (TYPE)0.5));
		TYPE bott = -top;
		TYPE right = top * aspect;
		TYPE left = -right;
		
		clear();
		
		TYPE znzn = zNear + zNear;
		TYPE inv_w = (TYPE)1 / (right - left);
		TYPE inv_h = (TYPE)1 / (top - bott);
		TYPE inv_depth = (TYPE)1 / (zFar - zNear);
		
		a00 = znzn * inv_w;
		a11 = znzn * inv_h;
		a22 = -(zFar + zNear) * inv_depth;
		a23 = -znzn * zFar * inv_depth;
		a32 = -1;
		
		
		/*
		TYPE half = fovY * (TYPE)0.5;
		TYPE h = (float)(cos(half) / sin (half));
		TYPE w = h * aspect;
		
		  TYPE dist_1 = 1 / (zNear - zFar);
		  
			clear();
			
			  TYPE znzn = zNear + zNear;
			  
				a00 = znzn / w;
				a11 = znzn / h;
				a22 = zFar * dist_1;
				a23 = -1;
				a32 = znzn * zFar * dist_1;
				
		*/
		return *this;
	}
	
	matrix4& loadTranslation(TYPE dx, TYPE dy, TYPE dz)
	{
		loadIdentity();
		x = dx;
		y = dy;
		z = dz;
		w = 1;
		return *this;
	}
	
	matrix4& clearRow(int i)
	{
		int pos = i*4;
		mat_array[pos + 0] = 0;
		mat_array[pos + 1] = 0;
		mat_array[pos + 2] = 0;
		mat_array[pos + 3] = 0;
		
		return *this;
	}
	
	matrix4& clearColumn(int i)
	{
		mat_array[i + 0] = 0;
		mat_array[i + 4] = 0;
		mat_array[i + 8] = 0;
		mat_array[i + 12] = 0;
		
		return *this;
	}
	
	matrix4& loadLookAtRH(TYPE eyeX, TYPE eyeY, TYPE eyeZ, TYPE atX, TYPE atY, TYPE atZ, TYPE upX, TYPE upY, TYPE upZ)
	{
		vector3<TYPE> eye(eyeX, eyeY, eyeZ);
		vector3<TYPE> at(atX, atY, atZ);
		vector3<TYPE> up(upX, upY, upZ);
		
		vector3<TYPE> zaxis = (eye - at).normalize();
		vector3<TYPE> xaxis;
		vector3<TYPE> yaxis;
		cross(xaxis, up, zaxis).normalize();
		cross(yaxis, zaxis, xaxis);
		
		
		setColumn(0, xaxis);
		setColumn(1, yaxis);
		setColumn(2, zaxis);
		clearColumn(3);
		x = -dot3(xaxis, eye);
		y = -dot3(yaxis, eye);
		z = -dot3(zaxis, eye);
		w = 1;
		
		
		return *this;
	}
	
	/*
	*	Fix needed. Don't use
	*/
	matrix4& loadLookAtLH(TYPE eyeX, TYPE eyeY, TYPE eyeZ, TYPE atX, TYPE atY, TYPE atZ, TYPE upX, TYPE upY, TYPE upZ)
	{
		vector3<TYPE> eye(eyeX, eyeY, eyeZ);
		vector3<TYPE> at(atX, atY, atZ);
		vector3<TYPE> up(upX, upY, upZ);
		
		vector3<TYPE> zaxis = (at - eye).normalize();
		vector3<TYPE> xaxis;
		vector3<TYPE> yaxis;
		cross(xaxis, up, zaxis).normalize();
		cross(yaxis, zaxis, xaxis);
		
		
		setRow(0, xaxis);
		setRow(1, yaxis);
		setRow(2, zaxis);
		clearRow(3);
		x = -dot3(xaxis, eye);
		y = -dot3(yaxis, eye);
		z = -dot3(zaxis, eye);
		w = 1;
		
		
		return *this;
	}
	
	
	matrix4& loadRotation(TYPE angle, TYPE ux, TYPE uy, TYPE uz)
	{
		TYPE inv_len = (TYPE)(1.0 / sqrt(ux * ux + uy * uy + uz * uz));
		ux *= inv_len;
		uy *= inv_len;
		uz *= inv_len;
		
		TYPE c = (TYPE)cos(-angle);
		TYPE s = (TYPE)sin(-angle);
		TYPE one_minus_c = (TYPE)1 - c;
		
		TYPE uxuy = ux * uy;
		TYPE uxuz = ux * uz;
		TYPE uyuz = uy * uz;
		
		TYPE sux = s * ux;
		TYPE suy = s * uy;
		TYPE suz = s * uz;
		
		a00 = c + one_minus_c * ux * ux;
		a10 = one_minus_c * uxuy + suz;
		a20 = one_minus_c * uxuz - suy;
		
		a01 = one_minus_c * uxuy - suz;
		a11 = c + one_minus_c * uy * uy;
		a21 = one_minus_c * uyuz + sux;
		
		a02 = one_minus_c * uxuz + suy;
		a12 = one_minus_c * uyuz - sux;
		a22 = c + one_minus_c * uz * uz;
		
		a30 = a31 = a32 = a03 = a13 = a23 = 0;
		a33 = 1;
		
		return *this;
	}
	
	TYPE& operator[](unsigned int index)
	{
		return mat_array[index];
	}
	
	const TYPE& operator[](unsigned int index)const
	{
		return mat_array[index];
	}
	
	union 
	{
		struct 
		{
			TYPE a00, a10, a20;
			union
			{
				TYPE a30;
				TYPE p;
			};
			TYPE a01, a11, a21;
			union
			{
				TYPE a31;
				TYPE q;
			};
			TYPE a02, a12, a22;
			union
			{
				TYPE a32;
				TYPE r;
			};
			
			union
			{
				TYPE a03;
				TYPE x;
			};
			union
			{
				TYPE a13;
				TYPE y;
			};
			union
			{
				TYPE a23;
				TYPE z;
			};
			union
			{
				TYPE a33;
				TYPE w;
			};
			
		};
		
		TYPE mat_array[16];
	};
};

template <class TYPE>
matrix4<TYPE>& mult(matrix4<TYPE> &result, const matrix4<TYPE> &mat1, const matrix4<TYPE> &mat2)
{
	result.a00 = mat1.a00*mat2.a00 + mat1.a01*mat2.a10 + mat1.a02*mat2.a20 + mat1.a03*mat2.a30;
	result.a10 = mat1.a10*mat2.a00 + mat1.a11*mat2.a10 + mat1.a12*mat2.a20 + mat1.a13*mat2.a30;
	result.a20 = mat1.a20*mat2.a00 + mat1.a21*mat2.a10 + mat1.a22*mat2.a20 + mat1.a23*mat2.a30;
	result.a30 = mat1.a30*mat2.a00 + mat1.a31*mat2.a10 + mat1.a32*mat2.a20 + mat1.a33*mat2.a30;
	
	result.a01 = mat1.a00*mat2.a01 + mat1.a01*mat2.a11 + mat1.a02*mat2.a21 + mat1.a03*mat2.a31;
	result.a11 = mat1.a10*mat2.a01 + mat1.a11*mat2.a11 + mat1.a12*mat2.a21 + mat1.a13*mat2.a31;
	result.a21 = mat1.a20*mat2.a01 + mat1.a21*mat2.a11 + mat1.a22*mat2.a21 + mat1.a23*mat2.a31;
	result.a31 = mat1.a30*mat2.a01 + mat1.a31*mat2.a11 + mat1.a32*mat2.a21 + mat1.a33*mat2.a31;
	
	result.a02 = mat1.a00*mat2.a02 + mat1.a01*mat2.a12 + mat1.a02*mat2.a22 + mat1.a03*mat2.a32;
	result.a12 = mat1.a10*mat2.a02 + mat1.a11*mat2.a12 + mat1.a12*mat2.a22 + mat1.a13*mat2.a32;
	result.a22 = mat1.a20*mat2.a02 + mat1.a21*mat2.a12 + mat1.a22*mat2.a22 + mat1.a23*mat2.a32;
	result.a32 = mat1.a30*mat2.a02 + mat1.a31*mat2.a12 + mat1.a32*mat2.a22 + mat1.a33*mat2.a32;
	
	result.a03 = mat1.a00*mat2.a03 + mat1.a01*mat2.a13 + mat1.a02*mat2.a23 + mat1.a03*mat2.a33;
	result.a13 = mat1.a10*mat2.a03 + mat1.a11*mat2.a13 + mat1.a12*mat2.a23 + mat1.a13*mat2.a33;
	result.a23 = mat1.a20*mat2.a03 + mat1.a21*mat2.a13 + mat1.a22*mat2.a23 + mat1.a23*mat2.a33;
	result.a33 = mat1.a30*mat2.a03 + mat1.a31*mat2.a13 + mat1.a32*mat2.a23 + mat1.a33*mat2.a33;
	
	return result;
}

typedef matrix4<float>	floatMatrix4;
typedef matrix4<double>	doubleMatrix4;
typedef vector3<float>	floatVector3;
typedef vector3<double>	doubleVector3;
typedef vector4<float>	floatVector4;
typedef vector4<double>	doubleVector4;

#endif // !defined(AFX_MATH_H__B85D40A6_A4D9_45D7_8E67_29EE897C2F18__INCLUDED_)
