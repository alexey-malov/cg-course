// MapTile.h: interface for the MapTile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPTILE_H__08423551_D9BE_4B22_A4F9_0F5B7E09EDFA__INCLUDED_)
#define AFX_MAPTILE_H__08423551_D9BE_4B22_A4F9_0F5B7E09EDFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TILE_SIZE 64
#define VERTEX_TILE_SIZE (TILE_SIZE + 1)

#include "../libgl/Graphics.h"
#include <cstring>

struct MapTile
{
	
	// карта высот
	GLfloat heightMap[VERTEX_TILE_SIZE * VERTEX_TILE_SIZE];

	int startX, startY; // координаты внутри карты
	int numRows, numColumns;
	int mapStep;
	float detailStep;

	// номер дисплейного списка для отрисовки тайла
	GLuint displayList;
	GLuint textureName;
	GLuint detailTextureName;

	void Load(int x, int y, int columns, int rows, int step, GLfloat const* mapData,
		GLuint texture = 0, GLuint detailTexture = 0)
	{
		startX = x;
		startY = y;
		numRows = columns;
		numColumns = rows;
		mapStep = step;
		displayList = 0;
		textureName = texture;
		detailTextureName = detailTexture;
		detailStep = 8.0f / TILE_SIZE;
		memcpy(heightMap, mapData, numRows * numColumns * sizeof(GLfloat));
	}

	void Upload()
	{
		displayList = glGenLists(1);

		glNewList(displayList, GL_COMPILE);
		{
			int y0 = startY;
			for (int row = 0; row < numRows - 1; ++row, y0 += mapStep)
			{
				int x0 = startX;
				glBegin(GL_TRIANGLE_STRIP);
				for (int column = 0; column < numColumns; ++column, x0 += mapStep)
				{
					glVertex3f((float)x0, (float)(y0 + mapStep), heightMap[(row + 1) * numColumns + column]);
					glVertex3f((float)x0, (float)y0, heightMap[row * numColumns + column]);
				}
				glEnd();
			}
		}
		glEndList();
	}

	void UploadWithTexture()
	{
		displayList = glGenLists(1);

		glNewList(displayList, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, textureName);
		{
			int y0 = startY;
			float du = 1.0f / TILE_SIZE;
			float dv = 1.0f / TILE_SIZE;
			float v = 0;
			for (int row = 0; row < numRows - 1; ++row, y0 += mapStep, v += dv)
			{
				float u = 0;
				int x0 = startX;
				glBegin(GL_TRIANGLE_STRIP);
				for (int column = 0; column < numColumns; ++column, x0 += mapStep, u += du)
				{
					glTexCoord2f(v + dv, u);
					glVertex3f((float)x0, (float)(y0 + mapStep), heightMap[(row + 1) * numColumns + column]);

					glTexCoord2f(v, u);
					glVertex3f((float)x0, (float)y0, heightMap[row * numColumns + column]);
				}
				glEnd();
			}
		}
		glEndList();
	}

	void UploadWithTextureAndDetail()
	{
		displayList = glGenLists(1);

		glNewList(displayList, GL_COMPILE);

		if (multiTextureSupported)
		{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, detailTextureName);
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
		glBindTexture(GL_TEXTURE_2D, textureName);
		{
			int y0 = startY;
			float du = 1.0f / TILE_SIZE;
			float dv = 1.0f / TILE_SIZE;
			float v = 0;
			float v1 = 0;
			for (int row = 0; row < numRows - 1; ++row, y0 += mapStep, v += dv, v1 += detailStep)
			{
				float u = 0;
				float u1 = 0;
				int x0 = startX;
				glBegin(GL_TRIANGLE_STRIP);
				if (multiTextureSupported)
				{
					for (int column = 0; column < numColumns; ++column, x0 += mapStep, u += du, u1 += detailStep)
					{
						glMultiTexCoord2f(GL_TEXTURE0_ARB, v + dv, u);
						glMultiTexCoord2f(GL_TEXTURE1_ARB, v1 + detailStep, u1);
						glVertex3f((float)x0, (float)(y0 + mapStep), heightMap[(row + 1) * numColumns + column]);

						glMultiTexCoord2f(GL_TEXTURE0_ARB, v, u);
						glMultiTexCoord2f(GL_TEXTURE1_ARB, v1, u1);
						glVertex3f((float)x0, (float)y0, heightMap[row * numColumns + column]);
					}
				}
				else
				{
					for (int column = 0; column < numColumns; ++column, x0 += mapStep, u += du)
					{
						glTexCoord2f(v + dv, u);
						glVertex3f((float)x0, (float)(y0 + mapStep), heightMap[(row + 1) * numColumns + column]);

						glTexCoord2f(v, u);
						glVertex3f((float)x0, (float)y0, heightMap[row * numColumns + column]);
					}
				}
				glEnd();
			}
		}
		glEndList();
	}

	void Draw()
	{
		if (!displayList)
		{
			if (textureName == 0)
				Upload();
			else if (detailTextureName == 0)
				UploadWithTexture();
			else
				UploadWithTextureAndDetail();
		}
		glCallList(displayList);
	}

	void Destroy()
	{
		if (displayList != 0)
		{
			glDeleteLists(displayList, 1);
		}
	}
};

#endif // !defined(AFX_MAPTILE_H__08423551_D9BE_4B22_A4F9_0F5B7E09EDFA__INCLUDED_)
