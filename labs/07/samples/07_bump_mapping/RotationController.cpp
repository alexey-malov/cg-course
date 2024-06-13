#include "StdAfx.h"
#include "RotationController.h"
#include "IApplication.h"
#include "IRotationControllerListener.h"
#include <numbers>

CRotationController::CRotationController(int windowWidth, int windowHeight)
:m_leftButtonPressed(false)
,m_windowWidth(windowWidth)
,m_windowHeight(windowHeight)
, m_mouseY(0)
, m_mouseX(0)
, m_modelViewMatrix({})
{
}

void CRotationController::SetModelViewMatrix(glm::dmat4x4 const& matrix)
{
	m_modelViewMatrix = matrix;
}

glm::dmat4x4 const& CRotationController::GetModelViewMatrix() const
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
		GLdouble rotateX = GLdouble(dy) * std::numbers::pi / m_windowHeight;
		GLdouble rotateY = GLdouble(dx) * std::numbers::pi / m_windowWidth;

		RotateCamera(rotateX, rotateY);

		// Сохраняем текущие координаты мыши
		m_mouseX = x;
		m_mouseY = y;
	}
}

// Вращаем камеру вокруг начала координат на заданный угол
void CRotationController::RotateCamera(GLdouble rotateX, GLdouble rotateY)
{
	// Поворачиваем вокруг осей x и y камеры
	const glm::dvec3 xAxis{
		m_modelViewMatrix[0][0], m_modelViewMatrix[1][0], m_modelViewMatrix[2][0]
	};
	const glm::dvec3 yAxis{
		m_modelViewMatrix[0][1], m_modelViewMatrix[1][1], m_modelViewMatrix[2][1]
	};
	m_modelViewMatrix = glm::rotate(m_modelViewMatrix, rotateX, xAxis);
	m_modelViewMatrix = glm::rotate(m_modelViewMatrix, rotateY, yAxis);

	m_modelViewMatrix = Orthonormalize(m_modelViewMatrix);

	// Событие об обновлении матрицы моделирования-вида
	class CRotationEvent
	{
	public:
		void operator()(IRotationControllerListener * pListener)const
		{
			pListener->OnRotationControllerUpdate();
		}
	};

	// Диспетчеризуем событие
	DispatchEvent(CRotationEvent());
}

// Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
glm::dmat4x4 CRotationController::Orthonormalize(const glm::dmat4x4& m)
{
	// Извлекаем подматрицу 3*3 из матрицы m и ортонормируем её
	const auto normalizedMatrix = glm::orthonormalize(glm::dmat3x3{ m });
	// Заменяем 3 столбца исходной матрицы
	return {
		glm::dvec4{ normalizedMatrix[0], 0.0 },
		glm::dvec4{ normalizedMatrix[1], 0.0 },
		glm::dvec4{ normalizedMatrix[2], 0.0 },
		m[3]
	};
}