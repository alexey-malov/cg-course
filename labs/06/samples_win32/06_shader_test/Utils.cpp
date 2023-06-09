//////////////////////////////////////////////////////////////////////

#include "../libgl/libgl.h"

#include "stdafx.h"
#include "Utils.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// вычисляем нормаль к треугольнику
void CalculateTriangleNormal(Vector3d const& a, Vector3d const& b, Vector3d const& c, Vector3d *normal)
{
	/*
	      A
	     / \
	    /   \
	   /     \
	  /       \
	 /         \
	B-----------C
	Нормаль к треугольнику ABC - векторное произведение BC(x1, y1, z1) x BA(x2, y2, z2):
	|i  j  k |   |y1*z2 - y2*z1|
	|x1 y1 z1| = |z1*x2 - z2*x1|
	|x2 y2 z2|   |x1*y2 - x2*y1|
	где:
		x1 = (c-b).x
		y1 = (c-b).y
		z1 = (c-b).z
		x2 = (a-b).x
		y2 = (a-b).y
		z2 = (a-b).z
	*/
	// вычисляем координаты вектора BC
	Vector3d bc = c - b;
	// вычисляем координаты вектора BA
	Vector3d ba = a - b;
	// вычисляем их векторное произведение - нормаль к треугольнику
	normal->CrossProduct(bc, ba);
	// нормируем результат, т.к. вектор нормали должен быть единичной длины
	normal->Normalize();
}

bool IsPowerOfTwo(int n)
{
	return (n & (n-1)) == 0;
}

GLuint LoadTextureFromRgbBitmapFile(const char fileName[])
{
	FILE* pFile;
	fopen_s(&pFile, fileName, "rb");
	if (!pFile)
	{
		return 0;
	}

	BITMAPFILEHEADER bfh;
	if (!fread(&bfh, sizeof(bfh), 1, pFile))
	{
		fclose(pFile);
		return 0;
	}

	BITMAPINFOHEADER bih;
	if (!fread(&bih, sizeof(bih), 1, pFile))
	{
		fclose(pFile);
		return 0;
	}

	int height = bih.biHeight > 0 ? bih.biHeight : -bih.biHeight;
	int width = bih.biWidth;

	if (
		(bfh.bfType != 'MB') ||				// в начале bmp-файла идут буквы BM
		(bih.biCompression != BI_RGB) ||	// обрабатываем только RGB-значения
		((bih.biBitCount != 24) && (bih.biBitCount != 32)) ||	// только 24 или 32 бита на пиксель
		(!IsPowerOfTwo(width)) ||		// ширина должна быть степенью двойки
		(!IsPowerOfTwo(height))		// высота должна быть степенью двойки
		)
	{
		fclose(pFile);
		return 0;
	}

	int numberOfComponents = bih.biBitCount / 8;
	int linePitch = bih.biWidth * numberOfComponents;
	int bufferSize = bih.biHeight * linePitch;
	BYTE *pBuffer = new BYTE [bufferSize];
	fseek(pFile, bfh.bfOffBits, SEEK_SET);

	for (int y = 0; y < height; ++y)
	{
		if (bih.biHeight < 0)
		{
			if (!fread(pBuffer + linePitch * y, linePitch, 1, pFile))
			{
				delete [] pBuffer;
				fclose(pFile);
				return 0;
			}
		}
		else
		{
			if (!fread(pBuffer + linePitch * (height - 1 - y), linePitch, 1, pFile))
			{
				delete [] pBuffer;
				fclose(pFile);
				return 0;
			}
		}
	}

	fclose(pFile);


	
	GLuint textureName = 0;
	glGenTextures(1, &textureName);
	glBindTexture(GL_TEXTURE_2D, textureName);
	gluBuild2DMipmaps(GL_TEXTURE_2D, numberOfComponents, width, height, numberOfComponents == 3 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, pBuffer);
	delete [] pBuffer;
	return textureName;
}

void InverseTransposeMatrix(const float *mat, float *rv)
{
	rv[0] = mat[0];
	rv[1] = mat[4];
	rv[2] = mat[8];
	rv[3] = mat[3];
	rv[4] = mat[1];
	rv[5] = mat[5];
	rv[6] = mat[9];
	rv[7] = mat[7];
	rv[8] = mat[2];
	rv[9] = mat[6];
	rv[10] = mat[10];
	rv[11] = mat[11];
	
	rv[12] = -(mat[12]*rv[0] + mat[13]*rv[4] + mat[14]*rv[8]); //-mat[12] - (mat[0] * mat[0]  );
	rv[13] = -(mat[12]*rv[1] + mat[13]*rv[5] + mat[14]*rv[9]);	//-mat[13];
	rv[14] = -(mat[12]*rv[2] + mat[13]*rv[6] + mat[14]*rv[10]);	//-mat[14];
	
	rv[15] = mat[15];
	
}


Vector3d TransformVector(const float *mat, Vector3d const & vec)
{
	return Vector3d(
		mat[0] * vec.x + mat[4] * vec.y + mat[8] * vec.z,
		mat[1] * vec.x + mat[5] * vec.y + mat[9] * vec.z,
		mat[2] * vec.x + mat[6] * vec.y + mat[10] * vec.z
		);
}

Vector3d TransformPoint(const float *mat, Vector3d const & pnt)
{
	return Vector3d(
		mat[0] * pnt.x + mat[4] * pnt.y + mat[8] * pnt.z + mat[12],
		mat[1] * pnt.x + mat[5] * pnt.y + mat[9] * pnt.z + mat[13],
		mat[2] * pnt.x + mat[6] * pnt.y + mat[10] * pnt.z + mat[14]
		);
}

