#include "StdAfx.h"
#include "RotationController.h"
#include "Vector3.h"

CRotationController::CRotationController(int windowWidth, int windowHeight)
:m_leftButtonPressed(false)
,m_windowWidth(windowWidth)
,m_windowHeight(windowHeight)
,m_mouseX(0)
,m_mouseY(0)
{
}

CRotationController::~CRotationController(void)
{
}

void CRotationController::ResizeWindow(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
}

void CRotationController::OnMouse(int button, int state, int x, int y)
{
	// —обытие от левой кнопки мыши
	if (button == GLUT_LEFT_BUTTON)
	{
		// —охран€ем состо€ние левой кнопки мыши
		m_leftButtonPressed = (state == GLUT_DOWN);
		// —охран€ем координаты мыши
		m_mouseX = x;
		m_mouseY = y;
	}
}

void CRotationController::OnMotion(int x, int y)
{
	// ≈сли нажата лева€ кнопка мыши
	if (m_leftButtonPressed)
	{
		// ¬ычисл€ем смещение курсора мыши
		int dx = x - m_mouseX;
		int dy = y - m_mouseY;

		// ¬ычисл€ем угол поворота вокруг осей Y и X как линейно завис€щие
		// от смещени€ мыши по ос€м X и Y
		GLfloat rotateX = GLfloat(dy) * 180 / m_windowHeight;
		GLfloat rotateY = GLfloat(dx) * 180 / m_windowWidth;

		RotateCamera(rotateX, rotateY);

		// —охран€ем текущие координаты мыши
		m_mouseX = x;
		m_mouseY = y;
	}
}


// ¬ращаем камеру вокруг начала кординат на заданный угол
void CRotationController::RotateCamera(GLfloat rotateX, GLfloat rotateY)
{
	// »звлекаем текущее значение матрицы моделировани€-вида
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, &modelView[0]);

	// »звлекаем направлени€ координатных осей камеры в 3д пространстве
	// как коэффициенты строк матрицы моделировани€-вида
	CVector3f xAxis(modelView[0], modelView[4], modelView[8]);
	CVector3f yAxis(modelView[1], modelView[5], modelView[9]);

	// ѕоворачиваем вокруг осей x и y камеры
	glRotatef(rotateX, xAxis.x, xAxis.y, xAxis.z);
	glRotatef(rotateY, yAxis.x, yAxis.y, yAxis.z);

	// ¬ ходе умножени€ матриц могут возникать погрешности, которые,
	// накаплива€сь могут сильно искажать картинку
	// ƒл€ их компенсации после каждой модификации матрицы моделировани€-вида
	// проводим ее ортонормирование
	NormalizeModelViewMatrix();
}

// ѕроизводим ортонормирование матрицы моделировани€-вида
void CRotationController::NormalizeModelViewMatrix(void)
{
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	/*
	ќртонормирование - приведение координатных осей к единичной длине (нормирование)
	и взаимной перпендикул€рности (ортогонализаци€)
	ƒостичь этого можно при помощи нормализации координатных осей
	и векторного произведени€
	*/
	CVector3f xAxis(modelView[0], modelView[4], modelView[8]);
	xAxis.Normalize();	
	CVector3f yAxis(modelView[1], modelView[5], modelView[9]);
	yAxis.Normalize();

	// ќсь Z вычисл€ем через векторное произведение X и Y
	// Z будет перпендикул€рна плоскости векторов X и Y
	CVector3f zAxis = Cross(xAxis, yAxis);
	// » иметь единичную длину
	zAxis.Normalize();
	// “о же самое проделываем с ос€ми x и y
	xAxis = Cross(yAxis, zAxis);
	xAxis.Normalize();
	yAxis = Cross(zAxis, xAxis);
	yAxis.Normalize();

	// —охран€ем вектора координатных осей обратно в массив
	modelView[0] = xAxis.x; modelView[4] = xAxis.y, modelView[8] = xAxis.z;
	modelView[1] = yAxis.x; modelView[5] = yAxis.y, modelView[9] = yAxis.z;
	modelView[2] = zAxis.x; modelView[6] = zAxis.y, modelView[10] = zAxis.z;

	// » загружаем матрицу моделирвани€-вида
	glLoadMatrixf(modelView);
}

bool CRotationController::LeftButtonIsPressed()const
{
	return m_leftButtonPressed;
}