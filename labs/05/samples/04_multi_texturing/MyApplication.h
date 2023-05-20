#pragma once

#include "../libgl/AnimationController.h"
#include "../libgl/GLApplication.h"
#include "../libgl/Texture.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char* title, int width, int height);

protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnIdle();

private:
	void DrawRectangle() const;
	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;

	// Расстояние до ближней и дальней плоскостей отсечения
	static const double ZNEAR;
	static const double ZFAR;
	static const float ROTATION_SPEED;

	CAnimationController m_animationController;
	CTexture2D m_car;
	CTexture2D m_smile;
	CTexture2D m_sun;
	CTexture2D m_medved;

	float m_rotationAngle;
};
