#include "StdAfx.h"
#include "Flower.h"

CFlower::CFlower(float minRadius, float maxRadius, int petals, COLORREF centerColor, COLORREF petalColor)
:m_minRadius(minRadius)
,m_maxRadius(maxRadius)
,m_petals(petals)
,m_centerColor(centerColor)
,m_petalColor(petalColor)
,m_displayList(0)
{
}

CFlower::CFlower(CFlower const& f)
:m_minRadius(f.m_minRadius)
,m_maxRadius(f.m_maxRadius)
,m_petals(f.m_petals)
,m_centerColor(f.m_centerColor)
,m_petalColor(f.m_petalColor)
,m_displayList(0)	// Дисплейный список у копии будет создан свой
{
}

CFlower::~CFlower(void)
{
	if (m_displayList)
	{
		glDeleteLists(m_displayList, 1);
		m_displayList = 0;
	}
}

void CFlower::Draw()const
{
	// если дисплейный список еще не был создан, то для начала создаем его
	if (!m_displayList)
	{
		// Генерируем новый дисплейный список для цветка
		m_displayList = glGenLists(1);

		{
			glNewList(m_displayList, GL_COMPILE);

			static const float M_PI = 3.14159265358979323846f;

			// угловой шаг (для рисования каждого лепестка 
			// нам понадобится как минимум 20 вершин)
			const float step = 2 * M_PI / (m_petals * 20);

			glBegin(GL_TRIANGLE_FAN);

			// Центральная вершина будет иметь цвет centerColor
			// суффикс 3ub означает, что функция принимает 3 параметра типа unsigned byte
			glColor3ub(GetRValue(m_centerColor), GetGValue(m_centerColor), GetBValue(m_centerColor));
			glVertex2f(0, 0);

			// вершины лепестка будут иметь цвет petalColor
			glColor3ub(GetRValue(m_petalColor), GetGValue(m_petalColor), GetBValue(m_petalColor));

			for (float angle = 0; angle <= 2 * M_PI + step / 2; angle += step)
			{
				// в дискретном мире компьютеров sin(2 * PI) может отличаться от sin(0)
				// поэтому если angle подошел близко к 2*PI, считаем его равным 0
				// это нужно для того, чтобы начало и конец цветка точно сошлись
				float a = (fabsf(angle - 2 * M_PI) < 1e-4) ? 0 : angle;

				// вычисляем радиус искривленной окружности для данного угла
				float radius = 0.5f * (m_maxRadius - m_minRadius) * cosf(a * m_petals) + m_minRadius;
				float x = radius * cosf(a);
				float y = radius * sinf(a);

				// создаем новую вершину
				glVertex2f(x, y);
			}

			glEnd();
		}
		// завершаем дисплейный список
		glEndList();
	}

	// вызываем ранее созданный дисплейный список
	glCallList(m_displayList);
}
