#pragma once
#include "glapplication.h"

#include "shaders.h"
#include "RotationController.h"
#include "IRotationControllerListener.h"
#include "Texture.h"
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
	void DrawQuad();
	void LoadTextures();

	CRotationController m_rotationController;

	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;
	static const double ZNEAR;
	static const double ZFAR;

	CTexture2D m_normalMap;
	CTexture2D m_diffuseMap;

	// программный объект
	CProgram m_program;
	GLint m_tangentLocation;
	GLint m_normalMapLocation;
	GLint m_diffuseMapLocation;

	COmniLight m_light;
	CMaterial m_material;
};
