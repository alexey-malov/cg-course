#include "StdAfx.h"
#include "Sphere.h"

#define M_PI       3.14159265358979323846

CSphere::CSphere(
	float radius, 
	unsigned slices, 
	unsigned stacks)
:m_radius(radius)
,m_slices(slices)
,m_stacks(stacks)
,m_displayList(0)
{
}

/*
Рисуем сферу при помощи последовательности triangle strip-ов
Каждая лента соответствует линии параллелей
*/
void CSphere::DrawSphere()const
{
	// шаг по параллелям
	const float stackStep = static_cast<float>(M_PI) / m_stacks;

	// шаг по меридианам
	const float sliceStep = 2 * static_cast<float>(M_PI) / m_slices;

	// начальный угол по параллелям
	for (unsigned stack = 0; stack < m_stacks; ++stack)
	{
		float stackAngle = static_cast<float>(M_PI) * 0.5f - stack * stackStep;
		float nextStackAngle = stackAngle - stackStep;

		const float stackRadius = m_radius * cosf(stackAngle);
		const float nextStackRadius = m_radius * cosf(nextStackAngle);
		const float z0 = m_radius * sinf(stackAngle);
		const float z1 = m_radius * sinf(nextStackAngle);

		glBegin(GL_TRIANGLE_STRIP);
		// цикл по меридианам
		for (unsigned slice = 0; slice <= m_slices; ++slice)
		{
			// вычисляем угол, текущего меридиана
			float sliceAngle = (slice != m_slices) ? slice * sliceStep : 0;

			// Вычисляем координаты на текущей параллели
			float x0 = stackRadius * cosf(sliceAngle);
			float y0 = stackRadius * sinf(sliceAngle);
			// вычисляем и задаем вектор нормали, текстурные координаты 
			// и положение вершины в пространстве
			glm::vec3 normal0(x0, y0, z0);
			glm::vec3 normal = glm::normalize(normal0);
			glNormal3fv(glm::value_ptr(normal));
			glTexCoord2f(float(slice) / m_slices, float(stack) / m_stacks);
			glVertex3f(x0, y0, z0);

			float x1 = nextStackRadius * cosf(sliceAngle);
			float y1 = nextStackRadius * sinf(sliceAngle);
			glm::vec3 normal1(x1, y1, z1);
			glm::vec3 normalVec1 = glm::normalize(normal1);
			glNormal3fv(glm::value_ptr(normalVec1));
			glTexCoord2f(float(slice) / m_slices, float(stack + 1) / m_stacks);
			glVertex3f(x1, y1, z1);

		}
		glEnd();
	}
}

void CSphere::Draw()const
{
	if (!m_displayList)
	{
		m_displayList = glGenLists(1);
		glNewList(m_displayList, GL_COMPILE);

		DrawSphere();

		glEndList();
	}
	glCallList(m_displayList);
}

CSphere::~CSphere(void)
{
	if (m_displayList)
	{
		glDeleteLists(m_displayList, 1);
	}
}
