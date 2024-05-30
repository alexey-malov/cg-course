#pragma once
#include "glapplication.h"
#include "RotationController.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char * title, int width, int height);
	~CMyApplication(void);
protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnMotion(int x, int y);
	virtual void OnReshape(int width, int height);
private:
	void DrawTriangle()const;
	void DrawCube()const;

	CRotationController m_rotationController;
	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;

	// Расстояние до ближней и дальней плоскостей отсечения
	static const double ZNEAR;
	static const double ZFAR;
};
