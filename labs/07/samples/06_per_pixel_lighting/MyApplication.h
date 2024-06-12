#pragma once
#include "glapplication.h"

#include "shaders.h"
#include "RotationController.h"
#include "Sphere.h"
#include "texture.h"
#include "IRotationControllerListener.h"
#include "OmniLight.h"
#include "Material.h"

class CMyApplication 
	: public CGLApplication
	, private IRotationControllerListener
{
public:
	CMyApplication(const char * title, int width, int height);
	~CMyApplication(void);
protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnRotationControllerUpdate();
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

	COmniLight m_light;
	CMaterial m_material;
};
