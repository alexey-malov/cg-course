#pragma once

#include "IApplicationListener.h"
#include "IEventDispatcher.h"
#include "Matrix4.h"

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
	// которой упавляет контроллер вращения
	void SetModelViewMatrix(CMatrix4d const& matrix);
	CMatrix4d const& GetModelViewMatrix() const;

private:
	// Обработчики событий о нажатии кнопки мыши и перемещении
	// курсора, а также события об изменении размеров окна
	virtual void OnReshape(int windowWidth, int windowHeight);
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnMotion(int x, int y);

	// Вращение камеры на заданные углы
	void RotateCamera(GLfloat rotateX, GLfloat rotateY);

	bool m_leftButtonPressed;
	int m_mouseX;
	int m_mouseY;
	int m_windowWidth;
	int m_windowHeight;

	// Матрица моделирования-вида, управляемая контроллером вращения
	CMatrix4d m_modelViewMatrix;
};
