#pragma once
#include "glapplication.h"
#include "Cube.h"
#include "RotationController.h"
#include "IRotationControllerListener.h"
#include "OmniLight.h"
#include "Material.h"
#include "AnimationController.h"

class CMyApplication 
	: public CGLApplication
	, private IRotationControllerListener
{
public:
	CMyApplication(const char * title, int width, int height);
private:
	virtual void OnIdle();
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnRotationControllerUpdate();
private:
	// Рисуем источник света в виде сферы
	void DrawLightSource()const;
	// Рисуем освещенную, либо неосвещенную сцену
	void DrawScene(bool enableLighting)const;
	// Рисуем теневой объем куба
	void DrawShadowVolume()const;
	// Рисуем ребра теневого объема куба
	void DrawShadowVolumeEdges()const;
private:
	CCube m_cube;

	// Параметры камеры (ZFAR = +бесконечность)
	static const double FOV;
	static const double ZNEAR;

	// Коэффициент вытягивания силуэтных граней
	static const float EXTRUSION_FACTOR;

	// Контроллеры вращения и анимации
	CRotationController m_rotationController;
	CAnimationController m_animationController;

	// Точечный источник света и его положение в пространстве
	COmniLight m_light;
	glm::vec3 m_lightPosition;

	// Материал объектов сцены
	CMaterial m_material;

	// Фаза анимации источника света (0..2pi)
	float m_lightAnimationPhase;

	// Фаза анимации (угол вращения) торов
	float m_torusAnimationPhase;
};
