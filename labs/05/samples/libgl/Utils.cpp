//////////////////////////////////////////////////////////////////////
#include "pch.h"

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
	/*
	fseek(pFile, 0, SEEK_END);
	long fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	*/

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
