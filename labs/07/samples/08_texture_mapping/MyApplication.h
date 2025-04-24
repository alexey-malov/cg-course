#pragma once
#include "glapplication.h"

#include "RotationController.h"
#include "Sphere.h"
#include "shaders.h"
#include "texture.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char* title, int width, int height);
	~CMyApplication(void);

protected:
	void OnInit() override;
	void OnDisplay() override;
	void OnReshape(int width, int height) override;
	void OnMouse(int button, int state, int x, int y) override;
	void OnMotion(int x, int y) override;
	void OnKeyboard(unsigned char key, int /*x*/, int /*y*/) override;

private:
	void InitShaders();
	void LoadTextures();

	CRotationController m_rotationController;

	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;
	static const double ZNEAR;
	static const double ZFAR;

	CTexture2D m_sphereTexture;
	CTexture2D m_cloudsTexture;
	CSphere m_sphere;

	// программный объект
	CProgram m_program;
	// расположение uniform-переменной mainTexture
	GLint m_mainTextureSamplerLocation;
	// расположение uniform-переменной cloudsTexture
	GLint m_cloudsTextureSamplerLocation;
};
