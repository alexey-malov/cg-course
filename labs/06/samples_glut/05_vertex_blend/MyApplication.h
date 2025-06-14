#pragma once
#include "../libgl/GLApplication.h"

#include "../libgl/Shaders.h"
#include "../libgl/AnimationController.h"
#include "MorphShape.h"


class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char * title, int width, int height);
	~CMyApplication(void);
protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnIdle();
private:
	void InitShaders();

	// Количество вершин в фигуре
	static const int NUMBER_OF_VERTICES;
	// Максимальный радикус
	static const float SHAPE_MAX_RADIUS;
	// Минимальный радиус
	static const float SHAPE_MIN_RADIUS;
	// Период полной анимации морфинга
	static const float ANIMATION_PERIOD;

	CAnimationController m_animationController;

	// Трансформируемая фигура
	CMorphShape m_morphShape;

	// программный объект
	CProgram m_program;

	// Текущая фаза анимации
	float m_phase;
};
