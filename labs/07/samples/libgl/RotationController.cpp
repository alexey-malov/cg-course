#include "pch.h"
#include "RotationController.h"
#include "IApplication.h"
#include "IRotationControllerListener.h"
#include "Vector3.h"

CRotationController::CRotationController(int windowWidth, int windowHeight)
	: m_leftButtonPressed(false)
	, m_windowWidth(windowWidth)
	, m_windowHeight(windowHeight)
{
}

void CRotationController::SetModelViewMatrix(CMatrix4d const& matrix)
{
	m_modelViewMatrix = matrix;
}

CMatrix4d const& CRotationController::GetModelViewMatrix() const
{
	return m_modelViewMatrix;
}

void CRotationController::OnReshape(int windowWidth, int windowHeight)
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
	// Поворачиваем вокруг осей x и y камеры
	m_modelViewMatrix.Rotate(rotateX, m_modelViewMatrix[0], m_modelViewMatrix[4], m_modelViewMatrix[8]);
	m_modelViewMatrix.Rotate(rotateY, m_modelViewMatrix[1], m_modelViewMatrix[5], m_modelViewMatrix[9]);

	// В ходе умножения матриц могут возникать погрешности, которые,
	// накапливаясь могут сильно искажать картинку
	// Для их компенсации после каждой модификации матрицы моделирования-вида
	// проводим ее ортонормирование
	m_modelViewMatrix.Normalize();

	// Событие об обновлении матрицы моделирования-вида
	class CRotationEvent
	{
	public:
		void operator()(IRotationControllerListener* pListener) const
		{
			pListener->OnRotationControllerUpdate();
		}
	};

	// Диспетчеризуем событие
	DispatchEvent(CRotationEvent());
}
