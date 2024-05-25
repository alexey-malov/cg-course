#pragma once

#include "IApplicationListener.h"
#include "IEventDispatcher.h"

class IRotationControllerListener;

class CRotationController 
	: public CApplicationListener
	, public CEventDispatcherImpl<
		IEventDispatcher<IRotationControllerListener>, 
		IRotationControllerListener>
{
public:
	CRotationController(int windowWidth, int windowHeight);

	// Установка и получение матрицы моделирования-вида,
	// которой управляет контроллер вращения
	void SetModelViewMatrix(glm::dmat4x4 const& matrix);
	glm::dmat4x4 const& GetModelViewMatrix() const;


private:
	// Обработчики событий о нажатии кнопки мыши и перемещении 
	// курсора, а также события об изменении размеров окна
	virtual void OnReshape(int windowWidth, int windowHeight);
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnMotion(int x, int y);

	glm::dmat4x4 Orthonormalize(const glm::dmat4x4& m);

	// Вращение камеры на заданные углы
	void RotateCamera(GLdouble rotateX, GLdouble rotateY);

	bool m_leftButtonPressed;
	int m_mouseX;
	int m_mouseY;
	int m_windowWidth;
	int m_windowHeight;

	// Матрица моделирования-вида, управляемая контроллером вращения
	glm::dmat4x4 m_modelViewMatrix;
};
