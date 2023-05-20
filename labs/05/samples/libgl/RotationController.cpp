#include "pch.h"
#include "RotationController.h"
#include "Vector3.h"

CRotationController::CRotationController(int windowWidth, int windowHeight)
	: m_leftButtonPressed(false)
	, m_windowWidth(windowWidth)
	, m_windowHeight(windowHeight)
{
}

void CRotationController::ResizeWindow(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
}

void CRotationController::OnMouse(int button, int state, int x, int y)
{
	// Событие от левой кнопки мыши
	if (button == GLUT_LEFT_BUTTON)
	{
		// Сохраняем состояние левой кнопки мыши
		m_leftButtonPressed = (state == GLUT_DOWN);
		// Сохраняем координаты мыши
		m_mouseX = x;
		m_mouseY = y;
	}
}

void CRotationController::OnMotion(int x, int y)
{
	// Если нажата левая кнопка мыши
	if (m_leftButtonPressed)
	{
		// Вычисляем смещение курсора мыши
		int dx = x - m_mouseX;
		int dy = y - m_mouseY;

		// Вычисляем угол поворота вокруг осей Y и X как линейно зависящие
		// от смещения мыши по осям X и Y
		GLfloat rotateX = GLfloat(dy) * 180 / m_windowHeight;
		GLfloat rotateY = GLfloat(dx) * 180 / m_windowWidth;

		RotateCamera(rotateX, rotateY);

		// Сохраняем текущие координаты мыши
		m_mouseX = x;
		m_mouseY = y;
	}
}

// Вращаем камеру вокруг начала кординат на заданный угол
void CRotationController::RotateCamera(GLfloat rotateX, GLfloat rotateY)
{
	// Извлекаем текущее значение матрицы моделирования-вида
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, &modelView[0]);

	// Извлекаем направления координатных осей камеры в 3д пространстве
	// как коэффициенты строк матрицы моделирования-вида
	CVector3f xAxis(modelView[0], modelView[4], modelView[8]);
	CVector3f yAxis(modelView[1], modelView[5], modelView[9]);

	// Поворачиваем вокруг осей x и y камеры
	glRotatef(rotateX, xAxis.x, xAxis.y, xAxis.z);
	glRotatef(rotateY, yAxis.x, yAxis.y, yAxis.z);

	// В ходе умножения матриц могут возникать погрешности, которые,
	// накапливаясь могут сильно искажать картинку
	// Для их компенсации после каждой модификации матрицы моделирования-вида
	// проводим ее ортонормирование
	NormalizeModelViewMatrix();
}

// Производим ортонормирование матрицы моделирования-вида
void CRotationController::NormalizeModelViewMatrix(void)
{
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	/*
	Ортонормирование - приведение координатных осей к единичной длине (нормирование)
	и взаимной перпендикулярности (ортогонализация)
	Достичь этого можно при помощи нормализации координатных осей
	и векторного произведения
	*/
	CVector3f xAxis(modelView[0], modelView[4], modelView[8]);
	xAxis.Normalize();
	CVector3f yAxis(modelView[1], modelView[5], modelView[9]);
	yAxis.Normalize();

	// Ось Z вычисляем через векторное произведение X и Y
	// Z будет перпендикулярна плоскости векторов X и Y
	CVector3f zAxis = Cross(xAxis, yAxis);
	// И иметь единичную длину
	zAxis.Normalize();
	// То же самое проделываем с осями x и y
	xAxis = Cross(yAxis, zAxis);
	xAxis.Normalize();
	yAxis = Cross(zAxis, xAxis);
	yAxis.Normalize();

	// Сохраняем вектора координатных осей обратно в массив
	modelView[0] = xAxis.x;
	modelView[4] = xAxis.y, modelView[8] = xAxis.z;
	modelView[1] = yAxis.x;
	modelView[5] = yAxis.y, modelView[9] = yAxis.z;
	modelView[2] = zAxis.x;
	modelView[6] = zAxis.y, modelView[10] = zAxis.z;

	// И загружаем матрицу моделирвания-вида
	glLoadMatrixf(modelView);
}

bool CRotationController::LeftButtonIsPressed() const
{
	return m_leftButtonPressed;
}