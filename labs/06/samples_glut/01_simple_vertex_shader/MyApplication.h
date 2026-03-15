#pragma once
#include "../libgl/GLApplication.h"
#include "../libgl/RotationController.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char * title, int width, int height);
	~CMyApplication(void);
protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnMotion(int x, int y);
private:
	void InitShaders();

	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;
	static const double ZNEAR;
	static const double ZFAR;

	CRotationController m_rotationController;

	GLuint	m_program;
};
