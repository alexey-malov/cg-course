#pragma once

#include "../libgl/GLApplication.h"
#include "../libgl/AnimationController.h"
#include "KeplerMotionLaws.h"

class CMainApp
	: public CGLApplication
{
	typedef CEllipticOrbit<double> CEllipticOrbitd;

public:
	CMainApp(const char* title, int width, int height);
	~CMainApp(void);

private:
	void OnDisplay();
	void OnReshape(int width, int height);
	void OnIdle();
	void OnInit();
	void OnKeyboard(unsigned char key, int x, int y);
	void OnSpecialKey(int key, int x, int y);
	void DrawPlanets();
	void DrawSun();
	void DrawOrbit(CEllipticOrbitd const& orbit);

private:
	static const double DEFAULT_SCALE;
	static const double MIN_SCALE;
	static const double MAX_SCALE;
	static const double SCALE_FACTOR;

	static const double DEFAULT_TIME_SPEED;
	static const double MIN_TIME_SPEED;
	static const double MAX_TIME_SPEED;
	static const double TIME_ADJUSTMENT;

	CAnimationController m_animationController;

	double m_time;

	double m_scale;

	double m_timeSpeed;

	int m_currentYear;

	enum
	{
		PLANET_COUNT = 10
	};

	struct PlanetInfo
	{
		CEllipticOrbitd orbit;
		DWORD color;
		float size;
	};
	static const PlanetInfo m_planets[PLANET_COUNT];
};
