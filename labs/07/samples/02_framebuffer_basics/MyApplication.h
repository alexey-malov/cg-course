#pragma once

#include "../libgl/AnimationController.h"
#include "../libgl/GLApplication.h"
#include "../libgl/IRotationControllerListener.h"
#include "../libgl/Matrix4.h"
#include "../libgl/RotationController.h"
#include "../libgl/Texture.h"

class CMyApplication
	: public CGLApplication
	, private IRotationControllerListener
{
public:
	CMyApplication(const char* title, int width, int height);
	~CMyApplication();

private:
	// Обработчики событий от приложения
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnIdle();

	// Инициализация вспомогательного буфера кадра
	void InitFrameBuffer();

	// Рисуем объект во текстуру, выбранную в качестве
	// буфера цвета вспомогательного буфера кадра
	void DrawObjectToTexture() const;

	// Получаем уведомление об обновлении состояния
	// контроллера вращения
	void OnRotationControllerUpdate();

	// Установка видового порта и матрицы проецирования
	static void SetupViewportAndProjectionMatrix(
		int width, int height,
		double fov, double znear, double zfar);

	// Параметры камеры
	static const double FIELD_OF_VIEW;
	;
	static const double ZNEAR;
	static const double ZFAR;

	// Скорость вращения объекта вокруг осей X, Y, Z
	static const double X_ROTATION_SPEED;
	static const double Y_ROTATION_SPEED;
	static const double Z_ROTATION_SPEED;

	// Ширина и высота вспомогательного буфера кадра
	static const GLuint FRAME_BUFFER_WIDTH;
	static const GLuint FRAME_BUFFER_HEIGHT;

	// Имя вспомогательного буфера кадра
	GLuint m_frameBuffer;
	// Имя вспомогательного буфера глубины
	GLuint m_depthBuffer;

	// Текстура, выполняющая роль буфера цвета во вспомогательном
	// буфере кадра
	CTexture2D m_colorBufferTexture;

	// Ширина и высота окна
	int m_windowWidth;
	int m_windowHeight;

	// Матрица моделирования-вида, используемая для вывода объекта
	// во вспомогательный буфер кадра
	CMatrix4d m_modelViewFrameBuffer;

	CRotationController m_rotationController;
	CAnimationController m_animationController;
};
