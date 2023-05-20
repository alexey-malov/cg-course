#pragma once

#include "../libgl/GLApplication.h"
#include "common_inc.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char* title, int width, int height);
	~CMyApplication();

protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);

private:
	static GLuint LoadTexture(std::wstring const& fileName);
	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;

	// Расстояние до ближней и дальней плоскостей отсечения
	static const double ZNEAR;
	static const double ZFAR;

	GLuint m_carTexture;
};
